CC=clang
CFLAGS+=-Wall -Wextra -Werror
# CFLAGS += -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function
LDFLAGS=
SOURCES=$(shell find src/ -type f -name '*.c')
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=csv2json

all: $(SOURCES) $(EXECUTABLE)

test:
	./csv2json hello

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.o:
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@

clean:
	rm -f $(EXECUTABLE)
	@find src/ -type f -name '*.o' -exec rm -vf {} \;

.PHONY: clean
