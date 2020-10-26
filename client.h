/* 
 * File:   client.h
 * Author: rway
 *
 */

#ifndef CLIENT_H
#define	CLIENT_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "common.h"
#include "paramParser.h"
#include "configuration.h"
#include "shell.h"

struct client{
    int client_socket;
    char* server_address;
    int port;
    struct sockaddr_in s_address;
} *cln;

struct params;

void global_signal_handler();
int init_client(struct params);
void destroy_client();
void client_exit(int);
int client_connect();
void client_disconnect();
int prepare_client();

#ifdef	__cplusplus
}
#endif

#endif	/* CLIENT_H */

