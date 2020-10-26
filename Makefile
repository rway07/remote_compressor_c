CC=gcc
CFLAGS= -Wall -c -MMD -MP -MF $@.d
SERVER_FLAGS= -pthread -lm
CLIENT_FLAGS= -lm

CLIENT_DIR=client
SERVER_DIR=server
COMMON_DIR=common
HASHLIB_DIR=hashlib2plus

CLIENT_SOURCES= ${CLIENT_DIR}/client.o  ${CLIENT_DIR}/shell.o ${CLIENT_DIR}/configuration.o

COMMON=  \
		${COMMON_DIR}/md5.o \
		${COMMON_DIR}/network.o \
		${COMMON_DIR}/fileHandler.o \
		${COMMON_DIR}/network.o \
		${COMMON_DIR}/paramParser.o

SERVER_SOURCES= \
		${SERVER_DIR}/server.o \
		${SERVER_DIR}/serverSideConfiguration.o \
		${SERVER_DIR}/commandHandler.o \
		${SERVER_DIR}/compressor.o


MD5_HEADERS= ${COMMON_DIR}/hl_md5wrapper.o ${COMMON_DIR}/hl_md5.o
MD5_LIB= ${COMMON_DIR}/${HASHLIB_DIR}/libhl++.a

CLIENT_OUT=client
SERVER_OUT=server

default: all

all:	client server
	
client:	${COMMON} ${CLIENT_SOURCES}
	@${CC} ${CLIENT_FLAGS} -o ${CLIENT_OUT} ${COMMON} ${CLIENT_SOURCES}			
	@echo [CC] $@
	@echo "[--] compressor-client compiled!!"

server: ${COMMON} ${SERVER_SOURCES}
	@${CC} ${SERVER_FLAGS} -o ${SERVER_OUT} ${COMMON} ${SERVER_SOURCES}
	@echo [CC] $@
	@echo "[--] compressor-server compiled!!"

.PHONY: clean
clean:
	@echo "Cleaning...."
	-@rm ${COMMON_DIR}/*.o 2>/dev/null || true
	-@rm ${COMMON_DIR}/*.o.d 2>/dev/null || true
	-@rm ${CLIENT_DIR}/*.o 2>/dev/null || true
	-@rm ${CLIENT_DIR}/*.o.d 2>/dev/null || true
	-@rm ${SERVER_DIR}/*.o 2>/dev/null || true
	-@rm ${SERVER_DIR}/*.o.d 2>/dev/null || true
	-@rm client server 2>/dev/null || true
	@echo "Done!"	
	
${COMMON_DIR}/network.o: ${COMMON_DIR}/network.c
	@${CC} ${CFLAGS} -o ${COMMON_DIR}/network.o ${COMMON_DIR}/network.c
	@echo [CC] $@
	
${CLIENT_DIR}/client.o: ${CLIENT_DIR}/client.c
	@${CC} ${CFLAGS} -o ${CLIENT_DIR}/client.o ${CLIENT_DIR}/client.c
	@echo [CC] $@
	
${CLIENT_DIR}/configuration.o: ${CLIENT_DIR}/configuration.c
	@${CC} ${CFLAGS} -o ${CLIENT_DIR}/configuration.o ${CLIENT_DIR}/configuration.c
	@echo [CC] $@
	
${SERVER_DIR}/serverSideConfiguration.o: ${SERVER_DIR}/serverSideConfiguration.c
	@${CC} ${CFLAGS} -o ${SERVER_DIR}/serverSideConfiguration.o ${SERVER_DIR}/serverSideConfiguration.c
	@echo [CC] $@
		
${COMMON_DIR}/fileHandler.o: ${COMMON_DIR}/fileHandler.c
	@${CC} ${CFLAGS} -o ${COMMON_DIR}/fileHandler.o ${COMMON_DIR}/fileHandler.c
	@echo [CC] $@
	
${COMMON_DIR}/md5.o: ${COMMON_DIR}/md5.c
	@${CC} ${CFLAGS} -o ${COMMON_DIR}/md5.o ${COMMON_DIR}/md5.c
	@echo [CC] $@
	
${CLIENT_DIR}/main.o: ${CLIENT_DIR}/client.c
	@${CC} ${CFLAGS} -o ${CLIENT_DIR}/client.o ${CLIENT_DIR}/client.c
	@echo [CC] $@
	
${CLIENT_DIR}/paramParser.o: ${CLIENT_DIR}/paramParser.c
	@${CC} ${CFLAGS} -o ${CLIENT_DIR}/paramParser.o ${CLIENT_DIR}/paramParser.c
	@echo [CC] $@
	
${CLIENT_DIR}/shell.o: ${CLIENT_DIR}/shell.c
	@${CC} ${CFLAGS} -o ${CLIENT_DIR}/shell.o ${CLIENT_DIR}/shell.c
	@echo [CC] $@
	
${SERVER_DIR}/main.o: ${SERVER_DIR}/server.c
	@${CC} ${CFLAGS} -o ${SERVER_DIR}/server.o ${SERVER_DIR}/server.c
	@echo [CC] $@
	
${SERVER_DIR}/paramParser.o: ${SERVER_DIR}/paramParser.c
	@${CC} ${CFLAGS} -o ${SERVER_DIR}/paramParser.o ${SERVER_DIR}/paramParser.c
	@echo [CC] $@
	
${SERVER_DIR}/commandHandler.o: ${SERVER_DIR}/commandHandler.c
	@${CC} ${CFLAGS} -o ${SERVER_DIR}/commandHandler.o ${SERVER_DIR}/commandHandler.c
	@echo [CC] $@
	
${SERVER_DIR}/compressor.o: ${SERVER_DIR}/compressor.c
	@${CC} ${CFLAGS} -o ${SERVER_DIR}/compressor.o ${SERVER_DIR}/compressor.c
	@echo [CC] $@
	
${SERVER_DIR}/server.o: ${SERVER_DIR}/server.c
	@${CC} ${CFLAGS} -o ${SERVER_DIR}/server.o ${SERVER_DIR}/server.c
	@echo [CC] $@
	
