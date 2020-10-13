#program executable name
TARGET = butterfy.exe

#compiler flags
CFLAGS = -g -Wall -std=gnu11  -lpthread -lcomedi -lm 


#linker flags
LDFLAGS = -g -lpthread -lcomedi -lm -std=gnu11


#listfo sources
SOURCES = $(shell find -name "*.c")

#default rule, to compile everything
all: $(TARGET)

#define object files
OBJECTS = $(SOURCES:.c=.o)

#link programs
$(TARGET): $(OBJECTS)
	clang $(LDFLAGS) -o $@ $^

#compile 
%.o : %.c
	clang $(CFLAGS) -c -o $@ $<

#cleaning
clean:
	rm -f $(TARGET) $(OBJECTS)
