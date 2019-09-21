#include "stdio.h"
#include "string.h"
#include "files.h"

#define K_NOKEY		0
#define K_ESCAPE	27
#define K_BACKSPACE	127
#define K_CONTROL_U	21
#define K_CONTROL_D	4
#define K_CONTROL_S	19
#define K_CONTROL_E	5

static int size;
static int cursor;
static int skipLines;
static char *p;
static char *name;
static char buff[FS_MAXBYTES+1];


void refreshScreen(){
	int i, l;

	clear();
	puts(YELLOW);
	puts(" ");
	puts(name);
	puts(" (");
	putn(size);
	puts(" B)\n\n");
	puts(WHITE);

	i=0;
	l=0;
	while(i<=size){
		if(l>=skipLines && (l-skipLines)<16){
			putn(l+1);
			puts("  ");
			while(buff[i]!='\n' && buff[i]!='\0'){
				putc(buff[i]);
				++i;
			}	
			if((buff+i)==p){
				puts(B_RED);
				puts("<");
				puts(B_BLACK);
			}
			puts("\n");
			if(buff[i]==0){
				break;
			}
			++l;
		}else if(buff[i]=='\n'){
			++l;
		}
		++i;
	}
	puts(B_DARKGRAY);
	puts(" ^U-up ^D-down ^S-save ESC-exit");
	puts(B_BLACK);
}

void scrollUp(){
	--cursor;
	if(cursor<0){
		cursor=0;
		--skipLines;
	}

}

void scrollDown(){
	++cursor;
	if(cursor>=16){
		cursor=15;
		++skipLines;
	}
}

void moveTextDown(){
	int x=0;
	while(size!=0 && (buff+size-x)!=p){
		buff[size-x]=buff[size-x-1];
		++x;
	}
	buff[++size]=0;
}

void moveTextUp(){
	char *tp=p;
	if(p>buff){
		if(*(p-1)=='\n'){
			scrollUp();
		}
		while(tp<=(buff+size)){
			*(tp-1)=*(tp);
			++tp;
		}
		--p;
		--size;
	}else{
		while(tp<(buff+size)){
			*(tp)=*(tp+1);
			++tp;
		}
		p=buff;
		while(*p!='\n' && *p!='\0') ++p;
		--size;
	}
}

void moveCursorUp(){
	char *tp=p-1;
	while(tp>buff && *tp!='\n') --tp;
	if(tp>buff || (*tp=='\n' && tp==buff)){
		p=tp;
		scrollUp();
	}	
}

void moveCursorDown(){
	if(*p){
		++p;
		while(*p!='\n' && *p!='\0') ++p;
		scrollDown();
	}
}

void saveFile(){
	int c;
	puts("\nSave changes ? y/n>");
	while((c=getkey())==0);
	if(c=='y' || c=='Y'){
		if(save(name, buff, size)<0){
			warn("No memory.\n");
		}else{
			puts(" File saved.\n");
		}
	}
}


void edit(char *args){
	int c;
	if(!*args){
		warn("No argument. Usage: edit [file]\n");
		return;
	}

	size=load(args, buff);
	if(size<0) size=0;
	name=args;
	buff[size]=0;
	cursor=0;
	skipLines=0;

	p=buff;
	while(*p!='\n' && *p!='\0') ++p;

	refreshScreen();
	while(1){
		c=getkey();
		if(c==K_NOKEY) continue;
		else if(c==K_CONTROL_U) moveCursorUp();
		else if(c==K_CONTROL_D) moveCursorDown();
		else if(c==K_CONTROL_S) saveFile();
		else if(c==K_BACKSPACE){
			if(size!=0){
				moveTextUp();
			}
		}else if(c==K_ESCAPE){
			saveFile();
			break;
		}else if(size<FS_MAXBYTES){
			moveTextDown();
			if(c=='\r'){
				*p='\n';
				scrollDown();
			}else{
				*p=c;
			}
			++p;
		}
		refreshScreen();
	}
	puts("\n");
}
