#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/* ****************************************************************************************************************** */
/*                                                Constants Definitions                                               */
/* ****************************************************************************************************************** */
#define FIFO_NAME         "myfifo"
#define MAX_TXT_LEN       300
#define START_OF_USR_TXT  5
#define SIG_IND           5

/* ****************************************************************************************************************** */
/*                                                  Types Definitions                                                 */
/* ****************************************************************************************************************** */
typedef void SignalHandler_t(int);

/* ****************************************************************************************************************** */
/*                                                  Private Functions                                                 */
/* ****************************************************************************************************************** */

/**
 * @brief Waits until a reader is ready and assings the FIFO's file descriptor.
 * 
 * @param[out] file_desc file descriptor corresponding to the named FIFO.
 * @param[in] fifoname name of the FIFO used to communicate with another process.
 * @note exits the program on failure.
 */
static void _wait_for_readers(const char* fifoname, int32_t* file_desc) {
  printf("Waiting for readers...\n");
  if ((*file_desc = open(fifoname, O_WRONLY)) < 0) {
    printf("Error opening named fifo file: %d\n", *file_desc);
    exit(1);
  }
}

/**
 * @brief Creates a named FIFO.
 * 
 * @param fifoname[in] name of the naned FIFO to be created.
 * @note exists the program on failure.
 */
static void _create_fifo(const char* fifoname) { 
  int32_t returnCode;
  if ((returnCode = mknod(FIFO_NAME, S_IFIFO | 0666, 0)) < -1) {
    printf("Error creating named fifo: %d\n", returnCode);
    exit(1);
  }
}

/**
 * @brief Registers a handler to be called whenver SIGUSR1 or SIGUSR2 is triggered.
 * 
 * @param handler to be called when the aforementioned event occurs.
 */
static void _register_usr_signal_handler(SignalHandler_t handler) {
  struct sigaction sa;
  sa.sa_handler = handler;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGUSR1, &sa, NULL);
  sigaction(SIGUSR2, &sa, NULL);
}

/* ****************************************************************************************************************** */
/*                                                  Private Variables                                                 */
/* ****************************************************************************************************************** */
/* Buffers.*/
static char textBuffer[MAX_TXT_LEN] = "DATA:";
static char signalMsg[] = "SIGN:1";

/* File Descriptor. */
static int32_t fd;

/* ****************************************************************************************************************** */
/*                                                   Signal Handler                                                   */
/* ****************************************************************************************************************** */
void on_sigusr(int sig) {
  uint32_t bytesWrote;
 
  switch (sig) {
    case SIGUSR1: signalMsg[SIG_IND] = '1'; break;
    case SIGUSR2: signalMsg[SIG_IND] = '2'; break;
    default: return;
  }

  if ((bytesWrote = write(fd, signalMsg, strlen(signalMsg))) == -1) {
    perror("write");
  }
}

/* ****************************************************************************************************************** */
/*                                                    Main Program                                                    */
/* ****************************************************************************************************************** */
int main(void) {

  _register_usr_signal_handler(on_sigusr);
  _create_fifo(FIFO_NAME);
  _wait_for_readers(FIFO_NAME, &fd);
  printf("Got a reader, type something now!\n");

  uint32_t bytesWrote;
  
  for (;;) {
    char* outputBuffer = textBuffer + START_OF_USR_TXT;
    fgets(outputBuffer, MAX_TXT_LEN - START_OF_USR_TXT, stdin);
    if (strlen(outputBuffer) >= 1) { // Ignore if msg empty. Avoids capturing input if interrupted by signal.
      if ((bytesWrote = write(fd, textBuffer, strlen(textBuffer) - 1)) == -1) { // Remove the '\0' char.
        perror("write");
      } else if (bytesWrote) {
        printf("writer: wrote %d bytes\n", bytesWrote);
      }
      outputBuffer[0] = '\0'; // Clear buffer -- required by strlen above.
    }
  }

  return 0; // Should never reach here.
}
