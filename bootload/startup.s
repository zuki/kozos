    .h8300h
    .section    .text
    .global     _start
    .type       _start, @function

_start:
    mov.l   #_bootstack, sp   # スタックポインタの設定
    jsr     @_main          # main()の呼び差し

1:
    bra     1b              # 無限ループ
