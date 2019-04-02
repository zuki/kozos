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

OBJS = startup.o main.o interrupt.o
OBJS += lib.o serial.o
#OBJS += timer.o

# sources of kozos
OBJS += thread.o syscall.o memory.o idle.o
OBJS += extintr.o
OBJS += kozos.o command.o

TARGET = kozos

CFLAGS = -Wall -mh -nostdinc -nostdlib -fno-builtin
#CFLAGS += -mint32 # intを32ビットにすると掛算／割算ができなくなる
CFLAGS += -I.
#CFLAGS += -g
#CFLAGS += -O
CFLAGS += -Os -Wno-pointer-sign
CFLAGS += -DKOZOS

LFLAGS += -static -T ld.scr -L. -Wl,--unresolved-symbols=ignore-in-object-files

.SUFFIXES: .c .o
.SUFFIXES: .s .o

all :		$(TARGET)

$(TARGET) :	$(OBJS)
		$(CC) $(OBJS) -o $(TARGET) $(CFLAGS) $(LFLAGS)
		cp $(TARGET) $(TARGET).elf
		$(STRIP) $(TARGET)

.c.o :		$<
		$(CC) -c $< $(CFLAGS)

.s.o :		$<
		$(CC) -c $< $(CFLAGS)

$(TARGET).mot :	$(TARGET)
		$(OBJCOPY) -O srec $(TARGET) $(TARGET).mot

$(TARGET).uu :	$(TARGET)
		uuencode -o $(TARGET).uu $(TARGET) $(TARGET)

image :		$(TARGET).mot $(TARGET).uu

clean :
		rm -f $(OBJS) $(TARGET) $(TARGET).elf $(TARGET).mot $(TARGET).uu
