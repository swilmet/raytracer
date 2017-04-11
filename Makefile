CC = gcc
CFLAGS = -W -Wall `pkg-config --cflags graphene-1.0 gtk+-3.0`
LDFLAGS = `pkg-config --libs graphene-1.0 gtk+-3.0`
SRCS = raytracer.c
OBJS = $(SRCS:.c=.o)
EXECUTABLE = raytracer

.PHONY: clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(EXECUTABLE)

clean:
	rm -f $(OBJS) $(EXECUTABLE)
