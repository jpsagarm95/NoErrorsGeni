#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

extern int identifier;
extern int NUMBER_OF_ROUTERS;
extern int NUMBER_OF_EDGES;
extern int NUMBER_OF_NEIGHBORS;
extern int* NEIGHBOR_IDS;
extern int sock;
extern struct sockaddr_in my_addr;
extern struct hostent *host;
extern int MAX_POSSIBLE_DIST;
extern int** neighbor_link_details;
extern int** actual_link_costs;
extern int LSA_INTERVAL;
extern int lsa_seq_num;
extern int*** every_node_lsa_details;
extern int* every_node_neighbors;


extern pthread_mutex_t lock;

// char exchange(int i){
// 	switch(i){
// 		case 0: return '0';
// 		case 1: return '1';
// 		case 2: return '2';
// 		case 3: return '3';
// 		case 4: return '4';
// 		case 5: return '5';
// 		case 6: return '6';
// 		case 7: return '7';
// 		case 8: return '8';
// 		case 9: return '9';
// 	}
// }

void* lsa_packet_sender(void* param){
	printf("%s\n", "In lsa packet sender");
	int i, offset, peer_id;
	char send_data[1024];
	char addr[10] = "node-";
	addr[6] = '\0';
	
	struct sockaddr_in peer_addr;
	while(1){
		sleep(LSA_INTERVAL);
		// printf("%s\n", "Lsa packets sending");
		strncpy(send_data, "LSA", 3);
		strncpy(send_data + 3, (char*)&identifier, 4);
		// printf("%d ", identifier);
		strncpy(send_data + 7, (char*)&lsa_seq_num, 4);
		// printf("%d ", lsa_seq_num);
		lsa_seq_num++;
		strncpy(send_data + 11, (char*)&NUMBER_OF_NEIGHBORS, 4);
		// printf("%d ", NUMBER_OF_NEIGHBORS);
		offset = 15;
		pthread_mutex_lock(&lock);
		for(i = 0 ; i < NUMBER_OF_NEIGHBORS ; i++){
			strncpy(send_data + offset, (char*)&actual_link_costs[i][0], 4);
			// printf("%d ", actual_link_costs[i][0]);
			strncpy(send_data + offset + 4, (char*)&actual_link_costs[i][1], 4);
			// printf("%d ", actual_link_costs[i][1]);
			offset += 8;
		}
		// printf("\n");
		pthread_mutex_unlock(&lock);
		for(i = 0; i < NUMBER_OF_NEIGHBORS; i++){
			peer_id = NEIGHBOR_IDS[i];
			addr[5] = exchange(peer_id);
			host = (struct hostent *) gethostbyname(addr);
			peer_addr.sin_family = AF_INET;
		    peer_addr.sin_addr = *((struct in_addr *) host->h_addr);
			peer_addr.sin_port = htons(20039);
		    bzero(&(peer_addr.sin_zero), 8);
			sendto(sock, send_data, offset, 0, (struct sockaddr *) &peer_addr, sizeof (struct sockaddr));
		}
	}
}