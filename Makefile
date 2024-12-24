EXECUTABLE = MyCalculator
SRCS = main.c exprparser.c ini.c
OBJS = $(SRCS:.c=.o)
OBJS_PAR = $(SRCS:.c=-par.o)
CC = gcc
CFLAGS =
LDFLAGS = -lm

$(EXECUTABLE): $(OBJS)
	$(CC)  -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(EXECUTABLE)-par: $(OBJS_PAR)
	$(CC)  -fopenmp -o $@ $^ $(LDFLAGS)

%-par.o: %.c
	$(CC) $(CFLAGS) -fopenmp -c -o $@ $<

.PHONY: clean
clean:
	rm -f $(OBJS) $(EXECUTABLE) $(OBJS_PAR) $(EXECUTABLE)-par
