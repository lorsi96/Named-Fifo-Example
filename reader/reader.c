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
/*                                                Constants Definition                                                */
/* ****************************************************************************************************************** */
#define BUFFER_SIZE   300
#define FIFO_NAME     "myfifo"
#define BASE_PATH     "./logs/"
#define TEXTLOG       BASE_PATH "Log.txt"
#define SIGNLOG       BASE_PATH "Sign.txt"

/* ****************************************************************************************************************** */
/*                                                  Types Definitions                                                 */
/* ****************************************************************************************************************** */
typedef void SignalHandler_t(int);

/* ****************************************************************************************************************** */
/*                                                  Private Variables                                                 */
/* ****************************************************************************************************************** */
static FILE *txtLog;
static FILE *signLog;

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
static void _wait_for_writers(const char* fifoname, int32_t* file_desc) {
  printf("Waiting for writers...\n");
  if ((*file_desc = open(fifoname, O_RDONLY)) < 0) {
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
 * @brief Close opened files. 
 * 
 * Meant to be called when the program finishes.
 * 
 * @param _ required and unused parameter to fit as a signal handler.
 */
static void _closeFiles(int _) {
  if (txtLog != NULL) fclose(txtLog);
  if (signLog != NULL) fclose(signLog);
}

/**
 * @brief Registers a handler to be called whenver SIGUSR1 or SIGUSR2 is triggered.
 * 
 * @param handler to be called when the aforementioned event occurs.
 */
static void _register_sigint_handler(SignalHandler_t handler) {
  struct sigaction sa;
  sa.sa_handler = handler;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGINT, &sa, NULL);
}

/* ****************************************************************************************************************** */
/*                                                    Main Program                                                    */
/* ****************************************************************************************************************** */
int main(void) {
  uint8_t inputBuffer[BUFFER_SIZE];
  int32_t bytesRead, fd;
  txtLog = fopen(TEXTLOG, "wr+");
  signLog = fopen(SIGNLOG, "wr+");

  _register_sigint_handler(_closeFiles);
  _create_fifo(FIFO_NAME);
  _wait_for_writers(FIFO_NAME, &fd);
  printf("Got a writer!\n");

  do {
    if ((bytesRead = read(fd, inputBuffer, BUFFER_SIZE)) == -1) {
      perror("read");
    } else {
      inputBuffer[bytesRead] = '\0';
      fprintf(inputBuffer[0] == 'S' ? signLog : txtLog , "%s\n", inputBuffer);
      printf("reader: read %d bytes: \"%s\"\n", bytesRead, inputBuffer);
    }
  } while (bytesRead > 0);

  return 0;
}
