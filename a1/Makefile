SHELL = /bin/bash
FLAGS = -Wall -Werror -std=c99
.PHONY: test_part1 test_part2 clean

count_large: count_large.o 
	gcc ${FLAGS} -o $@ $^

%.o: %.c 
	gcc ${FLAGS} -c $<

test_part1: count_large
	@test_cl_output=`./count_large 1000 rwx------ < handout.test`; \
	if [ ! -z $$test_cl_output ] && [ $$test_cl_output -eq 2 ]; then \
		echo Compiled and sanity check passed; \
	else \
		echo Failed sanity check; \
	fi 

validate_sin: validate_sin.o sin_helpers.o
	gcc ${FLAGS} -o $@ $^

test_part2: validate_sin
	@test_vs_output=`./validate_sin 810620716`; \
	if [ "$$test_vs_output" == "Valid SIN" ]; then \
		echo Compiled and sanity check passed; \
	else \
		echo Failed sanity check; \
	fi

clean:
	rm -f *.o validate_sin count_large
