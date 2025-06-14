CC = gcc
SRC_SUBDIR := src
INCDIR := ./src
BUILDDIR := .
OBJDIR := $(BUILDDIR)/obj
DEBUG := 0

ifeq ($(DEBUG),1)
CFLAGS = -Wall -g -D_DEBUG
else
CFLAGS = -Wall -O2
endif

SRCS := $(shell find $(SRC_SUBDIR) -name '*.c')
OBJS := $(patsubst $(SRC_SUBDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))
LIBS :=-lncurses

# Pattern rule
$(OBJDIR)/%.o: $(SRC_SUBDIR)/%.c
	mkdir -p $(dir $@)
	$(CC) -I$(INCDIR) -c -o $@ $< $(CFLAGS)

ascii_frontend: $(OBJS) 
	$(CC) -I$(INCDIR) -o $@ $(OBJS) $(LIBS) $(CFLAGS)

all: ascii_frontend
	@echo "Building ascii_frontend"

clean: 
	rm -rf $(OBJDIR) ascii_frontend
