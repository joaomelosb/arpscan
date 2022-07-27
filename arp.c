#include <net/if.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/ethernet.h>
#include <netpacket/packet.h>
#include "opt.h"
#include "common.h"

#define IFNAME _if.ifr_name
#define SA(x) ((struct sockaddr *)x)

#define INETLEN (4)

static struct ifreq _if;
static struct sockaddr_ll eth;
static int sockfd;
static uint8_t ethaddr[ETH_ALEN], inetaddr[INETLEN];

enum {
	ARPREQ = 1,
	ARPREPLY = 2
};

struct arppkt {
	uint16_t hw, proto;
	uint8_t hw_sz, proto_sz;
	uint16_t opcode;
	uint8_t hw_src[ETH_ALEN],
		ip_src[INETLEN],
		hw_dst[ETH_ALEN],
		ip_dst[INETLEN];
};

char *ether_ntoa(uint8_t const *ptr) {

#define HEX ("0123456789abcdef")

	// ff:ff:ff:ff:ff:ff
	static char buf[18];
	uint8_t i = 0;

	for (; i < sizeof (buf) - 1;
		buf[i++] = HEX[*ptr >> 4],
		buf[i++] = HEX[*ptr & 0xf],
		i < sizeof (buf) - 1 && (buf[i++] = ':', ptr++));

	return buf;

}

void arpInit() {

	// Init socket
	NCHECK(sockfd = socket(AF_PACKET,
		SOCK_DGRAM, htons(ETH_P_ARP)), "socket()")

	// use timeout arg in opt.h
	struct timeval timeo = {0, 100};

	if (timeout != NULL && (timeo.tv_usec = strtol(timeout, NULL, 10)) < 1) {

		ERR("\"%s\": invalid timeout supplied", timeout)

	}

	if (timeo.tv_usec >= 1000) {

		timeo.tv_sec = timeo.tv_usec / 1000;
		timeo.tv_usec -= timeo.tv_sec * 1000;

	}

	timeo.tv_usec *= 1000;

	// printf("Secs: %ld, MicroSecs: %ld\n", timeo.tv_sec, timeo.tv_usec);

	NCHECK(setsockopt(sockfd, SOL_SOCKET,
		SO_RCVTIMEO, &timeo, sizeof (timeo)), "setsockopt()")

	// Init iface
	if (iface == NULL) {

		_if.ifr_ifindex = 2;

		NCHECK(ioctl(sockfd, SIOCGIFNAME, &_if), "iface id %d", _if.ifr_ifindex)

	} else {

		size_t ln;

		if ((ln = strlen(iface) + 1) > IFNAMSIZ) {

			ERR("%s: iface name too long", iface)

		} else
			memcpy(IFNAME, iface, ln);

	}

	if (iface != NULL) {

		NCHECK(ioctl(sockfd, SIOCGIFINDEX, &_if), "%s", IFNAME);

	}

	// Setup the l2 address
	eth.sll_family = AF_PACKET;
	eth.sll_protocol = htons(ETH_P_ARP);
	eth.sll_ifindex = _if.ifr_ifindex;
	eth.sll_hatype = 0;
	eth.sll_pkttype = 0;
	eth.sll_halen = ETH_ALEN;

	memset(eth.sll_addr, 0xff, eth.sll_halen);

	// Bind and get mac address
	NCHECK(bind(sockfd, SA(&eth), sizeof (eth)), "bind to %s", IFNAME)

	printf("Bound to if %s\n", IFNAME);

	NCHECK(ioctl(sockfd, SIOCGIFHWADDR, &_if), "cannot get %s mac", IFNAME)

	memcpy(ethaddr, _if.ifr_hwaddr.sa_data, sizeof (ethaddr));

	printf(" MAC: %s\n", ether_ntoa(ethaddr));

	NCHECK(ioctl(sockfd, SIOCGIFADDR, &_if), "cannot get %s ipv4", IFNAME)

	memcpy(inetaddr, &((struct sockaddr_in *)&_if.ifr_addr)->sin_addr.s_addr, INETLEN);

	// Shall be removed
	printf("IPv4: %s\n", inet_ntoa(*(struct in_addr *)inetaddr));

}

int whoHas(struct in_addr target, uint8_t *buf) {

	struct arppkt pkt = {
		htons(1),
		htons(ETH_P_IP),
		6,
		4,
		htons(ARPREQ)
	};

	memcpy(pkt.hw_src, ethaddr, sizeof (ethaddr));

	memcpy(pkt.ip_src, inetaddr, INETLEN);

	memset(pkt.hw_dst, 0, sizeof (pkt.hw_dst));

	memcpy(pkt.ip_dst, &target.s_addr, INETLEN);

	NCHECK(sendto(sockfd, &pkt, sizeof (pkt),
		0, SA(&eth), sizeof (eth)), "sendto()")

	ssize_t ln = recvfrom(sockfd, &pkt, sizeof (pkt), 0,
		NULL, NULL);

	if (ln == -1 || ntohs(pkt.opcode) != ARPREPLY)
		return 0;

	if (memcmp(&target.s_addr, pkt.ip_src, INETLEN)) {

		printf("\r* Received a wrong packet, dropping.\n");

		return 0;

	}

	memcpy(buf, pkt.hw_src, ETH_ALEN);

	return 1;

}