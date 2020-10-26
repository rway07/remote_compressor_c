/* 
 * File:   common.h
 * Author: rway
 *
 */

#ifndef COMMON_H
#define	COMMON_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>
#include <pthread.h>
#include <sched.h>
    
#ifdef __linux__
#include <linux/limits.h>
#include <arpa/inet.h>
#elif __FreeBSD__
#include <limits.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#elif __APPLE__
#include <netinet/in.h>
    #ifndef MSG_NOSIGNAL
        #define MSG_NOSIGNAL SO_NOSIGPIPE
    #endif
#endif

#define SA struct sockaddr

#define VERSION "1.1"
#define CLIENT_PARAM_HOST_POS 1
#define CLIENT_PARAM_PORT_POS 2
#define SERVER_PARAM_PORT_POS 1
#define MIN_PORT 1024
#define MAX_PORT 65535
#define CMD_POS 0
#define PARAM_POS 1
#define ADDRESS_LEN 15
#define MAX_FILENAME_LEN 32
#define MAX_CMD_LEN 128
#define MIN_CMD_LEN 4
#define MAX_CMD_TOKENS 2
    // Comandi
#define NET_CMD_LEN 4
#define NET_EXIT_CMD "cmd0"
#define NET_HELP_CMD "cmd1"
#define NET_SEND_CMD "cmd4"
#define NET_COMPRESS_CMD "cmd3"
#define NET_SHOW_CFG_CMD "cmd2"
#define NET_CFG_NAME_CMD "cmd5"
#define NET_CFG_COMP_CMD "cmd6"
#define NET_CFG_CHECK_CMD "cmd9"
#define NET_EXIT_INDEX 0
#define NET_HELP_INDEX 1
#define NET_SHOW_CFG_INDEX 2
#define NET_COMPRESS_INDEX 3
#define NET_SEND_INDEX 4
#define NET_CFG_NAME_INDEX 5
#define NET_CFG_COMP_INDEX 6
#define NET_CFG_CHECK_INDEX 9
#define EXIT_CMD "quit"
#define HELP_CMD "help"
#define SEND_CMD "send"
#define COMPRESS_CMD "compress"
#define SHOW_CFG_CMD "show-configuration"
#define CFG_NAME_CMD "configure-name"
#define CFG_COMP_CMD "configure-compressor"
    // Risposte
#define NET_RESPONSE_LEN 3
#define NET_RESPONSE_OK "ack"
#define NET_RESPONSE_BYE "bye"
#define NET_RESPONSE_ERROR "err"
#define NET_RESPONSE_CONTINUE "cnt"
#define CLIENT_RECV_FOLDER "/received_"
#define SERVER_FILES_FOLDER "/files_"
#define SERVER_ARCHIVES_FOLDER "/archives_"
#define DEFAULT_ARCHIVE_NAME "archivio"
#define SUPPORTED_COMPRESSORS 4
#define COMPRESSOR_GNUZIP "gnuzip"
#define COMPRESSOR_BZIP2 "bzip2"
#define COMPRESSOR_ZIP "zip"
#define COMPRESSOR_XZ "xz"
#define BZIP2_EXT ".tar.bz2"
#define GNUZIP_EXT ".tar.gz"
#define XZ_EXT ".tar.xz"
#define ZIP_EXT ".zip"
#define ZIP_COMMAND "zip -q -r -j "
#define XZ_COMMAND "tar cJf "
#define BZIP2_COMMAND "tar cjf "
#define GNUZIP_COMMAND "tar czf "
#define XZ_CHECK_COMMAND "tar --help | grep xz -q"
#define TAR_PARAM_1 " -C "
#define TAR_PARAM_2 " ."
#define TAR_PATH "/usr/bin/tar"
#define ZIP_PATH "/usr/bin/zip"
#define DELETE_COMMAND "rm -rf "
#define FATAL_ERROR -2
#define EXIT_COMMAND -3
#define COMMAND_ERROR -4
#define COMMAND_CONTINUE -5
#define NET_ERROR -6
#define MAX_CLIENTS_NUMBER -7
#define NOT_CONNECTED -8
#define MAX_CLIENTS 8
#define INT_DIGITS_NUM 16
#define MD5_DATA 4096
#define MD5_DIGEST_LENGTH 16

    enum COMPRESSOR {
        GNUZIP,
        BZIP2,
        ZIP,
        XZ
    };

#ifdef	__cplusplus
}
#endif

#endif	/* COMMON_H */

