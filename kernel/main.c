#include "uart.h"
#include "stdio.h"
#include "files.h"
#include "string.h"
#include "rand.h"
#include "spi.h"

#define CMD_LEN		64

static char buff[FS_MAXSIZE];

extern void enable_cache();

void command_line();
void basic(char *args);
void edit(char *args);

void help();
void printInfo();
void listFiles();
void deleteFile(char *args);
void renameFile(char *args);
void createFile(char *args);
void printFile(char *args);
void copyFile(char *args);
void formatFlash(char *args);
void printLogo();



void main(){
	uart_init();
	rand_init();
	spi_init();
	enable_cache();
	command_line();
}

void command_line(){
	char *arguments;
	char command[CMD_LEN+1];

	clear();
	printLogo();
	puts("Ready\n");

	while(1){
		puts(">");
		// clear buffer
		memset(command, 0, CMD_LEN+1);
		// read line
		gets(command, CMD_LEN);
		// get arguments
		arguments=command;
		while(*arguments!=' ' && *arguments!='\0') ++arguments;
		*arguments=0;
		++arguments;
		while(*arguments==' ' && *arguments!='\0') ++arguments;

		if(strcmp(command, "")==0) continue;
		else if(strcmp(command, "exit")==0) break;
		else if(strcmp(command, "help")==0) help();
		else if(strcmp(command, "clear")==0) clear();
		else if(strcmp(command, "if")==0) printInfo();
		else if(strcmp(command, "ls")==0) listFiles();
		else if(strcmp(command, "edit")==0) edit(arguments);
		else if(strcmp(command, "basic")==0) basic(arguments);
		else if(strcmp(command, "rm")==0) deleteFile(arguments);
		else if(strcmp(command, "mv")==0) renameFile(arguments);
		else if(strcmp(command, "nf")==0) createFile(arguments);
		else if(strcmp(command, "pr")==0) printFile(arguments);
		else if(strcmp(command, "cp")==0) copyFile(arguments);
		else if(strcmp(command, "fm")==0) formatFlash(arguments);
		else warn("Unknown command. Type 'help'.\n");
	}
	puts("Shutdown...\n");
}

void help(){
	puts(" basic - run basic interpreter.\n");
	puts(" edit - text editor\n");
	puts(" clear - clear screen\n");
	puts(" help - print help\n");
	puts(" exit - shutdown\n");
	puts(" if - print flash info\n");
	puts(" ls - list all files\n");
	puts(" rm - remove file\n");
	puts(" mv - rename file\n");
	puts(" nf - create empty\n");
	puts(" pr - print file\n");
	puts(" cp - copy file\n");
	puts(" fm - format flash\n");
}

void deleteFile(char *args){
	if(!*args){
		warn("Illegal arguments. Usage: rm [file]\n");
		return;
	}
	if(!remove(args)){
		puts(GREEN);
		puts(" File removed.\n");
		puts(WHITE);
	}else{
		warn("File not exist.\n");
	}
}

void renameFile(char *args){
	char *n1=args;
	char *n2=args;

	if(!*args){
		warn("Illegal arguments. Usage: mv [fold] [fnew]\n");
		return;
	}

	while(*n2!=' ' && *n2!='\0') ++n2;
	*n2=0;
	++n2;
	while(*n2==' ' && *n2!='\0') ++n2;

	if(!*n2){
		warn("Illegal arguments.\n");
		return;
	}

	if(!rename(n1, n2)){
		puts(GREEN);
		puts(" File renamed.\n");
		puts(WHITE);
	}else{
		warn("File not exist.\n");
	}
}

void createFile(char *args){
	if(!*args){
		warn("Illegal arguments. Usage: nf [file]\n");
		return;
	}

	if(exist(args)>=0){
		warn("File already exist.\n");
		return;
	}

	if(!save(args, 0, 0)){
		puts(GREEN);
		puts(" File created.\n");
		puts(WHITE);
	}else{
		warn("No free space.\n");
	}
}

void printFile(char *args){
	int i, size;
	if(!*args){
		warn("Illegal arguments. Usage: pr [file]\n");
		return;
	}

	size=load(args, buff);
	if(size<0){
		warn("File not exist.\n");
		return;
	}

	for(i=0;i<size;++i){
		putc(buff[i]);
	}
	puts("\n");
}

void listFiles(){
	int n=0;
	char buff[FS_MAXNAME+1];

	while(list(&n, buff)>=0){
		puts(" ");
		puts(buff);
		puts("\t\t");
		itoa(exist(buff), buff, 10);
		puts(buff);
		puts(" B\n");
	}
}

void formatFlash(char *args){
	char b[8];
	if(!*args){
		warn("Illegal arguments. Usage: fm [name]\n");
		return;
	}

	warn("All data on flash memory will be removed.\n");
	puts(" yes - format\n");
	puts(" no - cancel\n");
	puts("format>");

	gets(b, 8);

	if(strcmp(b, "yes")==0){
		format(args);
		puts(GREEN);
		puts(" Flash formated.\n");
		puts(WHITE);
	}	
}

void printInfo(){
	int n, c;
	char b[16];

	puts(" Capacity: ");
	itoa(FS_CAPACITY/1024, b, 10);
	puts(b);
	puts(" kB\n");

	puts(" Max file size: ");
	itoa(FS_MAXSIZE/1024, b, 10);
	puts(b);
	puts(" kB\n");

	c=0;
	n=0;
	while(list(&n, b)>=0) ++c;

	puts(" Free: ");
	itoa(FS_MAXFILES-c, b, 10);
	puts(b);
	puts("/");
	itoa(FS_MAXFILES, b, 10);
	puts(b);
	puts("\n");
}

void copyFile(char *args){
	int size;
	char *n1=args;
	char *n2=args;

	if(!*args){
		warn("Illegal arguments. Usage cp [f1] [f2]\n");
		return;
	}

	while(*n2!=' ' && *n2!='\0') ++n2;
	*n2=0;
	++n2;
	while(*n2==' ' && *n2!='\0') ++n2;

	if(!*n2){
		warn("Illegal arguments.\n");
		return;
	}
	if(exist(n1)<0){
		warn("File 'f1' not exist.\n");
		return;
	}
	if(exist(n2)>=0){
		warn("File 'f2' already exist.\n");
		return;
	}

	size=load(n1, buff);
	if(save(n2, buff, size)<0){
		warn("No free space.\n");
	}else{
		puts(GREEN);
		puts(" File copy finish.\n");
		puts(WHITE);
	}
}

void printLogo(){
	puts("\x1B[0m  \x1B[41m  \x1B[0m  \x1B[41m  \x1B[0m  \x1B[0m  \x1B[44m  \x1B[0m  \x1B[0m  \x1B[0m  \x1B[42m  \x1B[42m  \x1B[0m\n");
	puts("\x1B[0m  \x1B[41m  \x1B[41m  \x1B[41m  \x1B[0m  \x1B[44m  \x1B[0m  \x1B[44m  \x1B[0m  \x1B[42m  \x1B[0m  \x1B[0m  \x1B[0m\n");
	puts("\x1B[0m  \x1B[41m  \x1B[41m  \x1B[41m  \x1B[0m  \x1B[44m  \x1B[0m  \x1B[44m  \x1B[0m  \x1B[0m  \x1B[42m  \x1B[0m  \x1B[0m\n");
	puts("\x1B[0m  \x1B[41m  \x1B[0m  \x1B[41m  \x1B[0m  \x1B[44m  \x1B[0m  \x1B[44m  \x1B[0m  \x1B[0m  \x1B[0m  \x1B[42m  \x1B[0m\n");
	puts("\x1B[0m  \x1B[41m  \x1B[0m  \x1B[41m  \x1B[0m  \x1B[0m  \x1B[44m  \x1B[0m  \x1B[0m  \x1B[42m  \x1B[42m  \x1B[0m  \x1B[0m\n");
	puts("\n");
}
