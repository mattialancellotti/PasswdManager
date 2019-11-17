objects = main.o utils.o mem.o args.o json.o

PasswdGen : $(objects)
	cc -lm -o PasswdGen $(objects)

args.o : args.c mem.h args.h utils.h
mem.o : mem.c mem.h main.h
json.o : json.c json.h
utils.o : utils.c utils.h
	cc -c utils.c
main.o : main.c main.h

.PHONY : clean
clean :
	rm $(objects)
