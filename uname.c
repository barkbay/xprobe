#include <stdio.h>
	 
void get_machine_id(char * buffin) {
    FILE *in;
    extern FILE *popen();
    char buff[16];
    char * buff2;
    buff2 = (char * )malloc(16);
 
    if(!(in = popen("uname -F", "r"))){
        printf("Impossible de determiner le nom de la machine avec uname -F");
        exit(1);
    }
 
    if (fgets(buff, sizeof(buff), in)==NULL){
        printf("uname -F retourne NULL");
        exit(1);
    }
    pclose(in);
    strcpy(buffin, buff);
}
