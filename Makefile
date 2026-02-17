# Compiler and flags
CC = gcc
CFLAGS = -Wall -g 

# Output binary name
OUTPUT = compile

# Source files
SRCS = driver.c scanner.c parser.c symbol.c ast-print.c code_gen.c tac.c

# Header files
HEADERS = symbol.h scanner.h ast.h code_gen.h parse.h tac.h

# Object files
OBJS = $(SRCS:.c=.o)

# Default target to build the final executable
all: $(OUTPUT)

# Linking the object files to create the executable
$(OUTPUT): $(OBJS)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(OBJS)

# Rule to compile each .c file into a .o file
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

# Clean up object files and the output binary
clean:
	rm -f $(OBJS) $(OUTPUT)
