# Mailbox offset.
movz w1, #0x3f00, lsl #16
movk w1, #0xb880

# E flag.
movz w2, #0x4000, lsl #16

# F flag.
movz w3, #0x8000, lsl #16 

# Local buffer address
# with register offset.
add x4, x4, #0xf00 

# w5 builds the buffer

# Local buffer address
# with start offset
# and channel.
movz w6, #0x8, lsl #16
movk w6, #0x0f08

# w7 is a counter register

# Wait time
movz w8, #0xf, lsl #16 

# w9 saves the response

# Sets on/off
movz w10, #1
movz w11, #1

# Number of repetitions
movz w12, #10
movz w13, #0

Loop:
# Uncomment the next line
# to stop the program
# from going forever.
# add w13, w13, #1

# Building the buffer
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

# Check write flag
WaitF:
movz w7, #0
ldr w7, [w1, #0x18]
tst w7, w3
b.ne WaitF

# Send request
str w6, [w1, #0x20]

# Check read flag
WaitE:
movz w7, #0
ldr w7, [w1, #0x18]
tst w7, w2
b.ne WaitE

# Read response
movz w9, #0
ldr w9, [w1]

# Switch on/off
sub w11, w10, w11

# Wait
movz w7, #0x0
Wait:
add w7, w7, #1
cmp w8, w7
b.ne Wait

cmp w12, w13
b.ne Loop

and x0, x0, x0 

