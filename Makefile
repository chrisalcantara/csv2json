all: csv2json

csv2json:
	@echo "Compiling csv2json"
	go build -o /usr/local/bin/csv2json ./main.go;
