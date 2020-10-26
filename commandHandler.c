#include "commandHandler.h"
#include "server.h"
#include "serverSideConfiguration.h"
#include "compressor.h"

// Comunicate to the client which compressors enable
int server_enable_compressors(int socket)
{
        int i = 0;
        int ret = 0;

        for (i = 2; i < SUPPORTED_COMPRESSORS; i++) {
                ret = send_int(socket, globalCfg->compressor_enabled[i]);
                ret = receive_response(socket);
                if (ret != 0) {
                        return ret;
                }
        }

        return 0;
}

int init_client_params(int id, int socket)
{
        int ret = 0;
        
        if ((ret = send_int(socket, id)) != 0) {
                printf("ERRORE: Errore nell'invio del client_ID, connessione bloccata!\n");
                return -1;
        }

        if ((ret = receive_response(socket)) != 0) {
                return -1;
        }

        if ((ret = server_enable_compressors(socket)) != 0){
                printf("ERRORE: Errore nell'abilitazione dei compressori!\n");
                return -1;
        }
        
        return 0;
}

void thread_clean(int id)
{
        client_disconnect(id);
        destroy_client(id);
        pthread_mutex_lock(&conf_mutex);
        running_threads = running_threads - 1;
        //printf("DEBUG: running_threads: %d\n", running_threads);
        pthread_mutex_unlock(&conf_mutex);
}

// Main thread function that manage commands sent to the server
void *handle_command(void* data)
{
        char command[NET_CMD_LEN + 1];
        int ret = 0;
        int id = *((int*) data);
        int socket = get_socket(id);
        sigset_t mask;
        struct sched_param param;
        
	#ifdef __LINUX__
        	param.__sched_priority = 0;
        #endif
	sigemptyset(&mask);
        sigaddset(&mask, SIGINT);
        pthread_sigmask(SIG_BLOCK, &mask, NULL);
        pthread_detach(pthread_self());
        if ((ret = pthread_setschedparam(pthread_self(), SCHED_RR, &param)) != 0) {
                switch (ret){
                        case EINVAL:
                                perror("EINVAL");
                                break;
                        case EPERM:
                                perror("EPERM");
                                break;
                        default:
                                printf("%d\n",ret);
                                break;
                }
        }
        //printf("DEBUG: Thread %d started\n", id);

        if (init_client_params(id, socket) != 0) {
                thread_clean(id);
                pthread_exit(NULL);
        }
        
	bzero(command, sizeof(command));
        //printf("DEBUG: Configuring working directory...\n");
        ret = create_working_directory(id);
        if (ret == -1) {
                printf("ERRORE: Errore nella creazione delle directory di lavoro, uscita forzata!\n");
        } else {
                while ((strcmp(NET_EXIT_CMD, command) != 0) || (ret != NOT_CONNECTED) || (ret != FATAL_ERROR)) {
                        if ((ret = receive_command(socket, command)) != 0) {
                                ret = handle_rcv_error(socket, ret);
                                break;
                        }

                        if ((ret = send_response(socket, OK)) != 0) {
                                break;
                        }

                        ret = command_parser(id, command);
                }
        }
        
        thread_clean(id);
        /*client_disconnect(id);
        destroy_client(id);
        pthread_mutex_lock(&conf_mutex);
        running_threads = running_threads - 1;
        printf("DEBUG: running_threads: %d\n", running_threads);
        pthread_mutex_unlock(&conf_mutex);
*/
        pthread_exit(NULL);      
}

// For each command call the function to manage it
int command_parser(int id, const char* cmd)
{
        int ret = 0;
        int value = cmd[3] - '0';
        char* address = NULL;

        address = get_client_address(id);
        switch (value) {
                case 0:
                        printf("CLIENT %d::%s: eseguito comando quit\n",id ,address);
                        break;
                case 1:
                        printf("CLIENT %d::%s: eseguito comando help\n",id ,address);
                        break;
                case 2:
                        printf("CLIENT %d::%s: eseguito comando show-configuration\n",id ,address);
                        ret = handle_show_configuration(id);
                        break;
                case 3:
                        printf("CLIENT %d::%s: eseguito comando compress\n",id ,address);
                        ret = handle_compress(id);
                        break;
                case 4:
                        printf("CLIENT %d::%s: eseguito comando send\n",id ,address);
                        ret = handle_send(id);
                        break;
                case 5:
                        printf("CLIENT %d::%s: eseguito comando configure-name\n",id ,address);
                        ret = handle_configure_name(id);
                        break;
                case 6:
                        printf("CLIENT %d::%s: eseguito comando configure-compressor\n",id ,address);
                        ret = handle_configure_compressor(id);
                        break;
                case 9:
                        printf("CLIENT %d::%s: controllo configurazione\n",id ,address);
                        ret = handle_configuration_check(id);
                        break;
                default:
                        printf("? UNKNOWN COMMAND, POSSIBLE BUG\n");
                        ret = FATAL_ERROR;
                        break;
        }
        free(address);

        if ((ret == -1) || (ret == FATAL_ERROR)){
                printf("ERRORE: Errore nell'esecuzione del comando!!\n");
                return -1;
        } else if (ret == NOT_CONNECTED) {
                printf("SERVER: client disconnesso!\n");
                return NOT_CONNECTED;
        }

        return 0;

}

// Handle configuration check command
int handle_configuration_check(int id)
{
        int len = 0;
        int ret = 0;
        int socket = get_socket(id);
        char* buffer = get_archive_name(id);

        len = strlen(buffer);
        if ((ret = send_int(socket, len)) != 0) {
                return ret;
        }

        if ((ret = receive_response(socket)) != 0) {
                free(buffer);
                return -1;
        }

        if ((ret = send_data(socket, buffer, len)) != 0) {
                return ret;
        }

        if ((ret = receive_response(socket)) != 0) {
                free(buffer);
                return -1;
        }

        if ((ret = send_int(socket, get_compressor_index(id))) != 0) {
                return ret;
        }

        if ((ret = receive_response(socket)) != 0) {
                free(buffer);
                return -1;
        }
        free(buffer);

        return 0;
}

// Handle show-configuration command
int handle_show_configuration(int id)
{
        int len = 0;
        int ret = 0;
        int index = 0;
        int socket = get_socket(id);
        char *buffer = get_archive_name(id);

        len = strlen(buffer);
        if ((ret = send_int(socket, len)) != 0) {
                return ret;
        }

        if ((ret = receive_response(socket)) != 0) {
                return ret;
        }

        if ((ret = send_data(socket, buffer, len)) != 0) {
                return ret;
        }

        if ((ret = receive_response(socket)) != 0) {
                free(buffer);
                return ret;
        }

        index = get_compressor_index(id);
        if ((ret = send_int(socket, index)) != 0) {
                return ret;
        }

        if ((ret = receive_response(socket)) != 0) {
                free(buffer);
                return ret;
        }
        printf("SERVER: Comando show-configuration eseguito correttamente\n");
        free(buffer);

        return 0;
}

// Handle configure-name command
int handle_configure_name(int id)
{
        int len = 0;
        int ret = 0;
        int socket = get_socket(id);
        char* buffer = NULL;
        char* addr = NULL;

        if ((ret = receive_int(socket, &len)) != 0) {
                return handle_rcv_error(socket, ret);
        } else {
                if ((ret = send_response(socket, OK)) != 0) {
                        return ret;
                }
        }

        buffer = (char*)malloc((len + 1) * sizeof(char));
        if (buffer == NULL) {
		return FATAL_ERROR;
	}

	if ((ret = receive_data(socket, buffer, len)) != 0) {
                return handle_rcv_error(socket, ret);
        } else {
                if ((ret = send_response(socket, OK)) != 0) {
                        return ret;
                }
        }

        set_archive_name(id, buffer);
        addr = get_client_address(id);
        printf("SERVER: %d::%s Comando configure-name eseguito correttamente\n",id ,addr);
        free(buffer);
        free(addr);

        return 0;
}

// Handle configure-compressor command
int handle_configure_compressor(int id)
{
        int ret = 0;
        int type = 0;
        int socket = get_socket(id);
        char* addr = NULL;

        if ((ret = receive_int(socket, &type)) != 0) {
                return handle_rcv_error(socket, ret);
        } else {
                if ((ret = send_response(socket, OK)) != 0) {
                        return ret;
                }
        }
        set_compressor_index(id, type);
        addr = get_client_address(id);
	printf("SERVER: %d::%s Comando configure-compressor eseguito correttamente\n",id ,addr);
        free(addr);

        return 0;
}

// Handle send command
int handle_send(int id)
{
        int socket = get_socket(id);
        int ret = 0;
        int file_len = 0;
        char* file_name = NULL;
        char* complete_file_name = NULL;
        char *buffer = NULL;
        unsigned int len = 0;
        unsigned char* r_hash = NULL;
        unsigned char* c_hash = NULL;

        if ((ret = receive_int(socket, (int*) &len)) != 0) {
                return handle_rcv_error(socket, ret);
        } else {
                if ((ret = send_response(socket, OK)) != 0) {
                        return ret;
                }
        }

        file_name = (char*)malloc((len + 1) * sizeof(char));
        if ((ret = receive_data(socket, file_name, len)) != 0) {
                return handle_rcv_error(socket, ret);
        } else {
                if ((ret = send_response(socket, OK)) != 0) {
                        return ret;
                }
        }

        if ((ret = receive_int(socket, (int*) &file_len)) != 0) {
                return handle_rcv_error(socket, ret);
        } else {
                if ((ret = send_response(socket, OK)) != 0) {
                        return ret;
                }
        }

        r_hash = (unsigned char*)malloc(MD5_DIGEST_LENGTH + 1);
        if (r_hash == NULL) {
		return FATAL_ERROR;
	}
	if ((ret = receive_data(socket, (char*) r_hash, MD5_DIGEST_LENGTH)) != 0) {
                return handle_rcv_error(socket, ret);
        } else {
                if ((ret = send_response(socket, OK)) != 0) {
                        return ret;
                }
        }
        /*buffer = get_files_dir(id);
        len = strlen(buffer) + strlen(file_name) + 1;
        complete_file_name = (char*)malloc((len + 1) * sizeof(char));
        strcpy(complete_file_name, buffer);
        strcat(complete_file_name, "/");
        strcat(complete_file_name, file_name);
        free(buffer);*/

        complete_file_name = get_complete_file_path(id, file_name);
        if (file_exist(complete_file_name) == 0) {
                c_hash = calculate_MD5(complete_file_name);
                
                if (strncmp((const char*)c_hash, (const char*)r_hash, MD5_DIGEST_LENGTH) == 0) {
                        free(c_hash);
                        free(r_hash);
                        free(file_name);
                        free(complete_file_name);
                        printf("CLIENT: File già inviato!!\n");
                        if ((ret = send_response(socket, CONTINUE)) != 0) {
                                return ret;
                        }
                        return 0;
                } else {
                        printf("DEBUG: MD5 non corretto!! Elimino il file\n");
                        if ((ret = send_response(socket, OK)) != 0) {
                                return ret;
                        }
                        delete_file(complete_file_name);
                }
        } else {
                if ((ret = send_response(socket, OK)) != 0) {
                        return ret;
                }
        }

        ret = receive_and_write_file(socket, complete_file_name, file_len);
        if (ret != 0) {
                free(r_hash);
                free(buffer);
                free(file_name);
                free(complete_file_name);
                return -1;
        }

        c_hash = calculate_MD5(complete_file_name);
        /*printf("SERVER: MD5 ricevuto: ");
        print_MD5(r_hash);
        printf("SERVER: MD5 corrente: ");
        print_MD5(c_hash);

        if (strncmp((const char*) c_hash, (const char*) r_hash, MD5_DIGEST_LENGTH) == 0) {
                printf("SERVER: MD5 corretto!!\n");
                ret = send_response(socket, OK);
        } else {
                free(c_hash);
                free(r_hash);
                ret = send_response(socket, ERROR);
                return -1;
        }

        free(c_hash);
        free(r_hash);*/
        if (compare_MD5("SERVER", r_hash, c_hash) != 0) {
                send_response(socket, ERROR);
                return -1;
        } else {
                ret = send_response(socket, OK);
        }

        buffer = get_client_address(id);
        printf("SERVER: Ricevuto il file %s dal client %d::%s\n",file_name ,id ,buffer);

        free(buffer);
        free(file_name);
        free(complete_file_name);

        return 0;
}

// Handle compress command
int handle_compress(int id)
{
        int socket = get_socket(id);
        int ret = 0;
        unsigned int len = 0;
        char* file_name = NULL;
        char* client_address = get_client_address(id);
        unsigned char *hash = NULL;

        printf("SERVER: compressione richiesta dal client %d::%s\n",id ,client_address);
        file_name = compress(id);

        if (file_name == NULL) {
                ret = send_response(socket, ERROR);
                printf("ERRORE: Errore nella creazione dell'archivio!\n");
                return -1;
        }

        printf("SERVER: Creato archivio %s\n", file_name);
        if (file_exist(file_name) != 0) {
                ret = send_response(socket, ERROR);
                printf("ERRORE: Il file richiesto non è disponibile!!\n");
                return -1;
        }

        if ((ret = send_response(socket, OK)) != 0) {
                return ret;
        }

        len = get_file_len(file_name);
        if ((ret = send_int(socket, len)) != 0) {
                return ret;
        }

        if ((ret = receive_response(socket)) != 0) {
                free(file_name);
                return -1;
        }

        hash = calculate_MD5(file_name);
        if ((ret = send_data(socket, (char*) hash, MD5_DIGEST_LENGTH)) != 0) {
                return ret;
        }

        if ((ret = receive_response(socket)) != 0) {
                free(file_name);
                free(hash);
        }

        printf("SERVER: Hash MD5: ");
        print_MD5(hash);
        printf("SERVER: Invio file in corso...\n");

        ret = read_and_send_file(socket, file_name, len);
        if (ret != 0) {
                free(file_name);
                free(hash);
                free(client_address);
                return -1;
        }

        if ((ret = receive_response(socket)) != 0) {
                free(file_name);
                free(hash);
                free(client_address);
                return -1;
        }
        delete_file(file_name);

        printf("SERVER: inviato il file %s al client %d::%s\n",basename(file_name) ,id ,client_address);
        free(hash);
        free(file_name);
        free(client_address);

        return 0;
}
