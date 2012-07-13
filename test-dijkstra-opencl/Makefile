BIN=test-dijkstra-opencl

CC=gcc
CFLAGS=-g -lOpenCL

$(BIN): $(BIN).c
	$(CC) $^ $(CFLAGS) -o $@

clean:
	$(RM) $(BIN)
