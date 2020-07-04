CC=cc

WARNINGS=-Wall -Wextra -Werror -Wstrict-prototypes

CFLAGS +=-O2 $(WARNINGS)

all: csv2json

SOURCES=$(shell find src -type f -name '*.c')
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=csv2json

all: $(SOURCES) $(EXECUTABLE)

test: all
	./csv2json <./test-data/small.csv | jsonlint > /dev/null && echo "OK"
	./csv2json ./test-data/small.csv | jsonlint > /dev/null && echo "OK"
	./csv2json ./test-data/20200512_612107715.csv | jsonlint > /dev/null && echo "OK"
	./csv2json <./test-data/20200512_612107715.csv | jsonlint > /dev/null && echo "OK"


$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	@rm -f $(EXECUTABLE) *.json
	@find src -type f -name '*.o' -exec rm -vf {} \;

.PHONY: clean
