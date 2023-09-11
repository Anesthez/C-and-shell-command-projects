#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main(){
	char phone[11];
	int place;
	scanf("%10s", phone);
	while(scanf("%d", &place) != -1){
		if(place == -1){
			printf("%s\n", phone);
		}else if(place <= 9){
			printf("%c\n", phone[place]);

		}else{
			printf("ERROR\n");
			return 1;
		}
	
	}
	return 0;
}
