PREFIX  = /usr/local/h8/h8-elf
ARCH    = h8300-elf
BINDIR  = $(PREFIX)/bin
ADDNAME = $(ARCH)-

AR      = $(BINDIR)/$(ADDNAME)ar
AS      = $(BINDIR)/$(ADDNAME)as
CC      = $(BINDIR)/$(ADDNAME)gcc
LD      = $(BINDIR)/$(ADDNAME)ld
NM      = $(BINDIR)/$(ADDNAME)nm
OBJCOPY = $(BINDIR)/$(ADDNAME)objcopy
OBJDUMP = $(BINDIR)/$(ADDNAME)objdump
RANLIB  = $(BINDIR)/$(ADDNAME)ranlib
STRIP   = $(BINDIR)/$(ADDNAME)strip
GCC_EXEC_PREFIX = $(PREFIX)/$(ARCH)/bin

H8WRITE = ../../../tools/h8write/h8write

H8WRITE_SERDEV = /dev/cu.usbserial

OBJS = vector.o startup.o intr.o main.o interrupt.o
OBJS += lib.o serial.o uudecode.o xmodem.o elf.o srec.o

TARGET = kzload

CFLAGS = -Wall -mh -nostdinc -nostdlib -fno-builtin
#CFLAGS += -mint32 # intを32ビットにすると掛算／割算ができなくなる
CFLAGS += -I.
CFLAGS += -g
CFLAGS += -Wno-pointer-sign
#CFLAGS += -O

LFLAGS = -static -T ld.scr -L.

.SUFFIXES: .c .o
.SUFFIXES: .s .o
.SUFFIXES: .S .o

all :		$(TARGET)

$(TARGET) :	$(OBJS)
		$(CC) $(OBJS) -o $(TARGET) $(CFLAGS) $(LFLAGS)
		cp $(TARGET) $(TARGET).elf
		$(STRIP) $(TARGET)

.c.o :		$<
		$(CC) -c $< $(CFLAGS)

.s.o :		$<
		$(CC) -c $< $(CFLAGS)

.S.o :		$<
		$(CC) -c $< $(CFLAGS)

$(TARGET).mot :	$(TARGET)
		$(OBJCOPY) -O srec $(TARGET) $(TARGET).mot

image :		$(TARGET).mot

write :		$(TARGET).mot
		$(H8WRITE) -3069 -f20 $(TARGET).mot $(H8WRITE_SERDEV)

clean :
		rm -f $(OBJS) $(TARGET) $(TARGET).elf $(TARGET).mot
