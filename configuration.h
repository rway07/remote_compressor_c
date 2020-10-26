/* 
 * File:   configuration.h
 * Author: rway
 *
 */

#ifndef CONFIGURATION_H
#define	CONFIGURATION_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "common.h"

struct configuration {
    char* archive_name;
    int compressor_index;
    int sent_files;
    int sent_files_at_compress;
    int client_ID;
    int dirty;
    char* compressor_name[SUPPORTED_COMPRESSORS];
    char* compressor_ext[SUPPORTED_COMPRESSORS];
    int compressor_enabled[SUPPORTED_COMPRESSORS];
};

struct configuration *cfg;

void init_configuration();
void destroy_configuration();
char* get_archive_name();
void set_archive_name(const char*);
char* get_complete_archive_path(const char*);
int check_compressor_arg(const char*);
char* get_recv_folder(const char*);
char* get_compressor_name(int);
void set_compressor_index(int);

#ifdef	__cplusplus
}
#endif

#endif	/* CONFIGURATION_H */

