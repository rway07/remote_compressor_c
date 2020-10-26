/* 
 * File:   shell.h
 * Author: Luca
 *
 * Contiene le funzioni che implementano i comandi del client
 */

#ifndef SHELL_H
#define	SHELL_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "common.h"
#include "configuration.h"
#include "network.h"
#include "client.h"
#include "fileHandler.h"

void flush_buffer();
int get_command_number(const char*);
int params_num_check(int, const char*);
int params_check(int, const char*);
int enable_compressors();
int check_name(const char*);
int check_configuration();
int check_compressor(const char*);
int check_compress(const char*);
int check_send(const char*);
int do_help();
int do_quit();
int do_send(char*);
int do_compress(const char*);
int do_conf_comp(int);
int do_conf_name(const char*);
int do_show_conf();
int do_send_command(int);
int parse_command(char*);
int command_line();

#ifdef	__cplusplus
}
#endif

#endif	/* SHELL_H */

