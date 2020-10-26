/* 
 * File:   fileHandler.h
 * Author: rway
 *
 */

#ifndef FILEHANDLER_H
#define	FILEHANDLER_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "common.h"
#include "network.h"

    unsigned char* calculate_MD5(const char*);
    void print_MD5(unsigned char*);
    int compare_MD5(char*, unsigned char*, unsigned char*);
    void show_error(int err);
    int file_exist(const char*);
    int is_directory(const char*);
    int create_directory(const char*);
    int delete_file(const char*);
    int delete_directory(const char*);
    int get_file_len(const char*);
    char* get_working_dir();
    int got_permissions(const char*);
    int read_and_send_file(int, const char*, unsigned int);
    int receive_and_write_file(int, const char*, unsigned int);

    pthread_mutex_t io_mutex;

#ifdef	__cplusplus
}
#endif

#endif	/* FILEHANDLER_H */

