#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>

#define MAX_TXT_LEN 300
#define FIFO_NAME "myfifo"


int main(void)
{
    static char textBuffer[MAX_TXT_LEN] = "DATA:";
    static uint32_t textBufferInd = 5;
    static char signal[] = "SIGNAL:1";

    uint32_t bytesWrote;
	int32_t returnCode, fd;

    /* Create named fifo. -1 means already exists so no action if already exists */
    if ( (returnCode = mknod(FIFO_NAME, S_IFIFO | 0666, 0) ) < -1 )
    {
        printf("Error creating named fifo: %d\n", returnCode);
        exit(1);
    }

    /* Open named fifo. Blocks until other process opens it */
	printf("waiting for readers...\n");
	if ( (fd = open(FIFO_NAME, O_WRONLY) ) < 0 )
    {
        printf("Error opening named fifo file: %d\n", fd);
        exit(1);
    }
    
    /* open syscalls returned without error -> other process attached to named fifo */
	printf("got a reader--type some stuff\n");

    /* Loop forever */
	for(;;) {
        char* outputBuffer = textBuffer + textBufferInd;
        /* Get some text from console */
		fgets(outputBuffer, MAX_TXT_LEN - 5, stdin);
        
        /* Write buffer to named fifo. Strlen - 1 to avoid sending \n char */
		if ((bytesWrote = write(fd, textBuffer, strlen(textBuffer)-1)) == -1) {
			perror("write");
        }
        else {
			printf("writer: wrote %d bytes\n", bytesWrote);
        }
	}
	return 0;
}
