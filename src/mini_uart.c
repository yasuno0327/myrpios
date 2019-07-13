#include "utils.h"
#include "peripherals/mini_uart.h"
#include "peripherals/gpio.h"


/*
  put32, get32 function
  32ビットレジスタへの書き込み、読み込みを行う。
*/

void uart_init(void) {
  unsigned int selector;

  selector = get32(GPFSEL1);
  selector &= ~(7<<12); // 7*2の12乗(12左ビットシフト)を反転させ(NOT),and演算したものをselectorに代入 (clean gpio14)
  selector |= 2<<12; // 2*2の12乗(12左ビットシフト)とselectorでOR演算したものをselectorに代入
  selector &= ~(7<<15); // 7*2の15乗を反転させたもの(NOT)をselectorとAND演算したものをselectorに代入
  selector |= 2<<15;
  put32(GPFSEL1, selector);

  put32(GPPUD, 0);
  delay(150);
  put32(GPPUDCLD0, 0);

  put32(AUX_ENABLES, 1);          // mini_uartの有効化
  put32(AUX_MU_CNTL_REG, 0);      // auto flow control, receiver, transmitterの無効化
  put32(AUX_MU_IER_REG, 0);       // 受け取りとトランスミットの割り込み無効化
  put32(AUX_MU_LCR_REG, 3);       // 8bitモードの有効化
  put32(AUX_MU_MCR_REG, 0);       // RTS lineを常にhighに設定
  put32(AUX_MU_BAUD_REG, 270);    // baudレートを115200に設定

  put32(AUX_MU_CNTL_REG, 3);      // transmitter, receiverの有効化
}