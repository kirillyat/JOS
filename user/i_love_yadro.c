#include <inc/lib.h>

int n_read = 100;

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
  int f;

  binaryname = "i_love_yadro";

  if (argc < 2) {
	printf("No arguments\n");
    return;
  }

  f = open(argv[1], O_RDONLY);
  if (f < 0)
    printf("can't open %s: %i\n", argv[1], f);
  else {
    read_n_read(f);
    close(f);
  };
}
