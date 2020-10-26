/* 
 * File:   network.h
 * Author: rway
 *
 */

#ifndef NETWORK_H
#define	NETWORK_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "common.h"

    enum networkResponse {
        ERROR = -1,
        OK,
        BYE,
        CONTINUE,
    };

    typedef int netResponse;

    int sender_negotiate_trunk_size(int);
    int receiver_negotiate_trunk_size(int);
    int get_snd_buffer_size(int);
    int get_rcv_buffer_size(int);
    int send_int(int, int);
    int receive_int(int, int*);
    int send_command(int, const char*);
    int receive_command(int, char*);
    int send_response(int, int);
    int receive_response(int);
    int send_data(int, const char*, int);
    int receive_data(int, char*, int);
    int receive_file(int, char*, int);
    int handle_rcv_error(int, int);

#ifdef	__cplusplus
}
#endif

#endif	/* NETWORK_H */
