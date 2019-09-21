#include "stdio.h"
#include "uart.h"
#include "string.h"


int puts(char *s){
	int i=0;
	while(s[i]){
		while(!uart_send(s[i]));
		if(s[i]=='\n'){
			while(!uart_send('\r'));
		}
		++i;
	}
	return i;
}

int putc(char c){
	while(!uart_send(c));
	if(c=='\n'){
		while(!uart_send('\r'));
	}
	return 1;
}

int putn(int n){
	char buff[16];
	itoa(n, buff, 10);
	return puts(buff);
}

int gets(char *s, int n){
	int c, i=0;
	while(i<n){
		while(!uart_recv(&c));
		if(c==127){
			if(--i<0){
				i=0;
				continue;
			}
			s[i]=0;
			puts("\b \b");
			continue;
		}
		while(!uart_send(c));
		s[i]=(char)c;
		if(c=='\r'){
			s[i]=0;
			break;
		}
		++i;
	}
	puts("\r\n");
	return i;
}

void warn(char *s){
	puts(YELLOW);
	puts(" WARNING: ");
	puts(s);
	puts(WHITE);
}

void err(char *s){
	puts(RED);
	puts(" ERROR: ");
	puts(s);
	puts(WHITE);
}

void clear(){
	puts(WHITE);
	puts("\033[H\033[J");
}

int getkey(){
	int c;
	if(uart_recv(&c)) return c;
	else return 0;
}
