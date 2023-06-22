movz w1, #0x3f00, lsl #16
movk w1, #0xb880

movz w2, #0x4000, lsl #16

movz w3, #0x8000, lsl #16 

add x4, x4, #0xf00 

movz w6, #0x8, lsl #16
movk w6, #0x0f08

movz w8, #0xf, lsl #16 

movz w10, #1
movz w11, #1

movz w12, #10
movz w13, #0

Loop:

movz w5, #32
str w5, [w4, #0x0]
movz w5, #0
str w5, [w4, #0x4]
movz w5, #0x0003, lsl #16
movk w5, #0x8041
str w5, [w4, #0x8]
eor w5, w5, w5
movz w5, #8
str w5, [w4, #0xc]
movz w5, #0
str w5, [w4, #0x10]
movz w5, #130
str w5, [w4, #0x14]
str w11, [w4, #0x18]
movz w5, #0
str w5, [w4, #0x1c]

WaitF:
movz w7, #0
ldr w7, [w1, #0x18]
tst w7, w3
b.ne WaitF

str w6, [w1, #0x20]

WaitE:
movz w7, #0
ldr w7, [w1, #0x18]
tst w7, w2
b.ne WaitE

movz w9, #0
ldr w9, [w1]

sub w11, w10, w11

movz w7, #0x0
Wait:
add w7, w7, #1
cmp w8, w7
b.ne Wait

cmp w12, w13
b.ne Loop

and x0, x0, x0 
