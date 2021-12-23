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
CFLAGS = -Wpedantic -Wextra -Werror -std=gnu17 -I$(INCLUDE)

# The EXPERIMENTAL flag is compiling into the binary experimental things that
# still needs to be debugged.
ifdef EXPERIMENTAL
  OBJS += $(addprefix $(OBJSDIR)/, term.o os.o crypto.o pwman.o stats.o services.o)
  CFLAGS += -D_IS_EXPERIMENTAL
endif

# This flag will probably gonna be deleted in the future. If enabled it's
# compiling the binary with libsodium and all the files needed to
# encrypt/decrypt passwords. Without this the password manager is just a
# password generator.
ifdef SODIUM
  CFLAGS += -D_HAVE_SODIUM
  LDLIBS += -lsodium
endif

# This flag will enable the C compiler to do multiple things.
# First it's gonna add gdb flags to the binary making debugging easy, secondly
# it's compiling logging infos and more developer-related messages.
# And finally it's using a C compiler's internal memory sanitizer to discover
# and report bad memory handling, like overflows or non-freed memory.
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

.PHONY: clean stow unstow docker
clean:
	$(RM) $(BINDIR) $(OBJSDIR)

stow:
	mkdir -p $(STOWDIR)
	cp -r $(BINDIR) $(STOWDIR)
	stow --dir=/usr/local/stow --target=/usr/local --stow $(BINARY)
	@echo Successfully stowed $(STOWDIR).

unstow:
	stow --dir=/usr/local/stow --target=/usr/local --delete $(BINARY)
	@echo Successfully unstowed $(BINARY).

docker:
	docker build -t ezpass-app .
	docker run -it ezpass-app
