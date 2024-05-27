CC = gcc

CFLAGS = -Wall -g

LIBS = -lcairo -lcrypto -lssl
TARGET = xitorrent

SRCS = main.c draw.c utils/torrent_parser.c

OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

install:
	cp $(TARGET) /usr/local/bin

.PHONY: all clean
