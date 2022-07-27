#include <errno.h>
#include <endian.h>
#include <string.h>
#include <arpa/inet.h>
#include "common.h"
#include "arp.h"
#include "opt.h"
#include "macvendor.h"

void scan(struct in_addr iaddr, unsigned char cidr) {

	printf(VERSION"\n");

	arpInit();

	FILE *fp;

	if ((fp = fopen(VENDORS_FILE, "r")) == NULL) {

		printf("%s: warning: could not open \""VENDORS_FILE"\": %s."
			   " The mac recognize will be disabled.\n", prog, strerror(errno));

	}

	in_addr_t mask = -1 << (32 - cidr),
		i = be32toh(iaddr.s_addr) & ~mask, max = ~mask;

	size_t c = 0, up = 0;
	ssize_t tt = -1;
	uint8_t b[ETH_ALEN];
	char *nnt;

	if (count != NULL && (tt = strtol(count, NULL, 10)) < 1) {

		ERR("\"%s\": invalid count supplied", count)

	}

	for (; i < max; i++, c++) {

		if (tt == c)
			break;

		iaddr.s_addr &= be32toh(mask);

		iaddr.s_addr |= be32toh(i);

		nnt = inet_ntoa(iaddr);

		printf("\rWho has %s?", nnt);

		// stdout didn't write the 0x0a char
		// So, tell it to put everything outside
		fflush(stdout);

		if (whoHas(iaddr, b)) {

			printf("\r%15s is at %s", nnt, ether_ntoa(b));

			if (fp != NULL) {

				printf(" (");

				macVendor(fp, b);

				putchar(')');

			}

			putchar(0xa);

			up++;

		}

	}

	printf("\rTotal scanned hosts: %zu\nHosts up: %zu\n", c,
		up);

}