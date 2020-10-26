#include "compressor.h"

char* compress(int id)
{
        int index = 0;
        int ret = 0;
        int len = 0;
        char* cmd = NULL;
        char* file_name = NULL;
        char* archive_dir = NULL;
        char* files_dir = NULL;
        char* buffer = NULL;
        
        index = get_compressor_index(id);
        file_name = get_archive_name(id);
        archive_dir = get_archive_dir(id);
        files_dir = get_files_dir(id);
        
        switch (index) {
        	case GNUZIP:
                	len = strlen(file_name) + strlen(GNUZIP_EXT);
	                buffer = (char*) malloc((len + 1) * sizeof (char));
	                strcpy(buffer, file_name);
	                strcat(buffer, GNUZIP_EXT);
	                len = strlen(GNUZIP_COMMAND) + strlen(archive_dir) + strlen(buffer) + strlen(TAR_PARAM_1)
        	                + strlen(files_dir) + strlen(TAR_PARAM_2) + 2;
                	cmd = (char*) malloc((len + 1) * sizeof (char));
	                strcpy(cmd, GNUZIP_COMMAND);
	                strcat(cmd, archive_dir);
	                strcat(cmd, "/");
	                strcat(cmd, buffer);
	                strcat(cmd, TAR_PARAM_1);
	                strcat(cmd, files_dir);
	                strcat(cmd, TAR_PARAM_2);
	                break;
	        case BZIP2:
        	        len = strlen(file_name) + strlen(BZIP2_EXT);
	                buffer = (char*) malloc((len + 1) * sizeof (char));
        	        strcpy(buffer, file_name);
	                strcat(buffer, BZIP2_EXT);
	                len = strlen(BZIP2_COMMAND) + strlen(archive_dir) + strlen(buffer) + strlen(TAR_PARAM_1)
        	                + strlen(files_dir) + strlen(TAR_PARAM_2) + 2;
	                cmd = (char*) malloc((len + 1) * sizeof (char));
	                strcpy(cmd, BZIP2_COMMAND);
        	        strcat(cmd, archive_dir);
	                strcat(cmd, "/");
	                strcat(cmd, buffer);
	                strcat(cmd, TAR_PARAM_1);
	                strcat(cmd, files_dir);
        	        strcat(cmd, TAR_PARAM_2);
	                break;
        	case ZIP:
	                len = strlen(file_name) + strlen(ZIP_EXT);
        	        buffer = (char*) malloc((len + 1) * sizeof (char));
	                strcpy(buffer, file_name);
        	        strcat(buffer, ZIP_EXT);
	                len = strlen(ZIP_COMMAND) + strlen(archive_dir) + strlen(buffer) + strlen(files_dir) + 2;
	                cmd = (char*) malloc((len + 1) * sizeof (char));
	                strcpy(cmd, ZIP_COMMAND);
	                strcat(cmd, archive_dir);
	                strcat(cmd, "/");
	                strcat(cmd, buffer);
	                strcat(cmd, " ");
	                strcat(cmd, files_dir);
	                break;
	        case XZ:
        	        len = strlen(file_name) + strlen(XZ_EXT);
	                buffer = (char*) malloc((len + 1) * sizeof (char));
	                strcpy(buffer, file_name);
	                strcat(buffer, XZ_EXT);
	                len = strlen(XZ_COMMAND) + strlen(archive_dir) + strlen(buffer) + strlen(TAR_PARAM_1)
	                        + strlen(files_dir) + strlen(TAR_PARAM_2) + 2;
        	        cmd = (char*) malloc((len + 1) * sizeof (char));
                	strcpy(cmd, XZ_COMMAND);
	                strcat(cmd, archive_dir);
	                strcat(cmd, "/");
        	        strcat(cmd, buffer);
	                strcat(cmd, TAR_PARAM_1);
	                strcat(cmd, files_dir);
	                strcat(cmd, TAR_PARAM_2);
        	        break;
	        default:
        	        break;
	}

        //printf("DEBUG: compress command line\n%s\n", cmd);
        ret = system(cmd);
        free(cmd);

        if (ret != 0) {
                free(buffer);
                free(archive_dir);
                free(files_dir);
                free(file_name);
                return NULL;
        }

        len = strlen(archive_dir) + strlen(buffer) + 1;
        free(file_name);
        file_name = (char*) malloc((len + 1) * sizeof (char));
        strcpy(file_name, archive_dir);
        strcat(file_name, "/");
        strcat(file_name, buffer);

        free(buffer);
        free(archive_dir);
        free(files_dir);

        return file_name;
}
