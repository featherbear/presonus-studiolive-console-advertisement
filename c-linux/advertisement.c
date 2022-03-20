#include <arpa/inet.h>
#include <errno.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

/* 
	96 bit (12 bytes) pseudo header needed for udp header checksum calculation 
*/
struct pseudo_header {
    uint32_t source_address;
    uint32_t dest_address;
    uint8_t placeholder;
    uint8_t protocol;
    uint16_t udp_length;
};

/*
	Generic checksum calculation function
*/
unsigned short csum(unsigned short *ptr, int nbytes) {
    register long sum;
    unsigned short oddbyte;
    register short answer;

    sum = 0;
    while (nbytes > 1) {
        sum += *ptr++;
        nbytes -= 2;
    }
    if (nbytes == 1) {
        oddbyte = 0;
        *((u_char *)&oddbyte) = *(u_char *)ptr;
        sum += oddbyte;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum = sum + (sum >> 16);
    answer = (short)~sum;

    return (answer);
}

static void showUsage(char *prog) {
    printf("Usage: %s <spoof_ip> [--once]\n", prog);
    exit(0);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        showUsage(argv[0]);
    }

    {
        struct in_addr inp;
        if (inet_aton(argv[1], &inp) == 0) showUsage(argv[0]);
    }

    bool runOnceOnly = argc == 3 && strcmp(argv[2], "--once") == 0;

    //Create a raw socket of type IPPROTO
    int s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    int enableBroadcast = 1;
    setsockopt(s, SOL_SOCKET, SO_BROADCAST, &enableBroadcast, sizeof(enableBroadcast));

    if (s == -1) {
        //socket creation failed, may be because of non-root privileges
        perror("Failed to create raw socket");
        exit(1);
    }

    //Datagram to represent the packet
    char datagram[4096], source_ip[32], *data, *pseudogram;

    //zero out the packet buffer
    memset(datagram, 0, 4096);

    //IP header
    struct iphdr *iph = (struct iphdr *)datagram;

    //UDP header
    struct udphdr *udph = (struct udphdr *)(datagram + sizeof(struct ip));

    struct sockaddr_in sin;
    struct pseudo_header psh;

    //Data part
    data = datagram + sizeof(struct iphdr) + sizeof(struct udphdr);

    char *payloadPrefix =
        "\x55\x43\x00\x01"                                      // UC.. tag
        "\x08\xcf"                                              // Port: 53000 (LE)
        "\x44\x41\x65\x00\x00\x00\x00\x04\x00\x80\x48\x1c\x48"  // // Honestly I
        "\x67\x23\x60\x51\x4f\x92\x4e\x1e\x46\x91\x50\x51\xd1"  // // have no clue.
        ;
    const uint8_t payloadPrefixLength = 32;
    memcpy(data, payloadPrefix, payloadPrefixLength);

    char *modelString = "StudioLive 24R";    // Used for console identification (i.e. the picture)
    char *serialNumber = "";                 // Console serial number, can be empty
    char *friendlyName = "Spoofed Console";  // Friendly name of the console

    const char *tag = "AUD";  // ???

    uint8_t offset = payloadPrefixLength;
    int len;

    len = strlen(modelString);
    strncpy(data + offset, modelString, ++len);
    offset += len;

    len = strlen(tag);
    strncpy(data + offset, tag, ++len);
    offset += len;

    len = strlen(serialNumber);
    strncpy(data + offset, serialNumber, ++len);
    offset += len;

    len = strlen(friendlyName);
    strncpy(data + offset, friendlyName, ++len);
    offset += len;

    const int payloadSize = offset;

    strcpy(source_ip, argv[1]);  // Get spoof IP from the program arguments

    sin.sin_family = AF_INET;
    sin.sin_port = htons(80);
    sin.sin_addr.s_addr = inet_addr("255.255.255.255");

    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof(struct iphdr) + sizeof(struct udphdr) + payloadSize;
    iph->id = htonl(54321);  //Id of this packet
    iph->frag_off = 0;
    iph->ttl = 255;
    iph->protocol = IPPROTO_UDP;
    iph->check = 0;
    iph->saddr = inet_addr(source_ip);
    iph->daddr = sin.sin_addr.s_addr;

    iph->check = csum((unsigned short *)datagram, iph->tot_len);

    //UDP header
    udph->source = htons(53000);  // Source port
    udph->dest = htons(47809);    // Dest port
    udph->len = htons(8 + payloadSize);
    udph->check = 0;  //leave checksum 0 now, filled later by pseudo header

    //Now the UDP checksum using the pseudo header
    psh.source_address = inet_addr(source_ip);
    psh.dest_address = sin.sin_addr.s_addr;
    psh.placeholder = 0;
    psh.protocol = IPPROTO_UDP;
    psh.udp_length = htons(sizeof(struct udphdr) + payloadSize);

    int psize = sizeof(struct pseudo_header) + sizeof(struct udphdr) + payloadSize;
    pseudogram = malloc(psize);

    memcpy(pseudogram, (char *)&psh, sizeof(struct pseudo_header));
    memcpy(pseudogram + sizeof(struct pseudo_header), udph, sizeof(struct udphdr) + payloadSize);

    udph->check = csum((unsigned short *)pseudogram, psize);

    do {
        if (sendto(s, datagram, iph->tot_len, 0, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
            perror("Transmit failure");
        }
    } while (!runOnceOnly && (usleep(1000 * 1000 * 5) || 1));

    return 0;
}
