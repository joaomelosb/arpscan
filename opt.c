#include <getopt.h>
#include "common.h"

char *iface = NULL, *not = NULL,
	*timeout = NULL, *count = NULL;

#define USAGE "Usage: %s [options] IP/CIDR\noptions:\n" \
			  "-i, --iface   <iface> Change the iface whose the packets are sent and received\n" \
			  "-t, --timeout <time>  Change the timeout of receiving packets, in miliseconds\n" \
			  "-c, --count   <count> Scan <count> hosts\n" \
			  "-h, --help            Show this help message and exit\n" \
			  "-V, --version         Prints the version info\n"

void parse(int argc, char * const argv[]) {

	int ch;

	struct option opts[] = {
		{
			"iface",
			required_argument,
			NULL,
			'i'
		},
		{
			"help",
			no_argument,
			NULL,
			'h'
		},
		{
			"timeout",
			required_argument,
			NULL,
			't'
		},
		{
			"count",
			required_argument,
			NULL,
			'c'
		},
		{
			"version",
			no_argument,
			NULL,
			'V'
		},
		{0, 0, 0, 0}
	};

	while ((ch = getopt_long(argc, argv, "-i:t:c:hV", opts, NULL)) != -1) {

		switch (ch) {

			case '?':
				exit(1);

			case 'i':

				iface = optarg;

				break;

			case 't':

				timeout = optarg;

				break;

			case 'h':

				printf(USAGE, prog);

				exit(0);

			case 'V':

				printf(VERSION"\n"AUTHOR"\n");

				exit(0);

			case 'c':

				count = optarg;

				break;

			case 1:

				if (not == NULL)
					not = optarg;

		}

	}

}