CC = gcc
CFLAGS = -Wpedantic -Wextra -Werror
LDLIBS = -lm -lyaml

BINARY = ezPass
OBJS = yaml_parser.o storage.o args.o mem.o utils.o main.o tree.o

ifdef LIBSODIUM
  CFLAGS += -D__libsodium__
  LDLIBS += -lsodium
endif

ifdef DEBUG
  CFLAGS += -ggdb -D__debug__
endif

$(BINARY): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(BINARY) $(LDLIBS)

tree.o : tree.c tree.h
	$(CC) $(CFLAGS) -c tree.c 
yaml_parser.o : yaml_parser.c yaml_parser.h tree.h
	$(CC) $(CFLAGS) -c yaml_parser.c 
storage.o : storage.c storage.h
	$(CC) $(CFLAGS) -c storage.c
args.o : args.c args.h main.h
	$(CC) $(CFLAGS) -c args.c
mem.o : mem.c mem.h main.h
	$(CC) $(CFLAGS) -c mem.c
utils.o : utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c
main.o : main.c main.h utils.h
	$(CC) $(CFLAGS) -c main.c

.PHONY: clean_objects clean
clean_objects:
	$(RM) $(OBJS)

clean: clean_objects
	$(RM) $(BINARY)
