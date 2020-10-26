#include "server.h"

// Manage client's disconnection from server
int client_disconnect(int id)
{
        int socket = get_socket(id);
        char* addr = get_client_address(id);
        //close(socket);
        shutdown(socket, SHUT_RDWR);
        printf("SERVER: client ID %d::%s chiude la connessione.\n", id, addr);
        free(addr);

        return 0;
}

// Global signal handling function
void global_signal_handler(int dummy)
{
        static int i = 0;

        printf("DEBUG: SIGNAL HANDLER CALL %d: ctrl+C captured\n", i);
        i++;

        if (running_threads > 0) {
                printf("DEBUG: There are %d client connected. At third call I kill them all!\n", running_threads);
        }

        if (i > 2) {
                printf("DEBUG: Third call, KILL THEM ALL!\n");
                server_exit(EXIT_FAILURE);
        }
}

// Create working directories for server functionality
int create_working_directory(int id)
{
        int ret = 0;
        char* arch_dir = get_archive_dir(id);
        char* files_dir = get_files_dir(id);

        ret = create_directory(arch_dir);
        if (ret == -2) {
                delete_directory(arch_dir);
                create_directory(arch_dir);
        } else if (ret == -1) {
                free(arch_dir);
                return -1;
        }
        free(arch_dir);

        ret = create_directory(files_dir);
        if (ret == -2) {
                delete_directory(files_dir);
                create_directory(files_dir);
        } else if (ret == -1) {
                free(files_dir);
                return -1;
        }

        free(files_dir);
        return 0;
}

// Destroy all working directories
void destroy_working_directory(int id)
{
        int ret = 0;
        char* arch_dir = get_archive_dir(id);
        char* files_dir = get_files_dir(id);

        ret = delete_directory(arch_dir);
        free(arch_dir);
        if (ret == -1) {
                printf("ERRORE: Error deleting archives directory...\n");
        }

        ret = delete_directory(files_dir);
        free(files_dir);
        if (ret == -1) {
                printf("ERRORE: Error deleting files directory...\n");
        }
}

// Generate an ID after client connection
int generate_client_ID()
{
        int i = 0;
        // stub
        pthread_mutex_lock(&conf_mutex);
        for (i = 0; i < MAX_CLIENTS; i++) {
                if (client_ids[i] == 0) {
                        client_ids[i] = 1;
                        threads[i] = (struct thread_info*) malloc(sizeof (struct thread_info));
                        pthread_mutex_unlock(&conf_mutex);
                        return i;
                }
        }
        pthread_mutex_unlock(&conf_mutex);
        return -1;
}

// Free client's ID after disconnection
int free_client_ID(int id)
{
        int ret = 0;

        pthread_mutex_lock(&conf_mutex);
        client_ids[id] = 0;
        pthread_mutex_unlock(&conf_mutex);

        ret = destroy_single_thread_conf(id);
        if (ret == -1) {
                printf("DEBUG: Client ID already free (?)\n");
                return -1;
        }

        return 0;
}

// Manage server shutdown
void server_exit(int status)
{
        // Qui dovrei killare tutti i thread!!
        destroy_all_thread_conf();
        destroy_global_configuration();
        destroy_mutex();
        exit(status);
}

// Lister for incoming connections
int server_listen()
{
        int ret = 0;
        int current_client_socket = 0;
        int client_ID = 0;
        unsigned int len = 0;
        char* address = NULL;
        struct serverSideConfiguration* new_cfg = NULL;

        globalCfg->server_socket = socket(PF_INET, SOCK_STREAM, 0);
        if (globalCfg->server_socket == -1) {
                //printf("ERRORE: Error in socket() function\n");
                perror("ERRORE: socket()");
                return -1;
        }

        memset(&globalCfg->s_address, 0, sizeof (globalCfg->s_address));
        globalCfg->s_address.sin_family = AF_INET;
        globalCfg->s_address.sin_port = htons(globalCfg->port);
        globalCfg->s_address.sin_addr.s_addr = htonl(INADDR_ANY);

        ret = bind(globalCfg->server_socket, (SA *) & globalCfg->s_address, sizeof (globalCfg->s_address));
        if (ret == -1) {
                //printf("ERRORE: Error in bind() function\n");
                perror("ERRORE: bind()");
                return -1;
        }

        ret = listen(globalCfg->server_socket, MAX_CLIENTS);
        if (ret == -1) {
                //printf("ERRORE: Error in listen() function()\n");
                perror("ERRORE: listen()");
                return -1;
        }

        printf("In attesa di connessioni...\n");
        for (;;) {
                len = sizeof (globalCfg->c_address);
                current_client_socket = accept(globalCfg->server_socket, (SA *) & globalCfg->c_address, &len);
                if (current_client_socket == -1) {
                        printf("ERRORE: Errore nella connessione del client, errno: %d\n", errno);
                        perror("accept()");
                        //server_exit(EXIT_FAILURE); // Qui torna all'inizio del for??
                        continue;
                }
                address = inet_ntoa(globalCfg->c_address.sin_addr);

                client_ID = generate_client_ID();
                if (client_ID == -1) {
                        printf("ERRORE: Raggiunto il numero massimo di client!! Riprovare più tardi!!\n");
                        send_int(current_client_socket, MAX_CLIENTS_NUMBER);
                        close(current_client_socket);
                        continue;
                }

                new_cfg = (struct serverSideConfiguration*) init_client_conf(client_ID, current_client_socket, address, globalCfg->base_directory);
                //pthread_mutex_lock(&conf_mutex);
                threads[client_ID]->clientCfg = new_cfg;
                
                /*if ((ret = send_int(threads[client_ID]->clientCfg->client_socket, client_ID)) != 0) {
                        printf("ERRORE: Errore nell'invio del client_ID, connessione bloccata!\n");
                        close(threads[client_ID]->clientCfg->client_socket);
                        continue;
                }

                if ((ret = receive_response(threads[client_ID]->clientCfg->client_socket)) != 0) {
                        close(threads[client_ID]->clientCfg->client_socket);
                        continue;
                }

                if ((ret = server_enable_compressors(threads[client_ID]->clientCfg->client_socket)) != 0){
                        close(threads[client_ID]->clientCfg->client_socket);
                        continue;
                }*/

                ret = pthread_create(&threads[client_ID]->thread_id, NULL, handle_command, &threads[client_ID]->clientCfg->client_ID);                
                running_threads++;
                //printf("DEBUG: running_threads: %d\n", running_threads);
                //pthread_mutex_unlock(&conf_mutex);
                printf("CLIENT %s connesso. ID => %d\n", address, client_ID);
                printf("ACCEPT LOOP ENDED\n");
        }
        
        return 0;
}

// Server main function
int main(int argc, char** argv)
{
        int ret = 0;
        char *working_dir = NULL;

	signal(SIGINT, global_signal_handler);
	printf("Remote Compressor v%s - Server\n",VERSION);

        if ((init_mutex() != 0) || (init_global_configuration() != 0) || (init_threads_conf() != 0)) {
                return -1;
        }

        if (check_compressors() == -1) {
                server_exit(EXIT_FAILURE);
        }

        if ((ret = s_parse_params(argc, argv)) == -1) {
                server_exit(EXIT_FAILURE);
        }
        globalCfg->port = ret;

        if ((working_dir = get_working_dir()) != NULL) {
                if ((ret = got_permissions(working_dir)) != 0) {
                        printf("ERRORE: Il server non ha i permessi di scrittura sulla directory corrente!! Uscita forzata!!\n");
                        free(working_dir);
                        server_exit(EXIT_FAILURE);
                }
                globalCfg->base_directory = (char*)malloc((strlen(working_dir) + 1) * sizeof(char));
                strcpy(globalCfg->base_directory, working_dir);
                free(working_dir);
        }

        server_listen();
        server_exit(EXIT_SUCCESS);

        return 0;
}
