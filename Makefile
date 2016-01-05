CC = gcc
CFLAGS = -W -Wall `pkg-config --cflags graphene-gobject-1.0`
LDFLAGS = `pkg-config --libs graphene-gobject-1.0`
SRCS = raytracer.c
OBJS = $(SRCS:.c=.o)
EXECUTABLE = raytracer

.PHONY: clean

$(EXECUTABLE): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(EXECUTABLE)

all: $(EXECUTABLE)

clean:
	rm -f $(OBJS) $(EXECUTABLE)
