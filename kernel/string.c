#include "string.h"


void *memset(void *s, int c, int n){
    int p;
    char *m=(char*)s;

    for(p=0;p<n;p++,m++) *m=(char)c;
    return s;
}

int strlen(const char *s){
    int size=0;
    while(*s++) ++size;
    return size;
}

char *strcpy(char *dest, const char *src){
    int i;
    for(i=0;src[i];i++){
        dest[i]=src[i];
    }
    dest[i]=0;
    return dest;
}

char *strncpy(char *dest, const char *src, int n){
    int i;
    for(i=0;src[i] && i<n;i++){
        dest[i]=src[i];
    }
    dest[i]=0;
    return dest;
}

int strcmp(const char *s1, const char *s2){
    int i;
    for(i=0;s1[i] || s2[i];i++){
        if(s1[i]<s2[i]) return -1;
        else if(s1[i]>s2[i]) return 1;
    }
    return 0;
}

int strncmp(const char *s1, const char *s2, int n){
    int i;
    for(i=0; i<n &&(s1[i] || s2[i]);i++){
        if(s1[i]<s2[i]) return -1;
        else if(s1[i]>s2[i]) return 1;
    }
    return 0;
}

int atoi(char *str){
    int i, res=0;
    for(i=0;str[i]!='\0';++i){
        res=res*10+str[i]-'0'; 
    }
    return res; 
}

char* itoa(int num, char *str, int base){
    int i=0; 
    int negative=0; 
  
    if(num==0){
        str[i++]='0'; 
        str[i]='\0'; 
        return str; 
    } 
  
    if(num<0 && base==10){
        negative=1; 
        num= -num; 
    } 
  
    while(num!=0){
        int rem=num%base; 
        str[i++]=(rem>9)?(rem-10)+'A' : rem+'0'; 
        num /= base; 
    } 
  
    if(negative){
        str[i++]='-'; 
    }
  
    str[i]='\0';
  
    // Reverse the string
    int x, h=i/2;
    char pom;
    for(x=0;x<h;++x){
    	pom=str[x];
    	str[x]=str[i-x-1];
    	str[i-x-1]=pom;
    } 
  
    return str; 
}
