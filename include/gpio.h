#pragma once

#define INPUT	0
#define OUTPUT	1
#define LOW 	0
#define HIGH 	1

#define PULL_OFF	0
#define PULL_DOWN	1
#define PULL_UP		2


void gpio_mode(int pin, int mode);
void gpio_pull(int pin, int pmode);
void gpio_set(int pin, int stat);
int  gpio_get(int pin);


