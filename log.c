#include "stdio.h"
#include "io.h"
#include "stdarg.h"
#include "common.h"
#include "ipc.h"
#include "pa1.h"
#include "log.h"
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

void log_init() {
    events_log_file = fopen(events_log, "w");
    pipe_log_file = fopen(pipes_log, "w");
}

void logprintf(const char *format, ...) {
    va_list va;

    va_start(va, format);
    vprintf(format, va);
    va_end(va);

    va_start(va, format);
    vfprintf(events_log_file, format, va);
    va_end(va);
}

void log_pipe_opened(int from, int to)
{
    printf(log_pipe_opened_fmt, from, to);
    fprintf(pipe_log_file, log_pipe_opened_fmt, from, to);
}

void log_started() {
    pid_t pid = getpid();
    pid_t parent_pid = getppid();
    logprintf(log_started_fmt, local_pid, pid, parent_pid);
}

void log_received_all_started() {
    logprintf(log_received_all_started_fmt, local_pid);
}

void log_done() {
    logprintf(log_done_fmt, local_pid);
}

void log_received_all_done() {
    logprintf(log_received_all_done_fmt, local_pid);
}

void log_close() {
    fclose(events_log_file);
}
