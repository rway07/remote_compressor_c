#include "configuration.h"

// Initialize global client configuration
void init_configuration()
{
	cfg = (struct configuration*)malloc(sizeof(struct configuration));
	if (cfg != NULL) {
                cfg->archive_name = (char*)malloc(sizeof(DEFAULT_ARCHIVE_NAME));
                if (cfg->archive_name != NULL) {
                        strcpy(cfg->archive_name, DEFAULT_ARCHIVE_NAME);
                }
                cfg->compressor_index = 0;
                cfg->sent_files = 0;
                cfg->sent_files_at_compress = 0;
                cfg->client_ID = -1;
                cfg->dirty = 0;
                cfg->compressor_enabled[GNUZIP] = 1;
                cfg->compressor_enabled[BZIP2] = 1;
                cfg->compressor_enabled[ZIP] = 0;
                cfg->compressor_enabled[XZ] = 0;

                cfg->compressor_name[GNUZIP] = (char*)malloc((strlen(COMPRESSOR_GNUZIP)+1)*sizeof(char));
                if (cfg->compressor_name[GNUZIP] != NULL) {
                        strcpy(cfg->compressor_name[GNUZIP], COMPRESSOR_GNUZIP);
                }
                cfg->compressor_name[BZIP2] = (char*)malloc((strlen(COMPRESSOR_BZIP2)+1)*sizeof(char));
                if (cfg->compressor_name[BZIP2] != NULL) {
                        strcpy(cfg->compressor_name[BZIP2], COMPRESSOR_BZIP2);
                }
                cfg->compressor_name[ZIP] = (char*)malloc((strlen(COMPRESSOR_ZIP)+1)*sizeof(char));
                if (cfg->compressor_name[ZIP] != NULL) {
                        strcpy(cfg->compressor_name[ZIP], COMPRESSOR_ZIP);
                }
                cfg->compressor_name[XZ] = (char*)malloc((strlen(COMPRESSOR_XZ)+1)*sizeof(char));
                if (cfg->compressor_name[XZ] != NULL) {
                        strcpy(cfg->compressor_name[XZ], COMPRESSOR_XZ);
                }
                cfg->compressor_ext[GNUZIP] = (char*)malloc((strlen(GNUZIP_EXT)+1)*sizeof(char));
                if (cfg->compressor_ext[GNUZIP] != NULL) {
                        strcpy(cfg->compressor_ext[GNUZIP], GNUZIP_EXT);
                }
                cfg->compressor_ext[BZIP2] = (char*)malloc((strlen(BZIP2_EXT)+1)*sizeof(char));
                if (cfg->compressor_ext[BZIP2] != NULL) {
                        strcpy(cfg->compressor_ext[BZIP2], BZIP2_EXT);
                }
                cfg->compressor_ext[ZIP] = (char*)malloc((strlen(ZIP_EXT)+1)*sizeof(char));
                if (cfg->compressor_ext[ZIP] != NULL) {
                        strcpy(cfg->compressor_ext[ZIP], ZIP_EXT);
                }
                cfg->compressor_ext[XZ] = (char*)malloc((strlen(XZ_EXT)+1)*sizeof(char));
                if (cfg->compressor_ext[XZ] != NULL) {
                        strcpy(cfg->compressor_ext[XZ], XZ_EXT);
                }
        }
}

// Destroy global client configuration
void destroy_configuration()
{
    	int i = 0;

        if (cfg != NULL) {
        	free(cfg->archive_name);
        	for (i = 0; i < SUPPORTED_COMPRESSORS; i++) {
            		free(cfg->compressor_name[i]);
            		free(cfg->compressor_ext[i]);
        	}
        	free(cfg);
    	}

}

// Return current archvive name
char* get_archive_name()
{
    	char *tmp = NULL;

	tmp = (char*)malloc((strlen(cfg->archive_name)+1)*sizeof(char));
    	if (tmp != NULL) {
		strcpy(tmp, cfg->archive_name);
	}

    	return tmp;
}

// Return the complete path of archive including file name
char* get_complete_archive_path(const char* dest)
{
        int len = 0;
        char* buffer = NULL;
        char* tmp = NULL;
        char* folder = NULL;
        char* path = NULL;

        folder = get_recv_folder(dest);
	tmp = get_archive_name();

        len = strlen(cfg->compressor_ext[cfg->compressor_index]);
        buffer = (char*)malloc((len + 1) * sizeof(char));
        strcpy(buffer, cfg->compressor_ext[cfg->compressor_index]);
        len = strlen(folder) + strlen(tmp) + strlen(buffer) + 1;
        path = (char*)malloc((len + 1) * sizeof(char));
        strcpy(path, folder);
        strcat(path, "/");
        strcat(path, tmp);
        strcat(path, buffer);
        free(folder);
        free(buffer);
        free(tmp);

        return path;
}

// Set current archive name
void set_archive_name(const char* name) {
        if ((name != NULL) && (cfg != NULL)) {
                free(cfg->archive_name);
                cfg->archive_name = (char*)malloc((strlen(name)+1)*sizeof(char));
                if (cfg->archive_name != NULL) {
                        strcpy(cfg->archive_name, name);
                }
                cfg->dirty = 1;
        }
}

// Set current compressor index
void set_compressor_index(int index) {
    cfg->compressor_index = index;
    cfg->dirty = 1;
}

// Check if the  client support a particular compressor
// NOTE: It is the server that communicate to the client which compressor enable/disable
int check_compressor_arg(const char* compressor)
{
    	if (strcmp(compressor, COMPRESSOR_GNUZIP) == 0)
    	{
        	return GNUZIP;
    	}
    	else if (strcmp(compressor, COMPRESSOR_BZIP2) == 0)
    	{
        	return BZIP2;
    	}
    	else if ((strcmp(compressor, COMPRESSOR_ZIP) == 0) && (cfg->compressor_enabled[ZIP] == 1))
    	{
        	return ZIP;
    	}
    	else if ((strcmp(compressor, COMPRESSOR_XZ) == 0) && (cfg->compressor_enabled[XZ] == 1))
    	{
        	return XZ;
    	}

    	return -1;
}

// Return the folder where the client save received archives
char* get_recv_folder(const char* folder) {
    	//unsigned int len = 0;
    	size_t len = 0;
        char* num = NULL;
    	char* path = NULL;

    	num = malloc(INT_DIGITS_NUM);
    	snprintf(num, INT_DIGITS_NUM, "%d", cfg->client_ID);

    	len = strlen(folder) + strlen(CLIENT_RECV_FOLDER) + strlen(num);
    	path = (char*)malloc((len +1)*sizeof(char));
    	if (path != NULL) {
                strcpy(path, folder);
                strcat(path, CLIENT_RECV_FOLDER);
                strcat(path, num);
        }
    	free(num);

    	return path;
}
