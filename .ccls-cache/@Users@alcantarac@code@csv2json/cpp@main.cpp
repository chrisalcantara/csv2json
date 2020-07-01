#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

void
print_use()
{
	std::cout << "Usage:" << std::endl;
	std::cout << "\t$ csv2json data.csv > outfile.json" << std::endl;
	std::cout << "\t$ cat data.csv | csv2json" << std::endl;
	std::cout << "\t$ csv2json <data.csv" << std::endl;
	exit(EXIT_SUCCESS);
}

int
main(void)
{
	std::string line;
	while (std::getline("../test-data/data.csv", line)) {
		std::istringstream iss(line);

		std::cout << line << std::endl;
	}

	return 0;
}
