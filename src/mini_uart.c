#include "utils.h"
#include "peripherals/mini_uart.h"
#include "peripherals/gpio.h"


/*
  put32, get32 function
  32ビットレジスタへの書き込み、読み込みを行う。
*/

// AUX_MU_LSR_REG registerを使ってデータの書き込みが可能かを検証する
// AUX_MU_IOREG registerを使ってデータ(character)の保存、読み込みを行う
// 5bit目に1がセット(100000)されている場合は書き込み可能
void uart_send(char c) {
  while(1) {
    if(get32(AUX_MU_LSR_REG & 0x20)) break;
  }
  put32(AUX_MU_IO_REG, c);
}

// 文字列を書き込む
void uart_send_string(char *str) {
  for (int i = 0; str[i] != '\0'; i++) {
    uart_send((char)str[i])
  }
}

// 0bit目に1がセットされている(01)場合はデータの読み込み可能
char uart_recv(void) {
  while(1) {
    if(get32(AUX_MU_LSR_REG & 0x01)) break;
  }
  return(get32(AUX_MU_IO_REG) & 0xFF);
}

void uart_init(void) {
  unsigned int selector;

  // 各GPIO pinに対応するビットはhttps://raw.githubusercontent.com/s-matyukevich/raspberry-pi-os/master/images/gpfsel1.png で参照可能
  selector = get32(GPFSEL1); // The GPFSEL1 register is used to control alternative functions for pins 10-19.
  selector &= ~(7<<12); // 7*2の12乗(12左ビットシフト)を反転させ(NOT),and演算したものをselectorに代入 (clean gpio14)
                        // (000111111111111)で論理和を取るのでFSEL14の領域を全て0にできる。
  selector |= 2<<12; // 2*2の12乗(12左ビットシフト)とselectorでOR演算したものをselectorに代入
                     // (10000000000000)なのでFSEL14を有効化
  selector &= ~(7<<15); // 7*2の15乗を反転させたもの(NOT)をselectorとAND演算したものをselectorに代入
                        // FSEL15領域を初期化
  selector |= 2<<15;    // FSEL15を有効化
  put32(GPFSEL1, selector); // GPFSEL1レジスタへ書き込み

  // Switching pin states
  // The GPIO Pull-up/down Clock Registers control the actuation of internal pull-downs on
  // the respective GPIO pins. These registers must be used in conjunction with the GPPUD
  // register to effect GPIO Pull-up/down changes. The following sequence of events is
  // required:
  // 1. Write to GPPUD to set the required control signal (i.e. Pull-up or Pull-Down or neither
  // to remove the current Pull-up/down)
  // 2. Wait 150 cycles – this provides the required set-up time for the control signal
  // 3. Write to GPPUDCLK0/1 to clock the control signal into the GPIO pads you wish to
  // modify – NOTE only the pads which receive a clock will be modified, all others will
  // retain their previous state.
  // 4. Wait 150 cycles – this provides the required hold time for the control signal
  // 5. Write to GPPUD to remove the control signal
  // 6. Write to GPPUDCLK0/1 to remove the clock
  put32(GPPUD, 0); // Write gppud to neither
  delay(150);      // wait 150 cycles
  put32(GPPUDCLK0,(1<<14) | (1<<15)); // pin statesを指定するGPIO pinを指定 (今回は14と15)
  delay(150);      // wait 150 cycles
  put32(GPPUDCLK0,0); // remove clock

  // Initialize Mini UART
  put32(AUX_ENABLES, 1);          // mini_uartの有効化
  put32(AUX_MU_CNTL_REG, 0);      // auto flow control(追加のGPIO pinの使用を要求されるため、またTTL=to-serial cableはこれをサポートしていないため), receiver, transmitterの無効化
  put32(AUX_MU_IER_REG, 0);       // 受け取りとトランスミットの割り込み無効化 lesson3でやる
  put32(AUX_MU_LCR_REG, 3);       // 8bitモードの有効化(ASCII codeが7bitのため7,8ビットモード、今回は8)
  put32(AUX_MU_MCR_REG, 0);       // RTS lineを常にhighに設定、flow controlで使われるが今回はflow controlを使わないため
  put32(AUX_MU_BAUD_REG, 270);    // baudレートを最大115200bpsに設定, terminal emulatorと同じ速度にしないといけない
  // baud rate => connection channelでの転送速度
  // baudrate = system_clock_freq / (8 * ( baudrate_reg + 1 ))
  // system_clock_freqは250MHzなのでbaundrate_reqは270になる
  put32(AUX_MU_CNTL_REG, 3);      // transmitter, receiverの有効化
}