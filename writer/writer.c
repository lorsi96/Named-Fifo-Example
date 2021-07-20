#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <signal.h>

#define MAX_TXT_LEN 300
#define FIFO_NAME "myfifo"
#define SIG_IND 6

// Local buffers.
static char textBuffer[MAX_TXT_LEN] = "DATA:";
static uint32_t textBufferInd = 5;
static char signalMsg[] = "SIGNAL:1";

// File descriptor.
int32_t fd;


void onSigUsr(int sig) {
    uint32_t bytesWrote;
    switch(sig) {
    case SIGUSR1:
        signalMsg[SIG_IND] = '1';
        if ((bytesWrote = write(fd, signalMsg, strlen(signalMsg)-1)) == -1) {
			perror("write");
        }
        break;
    case SIGUSR2:
        signalMsg[SIG_IND] = '2';
        if ((bytesWrote = write(fd, signalMsg, strlen(signalMsg)-1)) == -1) {
			perror("write");
        }
        break;
    }
}


int main(void)
{

    struct sigaction sa = {
        .sa_handler = onSigUsr,
        .sa_flags = 0
    };

    // Signals.
	sigemptyset(&sa.sa_mask);
	sigaction(SIGUSR1, &sa ,NULL);
    sigaction(SIGUSR2, &sa ,NULL);

	

    uint32_t bytesWrote;
	int32_t returnCode;

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
        if(strlen(outputBuffer) > 1) {
            if ((bytesWrote = write(fd, textBuffer, strlen(textBuffer)-1)) == -1) {
                perror("write");
            } else if(bytesWrote) {
                printf("writer: wrote %d bytes\n", bytesWrote);
            }
            outputBuffer[0]='\0';
        }
	}
	return 0;
}
