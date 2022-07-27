#ifndef OPT_H

#define OPT_H

extern char *iface, *not, *timeout,
	*count;

void parse(int, char * const []);

#endif