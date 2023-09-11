#include <stdio.h>
#include <stdlib.h>


// TODO: Implement a helper named check_permissions that matches the prototype below.
int check_permissions(char * intended, char * actual){
	int perm = 0;
	for (int i = 0; i < 9; i++){
		if(intended[i] != actual[i] && intended[i] != '-'){
			perm = 1;
		}
	}
	return perm;
};


int main(int argc, char** argv) {
    if (!(argc == 2 || argc == 3)) {
        fprintf(stderr, "USAGE: count_large size [permissions]\n");
        return 1;
    }
        int size = strtol(argv[1], NULL, 10);
        int num = 0;
	char sdr[31];
	char sdr1[31];
	scanf("%31s", sdr);
	scanf("%31s", sdr);
	if(argc == 2){
		while(scanf("%31s", sdr1) != -1)	{
			for(int i = 0; i< 4; i++){
				scanf("%31s", sdr);
			}
			int size2 = strtol(sdr, NULL, 10);
			

			if (size2 > size && sdr1[0] != 'd'){
				num += 1;
			}
			for(int i = 0; i< 4; i++){
				scanf("%31s", sdr);
			}	
		}
	printf("%d\n", num);
	}else if(argc == 3){
		while(scanf("%31s", sdr1) != -1)	{
			for(int i = 0; i< 4; i++){
				scanf("%31s", sdr);
			}
			int size2 = strtol(sdr, NULL, 10);
			if (size2 > size && check_permissions(&sdr1[1], argv[2]) == 0 && sdr1[0] != 'd'){
					num += 1;
			}
			for(int i = 0; i< 4; i++){
				scanf("%31s", sdr);
			}	
	
			
		}
	printf("%d\n", num);
	}
    return 0;
}
