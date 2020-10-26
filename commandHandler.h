/* 
 * File:   commandHandler.h
 * Author: rway
 *
 */

#ifndef COMMANDHANDLER_H
#define	COMMANDHANDLER_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "common.h"
#include "network.h"
#include "server.h" 

    extern int running_threads;

    void enable_compressors();
    void *handle_command(void*);
    int command_parser(int, const char*);
    int handle_configuration_check(int);
    int handle_show_configuration(int);
    int handle_configure_name(int);
    int handle_configure_compressor(int);
    int handle_send(int);
    int handle_compress(int);

#ifdef	__cplusplus
}
#endif

#endif	/* COMMANDHANDLER_H */

