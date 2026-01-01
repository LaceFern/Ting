#include <cstdarg>
#include <unistd.h>
#include <fstream>
#include <sys/time.h>
#include <execinfo.h>
#include <syscall.h>
#include "log.h"
void print_stack_trace() {
    printf("\n***************Start Stack Trace******************\n");
    int size = 100;
    void *buffer[100];
    char **strings;
    int j, nptrs;
    nptrs = backtrace(buffer, size);
    printf("backtrace() returned %d addresses\n", nptrs);
    strings = backtrace_symbols(buffer, nptrs);
    if (strings == NULL) {
        perror("backtrace_symbols");
        exit(EXIT_FAILURE);
    }
    for (j = 0; j < nptrs; j++) {
        printf("%s\n", strings[j]);
    }
    free(strings);
    printf("\n***************End Stack Trace******************\n");
}
