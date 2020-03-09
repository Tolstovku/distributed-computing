#ifndef __IFMO_DISTRIBUTED_CLASS_PA1__LOG__H
#define __IFMO_DISTRIBUTED_CLASS_PA1__LOG__H

#include "stdio.h"
#include "io.h"
#include "stdarg.h"
#include "common.h"
#include "ipc.h"
#include "pa1.h"

FILE *events_log_file;
FILE *pipe_log_file;

static const char * const log_pipe_opened_fmt = "Pipe from process %1d to %1d OPENED\n";

void log_init();

void logprintf(const char *format, ...);

void log_pipe_opened(int from, int to);

void log_started();

void log_received_all_started();

void log_done();

void log_received_all_done();

void log_close();

#endif  // __IFMO_DISTRIBUTED_CLASS_PA1__LOG__H
