#include <inc/lib.h>

int n_read = 20;

char buf[8192];

// не останавлевается, пока не считает по крайней мере n_read символов
void
read_n_read(int f) {
  int n;
  int already_read = 0;

  while ((n = read(f, buf, (long)sizeof(buf))) >= 0){
    if (n>0) {
      cprintf("%.*s\n", n, buf);
      already_read += n;
    }
    if (already_read >= n_read)
      break;
    sys_yield();
  };
  if (n < 0)
    panic("i_love_yadro error reading %i", (int)n);
  else
    cprintf("i_love_yadro read %d byts and exits\n", already_read);
}

void
umain(int argc, char **argv) {
  int f, i;

  binaryname = "i_love_yadro";
  for (i = 1; i < argc; i++) {
      f = open(argv[i], O_RDONLY);
      if (f < 0)
        printf("can't open %s: %i\n", argv[i], f);
      else {
        read_n_read(f);
        close(f);
      }
    }
}
