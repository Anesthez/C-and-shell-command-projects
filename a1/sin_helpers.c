// TODO: Implement populate_array
/*
 * Convert a 9 digit int to a 9 element int array.
 */
int populate_array(int sin, int *sin_array) {
    if(sin > 999999999 || sin < 100000000){
	return 1;
	
    }
    for(int i = 0 ; i < 9; i++){
    	sin_array[8 - i] = sin % 10;
	sin = sin / 10;
    }
    return 0;
}

// TODO: Implement check_sin
/*
 * Return 0 if the given sin_array is a valid SIN, and 1 otherwise.
 */
int check_sin(int *sin_array) {
	int sum = 0;
        for(int i = 0; i < 9; i++){
		int num = sin_array[i] + sin_array[i] * (i % 2);
		sum += num % 10 + num / 10;		
	}
	if(sum % 10 == 0 && sin_array[0] != 0){
		return 0;
	}
        return 1;
}
