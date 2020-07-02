// todo
// - read from stream
// - output to file
// - flags
// - shorten object creation
// - test varying data types
// - write tests

package main

import (
	"bytes"
	"encoding/csv"
	"fmt"
	"io/ioutil"
	"log"
	"os"
	"strconv"
	"strings"
)

func getFile(path string) string {
	data, err := ioutil.ReadFile(path)
	if err != nil {
		fmt.Fprintf(os.Stderr, "error: %s", err)
	}
	return string(data)
}

func parseData(data string) ([]string, [][]string, error) {
	r := csv.NewReader(strings.NewReader(data))
	record, err := r.ReadAll()
	if err != nil {
		return nil, nil, err
	}
	return record[0], record[1:], nil
}

func IsNumeric(s string) bool {
	_, err := strconv.ParseFloat(s, 64)
	return err == nil
}

func formatCell(cell *string) {

	*cell = strings.ReplaceAll(*cell, ",", "")
	*cell = strings.ReplaceAll(*cell, "\"", "\\\"")
}

func makeArray(objs *bytes.Buffer, headers []string, rows [][]string) {
	objs.WriteString("[")
	for i, row := range rows {
		objs.WriteString("{")
		for v, header := range headers {
			cell := &row[v]
			formatCell(cell)
			fmt.Fprintf(objs, "\"%s\":\"%s\"", header, *cell)
			if v < len(headers)-1 {
				objs.WriteString(",")
			}
		}
		if i < len(rows)-1 {
			objs.WriteString("},")
		} else {
			objs.WriteString("}")
		}
	}
	objs.WriteString("]")
}

func main() {
	var objs bytes.Buffer
	content := getFile(os.Args[1])
	headers, data, err := parseData(content)
	if err != nil {
		log.Fatal(err)
	}
	makeArray(&objs, headers, data)
	fmt.Println(objs.String())
}
