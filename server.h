/* 
 * File:   server.h
 * Author: rway
 *
 */

#ifndef SERVER_H
#define	SERVER_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "common.h"
#include "paramParser.h"
#include "fileHandler.h"
#include "serverSideConfiguration.h"
#include "network.h"
#include "commandHandler.h"

    extern struct globalConfiguration *globalCfg;
    extern struct thread_info **threads;
    extern int client_ids[MAX_CLIENTS];
    extern int running_threads;

    //int server_listen();
    void server_exit(int);
    //int serverEnable_compressors(int);
    int client_disconnect(int);
    //int generate_client_ID();
    int free_client_ID(int);
    int create_working_directory(int);
    void destroy_working_directory(int);
    //void global_signal_handler(int);

#ifdef	__cplusplus
}
#endif

#endif	/* SERVER_H */

