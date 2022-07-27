#include <stdbool.h>
#include "main.h"

char *prog;

int main(int argc, char * const argv[]) {

	prog = *argv;

	parse(argc, argv);

	if (not == NULL) {

		ERR("invalid usage. Use -h or --help to see help")

	}

	unsigned char cidr = -1;

	for (unsigned i = 0; not[i]; i++)
		if (*(not + i) == '/') {

			if ((cidr = strtol(not + i + 1, NULL, 10)) < 8 || cidr > 32) {

				ERR("\"%s\": must be a valid number between 8 and 32",
					not + i + 1);

			}

			not[i] = 0;

			break;
		}

	if (cidr == 0xff) {

		ERR("\"%s\": invalid format supplied", not)

	}

	struct in_addr target;

	if (!inet_aton(not, &target)) {

		ERR("\"%s\": invalid inet address", not);

	}

	scan(target, cidr);

}