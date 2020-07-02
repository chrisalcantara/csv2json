all: csv2json

<<<<<<< HEAD
SOURCES=$(shell find src -type f -name '*.c')
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=csv2json

all: $(SOURCES) $(EXECUTABLE)

build: all
	mv ./csv2json ~/.scripts/

test: all clean
	./csv2json <./test-data/data.csv
	# ./csv2json ./test-data/20200512_612107715.csv


$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.o:
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@

clean:
	# rm -f $(EXECUTABLE)
	@find src -type f -name '*.o' -exec rm -vf {} \;

.PHONY: clean
=======
csv2json:
	@echo "Compiling csv2json"
	go build -o /usr/local/bin/csv2json ./main.go;
>>>>>>> master
