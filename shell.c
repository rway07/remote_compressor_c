#include "shell.h"

// Flush the stdin buffer
void flush_buffer()
{
        char c;

        while ((c = getchar()) != '\n' && c != EOF);
}

// Interrogate server for supported compressors
int enable_compressors()
{// TO CHECK
        int ret = 0;
        int enable = 0;
        int i = 0;

        for (i = 2; i < SUPPORTED_COMPRESSORS; i++) {
                ret = receive_int(cln->client_socket, &enable);

                if (ret != 0) {
                        if (ret == 0) {
                                return NOT_CONNECTED;
                        } else {
                                ret = send_response(cln->client_socket, ERROR);
                        }
                } else {
			if (enable == 1) {
				cfg->compressor_enabled[i] = 1;
			} else {
				printf("ATTENZIONE: Compressore %s non disponibile!!\n",cfg->compressor_name[i]);
			}
			if ((ret = send_response(cln->client_socket, OK)) != 0) {
                                return ret;
                        }
                }
        }

        return 0;
}

// Check if the name passed by parameter is valid
int check_name(const char* name)
{
        if (strlen(name) > MAX_FILENAME_LEN) {
                printf("ERRORE: Nome del file troppo lungo!!\n");
                return -1;
        }
        if (strchr(name, '*') != NULL) {
                printf("ERRORE: Carattere non consentito nel nome del file!!\n");
                return -1;
        }
        if (strchr(name, ':') != NULL) {
                printf("ERRORE: Carattere non consentito nel nome del file!!\n");
                return -1;
        }
        if (strchr(name, '\\') != NULL) {
                printf("ERRORE: Carattere non consentito nel nome del file!!\n");
                return -1;
        }
        if (strchr(name, '/') != NULL) {
                printf("ERRORE: Carattere non consentito nel nome del file!!\n");
                return -1;
        }
        if (strchr(name, '<') != NULL) {
                printf("ERRORE: Carattere non consentito nel nome del file!!\n");
                return -1;
        }
        if (strchr(name, '>') != NULL) {
                printf("ERRORE: Carattere non consentito nel nome del file!!\n");
                return -1;
        }
        if (strchr(name, '|') != NULL) {
                printf("ERRORE: Carattere non consentito nel nome del file!!\n");
                return -1;
        }
        if (strchr(name, '"') != NULL) {
                printf("ERRORE: Carattere non consentito nel nome del file!!\n");
                return -1;
        }
        if (strchr(name, '?') != NULL) {
                printf("ERRORE: Carattere non consentito nel nome del file!!\n");
                return -1;
        }

        return 0;
}

// Check if the compressor passed by parameter is valid
int check_compressor(const char* compressor)
{
        if (strcmp(compressor, COMPRESSOR_GNUZIP) == 0) {
                return GNUZIP;
        } else if (strcmp(compressor, COMPRESSOR_BZIP2) == 0) {
                return BZIP2;
        } else if ((strcmp(compressor, COMPRESSOR_ZIP) == 0) && (cfg->compressor_enabled[ZIP] == 1)) {
                return ZIP;
        } else if ((strcmp(compressor, COMPRESSOR_XZ) == 0) && (cfg->compressor_enabled[XZ] == 1)) {
                return XZ;
        }

        return -1;
}

// Do various check before the execution of command compress
int check_compress(const char* param)
{
        int ret = 0;
        int sent_files = cfg->sent_files;
        int sent_files_at_compress = cfg->sent_files_at_compress;
        char* recv_folder = NULL;

        if (sent_files == 0) {
                printf("ERRORE: Nessun file inviato!\n");
                return -1;
        }

        if ((sent_files_at_compress == sent_files) && (cfg->dirty == 0)) {
                printf("ERRORE: Archivio già ricevuto e nessun file aggiunto!!\n");
                return -1;
        }

        if (is_directory(param) != 1) {
                printf("ERRORE: Il parametro passato non corrisponde a una directory!!\n");
                return -1;
        }

        if (got_permissions(param) != 0) {
                return -1;
        }

        recv_folder = get_recv_folder(param);
        ret = create_directory(recv_folder);
        free(recv_folder);
        if (ret == -1) {
                return -1;
        }

        if (check_configuration() != 0) {
                return -1;
        }

        return 0;
}

// Do various check before execution of command send
int check_send(const char* param)
{
        char* name = NULL;
        char* buffer = NULL;

        buffer = (char*)malloc((strlen(param) + 1) * sizeof(char));
        strcpy(buffer, param);

        if (file_exist(buffer) != 0) {
                free(buffer);
                return -1;
        } else if (is_directory(buffer) == 1) {
                printf("ERRORE: Il parametro fa riferimento a una directory!\n");
                free(buffer);
                return -1;
        }

        free(name);
        free(buffer);

        return 0;
}

// Do a integrity configuration check
int check_configuration()
{
        int current_index = cfg->compressor_index;
        int socket = cln->client_socket;
        int len = 0;
        int index = 0;
        int ret = 0;
        char* buffer = NULL;
        char* current_archive_name = NULL;

        ret = do_send_command(NET_CFG_CHECK_INDEX);
        if (ret != 0) {
                return -1;
        }

        if ((ret = receive_int(socket, &len) != 0)) {
                return handle_rcv_error(socket, ret);
        } else {
                if ((ret = send_response(socket, OK)) != 0) {
                        return ret;
                }
        }

        buffer = (char*)malloc((len + 1) * sizeof(char));
        if ((ret = receive_data(socket, buffer, len)) != 0) {
                return handle_rcv_error(socket, ret);
        } else {
                if ((ret = send_response(socket, OK)) != 0) {
                        return ret;
                }
        }

        if ((ret = receive_int(socket, &index)) != 0) {
                return handle_rcv_error(socket, ret);
        } else {
                if ((ret = send_response(socket, OK)) != 0) {
                        return ret;
                }
        }

        current_archive_name = get_archive_name();
        printf("Controllo configurazione...");
        if (strcmp(buffer, current_archive_name) != 0) {
                printf("NAME FIX!");
                set_archive_name(buffer);
        }
        if (index != current_index) {
                printf("INDEX FIX!");
                set_compressor_index(index);
        }
        free(current_archive_name);
        free(buffer);
        printf("OK\n");

        return 0;
}

// Obtain command number
int get_command_number(const char* command)
{
        int select = 0;

        if (strcmp(command, EXIT_CMD) == 0) {
                select = NET_EXIT_INDEX;
        } else if (strcmp(command, HELP_CMD) == 0) {
                select = NET_HELP_INDEX;
        } else if (strcmp(command, SHOW_CFG_CMD) == 0) {
                select = NET_SHOW_CFG_INDEX;
        } else if (strcmp(command, CFG_NAME_CMD) == 0) {
                select = NET_CFG_NAME_INDEX;
        } else if (strcmp(command, CFG_COMP_CMD) == 0) {
                select = NET_CFG_COMP_INDEX;
        } else if (strcmp(command, SEND_CMD) == 0) {
                select = NET_SEND_INDEX;
        } else if (strcmp(command, COMPRESS_CMD) == 0) {
                select = NET_COMPRESS_INDEX;
        } else {
                printf("ERRORE: Comando sconosciuto!!\n");
                select = -1;
        }

        return select;
}

// Check params number
int params_num_check(int cmd, const char* param)
{
        switch (cmd) {
                case 0:
                case 1:
                case 2:
                        if (param != NULL) {
                                if (strlen(param) > 0) {
                                        printf("ERRORE: Bad command!\n");
                                        return -1;
                                }
                        }
                        break;
                case 3:
                case 4:
                case 5:
                case 6:
                        if (param == NULL) {
                                printf("ERRORE: Comando incompleto!!\n");
                                return -1;
                        }
                        break;
                default:
                        break;
        }

        return 0;
}

// For each command call the paramter checking routine
int params_check(int cmd, const char* param)
{
        switch (cmd) {
                case 3:
                        if (check_compress(param) != 0) {
                                return -1;
                        }
                        break;
                case 4:
                        if (check_send(param) != 0) {
                                return -1;
                        }
                        break;
                case 5:
                        if (check_name(param) != 0) {
                                return -1;
                        }
                        break;
                case 6:
                        if (check_compressor(param) == -1) {
                                printf("ERRORE: compressore non supportato\n");
                                return -1;
                        }
                        break;
                }

        return 0;
}

// Execute help command
int do_help()
{
        int i = 0;

        printf("remote-compressor - lista comandi\n");
        printf("*) configure-compressor [compressor]");
        for (i = 0; i < SUPPORTED_COMPRESSORS; i++) {
                if (cfg->compressor_enabled[i] == 1) {
			printf(" %s", cfg->compressor_name[i]);
        	}
	}
        printf("\n");
        printf("*) configure-name [archive_name]\n");
        printf("*) show-configuration\n");
        printf("*) send [local_file]\n");
        printf("*) compress [local_path]\n");
        printf("*) help\n");
        printf("*) quit\n");
        printf("\n");

        return 0;
}

//Execute quit command
int do_quit()
{
        printf("Uscita...\n");

        if (errno != 0) {
                perror("LAST ERROR");
        }
        printf("Bye Bye\n");

        return EXIT_COMMAND;
}

// Execute send command
int do_send(char* param)
{
        int ret = 0;
        int socket = cln->client_socket;
        unsigned int len = 0;
        unsigned char* hash = NULL;
        char* file_name = NULL;

        file_name = basename(param);
        printf("Invio del file %s in corso...\n", file_name);

        len = strlen(file_name);
        if ((ret = send_int(socket, len)) != 0) {
                return ret;
        }

        if ((ret = receive_response(socket)) != 0) {
                return ret;
        }

        if ((ret = send_data(socket, file_name, len)) != 0) {
                return ret;
        }

        if ((ret = receive_response(socket)) != 0) {
                return ret;
        }

        len = get_file_len(param);
        if ((ret = send_int(socket, len)) != 0) {
                return ret;
        }

        ret = receive_response(socket);
        if (ret != 0) {
                return ret;
        }

        hash = calculate_MD5(param);
        if ((ret = send_data(socket, (char*) hash, MD5_DIGEST_LENGTH)) != 0) {
                return ret;
        }

        if ((ret = receive_response(socket)) != 0) {
                return ret;
        }

        printf("Hash MD5: ");
        print_MD5(hash);
        free(hash);

        if ((ret = receive_response(socket)) != 0) {
                if (ret == COMMAND_CONTINUE) {
                        printf("SERVER: file già inviato!!\n");
                }
                return ret;
        }

        ret = read_and_send_file(cln->client_socket, param, len);
        if (ret != 0) {
                return -1;
        }

        if (receive_response(socket) != 0) {
                return ret;
        }

        printf("File %s inviato con successo.\n", file_name);

        return 0;
}

// Execute compress command
int do_compress(const char* param)
{
        int ret = 0;
        int socket = cln->client_socket;
        int len = 0;
        //char* buffer = NULL;
        //char* tmp = NULL;
        char* file_name = NULL;
        //char* folder = NULL;
        unsigned int file_len = 0;
        unsigned char* r_hash = NULL;
        unsigned char* c_hash = NULL;

        printf("Richiesta compressione al server...\n");
        if ((ret = receive_response(socket)) != 0) {
                return ret;
        }

        if ((ret = receive_int(socket, &len)) != 0) {
                return handle_rcv_error(socket, ret);
        } else {
                if ((ret = send_response(socket, OK)) != 0) {
                        return ret;
                }
        }

        file_len = len;
        r_hash = (unsigned char *)malloc(MD5_DIGEST_LENGTH + 1);
        ret = receive_data(socket, (char*) r_hash, MD5_DIGEST_LENGTH);
        if (ret != 0) {
                return handle_rcv_error(socket, ret);
        } else {
                if ((ret = send_response(socket, OK)) != 0) {
                        return ret;
                }
        }

        /*folder = get_recv_folder(param);
        len = strlen(cfg->compressor_ext[cfg->compressor_index]);
        buffer = (char*)malloc((len + 1) * sizeof(char));
        strcpy(buffer, cfg->compressor_ext[cfg->compressor_index]);
        tmp = get_archive_name();
        len = strlen(folder) + strlen(tmp) + strlen(buffer) + 1;
        file_name = (char*) malloc((len + 1) * sizeof (char));
        strcpy(file_name, folder);
        strcat(file_name, "/");
        strcat(file_name, tmp);
        strcat(file_name, buffer);
        free(folder);
        free(buffer);
        free(tmp);*/
        file_name = get_complete_archive_path(param);

        if (file_exist(file_name) == 0) {
                delete_file(file_name);
        }

        if ((ret = receive_and_write_file(cln->client_socket, file_name, file_len)) != 0) {
                free(r_hash);
                free(file_name);
                return -1;
        }

        c_hash = calculate_MD5(file_name);
        /*printf("Hash MD5 ricevuto: ");
        print_MD5(r_hash);
        printf("Hash MD5 corrente: ");
        print_MD5(c_hash);

        if (strncmp((const char*)c_hash, (const char*)r_hash, MD5_DIGEST_LENGTH) == 0) {
                printf("MD5 corretto!!\n");
        } else {
                free(c_hash);
                free(r_hash);
                return -1;
        }
        free(c_hash);
        free(r_hash);*/
        if (compare_MD5("CLIENT", r_hash, c_hash) != 0) {
                send_response(socket, ERROR);
                printf("ERRORE: confronto Hash errato, file corrotto!!\n");
                return -1;
        }
        
        if ((ret = send_response(socket, OK)) != 0) {
                free(file_name);
                return ret;
        }

        cfg->sent_files_at_compress = cfg->sent_files;
        printf("Archivio %s ricevuto con successo.\n", basename(file_name));
        free(file_name);

        return 0;
}

// Execute configure-compressor command
int do_conf_comp(int type)
{
        int ret = 0;
        int socket = cln->client_socket;

        printf("Configurazione compressore...\n");
        if ((ret = send_int(socket, type)) != 0) {
                return ret;
        }

        if ((ret = receive_response(socket)) != 0) {
                return ret;
        }

        cfg->compressor_index = type;
        cfg->dirty = 1;
        printf("Compressore configurato con successo.\n");

        return 0;
}

// Execute configure-name command
int do_conf_name(const char* param)
{
        int len = 0;
        int ret = 0;
        int socket = cln->client_socket;
        char* buffer = NULL;

        printf("Configurazione nome...\n");
        len = (strlen(param)) * sizeof(char);
        if ((ret = send_int(socket, len)) != 0) {
                return ret;
        }

        if ((ret = receive_response(socket)) != 0) {
                return ret;
        }

        buffer = (char*)malloc((len + 1) * sizeof(char));
        strcpy(buffer, param);
        if ((ret = send_data(socket, buffer, len)) != 0) {
                return ret;
        }

        if ((ret = receive_response(socket)) != 0) {
                free(buffer);
                return ret;
        }

        set_archive_name(buffer);
        free(buffer);

        printf("Nome configurato con successo.\n");

        return 0;
}

// Execute show-configuration command
int do_show_conf()
{
        char *buffer = NULL;
        int len = 0;
        int index = 0;
        int ret = 0;
        int socket = cln->client_socket;

        if ((ret = receive_int(socket, &len)) != 0) {
                return handle_rcv_error(socket, ret);
        } else {
                if ((ret = send_response(socket, OK)) != 0) {
                        return ret;
                }
        }

        buffer = (char*)malloc((len + 1) * sizeof(char));
        if ((ret = receive_data(socket, buffer, len)) != 0) {
                return handle_rcv_error(socket, ret);
        } else {
                if ((ret = send_response(socket, OK)) != 0) {
                        return ret;
                }
        }

        if ((ret = receive_int(socket, &index)) != 0) {
                return handle_rcv_error(socket, ret);
        } else {
                if ((ret = send_response(socket, OK)) != 0) {
                        return ret;
                }
        }
        set_archive_name(buffer);
        set_compressor_index(index);
        printf("Configurazione corrente\n");
        printf("Nome archivio > %s\n", buffer);
        printf("Compressore > %s\n", cfg->compressor_name[index]);
        printf("\n");

        free(buffer);

        return 0;
}

// Send command string to the server
int do_send_command(int command_index)
{
        int socket = cln->client_socket;
        int ret = 0;
        char cmd[NET_CMD_LEN +1];

        switch (command_index) {
	        case 0:
        	        strcpy(cmd, NET_EXIT_CMD);
                	break;
	        case 1:
        	        strcpy(cmd, NET_HELP_CMD);
	                break;
        	case 2:
                	strcpy(cmd, NET_SHOW_CFG_CMD);
	                break;
	        case 3:
        	        strcpy(cmd, NET_COMPRESS_CMD);
	                break;
        	case 4:
                	strcpy(cmd, NET_SEND_CMD);
	                break;
        	case 5:
	                strcpy(cmd, NET_CFG_NAME_CMD);
        	        break;
	        case 6:
        	        strcpy(cmd, NET_CFG_COMP_CMD);
	                break;
        	case 9:
                	strcpy(cmd, NET_CFG_CHECK_CMD);
	                break;
        }

        if ((ret = send_command(socket, cmd)) != 0) {
                return -1;
        }

        if ((ret = receive_response(socket)) != 0) {
                return ret;
        }

        return 0;
}

// Divide the command line string in token and manage it
int tokenize_command(char* command_line, char** param)
{
        char* params[MAX_CMD_TOKENS];
        char* temp = NULL;
        char* p = NULL;
        int len = 0;
        int i = 0;
        int n = 0;

        len = strlen(command_line);
        if (strcmp(command_line, "\n") == 0) {
                return -1;
        } else if (len > MAX_CMD_LEN) {
                printf("ERRORE: Comando troppo lungo!!\n");
                return -1;
        } else if (len < MIN_CMD_LEN) {
                printf("ERRORE: Comando non valido!!\n");
                return -1;
        }

        for (i = 0; i < MAX_CMD_TOKENS; i++) {
                params[i] = NULL;
        }

        temp = (char*) malloc((strlen(command_line) + 1) * sizeof (char));
        strcpy(temp, command_line);
        for (p = strtok(temp, " \n"); p != NULL; p = strtok(NULL, " \n")) {
                if (n >= MAX_CMD_TOKENS) {
                        for (i = 0; i < n; i++) {
                                free(params[i]);
                        }
                        free(temp);
                        printf("ERRORE: comando non valido!!!\n");
                        return -1;
                }

                if (p != NULL) {
                        params[n] = (char*) malloc((strlen(p) + 1) * sizeof (char));
                        strcpy(params[n], p);
                }
                n++;
        }

        len = get_command_number(params[CMD_POS]);
        if (params[PARAM_POS] != NULL) {
                *param = (char*) malloc((strlen(params[PARAM_POS]) + 1) * sizeof (char));
                strcpy(*param, params[PARAM_POS]);
        }

        for (i = 0; i < n; i++) {
                free(params[i]);
        }
        free(temp);

        return len;
}

// Parse the command line
int parse_command(char* command)
{
        int select = 0;
        int ret = 0;
        int exit = 0;
        char* param = NULL;

        select = tokenize_command(command, &param);

        if (select != -1) {
                ret = params_num_check(select, param);
                if (ret == 0) {
                        ret = params_check(select, param);
                        if (ret == 0) {
                                ret = do_send_command(select);
                                if (ret != 0) {
                                        free(param);
                                        if (ret == NOT_CONNECTED) {
                                                printf("SERVER: Il server ha chiuso la connessione! errno: %d\n", errno);
                                        }
                                        return COMMAND_ERROR;
                                }

                                switch (select) {
                               		case 0:
                                        	exit = do_quit();
	                                        break;
        	                        case 1:
	                                        exit = do_help();
        	                                break;
                	                case 2:
						exit = do_show_conf();
        	                                break;
                	                case 3:
	                                        exit = do_compress(param);
        	                                if (exit == 0) {
	                                                cfg->dirty = 0;
        	                                }
                	                        break;
                        	        case 4:
	                                        exit = do_send(param);
						if (exit == 0) {
							cfg->sent_files++;
						}
                                	        break;
	                                case 5:
        	                                exit = do_conf_name(param);
                	                        break;
                        	        case 6:
                                	        exit = do_conf_comp(check_compressor(param));
	                                        break;
        	                        default:
                	                        printf("DEBUG: You should not see me...\n");
                        	                break;
                                }
                        }
                }
        }
        free(param);

        return exit;
}

// Show command line
int command_line()
{
	int ret = 0;
        char cmd[MAX_CMD_LEN];

        bzero(cmd, sizeof(cmd));
	do {
                printf("remote-compressor> ");
                strcpy(cmd, "");
                fgets(cmd, MAX_CMD_LEN, stdin);

                ret = parse_command(cmd);
                if (strlen(cmd) == MAX_CMD_LEN - 1) {
                        flush_buffer();
                }
        } while ((ret != EXIT_COMMAND) && (ret != COMMAND_ERROR));

        return 0;
}
