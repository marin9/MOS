#include "stdlib.h"
#include "timer.h"
#include "rand.h"


int rand(){
	return rand_get();
}

int pov(int x, int p){
	if(p==0) return 1;
	while(p>0){
		x *= x;
		--p;
	}
	return x;
}


unsigned int time_get(){
	return timer_get();
}

void pause(unsigned int ms){
	unsigned int t0=timer_get();
	ms *= 1000;
	while((timer_get()-t0)<ms);
}