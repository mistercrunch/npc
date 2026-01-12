BIN=MultoKrast
CC=gcc
CC_FLAGS=-Wall -O9
CC_STATIC=-static

all:
	$(CC) $(CC_FLAGS) -o $(BIN) $(BIN).c

static:
	$(CC) $(CC_FLAGS) $(CC_STATIC) -o $(BIN) $(BIN).c

clean:
	rm $(BIN)
