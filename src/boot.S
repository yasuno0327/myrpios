#include "mm.h"

; kernel imageの始まりに書かれているのでカーネルが起動すると
; この.text.bootセクションが呼び出されて_start関数が実行される。
.section ".text.boot"

.globl _start
_start:
    mrs    x0, mpidr_el1 ; processor id をmpidr_el1 system registerから取得
    and    x0, x0,#0xFF  ; Check processor id: 0ならmaster関数へ移動する。
                         ; 処理としてはmpidr_el1から取得した数値の最後のバイトを取っている。
                         ; 0xFFで論理積を取るので最下位バイトだけを取得できる。 => FFは(11111111)2だから
    ;  Raspberry Pi3は4コアだが、今回は他のコアはエンドレスループにして使わない。
    cbz    x0, master    ; 0(processor id 0が割り当てられたプロセッサーだった場合)だったらmaster関数へ分岐
    b    proc_hang       ; proc_hangへ無条件に分岐する。(今回は0以外のプロセッサー)

proc_hang:
    b proc_hang ; 無限ループ

; memzeroを呼び出して.bssセクションを削除する。
master:
    adr    x0, bss_begin ; label(ここではbss_begin)に関係するアドレスをtarget registerへロードする
    adr    x1, bss_end
    sub    x1, x1, x0    ; bss_end - bss_begin
    bl     memzero       ; memzeroを呼び出してbssをクリーンする。

    mov    sp, #LOW_MEMORY
    bl    kernel_main ; kernel_mainラベルに分岐, kernel_main funcへ移動