#include <inc/lib.h>

void
umain(int argc, char **argv)
{
	char buf[128];
	struct Stat stat;
	int fd1, fd2;
	int r;
	char path_fifo[] = "my_fifo";
	char test_string[] = "This is a message for testing FIFO file";
    int test_len = strlen(test_string);

	cprintf("testing fifo...\n");

	cprintf("fifo creation ");
	r = mkfifo(path_fifo);
	if ((r == 0) || (r == -E_FILE_EXISTS))
		// после перезагрузки qemu фаил сохраняется, 
		// поэтому он может уже существовать
		cprintf("is successful\n");
    else cprintf("FAILURE\n");

	cprintf("fifo repeated creation ");
	if (mkfifo(path_fifo) == -E_FILE_EXISTS)
		cprintf("is successful\n");
    else cprintf("FAILURE\n");

	cprintf("1th fifo opening ");
	if ((fd1 = open(path_fifo, O_RDONLY)) >= 0)
		printf("is successful\n");
    else cprintf("FAILURE\n");
    cprintf("2nd fifo opening ");
	if ((fd2 = open(path_fifo, O_WRONLY)) >= 0)
		printf("is successful\n");
    else cprintf("FAILURE\n");

	cprintf("getting fifo stat ");
	if (!(fstat(fd1, &stat)))
		printf("is successful with stat fd1: %s %d\n", stat.st_name, stat.st_isfifo);
    else cprintf("FAILURE\n");
    cprintf("\n");

	cprintf("writing into fifo ");
	if (write(fd2, test_string, test_len) == test_len)
		cprintf("is successful\n");
    else cprintf("FAILURE\n");

	cprintf("reading from fifo ");
	if (read(fd1, buf, test_len) == test_len)
		cprintf("is successful\n");
    else cprintf("FAILURE\n");

    cprintf("1th fifo closing ");
	if (!close(fd1))
		cprintf("is successful\n");
    else cprintf("FAILURE\n");

    cprintf("2nd fifo closing ");
	if (!close(fd2))
		cprintf("is successful\n");
    else cprintf("FAILURE\n");


	// Test fifo with 2 envs
	int fifo_read_fd, fifo_write_fd;
	envid_t child;

    if ((child = fork()) == 0) {
  		// Child
        fifo_read_fd = open(path_fifo, O_RDONLY);
        while ((r = read(fifo_read_fd, buf, 8)) > 0)
			cprintf("Read from fifo %d bytes: %.*s\n", r, r, buf);
		cprintf("End of reading with code %d\n", r);
		close(fifo_read_fd);
        return;

	} else {
		// Parent
		fifo_write_fd = open(path_fifo, O_WRONLY);
	   	r = write(fifo_write_fd, test_string, test_len);
	    cprintf("End of writing with code %d\n", r);
	    close(fifo_write_fd);
	    wait(child);
	}
} 
