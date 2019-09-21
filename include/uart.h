#pragma once

void uart_init();
int uart_send(int c);
int uart_recv(int *c);

