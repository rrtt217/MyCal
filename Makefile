EXECUTABLE = MyCalculator
SRCS = main.c exprparser.c
OBJS = $(SRCS:.c=.o)
OBJS_PAR = $(SRCS:.c=-par.o)
CC = gcc
CFLAGS =
LDFLAGS = -lm

$(EXECUTABLE): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(EXECUTABLE)-par: $(OBJS_PAR)
	$(CC) $(LDFLAGS) -fopenmp -o $@ $^

%-par.o: %.c
	$(CC) $(CFLAGS) -fopenmp -c -o $@ $<

.PHONY: clean
clean:
	rm -f $(OBJS) $(EXECUTABLE) $(OBJS_PAR) $(EXECUTABLE)-par
