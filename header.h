
#ifndef _HEADER_H
#define _HEADER_H 

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <signal.h>
#include <syscall.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <sys/user.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <elf.h>
#include <fcntl.h>
#include <capstone/capstone.h>

void procmsg(const char* format, ...);
void run_code (char* prog_name);

void affichage(char* programme);
void run_debugger(pid_t child_pid, char* prog_name);
#endif        
