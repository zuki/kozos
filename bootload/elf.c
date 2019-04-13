#include "defines.h"
#include "xmodem.h"
#include "elf.h"
#include "lib.h"

struct elf_header {
  struct {
    unsigned char magic[4];
    unsigned char class;
    unsigned char format;
    unsigned char version;
    unsigned char abi;
    unsigned char abi_version;
    unsigned char reserve[7];
  } id;
  short type;
  short arch;
  long version;
  long entry_point;
  long program_header_offset;
  long section_header_offset;
  long flags;
  short header_size;
  short program_header_size;
  short program_header_num;
  short section_header_size;
  short section_header_num;
  short section_name_index;
};

struct elf_program_header {
  long type;
  long offset;
  long virtual_addr;
  long physical_addr;
  long file_size;
  long memory_size;
  long flags;
  long align;
};

#define MAX_PROGROM_HEADERS   6

/* ELFヘッダのチェック */
static int elf_check(struct elf_header *header)
{
  if (memcmp(header->id.magic, "\x7f" "ELF", 4))
    return -1;

  if (header->id.class   != 1) return -1; /* ELF32 */
  if (header->id.format  != 2) return -1; /* Big endian */
  if (header->id.version != 1) return -1; /* version 1 */
  if (header->type       != 2) return -1; /* Executable file */
  if (header->version    != 1) return -1; /* version 1 */

  /* Hitachi H8/300 or H8/300H */
  if ((header->arch != 46) && (header->arch != 47)) return -1;

  return 0;
}

/* セグメント単位でのロード */
static int elf_load_program(struct elf_header *header)
{
  int i;
  struct elf_program_header *phdr;

  for (i = 0; i < header->program_header_num; i++) {
    /* プログラムヘッダを取得 */
    phdr = (struct elf_program_header *)
      ((char *)header + header->program_header_offset +
       header->program_header_size * i);

    if (phdr->type != 1)  /* ロード可能なセグメントか？ */
      continue;

    memcpy((char *)phdr->physical_addr, (char *)header + phdr->offset,
          phdr->file_size);
    memset((char *)phdr->physical_addr + phdr->file_size, 0,
          phdr->memory_size - phdr->file_size);
  }

  return 0;
}

char *elf_load(char *buf)
{
  struct elf_header *header = (struct elf_header *)buf;

  if (elf_check(header) < 0)  /* ELFヘッダのチェック */
    return NULL;

  if (elf_load_program(header) < 0) /* セグメント単位でのロード */
    return NULL;

  return (char *)header->entry_point;
}

static char *startaddr = NULL;  /* ロードするプログラムの開始アドレス */

int load_program(unsigned char block_number, char *buf)
{
  struct elf_header         *ehdr;
  static struct elf_program_header phdrs[MAX_PROGROM_HEADERS];
  static short psize, pnum;  /* phdrのサイズと数 */
  static long poff;          /* phdrオフセット   */

  static int  buf_ph = 0; /* phdr操作時のbufのカレントポインタ */
  static int  buf_ld = 0; /* ロード時のbufのカレントポインタ */

  static int set_idx = 0; /* セット済みphdrsのインデックス */
  static int ph_rem  = 0; /* 読み残しphdrのバイト数 */

  static int  ld_idx  = 0; /* ロード対象のphdrのインデックス */
  static long loaded  = 0; /* ロード済みバイト数 */

  int i, idx, load, size = 0;
  long startp, endp, load_p;

  startp = (block_number - 1) * XMODEM_BLOCK_SIZE;
  endp   = startp + XMODEM_BLOCK_SIZE;

  /* エルフヘッダの解析 */
  if (block_number == 1) {
    ehdr = (struct elf_header *)buf;
    if (elf_check(ehdr) < 0)
      return -1;
    if (ehdr->program_header_num > MAX_PROGROM_HEADERS)
      return -1;
    startaddr = (char *)ehdr->entry_point;
    psize = ehdr->program_header_size;
    pnum  = ehdr->program_header_num;
    poff  = ehdr->program_header_offset;
    buf_ph = (int)poff;
  }

  /* phdrsの設定 */
  if (set_idx < pnum) {
    /* 読み残しのphdrがある場合 */
    if (ph_rem > 0) {
      memcpy((char *)(phdrs+set_idx)+(psize - ph_rem), buf, ph_rem);
      buf_ph = ph_rem;
      set_idx++;
    }
    if (set_idx < pnum) {
      /* XMODEMブロック内にあるphdrの数 */
      idx = (XMODEM_BLOCK_SIZE - buf_ph) / psize;
      if (set_idx + idx > pnum)
        idx = pnum;
      else
        idx = set_idx + idx;
      for (i = set_idx; i < idx; i++) {
        memcpy((char *)(phdrs+i), buf + buf_ph, psize);
        buf_ph += psize;
        ph_rem = 0;
        set_idx++;
      }
      /* このXMODEM blockではphdrの設定が終わらなかった場合 */
      if (set_idx < pnum) {
        memcpy((char *)(phdrs+set_idx), buf + buf_ph, (XMODEM_BLOCK_SIZE - buf_ph));
        ph_rem = psize - (XMODEM_BLOCK_SIZE - buf_ph);
        buf_ph = 0;
      }
    }
  }

  /* programのロード */
  while (1) {
    if (ld_idx >= pnum) break;
    if (phdrs[ld_idx].type != 1) { /* ロード可能なセグメントか？ */
      ld_idx++;
      continue;
    }
    load_p = phdrs[ld_idx].offset + loaded;
    if (startp <= load_p && load_p < endp) {
      load = XMODEM_BLOCK_SIZE - buf_ld;
      if (loaded + load > phdrs[ld_idx].file_size) {
        load = phdrs[ld_idx].file_size - loaded;
      }
      memcpy((char *)phdrs[ld_idx].physical_addr+loaded, buf+buf_ld, load);
      size += load;
      if (loaded + load >= phdrs[ld_idx].file_size) {
        memcpy((char *)phdrs[ld_idx].physical_addr + phdrs[ld_idx].file_size, 0,              phdrs[ld_idx].memory_size - phdrs[ld_idx].file_size);
        size += phdrs[ld_idx].memory_size - phdrs[ld_idx].file_size;
        ld_idx++;
        if (ld_idx == pnum) break; /* すべてのprogramをロード */
        loaded = 0;
      } else {
        loaded += load;
      }
      buf_ld += load;
      if (buf_ld >= XMODEM_BLOCK_SIZE) {
        buf_ld = 0;
        break;
      }
    } else {
      break;
    }
  }

  return size;
}

char *elf_startaddr(void)
{
  return startaddr;
}
