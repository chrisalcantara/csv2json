#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

void
split_line(const std::string &str, std::vector<std::string> &cont, char delim)
{
	std::vector<std::string> columns;
	std::stringstream ss(str);
	std::string token;
	while (std::getline(ss, token, delim))
		cont.push_back(token);
}

struct object {
	std::vector<std::string> headers;
	std::vector<std::string> values;
};

int
main(void)
{

	std::ifstream file;
	std::vector<std::string> headers;

	struct object obj;
	std::vector<struct object *> objects;

	file.open("../test-data/data.csv");

	if (file.is_open()) {
		std::string line;
		int rows = -1;

		while (std::getline(file, line)) {
			if (rows == -1) {
				split_line(line, headers, ',');
				rows++;
				continue;
			}

			struct object *o = &obj;

			o->headers = headers;

			objects.push_back(o);
			rows++;
		}

		for (auto &obj : objects) {
			std::cout << obj->headers[1] << std::endl;
		}
	}

	file.close();

	// std::string row = "id,first_name,hello,friend,last_name,poop";

	// split2(row, &columns, ',');

	// for (size_t i = 0; i < columns.size(); ++i)
	// 	std::cout << columns[i] << std::endl;

	// return 0;
}
