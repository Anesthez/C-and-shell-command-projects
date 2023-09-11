#include <stdio.h>
#include <stdlib.h>

int main(){
	char phone[11];
	int place;
	scanf("%10s%d", phone, &place);
	if(place == -1){
		printf("%s\n", phone);
	}else if(place < 10){
		printf("%c\n", phone[place]);

	}else{
		printf("ERROR\n");
	}
	return 0;
}
