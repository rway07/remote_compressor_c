/* 
 * File:   serverSideConfiguration.h
 * Author: rway
 *
 */

#ifndef SERVERSIDECONFIGURATION_H
#define	SERVERSIDECONFIGURATION_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "common.h"
#include "server.h"

    struct globalConfiguration {
        int server_socket;
        int port;
        struct sockaddr_in s_address;
        struct sockaddr_in c_address;
        char* compressor_name[SUPPORTED_COMPRESSORS];
        char* compressor_ext[SUPPORTED_COMPRESSORS];
        int compressor_enabled[SUPPORTED_COMPRESSORS];
        char* base_directory;
    };

    struct serverSideConfiguration {
        int client_socket;
        int client_ID;
        char* client_address;
        char* files_directory;
        char* archive_directory;
        char* archive_name;
        int compressor_index;
        int sent_files;
        int sent_files_at_compress;
        int dirty;
    };

    struct thread_info {
        pthread_t thread_id;
        struct serverSideConfiguration *clientCfg;
    };

    struct globalConfiguration *globalCfg;
    struct thread_info **threads;
    int client_ids[MAX_CLIENTS];
    int running_threads;

    pthread_mutex_t global_mutex;
    pthread_mutex_t conf_mutex;
    extern pthread_mutex_t io_mutex;

    int init_global_configuration();
    int destroy_global_configuration();
    int init_threads_conf();
    int destroy_client(int);
    int init_mutex();
    int destroy_mutex();
    int destroy_single_thread_conf(int);
    int destroy_all_thread_conf();
    int destroy_ssc(struct serverSideConfiguration*);
    void* init_client_conf(int, int, char*, char*);
    int check_compressors();
    int get_socket(int);
    int set_compressor_index(int, int);
    int get_compressor_index(int);
    int set_archive_name(int, char*);
    char* get_archive_name(int);
    char* get_client_address(int);
    char* get_archive_dir(int);
    char* get_files_dir(int);
    char* get_complete_file_path(int, char*);
    int destroy_client(int);


#ifdef	__cplusplus
}
#endif

#endif	/* SERVERSIDECONFIGURATION_H */
