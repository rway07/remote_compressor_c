#include "serverSideConfiguration.h"

// Initialize the global configuration
int init_global_configuration()
{
        int err_flag = 0;

        globalCfg = (struct globalConfiguration*) malloc(sizeof (struct globalConfiguration));
        if (globalCfg != NULL) {
                globalCfg->server_socket = 0;
                globalCfg->port = 0;
                globalCfg->base_directory = NULL;

                globalCfg->compressor_enabled[GNUZIP] = 0;
                globalCfg->compressor_enabled[BZIP2] = 0;
                globalCfg->compressor_enabled[ZIP] = 0;
                globalCfg->compressor_enabled[XZ] = 0;

                // Check every malloc, DO IT
                globalCfg->compressor_name[GNUZIP] = (char*) malloc((strlen(COMPRESSOR_GNUZIP) + 1) * sizeof (char));
                strcpy(globalCfg->compressor_name[GNUZIP], COMPRESSOR_GNUZIP);

                globalCfg->compressor_name[BZIP2] = (char*) malloc((strlen(COMPRESSOR_BZIP2) + 1) * sizeof (char));
                strcpy(globalCfg->compressor_name[BZIP2], COMPRESSOR_BZIP2);

                globalCfg->compressor_name[ZIP] = (char*) malloc((strlen(COMPRESSOR_ZIP) + 1) * sizeof (char));
                strcpy(globalCfg->compressor_name[ZIP], COMPRESSOR_ZIP);

                globalCfg->compressor_name[XZ] = (char*) malloc((strlen(COMPRESSOR_XZ) + 1) * sizeof (char));
                strcpy(globalCfg->compressor_name[XZ], COMPRESSOR_XZ);

                globalCfg->compressor_ext[GNUZIP] = (char*) malloc((strlen(GNUZIP_EXT) + 1) * sizeof (char));
                strcpy(globalCfg->compressor_ext[GNUZIP], GNUZIP_EXT);
                globalCfg->compressor_ext[BZIP2] = (char*) malloc((strlen(BZIP2_EXT) + 1) * sizeof (char));
                strcpy(globalCfg->compressor_ext[BZIP2], BZIP2_EXT);
                globalCfg->compressor_ext[ZIP] = (char*) malloc((strlen(ZIP_EXT) + 1) * sizeof (char));
                strcpy(globalCfg->compressor_ext[ZIP], ZIP_EXT);
                globalCfg->compressor_ext[XZ] = (char*) malloc((strlen(XZ_EXT) + 1) * sizeof (char));
                strcpy(globalCfg->compressor_ext[XZ], XZ_EXT);
        } else {
                err_flag = 1;
        }

        return err_flag;
}

// Initialize the thread-specific configuration for each thread
int init_threads_conf()
{
        //threads = (struct thread_info **)malloc(MAX_CLIENTS*sizeof(struct thread_info *));
        threads = calloc(MAX_CLIENTS, sizeof(struct thread_info *));
        if (threads == NULL){
                printf("ERRORE: Errore nell'inizializzazione dei dati per i threads!\n");
                return -1;
        }

        running_threads = 0;

        return 0;
}

// Mutex initialiation
int init_mutex()
{
        int ret = 0;

        ret = pthread_mutex_init(&global_mutex, NULL);
        if (ret != 0) {
                printf("ERRORE: Errore nell'inizializzazione del mutex!\n");
                return -1;
        }

        ret = pthread_mutex_init(&conf_mutex, NULL);
        if (ret != 0) {
                printf("ERRORE: Errore nell'inizializzazione del mutex!\n");
                return -1;
        }

        ret = pthread_mutex_init(&io_mutex, NULL);
        if (ret != 0) {
                printf("ERRORE: Errore nell'inizializzazione del mutex!\n");
                return -1;
        }

        return 0;
}

// Destroy all mutex
int destroy_mutex()
{
        int ret = 0;

        // Before destroying this mutex, i must be sure that there is no thread running
        // temp solution
        /*
	do {
		printf("DEBUG: Checking if there is a thread blocking configuration mutex...\n");
		ret = pthread_mutex_trylock(&conf_mutex);
	        if (ret == EBUSY) {
			printf("DEBUG: There is a thread locking the mutex, waiting for 3 seconds...\n");
			printf("DEBUG: If the problem persist, please kill me!!\n");
			sleep(3);
		}
        } while (ret != 0);
        pthread_mutex_unlock(&conf_mutex);
	*/
        ret = pthread_mutex_destroy(&conf_mutex);
        if (ret != 0) {
                printf("ERRORE: Errore nella distruzione del mutex!\n");
                return -1;
        }
        ret = pthread_mutex_destroy(&io_mutex);
        if (ret != 0) {
                printf("ERRORE: Errore nella distruzione del mutex!\n");
                return -1;
        }

        ret = pthread_mutex_destroy(&global_mutex);
        if (ret != 0) {
                printf("ERRORE: Errore nella distruzione del mutex!\n");
                return -1;
        }

        return 0;
}

// Initialize the client configuration for one specific thread
void* init_client_conf(int client_ID, int socket, char* address, char* base_directory)
{
        int len = 0;
	char* num = NULL;
	char* archives_s = NULL;
	char* files_s = NULL;
        struct serverSideConfiguration *cfg;

	cfg = (struct serverSideConfiguration*)malloc(sizeof(struct serverSideConfiguration));
        if (cfg != NULL) {
		cfg->client_ID = client_ID;
	        cfg->client_socket = socket;
	        cfg->client_address = (char*) malloc((strlen(address) + 1) * sizeof (char));
        	strcpy(cfg->client_address, address);
	        cfg->archive_name = (char*) malloc((strlen(DEFAULT_ARCHIVE_NAME) + 1) * sizeof (char));
	        strcpy(cfg->archive_name, DEFAULT_ARCHIVE_NAME);

        	num = malloc(INT_DIGITS_NUM);
	        snprintf(num, INT_DIGITS_NUM, "%d", client_ID);

		archives_s = (char*)malloc((strlen(SERVER_ARCHIVES_FOLDER) +2)*sizeof(char));
		strcpy(archives_s, SERVER_ARCHIVES_FOLDER);
        	len = strlen(base_directory) + strlen(archives_s) + strlen(num) +2;
	        cfg->archive_directory = (char*) malloc((len + 1) * sizeof (char));
	        strcpy(cfg->archive_directory, base_directory);

		files_s = (char*)malloc((strlen(SERVER_FILES_FOLDER) +2)*sizeof(char));
		strcpy(files_s, SERVER_FILES_FOLDER);
	        len = strlen(base_directory) + strlen(files_s) + strlen(num) +2;
	        cfg->files_directory = (char*) malloc((len + 1) * sizeof (char));
	        strcpy(cfg->files_directory, base_directory);

        	strcat(archives_s, num);
	        strcat(files_s, num);
	        strcat(cfg->archive_directory, archives_s);
	        strcat(cfg->files_directory, files_s);
	        free(num);
	        free(archives_s);
	        free(files_s);

	        cfg->compressor_index = 0;
        	cfg->dirty = 0;
	        cfg->sent_files = 0;
	        cfg->sent_files_at_compress = 0;
	}

        return cfg;
}

// Destroy all client data
int destroy_client(int client_ID)
{
        //printf("DEBUG: cleaning client data...\n");
        destroy_working_directory(client_ID);
        free_client_ID(client_ID);

        return 0;
}

// Destroy a serverSideConfiguration struct referenced by the pointer
int destroy_ssc(struct serverSideConfiguration *cfg)
{
        if (cfg != NULL) {
                free(cfg->archive_directory);
                free(cfg->archive_name);
                free(cfg->client_address);
                free(cfg->files_directory);
                free(cfg);
        }

        return 0;
}

// Destroy all thread data
int destroy_single_thread_conf(int id)
{
        int ret = 0;

        pthread_mutex_lock(&conf_mutex);
        if (threads[id] != NULL) {
                destroy_ssc(threads[id]->clientCfg);
                free(threads[id]);
                threads[id] = NULL;
        } else {
                ret = -1;
        }
        pthread_mutex_unlock(&conf_mutex);

        return ret;
}

// Destroy all threads data
int destroy_all_thread_conf()
{
        int i = 0;
        int ret = 0;

        pthread_mutex_lock(&global_mutex);
        //printf("DEBUG: Cleaning threads configuration...\n");

        for (i = 0; i < MAX_CLIENTS; i++) {
                pthread_mutex_lock(&conf_mutex);
                if (threads[i] != NULL) {
                        ret = 0;
                } else {
                        ret = -1;
                }
                pthread_mutex_unlock(&conf_mutex);

                if (ret == 0) {
                        destroy_client(i);
                }
        }
        free(threads);
        pthread_mutex_unlock(&global_mutex);

        return 0;
}

// Destroy the global configuration
int destroy_global_configuration()
{
        int i = 0;

        for (i = 0; i < SUPPORTED_COMPRESSORS; i++) {
                free(globalCfg->compressor_name[i]);
                free(globalCfg->compressor_ext[i]);
        }

        if (globalCfg->base_directory != NULL) {
                free(globalCfg->base_directory);
        }
        free(globalCfg);
        //printf("DEBUG: destroy_global_configuration() done.\n");

        return 0;
}

// Check which compressors support the current system
int check_compressors()
{
        int ret = 0;
	char* path = NULL;
	struct stat buf;

        path = (char*)malloc((strlen(TAR_PATH) + 1) * sizeof(char));
        strcpy(path, TAR_PATH);
        ret = stat(path, &buf);
        free(path);
        if (ret == -1) {
                printf("ERRORE: tar è necessario per il funzionamento del programma! Uscita forzata!!\n");
                return -1;
        }
        globalCfg->compressor_enabled[GNUZIP] = 1;
        globalCfg->compressor_enabled[BZIP2] = 1;

        path = (char*)malloc((strlen(ZIP_PATH) + 1) * sizeof(char));
        strcpy(path, ZIP_PATH);
        ret = stat(path, &buf);
        free(path);
        if (ret == -1) {
                printf("WARNING: zip non disponibile.\n");
                globalCfg->compressor_enabled[ZIP] = 0;
        }
        else {
                globalCfg->compressor_enabled[ZIP] = 1;
        }

        path = (char*)malloc((strlen(XZ_CHECK_COMMAND) + 1) * sizeof(char));
        strcpy(path, XZ_CHECK_COMMAND);
        ret = system(path);
        free(path);
        if (ret != 0) {
                printf("WARNING: xz non disponibile.\n");
                globalCfg->compressor_enabled[XZ] = 0;
        }
        else {
                globalCfg->compressor_enabled[XZ] = 1;
        }

        return 0;
}

// Return the socket number for the client number id
int get_socket(int id)
{
        int socket = 0;

        pthread_mutex_lock(&conf_mutex);
        socket = threads[id]->clientCfg->client_socket;
        pthread_mutex_unlock(&conf_mutex);

        return socket;
}

// Return the client address associated to the client ID (id)
char* get_client_address(int id)
{
        char* address = NULL;

        pthread_mutex_lock(&conf_mutex);
        address = (char*)malloc((strlen(threads[id]->clientCfg->client_address) + 1) * sizeof(char));
        strcpy(address, threads[id]->clientCfg->client_address);
        pthread_mutex_unlock(&conf_mutex);

        return address;
}

// Set the compressor index for a specific client
int set_compressor_index(int id, int index)
{
        pthread_mutex_lock(&conf_mutex);
        threads[id]->clientCfg->compressor_index = index;
        pthread_mutex_unlock(&conf_mutex);

        return 0;
}

// Return the compressor index associated to a specific client
int get_compressor_index(int id)
{
        int index = 0;

        pthread_mutex_lock(&conf_mutex);
        index = threads[id]->clientCfg->compressor_index;
        pthread_mutex_unlock(&conf_mutex);

        return index;
}

// Set the archive name for a specific client
int set_archive_name(int id, char* name)
{
        pthread_mutex_lock(&conf_mutex);
        free(threads[id]->clientCfg->archive_name);
        threads[id]->clientCfg->archive_name = (char*)malloc((strlen(name) + 1) * sizeof(char));
        strcpy(threads[id]->clientCfg->archive_name, name);
        threads[id]->clientCfg->dirty = 1;
        pthread_mutex_unlock(&conf_mutex);

        return 0;
}

// Return the archive name for a specific client
char* get_archive_name(int id)
{
        char* name = NULL;

        pthread_mutex_lock(&conf_mutex);
        name = (char*)malloc((strlen(threads[id]->clientCfg->archive_name) + 1) * sizeof(char));
        strcpy(name, threads[id]->clientCfg->archive_name);
        pthread_mutex_unlock(&conf_mutex);

        return name;
}

// Return the directory where server saves archives for the id-st client
char* get_archive_dir(int id)
{
        char* archives_dir;

        pthread_mutex_lock(&conf_mutex);
        archives_dir = (char*)malloc((strlen(threads[id]->clientCfg->archive_directory) + 1) * sizeof(char));
        strcpy(archives_dir, threads[id]->clientCfg->archive_directory);
        pthread_mutex_unlock(&conf_mutex);

        return archives_dir;
}

// Return the directory where server saves files for the id-st client
char* get_files_dir(int id)
{
        char* files_dir = NULL;

        pthread_mutex_lock(&conf_mutex);
        files_dir = (char*)malloc((strlen(threads[id]->clientCfg->files_directory) + 1) * sizeof(char));
        strcpy(files_dir, threads[id]->clientCfg->files_directory);
        pthread_mutex_unlock(&conf_mutex);

        return files_dir;
}


char* get_complete_file_path(int id, char* file_name)
{
        int len = 0;
        char* buffer = NULL;
        char* path = NULL;
        
        buffer = get_files_dir(id);
        len = strlen(buffer) + strlen(file_name) + 1;
        path = (char*)malloc((len + 1) * sizeof(char));
        strcpy(path, buffer);
        strcat(path, "/");
        strcat(path, file_name);
        free(buffer);

        return path;
}

