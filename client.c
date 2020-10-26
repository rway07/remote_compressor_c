#include "client.h"

extern struct configuration *cfg;
extern struct client *cln;

// Signal handling function
void global_signal_handler()
{
        client_exit(EXIT_FAILURE);
}

// Function called at client shutdown
void client_exit(int status)
{
        destroy_configuration();
        destroy_client();
        exit(status);
}

// Initialize client parameters
int init_client(struct params p)
{
        cln = (struct client*)malloc(sizeof(struct client));
        if (cln != NULL) {
                cln->client_socket = 0;
                cln->port = p.port;
                cln->server_address = (char*) malloc((strlen(p.address) + 1) * sizeof (char));
                if (cln->server_address != NULL) {
                        strcpy(cln->server_address, p.address);
                }
        }

        return 0;
}

// Destroy client parameters
void destroy_client()
{
        if (cln != NULL) {
                free(cln->server_address);
                free(cln);
        }
}

// Connect to server
int client_connect()
{
        int ret = 0;

        if (cln->client_socket == 0) {
                cln->client_socket = socket(PF_INET, SOCK_STREAM, 0);
        }

        memset(&cln->s_address, 0, sizeof(cln->s_address));
        cln->s_address.sin_family = AF_INET;
        cln->s_address.sin_port = htons(cln->port);

        ret = inet_pton(AF_INET, cln->server_address, &cln->s_address.sin_addr);
        if (ret == 0) {
                printf("ERRORE: Indirizzo non valido!\n");
                return -1;
        }

        ret = connect(cln->client_socket, (SA*) (&cln->s_address), sizeof(cln->s_address));
        if (ret == -1) {
                printf("ERRORE: Errore nella connessione al server!! errno: %d\n", errno);
                perror("DEBUG");
                return -1;
        }
        printf("Connesso al server %s sulla porta %d\n", cln->server_address, cln->port);

        return 0;
}

// Prepare client configuration after connection to the server
int prepare_client()
{
        int id = 0;
        int ret = 0;

        ret = receive_int(cln->client_socket, &id);
        
        if (ret != 0) {
                ret = send_response(cln->client_socket, ERROR);
                printf("ERRORE: Errore nella ricezione del CLIENT_ID!! Uscita..\n");
	        return -1;
        } else {
                ret = send_response(cln->client_socket, OK);
        }
        
        if (id == MAX_CLIENTS_NUMBER) {
                printf("SERVER: Raggiunto il numero massimo di client connessi, uscita...\n");
                return -1;
        } else {
                cfg->client_ID = id;
                printf("SERVER: Client ID = %d\n", cfg->client_ID);
        }
        
        if (enable_compressors() == NOT_CONNECTED){
                return NOT_CONNECTED;
        }

        return 0;
}

// Disconnect from the server
void client_disconnect()
{
        //close(cln->client_socket);
        if (shutdown(cln->client_socket, SHUT_RDWR) != 0) {
                perror("ERRORE: Errore nella chiusura della connessione");
        }
}

// Main Client function
int main(int argc, char** argv)
{
        int ret = 0;
        struct params current_params;

        signal(SIGINT, global_signal_handler);
	init_configuration();

        printf("Remote Compressor v%s - Client\n",VERSION);
        printf("\n");
        printf("ATTENZIONE:\n");
        printf("La lunghezza massima dei comandi è di %d caratteri\n", MAX_CMD_LEN);
        printf("Il massimo numero di client è %d\n", MAX_CLIENTS);
        printf("\n");

        ret = c_parse_params(argc, argv, &current_params);
        if (ret != 0) {
                exit(EXIT_FAILURE);
        }

        init_client(current_params);
        ret = client_connect();
        if (ret != 0) {
                client_exit(EXIT_FAILURE);
        }

        if (prepare_client() != -1) {
                command_line();
                client_disconnect();
                client_exit(EXIT_SUCCESS);
        } else {
                client_exit(EXIT_FAILURE);
        }

        return 0;
}
