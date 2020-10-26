/*
 * File:   paramParser.h
 * Author: rway
 *
 */

#ifndef PARAMPARSER_H
#define	PARAMPARSER_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "common.h"

struct params{
    int port;
    char address[ADDRESS_LEN+1];
};

int check_ip(const char* address);
int check_port(const char* port);
int c_check_params_num(int argc);
int s_check_params_num(int argc);
int c_parse_params(int argc, char** argv, struct params* p);
int s_parse_params(int argc, char** argv);

#ifdef	__cplusplus
}
#endif

#endif	/* PARAMPARSER_H */
