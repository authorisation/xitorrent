CC = gcc
CXX = g++

CFLAGS = -Wall -g
CXXFLAGS = -Wall -g -std=c++17

LIBS = -lcairo -ltorrent-rasterbar -lboost_system
TARGET = xitorrent

SRCS = main.c draw.c utils/op.c
CXX_SRCS = utils/torrent_parser.cpp

OBJS = $(SRCS:.c=.o)
CXX_OBJS = $(CXX_SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS) $(CXX_OBJS)
	$(CXX) $(CFLAGS) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(CXX_OBJS) $(LIBS)
	strip -s $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

utils/%.o: utils/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(CXX_OBJS) $(TARGET)

install:
	cp $(TARGET) /usr/local/bin

appversion:
	./$(TARGET) -v > version.txt
	sed -i 's/version //g' version.txt
	
.PHONY: all clean
