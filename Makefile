objects = main.o utils.o mem.o args.o json.o
args = -Wall -Wpedantic -Wextra -Werror -ggdb

PasswdGen : $(objects)
	cc -lm -o PasswdGen $(objects)

args.o : args.c mem.h args.h utils.h
	cc $(args) -c args.c
mem.o : mem.c mem.h main.h
	cc $(args) -c mem.c
json.o : json.c json.h
	cc $(args) -c json.c
utils.o : utils.c utils.h
	cc $(args) -c utils.c
main.o : main.c main.h mem.h args.h utils.h json.h
	cc $(args) -c main.c

.PHONY : clean
clean :
	rm $(objects)
