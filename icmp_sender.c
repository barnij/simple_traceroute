// Bartosz Jaśkiewicz, 307893
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include "icmp_checksum.h"

int sendPacket(char* ip_address, int sockfd, int ttl)
{

    if(setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int)) < 0){
        fprintf(stderr, "setup socket error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    struct icmp header;
    header.icmp_type = ICMP_ECHO;
    header.icmp_code = 0;
    header.icmp_hun.ih_idseq.icd_id = getpid();
    header.icmp_hun.ih_idseq.icd_seq = ttl;
    header.icmp_cksum = 0;
    header.icmp_cksum = compute_icmp_checksum((uint16_t*)&header, sizeof(header));

    struct sockaddr_in recipient;
    bzero (&recipient, sizeof(recipient));
    recipient.sin_family = AF_INET;
    inet_pton(AF_INET, ip_address, &recipient.sin_addr);

    ssize_t bytes_sent = sendto(
        sockfd,
        &header,
        sizeof(header),
        0,
        (struct sockaddr*)&recipient,
        sizeof(recipient)
    );

    if(bytes_sent < 0)
    {
        fprintf(stderr, "sendto error: %s\n", strerror(errno)); 
		return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}