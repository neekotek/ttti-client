CC = gcc
CCOPTS = -O3
INCLUDES = 
LIBS = -lcurses
LIB_INCLUDES =
LINK_FLAGS = 

SOURCES = $(wildcard src/*.cpp)
OBJS = $(patsubst %.cpp, %.o, $(SOURCES))

.PHONY: all
all: ttti-client

.PHONY: clean
clean:
	rm -f src/*.o ttti-client


%.o: %.cpp
	$(CC) $(CCOPTS) $(INCLUDES) -o $@ -c $<

ttti-client: $(OBJS)
	$(CC) $(LINK_FLAGS) $(LIB_INCLUDES) $(OBJS) $(LIBS) -o ttti-client
