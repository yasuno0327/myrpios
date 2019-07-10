/*
  linker script : objectファイルを実行ファイルにマップする。
*/

SECTIONS {
  .text.boot : { *(.text.boot) }
  .text : { *(.text) }
  .rodata : { *(.rodata) }
  .data : { *(.data) }
  bss_begin = .;
  .bss : { *(.bss) }
  bss_end = .;
}