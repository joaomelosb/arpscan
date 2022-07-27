#ifndef COMMON_H

#define COMMON_H

#include <stdio.h>
#include <stdlib.h>

#define VERSION "ARPScan v1.0"
#define AUTHOR "Author: John Mello (https://github.com/jjvictor)"
#define VENDORS_FILE "vendors.txt"

extern char *prog;

#define ERR(fmt, ...) fprintf(stderr, "%s: "fmt"\n", prog, ##__VA_ARGS__); \
	exit(1);

#define PERROR(fmt, ...) fprintf(stderr, "%s: "fmt": ", prog, ##__VA_ARGS__); \
	perror(""); exit(1);

#define CHECK(expr, ...) \
	if (expr) { \
		PERROR(__VA_ARGS__); \
	}

#define NCHECK(expr, ...) CHECK((expr) == -1, __VA_ARGS__)

#endif