
CLIENT_OBJS = client.o
CFLAGS = -Wall -g -std=c99
all: client

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(CLIENT_OBJS)   client

