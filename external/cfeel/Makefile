CC = clang
CFLAGS = -Wall -Wextra -O2

SRCS = src/main.c src/lexer.c src/parser.c src/codegen_arm64.c src/codegen_x86_64.c
OBJS = $(SRCS:.c=.o)
EXEC = cfeelc

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC) *.s a.out a.o
