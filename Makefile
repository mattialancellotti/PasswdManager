INCLUDE = include
BINARY = ezpass

OBJSDIR = obj
BINDIR  = bin
SRCDIR  = src
STOWDIR = /usr/local/stow/ezpass

VPATH = src:include
OBJS := $(addprefix $(OBJSDIR)/, args.o main.o gen.o)
DEPS := $(patsubst %.c, %.d, $(SRCS))

CC = clang
RM = rm -rf
CFLAGS = -Wpedantic -Wextra -Werror -std=c11 -I$(INCLUDE)

ifdef EXPERIMENTAL
  OBJS += $(addprefix $(OBJSDIR)/, term.o os.o crypto.o pwman.o stats.o)
  CFLAGS += -D_IS_EXPERIMENTAL
endif

ifdef SODIUM
  CFLAGS += -D_HAVE_SODIUM
  LDLIBS += -lsodium
endif

ifdef DEBUG
  CFLAGS += -ggdb -D_HAVE_DEBUG -fsanitize=address,undefined
endif


# Actually linking everything into a single binary
$(BINARY): | $(OBJSDIR) $(OBJS) $(BINDIR)
	$(CC) $(CFLAGS) $(OBJS) -o $(BINDIR)/$(BINARY) $(LDLIBS)

$(BINDIR):
	@echo Creating $(abspath $(BINDIR)).
	@mkdir -p $(BINDIR)

$(OBJSDIR):
	@echo Creating $(abspath $(OBJSDIR)).
	@mkdir -p $(OBJSDIR)

$(SRCDIR)/%.d: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -MM -MT '$(patsubst %.c, $(OBJSDIR)/%.o, $(notdir $<))' $< -MF $@


$(OBJSDIR)/%.o: $(SRCDIR)/%.c $(SRCDIR)/%.d $(INCLUDE)/pass/%.h
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean stow
clean:
	$(RM) $(BINDIR) $(OBJSDIR)

stow:
	mkdir -p $(STOWDIR)
	cp -r $(BINDIR) $(STOWDIR)
	stow --dir=/usr/local/stow --target=/usr/local --stow $(BINARY)
	@echo Successfully stowed $(STOWDIR).

docker:
	docker build -t ezpass-app .
	docker run -it ezpass-app
