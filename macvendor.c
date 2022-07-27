#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "common.h"

#define CHHEX(x) (x >= 48 && x <= 57 ? x - 48 : \
	x >= 97 && x <= 102 ? x - 87 : x >= 65 && x <= 70 ? \
	x - 55 : 0)

void macVendor(FILE *fp, const uint8_t *ptr) {

	fseek(fp, 0, SEEK_SET);

	size_t line = 1, col = 0;
	uint8_t buf[3] = {0, 0, 0}, i = 0, sh = 4;
	bool jmp = false, eq = false;

	for (int ch; (ch = fgetc(fp)) != EOF; col++) {

		if ((ch == 0x9 || ch == 0x20 || ch == '#') && !jmp) {

			jmp = true;

			if (ch != '#') {

				// hex(buf, sizeof buf);
				eq = !memcmp(buf, ptr, sizeof buf);

				memset(buf, 0, sizeof buf);

			}

			continue;

		}

		if (ch == 0xa) {

			if (eq)
				return;

			jmp = false;
			col = 0;
			eq = false;
			line++;

			continue;

		}

		if (eq)
			putchar(ch);

		if (jmp)
			continue;

		// Is a hex digit?
		if ((ch < 48 || ch > 57) &&
			(ch < 97 || ch > 102) &&
			(ch < 65 || ch > 70)) {

			printf("\r%s: \""VENDORS_FILE"\": invalid hex digit \"%c\""
				   " at line %zu, col %zu\n", prog, ch, line, col);

			exit(1);

		}

		buf[i] |= CHHEX(ch) << sh;

		if (i == sizeof (buf) - 1 && !sh)
			i = 0;
		else if (!sh)
			i++;

		sh = sh == 4 ? 0 : 4;

	}

	printf("Unknown");

}