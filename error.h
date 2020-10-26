/* 
 * File:   error.h
 * Author: rway
 *
 */

#ifndef ERROR_H
#define	ERROR_H

#ifdef	__cplusplus
extern "C" {
#endif


enum transmissionExceptionSource {
    SEND_COMMAND,
    SEND_RESPONSE,
    SEND_DATA,
    RECEIVE_COMMAND,
    RECEIVE_RESPONSE,
    RECEIVE_DATA
};

enum serverErrorType {
    SOCKET = 1,
    BIND,
    LISTEN,
    ACCEPT,
    WORKINGDIR
};

enum commandExceptionType {
    UNKNOWN_COMMAND,
    BAD_COMMAND,
    INCOMPLETE_COMMAND,
    OUT_OF_LEN_COMMAND
};

enum commandExecutionExceptionType {
    COMMAND_ERROR,
    ALREADY_EXECUTED_COMMAND,
    ALREADY_COMPRESSED_ARCHIVE,
    NO_SENT_FILES,
};

void error_printf() {
    printf("ERRORE: ");
}

void debug_printf() {
    printf("DEBUG: ");
}

#ifdef	__cplusplus
}
#endif

#endif	/* ERROR_H */

