#include "network.h"

int get_network_error(int err)
{
        switch (err) {
        case ECONNRESET:
        case ENOTCONN:
                //printf("DEBUG: for now only NOT CONNECTED\n");
                return NOT_CONNECTED;
                break;
        default:
                return -1;
                break;
        }

        return -1;
}

int get_rcv_buffer_size(int sock)
{
        int rcvbuf_size = 0;
        unsigned int buf_size = 0;

        buf_size = sizeof(rcvbuf_size);
        getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void*)&rcvbuf_size, &buf_size);

        return rcvbuf_size;
}

int get_snd_buffer_size(int sock)
{
        int sndbuf_size = 0;
        unsigned int buf_size = 0;

        buf_size = sizeof(sndbuf_size);
        getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void*)&sndbuf_size, &buf_size);

        return sndbuf_size;
}

int send_int(int socket, int num)
{
        int ret = 0;
        int c_num = htonl(num);

        //ret = send(socket, &c_num, sizeof (c_num), MSG_WAITALL);
        ret = send(socket, &c_num, sizeof(c_num), MSG_NOSIGNAL);
        if (ret == -1) {
                printf("ERRORE: Errore nell'invio dei dati!!\n");
                return get_network_error(errno);
        }

        return 0;
}

int send_response(int socket, int response)
{
        int ret = 0;
        char rsp[NET_RESPONSE_LEN +1];

        switch (response) {
                case ERROR:
                        strcpy(rsp, NET_RESPONSE_ERROR);
                        break;
                case OK:
                        strcpy(rsp, NET_RESPONSE_OK);
                        break;
                case BYE:
                        strcpy(rsp, NET_RESPONSE_BYE);
                        break;
                case CONTINUE:
                        strcpy(rsp, NET_RESPONSE_CONTINUE);
                        break;
                default:
                        strcpy(rsp, "wtf");
                        break;
        }

        ret = send(socket, (void*)rsp, NET_RESPONSE_LEN, MSG_NOSIGNAL);
        if (ret == -1) {
                printf("ERRORE: Errore nell'invio della risposta!!\n");
                return get_network_error(errno);
        }

        return 0;
}

int send_command(int socket, const char* command)
{
        int ret = 0;

        ret = send(socket, (void*) command, NET_CMD_LEN, MSG_NOSIGNAL);
        if (ret == -1) {
                printf("ERRORE: Errore nell'invio del comando!!\n");
                return get_network_error(errno);
        }

        return 0;
}

int send_data(int socket, const char* buffer, int len)
{
	int ret = 0;
	int total = 0;
	int bytes_left = len;

	while (total < len) {
		ret = send(socket, (void*)buffer, len, MSG_NOSIGNAL);
		if (ret == -1) {
			printf("ERRORE: Errore nell'invio dei dati!!\n");
			return get_network_error(errno);
		}
		total += ret;
		bytes_left -= ret;
	}

	return 0;
}

int receive_int(int socket, int* num)
{
        int ret = 0;
        int r_num = 0;

        //ret = recv(socket, (void*)&len, sizeof(len), MSG_WAITALL);
        ret = recv(socket, &r_num, sizeof (r_num), MSG_WAITALL);
        if (ret <= 0) {
                printf("ERRORE: Errore nella ricezione dei dati!!\n");
                return ret;
        }
        *num = ntohl(r_num);

        return 0;
}

int receive_command(int socket, char* command)
{
        int ret = 0;

        ret = recv(socket, (void*) command, NET_CMD_LEN, MSG_WAITALL);
        command[NET_CMD_LEN] = '\0';

        switch (ret) {
        	case 0:
                	return NOT_CONNECTED;
	                break;
        	case -1:
                	printf("ERRORE: Errore nella ricezione del comando!!\n");
	                return -1;
        	        break;
	        default:
                	break;
        }

        return 0;
}

int receive_response(int socket)
{
        char response[NET_RESPONSE_LEN +1];
        int ret = 0;

        strcpy(response, "");
        ret = recv(socket, (void*) response, NET_RESPONSE_LEN, MSG_WAITALL);
        response[NET_RESPONSE_LEN] = '\0';

        if (ret == 0) {
                return NOT_CONNECTED;
        } else if (ret == -1) {
                printf("ERRORE: Errore nella ricezione della risposta\n");
                return ret;
        } else if (strcmp(response, NET_RESPONSE_ERROR) == 0) {
                printf("ERRORE: Errore nell'esecuzione del comando!!\n");
                return COMMAND_ERROR;
        } else if (strcmp(response, NET_RESPONSE_CONTINUE) == 0) {
                return COMMAND_CONTINUE;
        }

        return 0;
}

int receive_data(int socket, char* buffer, int len)
{
        int ret = 0;

        ret = recv(socket, (void*)buffer, len, MSG_WAITALL);
        buffer[len] = '\0';
        if (ret <= 0) {
                printf("ERRORE: Errore nella ricezione dei dati!!\n");
                return ret;
        }

        return 0;
}

int receive_file(int socket, char* buffer, int len)
{
        int ret = 0;

        ret = recv(socket, (void*)buffer, len, MSG_WAITALL);
        if (ret <= 0) {
                printf("ERRORE: Errore nella ricezione dei dati!!\n");
                return ret;
        }

        return 0;
}

int handle_rcv_error(int socket, int ret)
{
        perror("ERRORE");
        if (ret == 0) {
                return NOT_CONNECTED;
        } else {
                ret = send_response(socket, ERROR);
                return -1;
        }
}

int calculate_trunk_size(int snd_size, int rcv_size)
{
        if (snd_size > rcv_size) {
                return rcv_size;
        } else if (snd_size < rcv_size) {
                return snd_size;
        }

        return rcv_size;
}

int sender_negotiate_trunk_size(int sock)
{
        int ret = 0;
        int rcv_buff_size = 0;
        int snd_buff_size = 0;

        if ((ret = receive_int(sock, &rcv_buff_size)) != 0) {
                return handle_rcv_error(sock, ret);
        } else {
                if ((ret = send_response(sock, OK)) != 0) {
                        return ret;
                }
        }
        snd_buff_size = get_snd_buffer_size(sock);
        if ((ret = send_int(sock, snd_buff_size)) != 0) {
                return ret;
        }

        if ((ret = receive_response(sock)) != 0) {
                return ret;
        }

        return calculate_trunk_size(snd_buff_size, rcv_buff_size);
}

int receiver_negotiate_trunk_size(int sock)
{
        int ret = 0;
        int rcv_buff_size = 0;
        int snd_buff_size = 0;

        rcv_buff_size = get_rcv_buffer_size(sock);
        if ((ret = send_int(sock, rcv_buff_size)) != 0) {
                return ret;
        }

        if ((ret = receive_response(sock)) != 0) {
                return ret;
        }

        if ((ret = receive_int(sock, &snd_buff_size)) != 0) {
                return handle_rcv_error(sock, ret);
        } else {
                if ((ret = send_response(sock, OK)) != 0) {
                        return ret;
                }
        }

        return calculate_trunk_size(snd_buff_size, rcv_buff_size);
}
