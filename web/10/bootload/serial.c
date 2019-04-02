#include "types.h"
#include "serial.h"

#define H8_3069F_SCI0 ((volatile struct h8_3069f_sci *)0xffffb0)
#define H8_3069F_SCI1 ((volatile struct h8_3069f_sci *)0xffffb8)
#define H8_3069F_SCI2 ((volatile struct h8_3069f_sci *)0xffffc0)

struct h8_3069f_sci {
  volatile uint8 smr;
  volatile uint8 brr;
  volatile uint8 scr;
  volatile uint8 tdr;
  volatile uint8 ssr;
  volatile uint8 rdr;
  volatile uint8 scmr;
};

#define H8_3069F_SCI_SMR_CKS0   (1<<0)
#define H8_3069F_SCI_SMR_CKS1   (1<<1)
#define H8_3069F_SCI_SMR_MP     (1<<2)
#define H8_3069F_SCI_SMR_STOP   (1<<3)
#define H8_3069F_SCI_SMR_OE     (1<<4)
#define H8_3069F_SCI_SMR_PE     (1<<5)
#define H8_3069F_SCI_SMR_CHR    (1<<6)
#define H8_3069F_SCI_SMR_CA     (1<<7)

#define H8_3069F_SCI_SCR_CKE0   (1<<0)
#define H8_3069F_SCI_SCR_CKE1   (1<<1)
#define H8_3069F_SCI_SCR_TEIE   (1<<2)
#define H8_3069F_SCI_SCR_MPIE   (1<<3)
#define H8_3069F_SCI_SCR_RE     (1<<4)
#define H8_3069F_SCI_SCR_TE     (1<<5)
#define H8_3069F_SCI_SCR_RIE    (1<<6)
#define H8_3069F_SCI_SCR_TIE    (1<<7)

#define H8_3069F_SCI_SSR_MPBT   (1<<0)
#define H8_3069F_SCI_SSR_MPB    (1<<1)
#define H8_3069F_SCI_SSR_TEND   (1<<2)
#define H8_3069F_SCI_SSR_PER    (1<<3)
#define H8_3069F_SCI_SSR_FERERS (1<<4)
#define H8_3069F_SCI_SSR_ORER   (1<<5)
#define H8_3069F_SCI_SSR_RDRF   (1<<6)
#define H8_3069F_SCI_SSR_TDRE   (1<<7)

int serial_init()
{
  volatile struct h8_3069f_sci *sci = H8_3069F_SCI1;

  sci->scr = 0;
  sci->smr = 0;
  sci->brr = 64; /* 20MHz 9600bps */
  sci->scr = H8_3069F_SCI_SCR_RE | H8_3069F_SCI_SCR_TE;
  sci->ssr = 0;

  return 0;
}

int serial_putb(int c)
{
  volatile struct h8_3069f_sci *sci = H8_3069F_SCI1;

  while (!(sci->ssr & H8_3069F_SCI_SSR_TDRE))
    ;
  sci->tdr = c;
  sci->ssr &= ~H8_3069F_SCI_SSR_TDRE;

  return 0;
}

int serial_putc(int c)
{
  if (c == '\n')
    serial_putb('\r');
  return serial_putb(c);
}

int serial_tstc()
{
  volatile struct h8_3069f_sci *sci = H8_3069F_SCI1;
  return (sci->ssr & H8_3069F_SCI_SSR_RDRF);
}

int serial_getb()
{
  volatile struct h8_3069f_sci *sci = H8_3069F_SCI1;
  int c;

  while (!serial_tstc())
    ;
  c = sci->rdr;
  sci->ssr &= ~H8_3069F_SCI_SSR_RDRF;

  return c;
}

int serial_getc()
{
  int c = serial_getb();
  c = (c == '\r') ? '\n' : c;
  return c;
}
