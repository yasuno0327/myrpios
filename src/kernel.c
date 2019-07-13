#include "mini_uart.h"

// Works with mini uart

void kernel_main(void) {
  uart_init();
  uart_send_string("Hello world!!\r\n"); // print hello world

  while(1) {
    uart_send(uart_recv()); // ユーザーから入力を受け取って送信し、画面に戻る
  }
}