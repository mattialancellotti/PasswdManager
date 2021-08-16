INCLUDE = include
BINARY = ezPass
OBJSDIR = bin
SRCDIR = src

VPATH = src:include
OBJS := $(addprefix \
       $(OBJSDIR)/, yaml_parser.o storage.o args.o main.o tree.o)
DEPS := $(patsubst %.c, %.d, $(SRCS))

CC = clang
RM = rm -rf
CFLAGS = -Wpedantic -Wextra -Werror -std=c11 -I$(INCLUDE)
LDLIBS = -lm -lyaml

ifdef EXPERIMENTAL
  OBJS += $(addprefix $(OBJSDIR)/, term.o os.o crypto.o)
  CFLAGS += -D__experimental__
endif

ifdef SODIUM
  CFLAGS += -D_HAVE_SODIUM
  LDLIBS += -lsodium
endif

ifdef DEBUG
  CFLAGS += -ggdb -D_HAVE_DEBUG -fsanitize=address,undefined
endif


# Actually linking everything into a single binary
$(BINARY): | $(OBJSDIR) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(BINARY) $(LDLIBS)

$(OBJSDIR):
	@echo Creating $(abspath $(OBJSDIR)).
	@mkdir -p $(OBJSDIR)

$(SRCDIR)/%.d: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -MM -MT '$(patsubst %.c, $(OBJSDIR)/%.o, $(notdir $<))' $< -MF $@


$(OBJSDIR)/%.o: $(SRCDIR)/%.c $(SRCDIR)/%.d $(INCLUDE)/pass/%.h
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	$(RM) $(BINARY) $(OBJSDIR)
