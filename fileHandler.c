#include "fileHandler.h"
#include "md5.h"

// Calculate MD5 of the file
unsigned char* calculate_MD5(const char* file_name)
{
        MD5_CTX state;
        int bytes = 0;
        unsigned char digest[MD5_DIGEST_LENGTH];
        unsigned char* hash = NULL;
        unsigned char* data = NULL;
        FILE *inFile = NULL;

        inFile = fopen(file_name, "rb");
        if (inFile == NULL) {
                printf("ERRORE: Impossibile aprire il file!\n");
                return NULL;
        }

        data = (unsigned char*)malloc((MD5_DATA)*sizeof(char));
        MD5_Init(&state);
        while ((bytes = fread(data, 1, MD5_DATA, inFile)) != 0) {
                MD5_Update(&state, data, bytes);
        }
        MD5_Final(digest, &state);
        if (fclose(inFile) != 0) {
		perror("ERRORE: Errore nella  chiusura del file");
	}

        hash = (unsigned char*)malloc(MD5_DIGEST_LENGTH + 1);
        memcpy(hash, digest, sizeof(digest));
        free(data);

        return hash;
}

// Print a MD5 hash
void print_MD5(unsigned char hash[MD5_DIGEST_LENGTH])
{
        int j = 0;
        for (j = 0; j < MD5_DIGEST_LENGTH; j++)
                printf("%02x", hash[j]);
        printf("\n");
}

// Print and compare two MD5 hashes
int compare_MD5(char caller[7], unsigned char* recv_hash, unsigned char* calc_hash)
{
        printf("%s: Hash MD5 ricevuto: ",caller);
        print_MD5(recv_hash);
        printf("%s: Hash MD5 corrente: ",caller);
        print_MD5(calc_hash);

        if (strncmp((const char*)calc_hash, (const char*)recv_hash, MD5_DIGEST_LENGTH) == 0) {
                printf("%s: MD5 corretto!!\n",caller);
        } else {
                free(calc_hash);
                free(recv_hash);
                return -1;
        }
        free(calc_hash);
        free(recv_hash);

        return 0;
}

// Show the error related to a file system operation
void show_file_error(int err)
{
        switch (err) {
                case EACCES:
                        printf("ERRORE: Permesso negato nell'operazione sul File System!\n");
                        break;
                case EIO:
                        printf("ERRORE: Errore nella lettura del file!!\n");
                        break;
                case ENAMETOOLONG:
                        printf("ERRORE: Il nome passato come parametro è troppo lungo!\n");
                        break;
                case ENOENT:
                        printf("ERRORE: Il path/file non esiste!\n");
                        break;
                case ENOTDIR:
                        printf("ERRORE: Il path non è una cartella!\n");
                        break;
                case EOVERFLOW:
                        printf("ERRORE: Overflow durante l'operazione!\n");
                        break;
                default:
                        printf("ERROR: Errore sconosciuto durante l'operazione sul File System!\n");
                        break;
        }
}

// Check if a file exist
int file_exist(const char* file)
{
        int ret = 0;
        char *path = NULL;
        struct stat buf;

        path = (char*)malloc((strlen(file) + 1) * sizeof(char));
        strcpy(path, file);

        pthread_mutex_lock(&io_mutex);
        ret = stat(path, &buf);
        pthread_mutex_unlock(&io_mutex);
        free(path);
        if (ret == -1) {
                perror("file_exist()");
        }

        return ret;
}

// Check if the path passed by parameter is a directory
int is_directory(const char* directory)
{
        int ret = 0;
        char *path = NULL;
        struct stat buf;

        path = (char*)malloc((strlen(directory) + 1) * sizeof(char));
        strcpy(path, directory);

        pthread_mutex_lock(&io_mutex);
        ret = stat(path, &buf);
        pthread_mutex_unlock(&io_mutex);
        free(path);
        if (ret == -1) {
                perror("ERRORE");
                return -1;
        }

        return S_ISDIR(buf.st_mode);
}

// Create a directory
int create_directory(const char* directory)
{
        int ret = 0;
        char *path = NULL;

        path = (char*) malloc((strlen(directory) + 1) * sizeof (char));
        strcpy(path, directory);

        pthread_mutex_lock(&io_mutex);
        ret = mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        pthread_mutex_unlock(&io_mutex);

        free(path);
        if (ret == -1) {
                switch (errno) {
                case EEXIST:
                        //printf("DEBUG: Directory già esistente.\n");
                        return -2;
                        break;
                default:
                        show_file_error(errno);
                        return -1;
                }
        }

        return ret;
}

// Delete a file
int delete_file(const char* file_name)
{
        int ret = 0;
        int len = 0;
        char *command = NULL;

        len = strlen(DELETE_COMMAND) + strlen(file_name);
        command = (char*) malloc((len + 1) * sizeof (char));
        strcpy(command, DELETE_COMMAND);
        strcat(command, file_name);

        pthread_mutex_lock(&io_mutex);
        ret = system(command);
        pthread_mutex_unlock(&io_mutex);
        free(command);
        if (ret == -1) {
                show_file_error(errno);
        }

        return ret;
}

// Delete a directory
int delete_directory(const char* directory)
{
        int ret = 0;
        char* command = NULL;

        command = (char*) malloc((strlen(DELETE_COMMAND) + strlen(directory) + 1) * sizeof (char));
        strcpy(command, DELETE_COMMAND);
        strcat(command, directory);

        pthread_mutex_lock(&io_mutex);
        ret = system(command);
        pthread_mutex_unlock(&io_mutex);
        free(command);

        if (ret == -1) {
                show_file_error(errno);
        }

        return ret;
}

// Get the size of a file
int get_file_len(const char* file)
{
        int ret = 0;
        char *path = NULL;
        struct stat buf;

        path = (char*) malloc((strlen(file) + 1) * sizeof (char));
        strcpy(path, file);

        pthread_mutex_lock(&io_mutex);
        ret = stat(path, &buf);
        pthread_mutex_unlock(&io_mutex);
        free(path);
        if (ret == -1) {
                show_file_error(errno);
                return -1;
        }

        return buf.st_size;
}

// Get the current working directory
char* get_working_dir()
{
        char* buffer = NULL;
        char* pnt = NULL;
        char* ret = NULL;

        buffer = (char*)malloc((PATH_MAX +1) * sizeof(char));
        pthread_mutex_lock(&io_mutex);
        pnt = getcwd(buffer, PATH_MAX);
        pthread_mutex_unlock(&io_mutex);
        if (pnt == NULL) {
                show_file_error(errno);
                free(buffer);
                return NULL;
        }

        ret = (char*)malloc((strlen(buffer) + 1) * sizeof(char));
        strcpy(ret, buffer);
        free(buffer);

        return ret;
}

// Check if application have the permissions for read/write the resource
int got_permissions(const char* directory)
{
        int ret = 0;
        char *path = NULL;
        struct stat buf;

        path = (char*) malloc((strlen(directory) + 1) * sizeof (char));
        strcpy(path, directory);

        pthread_mutex_lock(&io_mutex);
        ret = stat(path, &buf);
        pthread_mutex_unlock(&io_mutex);
        free(path);
        if (ret == -1) {
                if (errno == EACCES) {
                        perror("DEBUG");
                } else show_file_error(errno);
                return -1;
        }

        return 0;
}

int read_and_send_file(int socket, const char* file_name, unsigned int file_size)
{
        int trunk = 0;
        int current_trunk = 0;
        int err_flag = 0;
        unsigned int next = 0;
        unsigned int i = 0;
        char* buffer = NULL;
        FILE* file = NULL;

        if ((trunk = sender_negotiate_trunk_size(socket)) <= 0){
                return -1;
        }
        current_trunk = trunk;
        //printf("DEBUG: Negotiated trunk size: %d\n",current_trunk);
        file = fopen(file_name, "rb");
        if (file != NULL) {
                buffer = (char*)malloc(trunk);
                while ((i <= file_size) && (err_flag == 0)) {
                        i += trunk;
                        if (i > file_size) {
                                current_trunk = file_size - next;
                        }

                        fread(buffer, current_trunk, 1, file);
                        if (ferror(file) != 0) {
                                err_flag = -1;
                        }

                        if (send_data(socket, buffer, current_trunk) != 0) {
                                err_flag = -1;
                        }

                        next = i;
                        printf("|");
                        fflush(stdout);
                }
                printf("\n");
        } else {
                perror("DEBUG");
                fclose(file);
                free(buffer);
                return -1;
        }
        free(buffer);
        fclose(file);

        return err_flag;
}

int receive_and_write_file(int socket, const char* file_name, unsigned int file_size)
{
        unsigned int i = 0;
        unsigned int next = 0;
        int trunk = 0;
        int current_trunk = 0;
        int err_flag = 0;
        char* buffer = NULL;
        FILE* file = NULL;

        if ((trunk = receiver_negotiate_trunk_size(socket)) <= 0) {
                return -1;
        }
        current_trunk = trunk;
        //printf("DEBUG: Negotiated trunk size %d\n",current_trunk);
        file = fopen(file_name, "ab");
        if (file != NULL) {
                buffer = (char*) malloc(trunk);
                while ((i <= file_size) && (err_flag == 0)) {
                        i += trunk;
                        if (i > file_size) {
                                current_trunk = file_size - next;
                        }

                        if (receive_file(socket, buffer, current_trunk) != 0) {
                                err_flag = -1;
                        }

                        fwrite(buffer, current_trunk, 1, file);
                        if (ferror(file) != 0) {
                                err_flag = -1;
                        }

                        next = i;
                        printf("|");
                        fflush(stdout);
                }
                printf("\n");
        } else {
                free(buffer);
                fclose(file);
                return -1;
        }
        free(buffer);
        fclose(file);

        return err_flag;
}
