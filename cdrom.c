#include <fcntl.h>
#include <linux/cdrom.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main (int argc, char* argv[])
{
	/* Open a file descriptor to the device specified on the command line. */
	int fd = open (argv[1], O_RDONLY);
	if (fd < 0)
		printf ("cannot open '%s'\n", argv[1]);
	/* Eject the CD-ROM. */
	if (ioctl (fd, CDROMEJECT) < 0)
		printf ("cannot invoke CDROMEJECT\n");
	/* Close the file descriptor. */
	close (fd);
	return 0;
}
