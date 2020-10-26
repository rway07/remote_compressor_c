#include "paramParser.h"

// Check if the IP address is valid
int check_ip(const char* address)
{
    struct sockaddr_in sa;
    int ret = 0;

    ret = inet_pton(AF_INET, address, &sa.sin_addr);
    if (ret == 0) {
        printf("ERRORE: Indirizzo IP non valido!\n");
        return -1;
    }

    return 0;
}

// Check if the port number is valid
int check_port(const char* port)
{
    	int tmp = 0;
    	int i = 0;
    	int p = 0;
	int len = 0;

    	len = strlen(port);
	for (i = 0; i < len; i++) {
        	tmp = (int)(port[i]) - '0';

	        if ((tmp < 0) || (tmp > 9)) {
        	    printf("ERRORE: Errore nel numero di porta!!\n");
	            return -1;
       		 }

	        p = p + (tmp * (int)(pow(10, (len - i - 1))));
    	}

    	if ((p < MIN_PORT) || (p > MAX_PORT)) {
        	printf("ERRORE: Errore nel numero di porta!!\n");
        	return -1;
    	}

    	return p;
}

// Check if the number of parameters is correct (client)
int c_check_params_num(int num)
{
    if (num == 1) {
        printf("USAGE: ./client [address] [port]\n");
        return -1;
    } else if (num != 3) {
        printf("ERRORE: Errore nei parametri passati al client!!\n");
        return -1;
    }

    return 0;
}

// Parse the params passed to the client
int c_parse_params(int argc, char** argv, struct params* p)
{
	int ret = 0;
	char* addr = NULL;

	ret = c_check_params_num(argc);

    	if (ret == 0){
		addr = (char *)malloc((strlen(argv[CLIENT_PARAM_HOST_POS])+1)*sizeof(char));
		strcpy(addr, argv[CLIENT_PARAM_HOST_POS]);

        	ret = check_ip(addr);
        	if (ret != 0) {
			return -1;
		}

	        ret = check_port(argv[CLIENT_PARAM_PORT_POS]);
        	if (ret == -1) {
			return -1;
		}

	        strcpy(p->address, addr);
        	p->port = ret;
        	free(addr);

    	    	return 0;
    	}

    	return -1;
}

// Check the number of parameters passed to the server
int s_check_params_num(int num)
{
    if (num == 1) {
        printf("USAGE: ./server [port]\n");
        return -1;
    } else if (num != 2) {
        printf("ERRORE: Errore nei parametri passati al server!!\n");
        return -1;
    }

    return 0;
}

// Parse the params passed to the server
int s_parse_params(int argc, char** argv)
{
    int ret = 0;

    ret = s_check_params_num(argc);

    if (ret == 0){
        ret = check_port(argv[SERVER_PARAM_PORT_POS]);
    }

    return ret;
}

