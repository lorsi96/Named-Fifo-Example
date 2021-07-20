#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>

#define FIFO_NAME "myfifo"
#define BUFFER_SIZE 300

#define TEXTLOG "/home/lorsi/Documents/SOpG/TP1/logs/Log.txt"
#define SIGNLOG "/home/lorsi/Documents/SOpG/TP1/logs/Sign.txt"

FILE *txtLog;
FILE *signLog;

void closeFiles(int _) {
    fclose(txtLog);
    fclose(signLog);
}

int main(void) {
	uint8_t inputBuffer[BUFFER_SIZE];
	int32_t bytesRead, returnCode, fd;
    txtLog = fopen(TEXTLOG,"wr+");
    signLog = fopen(SIGNLOG,"wr+");

    struct sigaction sa = {
        .sa_handler = closeFiles,
        .sa_flags = 0
    };

    // Signals.
	sigemptyset(&sa.sa_mask);
	sigaction(SIGINT, &sa ,NULL);
    
    /* Create named fifo. -1 means already exists so no action if already exists */
    if ( (returnCode = mknod(FIFO_NAME, S_IFIFO | 0666, 0) ) < -1  )
    {
        printf("Error creating named fifo: %d\n", returnCode);
        exit(1);
    }
    
    /* Open named fifo. Blocks until other process opens it */
	printf("waiting for writers...\n");
	if ( (fd = open(FIFO_NAME, O_RDONLY) ) < 0 )
    {
        printf("Error opening named fifo file: %d\n", fd);
        exit(1);
    }
    
    /* open syscalls returned without error -> other process attached to named fifo */
	printf("got a writer\n");

    /* Loop until read syscall returns a value <= 0 */
	do
	{
        /* read data into local buffer */
		if ((bytesRead = read(fd, inputBuffer, BUFFER_SIZE)) == -1)
        {
			perror("read");
        }
        else
		{
			inputBuffer[bytesRead] = '\0';
            if(inputBuffer[0] == 'D') {
                fprintf(txtLog, "%s\n" ,inputBuffer);
            } else {
                fprintf(signLog, "%s\n" ,inputBuffer);
            }
			printf("reader: read %d bytes: \"%s\"\n", bytesRead, inputBuffer);
		}
	}
	while (bytesRead > 0);

	return 0;
}
