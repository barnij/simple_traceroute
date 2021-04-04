// Bartosz Jaśkiewicz, 307893
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include "icmp_sender.h"
#include "icmp_receiver.h"
#include "stuff.h"

bool isValidIpAddress(char *ipAddress)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
    return result == 1;
}

int main(int argc, char* argv[])
{

    if (argc != 2 || !isValidIpAddress(argv[1]))
    {
        printf("Incorrect input\n");
        return EXIT_FAILURE;
    }

    char* ip = argv[1];

    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

	if (sockfd < 0)
    {
		fprintf(stderr, "socket error: %s\n", strerror(errno)); 
		return EXIT_FAILURE;
	}
    
    if (sockfd < 0)
    {
		fprintf(stderr, "socket error: %s\n", strerror(errno)); 
		return EXIT_FAILURE;
	}

    int ttl = 1;

    while(ttl <= 30)
    {
        for(int seq = 1; seq <= 3; seq++)
        {
            if(sendPacket(ip, sockfd, ttl) == EXIT_FAILURE)
                return EXIT_FAILURE;
        }

        fprintf(stdout, "%d. ", ttl);
        int status = receivePackets(sockfd, ttl, getTime());

        if(status == EXIT_FAILURE)
            return EXIT_FAILURE;

        if(status == EXIT_DESTINATION)
            break;

        ttl++;
    }


    return EXIT_SUCCESS;
}