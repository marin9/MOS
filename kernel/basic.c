#include "stdio.h"
#include "files.h"
#include "string.h"
#include "stdlib.h"
#include "gpio.h"

#define STACKSIZE	32

static char *p;
static int var[26];
static int sp, errf, size;
static char *stack[STACKSIZE];
static char buff[FS_MAXSIZE+1];
static char memory[FS_MAXSIZE*2];


static int parseExpression();
static void parseStatment();
static void skipSpaces();


static int isDigit(char c){
	if(c>='0' && c<='9') return 1;
	else return 0;
}

static int isAlpha(char c){
	if(c>='A' && c<='Z') return 1;
	if(c>='a' && c<='z') return 1;
	return 0;
}

static void error(char *msg){
	int ln=0;
	char *t=buff;
	if(!errf){
		errf=1;
		while(t!=p && *t){
			if(*t=='\n') ++ln;
			++t;
		}
		puts("\n");
		err(msg);
		puts(" Line: ");
		putn(ln+1);
		puts("\n");
	}
}

static void skipLabel(){
	char *t=p;
	skipSpaces();
	while(isAlpha(*p) || isDigit(*p)) ++p;

	if(*p==':') ++p;
	else p=t;
}

static void skipSpaces(){
	while(*p==' ' || *p=='\t') ++p;
}

static int parseNumber(){
	int i=0;
	char b[16];
	while(*p>='0' && *p<='9'){
		b[i++]=*p++;
	}
	b[i]=0;
	return atoi(b);
}

static int parseFactor(){
	int a=0;
	if(*p>='0' && *p<='9'){
		a=parseNumber();
	}else if(*p>='A' && *p<='Z'){
		a=var[(int)(*p-'A')];
		++p;
	}else if(*p=='('){
		++p;
		a=parseExpression();
		if(*p!=')') error("Missing bracket.");
		else ++p;
	}else{
		error("Syntax error.");
	}
	return a;
}

static int parseTerm(){
	int a, b;
	a=parseFactor();
	while(1){
		if(*p=='*'){
			p++;
			b=parseFactor();
			a*=b;
		}else if(*p=='/'){
			p++;
			b=parseFactor();
			if(b!=0) a/=b;
			else error("Divide by zero.");
		}else if(*p=='&'){
			p++;
			b=parseFactor();
			a=a&&b;
		}else if(*p=='|'){
			p++;
			b=parseFactor();
			a=a||b;
		}else if(*p=='%'){
			p++;
			b=parseFactor();
			a=(!a && b) || (a && !b);
		}else if(*p=='='){
			p++;
			b=parseFactor();
			a=(a==b);
		}else if(*p=='>'){
			p++;
			b=parseFactor();
			a=(a>b);
		}else if(*p=='<'){
			p++;
			b=parseFactor();
			a=(a<b);
		}else if(*p=='!'){
			p++;
			b=parseFactor();
			a=(a!=b);
		}else{
			return a;
		}
	}
}

static int parseExpression(){
	int a, b;
	if(*p=='-' || *p=='+') a=0;
	else a=parseTerm();

	while(1){
    	if(*p=='-'){
      		p++;
			b=parseTerm();
			a-=b;
		}else if(*p=='+'){
			p++;
			b=parseTerm();
			a+=b;
		}else{
			return a;
		}
	}
}

static void parseCR(){
	skipSpaces();
	if(*p==0) error("Try to execute out of memory.");
	else if(*p=='\n') ++p;
	else error("Syntax error.");
}

static void parseString(){
	if(*p=='"'){
		++p;
		while(*p!='"' && *p!='\n' && *p!='\0'){
			putc(*p++);
		}
		if(*p!='"') error("Syntax error.");
		else ++p;
	}
}

static void parsePrint(){
	while(1){
		skipSpaces();
		if(*p=='"'){
			parseString();
		}else{
			putn(parseExpression());
		}
		skipSpaces();
		if(*p==',') ++p;
		else break;
	}
	if(*p==';') ++p;
	else puts("\n");
}

static void parseEnd(){
	puts("\n");
	if(*p!='\n' && *p!=0) error("Syntax error.");
	else errf=1;
}

static void parseInput(){
	char num[16];
	if(*p>='A' && *p<='Z'){
		gets(num, 16-1);
		var[(int)(*p-'A')]=atoi(num);
		++p;
	}else{
		error("Syntax error.");
	}
}

static void parseLet(){
	int v;
	if(*p>='A' && *p<='Z'){
		v=(int)(*p-'A');
		++p;
		if(*p!='=') error("Syntax error.");
		++p;
		var[v]=parseExpression();
	}else{
		error("Syntax error.");
	}	
}

static void parseIf(){
	if(!parseExpression()){
		while(*p!='\n' && *p!='\0') ++p;
		return;
	}
	skipSpaces();		
	if(strncmp(p, "THEN ", 5)!=0){
		error("Syntax error.");
		return;
	}
	p+=5;
	skipSpaces();
	parseStatment();
}

static void parseGoto(){
	int ln, i=0;
	char *t=p;
	char label[32];
	while(isAlpha(*p) || isDigit(*p)){
		label[i++]=*p++;
	}
	label[i++]=':';
	label[i]=0;
	skipSpaces();
	if(*p!=0 && *p!='\n'){
		error("Syntax error.");
		return;
	}

	p=buff;
	ln=0;
	while(1){
		skipSpaces();
		if(strncmp(p, label, i)==0){
			skipLabel();
			skipSpaces();
			parseStatment();
			return;
		}
		while(*p!='\n' && *p!='\0') ++p;
		if(*p==0){
			p=t;
			error("Label not found.");
			break;
		}else{
			++ln;
			++p;
		}
	}
}

static void parseGosub(){
	char *t=p;
	while(*t!='\n' && *t!=0) ++t;
	stack[sp++]=++t;
	if(sp>=STACKSIZE){
		error("Stack overflow.");
		return;
	}
	parseGoto();
}

static void parseRet(){
	if(*p!='\n' && *p!=0){
		error("Syntax error.");
		return;
	}
	--sp;
	if(sp<0){
		error("RET without GOSUB.");
		return;
	}
	p=stack[sp];
	skipLabel();
	parseStatment();
}

static void parsePause(){
	int a=parseExpression();
	if(a<0) a=0;
	pause(a);
}

static void parseClear(){
	clear();
}

static void parseRand(){
	if(*p>='A' && *p<='Z'){
		var[(int)(*p-'A')]=rand();
		++p;
	}else{
		error("Syntax error.");
	}
}

static void parseGetkey(){
	if(*p>='A' && *p<='Z'){
		var[(int)(*p-'A')]=getkey();
		++p;
	}else{
		error("Syntax error.");
	}
}

static void parsePwrite(){
	int s, n=parseExpression();
	skipSpaces();
	if(*p!=','){
		error("Syntax error.");
		return;
	}
	++p;
	skipSpaces();
	s=parseExpression();
	gpio_set(n, s);
}

static void parsePread(){
	int n=parseExpression();
	skipSpaces();
	if(*p!=','){
		error("Syntax error.");
		return;
	}
	++p;
	skipSpaces();
	if(*p>='A' && *p<='Z'){
		var[(int)(*p-'A')]=gpio_get(n);
		++p;
	}else{
		error("Syntax error.");
	}
}

static void parsePmode(){
	int n=parseExpression();
	skipSpaces();
	if(*p!=','){
		error("Syntax error.");
		return;
	}
	++p;
	skipSpaces();
	if(strncmp(p, "IN", 2)==0){
		gpio_mode(n, INPUT);
		p+=2;
	}else if(strncmp(p, "OUT", 3)==0){
		gpio_mode(n, OUTPUT);
		p+=3;
	}else error("Syntax error.");
}

static void parseFcolor(){
	int c=parseExpression();
	if(c==0) puts(WHITE);
	else if(c==1) puts(RED);
	else if(c==2) puts(GREEN);
	else if(c==3) puts(YELLOW);
	else if(c==4) puts(BLUE);
	else if(c==5) puts(MAGENTA);
	else if(c==6) puts(CYAN);
	else if(c==7) puts(GRAY);
	else error("Illegal color number. Only [0, 7]");
}

static void parseBcolor(){
	int c=parseExpression();
	if(c==0) puts(B_BLACK);
	else if(c==1) puts(B_RED);
	else if(c==2) puts(B_GREEN);
	else if(c==3) puts(B_YELLOW);
	else if(c==4) puts(B_BLUE);
	else if(c==5) puts(B_MAGENTA);
	else if(c==6) puts(B_CYAN);
	else if(c==7) puts(B_GRAY);
	else error("Illegal color number. Only [0, 7]");
}

static void parsePutc(){
	int c=parseExpression();
	putc(c);
}

static void parseLd(){
	int v, a;
	if(*p>='A' && *p<='Z'){
		v=*p-'A';
		++p;	
	}else{
		error("Syntax error.");
		return;
	}
	skipSpaces();
	if(*p!=','){
		error("Syntax error.");
		return;
	}
	++p;
	skipSpaces();
	a=parseExpression();
	if(a<0 || a>(2*FS_MAXSIZE)){
		error("Out of memory.");
		return;
	}
	var[v]=memory[a];
}

static void parseSt(){
	int val, adr;
	val=parseExpression();
	skipSpaces();
	if(*p!=','){
		error("Syntax error.");
		return;
	}
	++p;
	skipSpaces();
	adr=parseExpression();
	if(adr<0 || adr>(2*FS_MAXSIZE)){
		error("Out of memory.");
		return;
	}
	memory[adr]=val;
}

static void parseGets(){
	int a, s;
	a=parseExpression();
	skipSpaces();
	if(*p!=','){
		error("Syntax error.");
		return;
	}
	++p;
	skipSpaces();
	s=parseExpression();
	if(a<0 || a>(2*FS_MAXSIZE) || s<0 || (a+s)>(2*FS_MAXSIZE)){
		error("Out of memory.");
		return;
	}
	var['S'-'A']=gets(memory+a, s);
}

static void parseData(){
	int a, d;
	a=parseExpression();
	if(a<0 || a>(2*FS_MAXSIZE)){
		error("Out of memory.");
		return;
	}
	skipSpaces();
	if(*p!=','){
		error("Syntax error.");
		return;
	}
	++p;
	skipSpaces();

	while(1){
		d=parseExpression();
		if(errf) return;
		memory[a++]=d;
		skipSpaces();
		if(*p==',') ++p;
		else break;
		skipSpaces();
	}
}

static void parseAtoi(){
	int a=parseExpression();
	skipSpaces();
	if(*p!=','){
		error("Syntax error.");
		return;
	}
	++p;
	skipSpaces();
	if(*p>='A' && *p<='Z'){
		var[*p-'A']=atoi(memory+a);
		++p;
	}else{
		error("Syntax error.");
	}
}

static void parseItoa(){
	int num, a;
	if(*p>='A' && *p<='Z'){
		num=var[*p-'A'];
		++p;
	}else{
		error("Syntax error.");
		return;
	}
	skipSpaces();
	if(*p!=','){
		error("Syntax error.");
		return;
	}
	++p;
	skipSpaces();
	a=parseExpression();
	itoa(num, memory+a, 10);
}

static void parsePuts(){
	int a=parseExpression();
	var['S'-'A']=puts(memory+a);
}

static void parseDelete(){
	int i=0;
	char name[FS_MAXNAME];
	while(*p!=' ' && *p!='\n' && *p!='\0' && i<(FS_MAXNAME-1)){
		name[i++]=*p++;
	}
	name[i]=0;
	var['S'-'A']=remove(name);
}

static void parseSize(){
	int i=0;
	char name[FS_MAXNAME];
	while(*p!=' ' && *p!='\n' && *p!='\0' && i<(FS_MAXNAME-1)){
		name[i++]=*p++;
	}
	name[i]=0;
	var['S'-'A']=exist(name);
}

static void parseLoad(){
	int i=0, a=parseExpression();
	char name[FS_MAXNAME];

	skipSpaces();
	if(*p!=','){
		error("Syntax error.");
		return;
	}
	++p;
	skipSpaces();

	while(*p!='\n' && *p!='\0' && *p!=' ' && i<(FS_MAXNAME-1)){
		name[i++]=*p++;
	}
	name[i]=0;

	var['S'-'A']=load(name, memory+a);
}

static void parseSave(){
	int s, i=0, a=parseExpression();
	char name[FS_MAXNAME];

	skipSpaces();
	if(*p!=','){
		error("Syntax error.");
		return;
	}
	++p;
	skipSpaces();
	s=parseExpression();

	skipSpaces();
	if(*p!=','){
		error("Syntax error.");
		return;
	}
	++p;
	skipSpaces();

	while(*p!='\n' && *p!='\0' && *p!=' ' && i<(FS_MAXNAME-1)){
		name[i++]=*p++;
	}
	name[i]=0;

	var['S'-'A']=save(name, memory+a, s);
}

static void parseFiles(){
	int i, n, a=parseExpression();
	char buff[FS_MAXNAME+1];

	if(a<0 || a>=(FS_MAXSIZE*2)){
		error("Out of memory.");
		return;
	}

	n=0;
	while(list(&n, buff)>=0){
		i=0;
		while(buff[i]){
			memory[a++]=buff[i++];
		}
		memory[a++]='\n';
	}
	memory[a++]=0;
	var['S'-'A']=a;
}


static void parseStatment(){
	skipSpaces();
	if(strncmp(p, "REM ", 4)==0){
		p+=4;
		while(*p!='\n' && *p!='\0') ++p;
	}else if(strncmp(p, "PRINT ", 6)==0){
		p+=6;
		skipSpaces();
		parsePrint();
	}else if(strncmp(p, "INPUT ", 6)==0){
		p+=6;
		skipSpaces();
		parseInput();
	}else if(strncmp(p, "GOTO ", 5)==0){
		p+=5;
		skipSpaces();
		parseGoto();
	}else if(strncmp(p, "GOSUB ", 6)==0){
		p+=6;
		skipSpaces();
		parseGosub();
	}else if(strncmp(p, "IF ", 3)==0){
		p+=3;
		skipSpaces();
		parseIf();
	}else if(strncmp(p, "LET ", 4)==0){
		p+=4;
		skipSpaces();
		parseLet();
	}else if(strncmp(p, "RET", 3)==0){
		p+=3;
		skipSpaces();
		parseRet();
	}else if(strncmp(p, "END", 3)==0){
		p+=3;
		skipSpaces();
		parseEnd();
	}else if(strncmp(p, "PAUSE ", 6)==0){
		p+=6;
		skipSpaces();
		parsePause();
	}else if(strncmp(p, "CLEAR", 5)==0){
		p+=5;
		skipSpaces();
		parseClear();
	}else if(strncmp(p, "RAND ", 5)==0){
		p+=5;
		skipSpaces();
		parseRand();
	}else if(strncmp(p, "GETKEY ", 7)==0){
		p+=7;
		skipSpaces();
		parseGetkey();
	}else if(strncmp(p, "PMODE ", 6)==0){
		p+=6;
		skipSpaces();
		parsePmode();
	}else if(strncmp(p, "PREAD ", 6)==0){
		p+=6;
		skipSpaces();
		parsePread();
	}else if(strncmp(p, "PWRITE ", 7)==0){
		p+=7;
		skipSpaces();
		parsePwrite();
	}else if(strncmp(p, "FCOLOR ", 7)==0){
		p+=7;
		skipSpaces();
		parseFcolor();
	}else if(strncmp(p, "BCOLOR ", 7)==0){
		p+=7;
		skipSpaces();
		parseBcolor();
	}else if(strncmp(p, "PUTC ", 5)==0){
		p+=5;
		skipSpaces();
		parsePutc();
	}else if(strncmp(p, "LD ", 3)==0){
		p+=3;
		skipSpaces();
		parseLd();
	}else if(strncmp(p, "ST ", 3)==0){
		p+=3;
		skipSpaces();
		parseSt();
	}else if(strncmp(p, "GETS ", 5)==0){
		p+=5;
		skipSpaces();
		parseGets();
	}else if(strncmp(p, "DATA ", 5)==0){
		p+=5;
		skipSpaces();
		parseData();
	}else if(strncmp(p, "ATOI ", 5)==0){
		p+=5;
		skipSpaces();
		parseAtoi();
	}else if(strncmp(p, "ITOA ", 5)==0){
		p+=5;
		skipSpaces();
		parseItoa();
	}else if(strncmp(p, "PUTS ", 5)==0){
		p+=5;
		skipSpaces();
		parsePuts();
	}else if(strncmp(p, "DELETE ", 7)==0){
		p+=7;
		skipSpaces();
		parseDelete();
	}else if(strncmp(p, "SIZE ", 5)==0){
		p+=5;
		skipSpaces();
		parseSize();
	}else if(strncmp(p, "LOAD ", 5)==0){
		p+=5;
		skipSpaces();
		parseLoad();
	}else if(strncmp(p, "SAVE ", 5)==0){
		p+=5;
		skipSpaces();
		parseSave();
	}else if(strncmp(p, "FILES ", 6)==0){
		p+=6;
		skipSpaces();
		parseFiles();
	}
}

void printGrammar(){
	puts(" BASIC grammar:\n");

	puts("  line := (label:)? statment? CR\n");
	puts("  statment := PRINT (str|expr) (, (str|expr))* ;?\n");
	puts("\tIF expr THEN statment\n");
	puts("\tGOTO label\n");
	puts("\tINPUT var\n");
	puts("\tLET var=expr\n");
	puts("\tGOSUB label\n");
	puts("\tREM ...\n");
	puts("\tRET\n");
	puts("\tEND\n");
	puts("\tPAUSE expr\n");
	puts("\tCLEAR\n");
	puts("\tRAND var\n");
	puts("\tGETKEY var\n");
	puts("\tPMODE p_expr, (IN|OUT)\n");
	puts("\tPREAD p_expr, var\n");
	puts("\tPWRITE p_expr, s_expr\n");
	puts("\tFCOLOR expr\n");
	puts("\tBCOLOR expr\n");
	puts("\tPUTC expr\n");
	puts("\tLD var, m_expr\n");
	puts("\tST expr, m_expr\n");
	puts("\tGETS m_expr, s_expr\n");
	puts("\tPUTS m_expr\n");
	puts("\tDELETE str\n");
	puts("\tSIZE str\n");
	puts("\tLOAD m_expr, str\n");
	puts("\tSAVE m_expr, s_expr, str\n");
	puts("\tFILES m_expr\n");

	puts("  expr := (+|-|e) term ((+|-) term)*\n");
	puts("  term := factor ((*|/) factor)*\n");
	puts("  factor := var|num|(expr)\n");
	puts("  str := \"...\"\n");
	puts("  label := name:\n");
	puts("\n");
}

void basic(char *args){
	if(*args==0){
		warn("Illegal arguments. Usage:\n\tbasic [file]\n\tbasic grammar\n");
		return;
	}

	if(strcmp(args, "grammar")==0){
		printGrammar();
		return;
	}

	size=load(args, buff);
	if(size<0){
		warn("File not exist.\n");
		return;
	}
	buff[size]=0;
	p=buff;
	errf=0;
	sp=0;

	while(!errf){
		skipLabel();
		parseStatment();
		parseCR();
	}
	puts(WHITE);
	puts(B_BLACK);
}


/* GRAMMAR
line := (label:)? statment? CR
statment := PRINT (str|expr) (, (str|expr))* ;?
			IF expr THEN statment
			GOTO label
			INPUT var
			LET var=expr
			GOSUB label
			REM ...
			RET
			END
			PAUSE expr
			CLEAR
			RAND var
			GETKEY var
			PMODE p_expr, (IN|OUT)
			PREAD p_expr, var
			PWRITE p_expr, s_expr
			FCOLOR expr
			BCOLOR expr
			PUTC expr
			LD var, m_expr
			ST expr, m_expr
			GETS m_expr, s_expr
			PUTS m_expr
			DELETE str
			SIZE str
			LOAD m_expr, str
			SAVE m_expr, s_expr, str
			FILES m_expr

expr := (+|-|e) term ((+|-) term)*
term := factor ((*|/) factor)*
factor := var|num|(expr)
str := "..."
label := name:
*/
