#include <stdio.h>
#include <string.h>

#define MAX_FILE_NAME_LEN 256
#define OTA_MAGIC_NUM (0x4D4D4F49)

int main(int argc,char **argv){
    char bin_old_file[MAX_FILE_NAME_LEN], bin_new_file[MAX_FILE_NAME_LEN], *tmp_name = "ota-";
    char readbuf = 0, tmpbuf;
    char len = 0;
    unsigned char module_id;
    unsigned int checksum = 0, magic = 0;
	int cnt = 0;
    FILE *F_O_BIN=NULL, *F_N_BIN=NULL;
    
    //printf ("argc=%d, module_id=%s file_name: %s\n", argc, argv[1], argv[2]);
    if(argc != 3) {
        printf("##########input parameters fail, please check##########\n");
        return;
    }
    F_O_BIN = fopen(argv[2],"r");
    if(!F_O_BIN) {
        printf("########## %s is not exist, please check##########\n", argv[1]);
        return;
    }
    
    module_id = atoi(argv[1]);
    strcpy(bin_old_file, argv[2]);
    strcpy(bin_new_file, tmp_name);
    strcpy(bin_new_file + strlen(tmp_name), bin_old_file);
    //printf ("bin_old_file: %s, bin_new_file: %s, module_id: %d\n", bin_old_file, bin_new_file, module_id);
    F_N_BIN = fopen(bin_new_file,"w");
    if(F_N_BIN) {
        fclose(F_N_BIN);
        remove(bin_new_file);
        F_N_BIN = fopen(bin_new_file,"w");
    }
    	
    while(!feof(F_O_BIN)) {
        len = fread(&readbuf, 1, 1, F_O_BIN);
        if(len == 1) {
            checksum = (0xff & readbuf) + checksum;
            fwrite(&readbuf, 1, 1, F_N_BIN);
        }
    }
    
    if(F_O_BIN != NULL) {
        fclose(F_O_BIN);
    }
    
    if(F_N_BIN != NULL) {
		magic = OTA_MAGIC_NUM;
        fwrite(&magic, sizeof(magic), 1, F_N_BIN);
        fwrite(&module_id, 1, 1, F_N_BIN);
        for(cnt = 0; cnt < sizeof(checksum); cnt++) {
            tmpbuf = (checksum >> (cnt * 8)) & 0xff;
            fwrite(&tmpbuf, 1, 1, F_N_BIN);
        }
        fclose(F_N_BIN);
    }
    return 0;
}
