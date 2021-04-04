// Bartosz Jaśkiewicz, 307893
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <time.h>
#include <sys/select.h>
#include "stuff.h"


void print_as_bytes (unsigned char* buff, ssize_t length)
{
	for (ssize_t i = 0; i < length; i++, buff++)
		printf ("%.2x ", *buff);	
}

bool ifLoop(double start_time)
{
	return getTime() - start_time < 1.0;
}

bool isNewIP(int* ips, int n, int ip)
{
	for(int i = 0; i < n; i++)
	{
		if(ips[i] == ip)
			return false;
	}
	return true;
}

int receivePackets(int sockfd, int actTtl, double start_time)
{
	fd_set 	descriptors;
	FD_ZERO (&descriptors);
	FD_SET 	(sockfd, &descriptors);
	struct 	timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;

	int 	ips[3];
	double 	times[3];
	double 	end_time;
	int 	received = 0;
	bool 	end = false;
	int 	goal = 3;

	while(received < goal && ifLoop(start_time))
	{
		int ready = select(sockfd+1, &descriptors, NULL, NULL, &tv);

		if(!ifLoop(start_time))
			break;
		
		if (ready < 0)
		{
			fprintf(stderr, "select error: %s\n", strerror(errno)); 
			return EXIT_FAILURE;
		}

		if (ready == 0){
			continue;
		}

		struct sockaddr_in 	sender;
		socklen_t 			sender_len = sizeof(sender);
		uint8_t 			buffer[IP_MAXPACKET];

		ssize_t packet_len = recvfrom (
			sockfd,
			buffer,
			IP_MAXPACKET,
			MSG_DONTWAIT,
			(struct sockaddr*)&sender,
			&sender_len
		);

		end_time = getTime();

		if (packet_len < 0)
		{
			if(errno == EWOULDBLOCK)
				continue;

			fprintf(stderr, "recvfrom error: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}

		struct ip* 			ip_header = (struct ip*) buffer;
		uint8_t*			icmp_packet = buffer + 4 * ip_header->ip_hl;
		struct icmp* 		icmp_header = (struct icmp*) icmp_packet;
		char 				sender_ip_str[20];

		if(icmp_header->icmp_type != ICMP_TIME_EXCEEDED && icmp_header->icmp_type != ICMP_ECHOREPLY)
			continue;
		

		if(icmp_header->icmp_type == ICMP_TIME_EXCEEDED)
		{
			ip_header = (struct ip*)((uint8_t *)icmp_header + 8);
			icmp_header = (struct icmp*)((uint8_t *)ip_header + ip_header->ip_hl * 4);
		}
			
			
		if(icmp_header->icmp_hun.ih_idseq.icd_id != getpid())
			continue;
		
			
		if(icmp_header->icmp_hun.ih_idseq.icd_seq != actTtl)
			continue;
		

		if(icmp_header->icmp_type == ICMP_ECHOREPLY)
			end = true;
		

		inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, sizeof(sender_ip_str));
		if(isNewIP(ips, received, sender.sin_addr.s_addr))
			printf ("%s ", sender_ip_str);
		
		ips[received] = sender.sin_addr.s_addr;
		times[received] = (end_time - start_time)*1000;
		received++;
	}

	if (received == 0)
		fprintf(stdout, "*\n");
	else if (received < goal)
		fprintf(stdout, "???\n");
	else
	{
		double avg = 0;
		for(int i = 0; i < goal; i++){
			avg += times[i];
		}
		avg /= 3;
		fprintf(stdout, "%.2lfms\n", avg);
	}
	
	if (end)
		return EXIT_DESTINATION;
	
	return EXIT_SUCCESS;
}
