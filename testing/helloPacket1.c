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
#include <pthread.h>

int sock;
int identifier;
struct sockaddr_in my_addr, peer_addr;


void* sender(void* param){
	int nei[] = {1, 2};
	int i = 0;
	while(1){
		for(i = 0 ; i < 2 ; i++){
			peer_addr.sin_port = htons(20000 + nei[i]);
			printf("Hello sent to %d\n", i);
			sendto(sock, "Hello", strlen("Hello"), 0, (struct sockaddr *) &peer_addr, sizeof (struct sockaddr));
		}
		sleep(30);
	}

}

void* receiver(void* param){
	struct sockaddr_in client_addr;
	int addr_len = sizeof (struct sockaddr);
	char recv_data[1024];
	int bytes_read;
	while(1){
		bytes_read = recvfrom(sock, recv_data, 1024, 0,
		        (struct sockaddr *) &client_addr, &addr_len);


		recv_data[bytes_read] = '\0';

		printf("\n(%s , %d) said : ", inet_ntoa(client_addr.sin_addr),
		        ntohs(client_addr.sin_port));
		printf("%s", recv_data);
		fflush(stdout);
		if(strcmp(recv_data, "Hello") == 0){
			sendto(sock, "Hi", strlen("Hi"), 0, (struct sockaddr *) &client_addr, sizeof (struct sockaddr));
			printf("\nTransaction done\n");
		}
	}

}

int main(){
    identifier = 0;
    struct hostent *host;

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(20000 + 0);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(my_addr.sin_zero), 8);
    
    
    if (bind(sock, (struct sockaddr *) &my_addr,
            sizeof (struct sockaddr)) == -1) {
        perror("Bind");
        exit(1);
    }
    
    host = (struct hostent *) gethostbyname("localhost");
    peer_addr.sin_family = AF_INET;
    peer_addr.sin_port = htons(20000 + 0);
    peer_addr.sin_addr = *((struct in_addr *) host->h_addr);
    bzero(&(peer_addr.sin_zero), 8);
    
    
    pthread_t sen;
    pthread_t rec;
    pthread_create(&sen, NULL, &sender, NULL);
    pthread_create(&rec, NULL, &receiver, NULL);
	while(1);
}


