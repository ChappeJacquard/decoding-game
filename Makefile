CC = gcc
CFLAGS = -Wall -Werror
LDFLAGS = -lm

TARGET_EXEC := crypt

BUILDDIR := build
SRCDIR:= src

# Find all the C files we want to compile
SRCS := $(wildcard $(SRCDIR)/*.c)

# String substitution for every C file.
# As an example, hello.cpp turns into ./build/hello.cpp.o
OBJS := $(SRCS:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)

# The final build step.
$(TARGET_EXEC): $(OBJS)
	@$(CC) $(OBJS) $(LDFLAGS) -o $@

# Build step for C source
# call with "DEBUG=1 make" to add the debug flag
$(OBJS): $(BUILDDIR)/%.o : $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	@$(CC) `echo $${DEBUG+-g}` $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@rm -r $(BUILDDIR)
	@rm -f $(TARGET_EXEC)
	@echo "build directory removed"
