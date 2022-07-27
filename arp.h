#ifndef ARP_H

#define ARP_H

#include <stdint.h>
#include <netinet/in.h>
#include <net/ethernet.h>

void arpInit(void);

char *ether_ntoa(uint8_t const *);

int whoHas(struct in_addr, uint8_t *);

#endif