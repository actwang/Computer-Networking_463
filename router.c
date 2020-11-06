#include "ne.h"
#include "router.h"
#include "stdlib.h"
#include <time.h>
#include <stdlib.h>


int main(int argc, char **argv){
    //check input
    if (argc != 5){	
        printf("Error: expect 5 arguments.\n");		
        return EXIT_FAILURE;
	}
    //input
    int udp_listenfd;
    int router_id = atoi(argv[1]);
    char *ne_name = argv[2];
    int ne_port = atoi(argv[3]);
    int router_port = atoi(argv[4]);
    struct pkt_INIT_REQUEST request;	
    struct pkt_INIT_RESPONSE response;

	//check router ID
	if (router_id < 0 || router_id > (MAX_ROUTERS - 1)){		
        printf ("Invalid ID.\n");
        return 0;
    }
	request.router_id = htonl(router_id);
	
	// log file
	char filename[11];
    FILE * fp;
    sprintf(filename, "%d_router.log", router_id);
    fp = fopen(filename, "w+"); 
    
    struct sockaddr_in ne_addr;  //network emulator address
    struct sockaddr_in router_addr;     //router address
    socklen_t ne_addrlen = sizeof(ne_addr);  
    

	//Server socket udp
	if ((udp_listenfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){	
        printf ("Error: UDP_listenfd creation fails\n");        
        return -1;
    }
	// bind address
	struct hostent *ne = gethostbyname(ne_name);
    if (ne == NULL){     
        printf ("Error: Host doesn't exist.\n");        
        return -1;     
    }

    bzero(ne_addr, sizeof(ne_addr));  ////
    ne_addr.sin_family = AF_INET;  ///
    memcpy(ne_addr.sin_addr, ne->h_addr_list[0], ne->h_length); //
    ne_addr.sin_port = htons((unsigned short) ne_port);///
        
    bzero(router_addr, sizeof (router_addr)); 
    router_addr.sin_addr.s_addr = htonl(); 
    router_addr.sin_family = AF_INET;  
    router_addr.sin_port = htons((unsigned short) router_port);

	//bind server sockets
	if (-1 == bind(udp_listenfd,  &router_addr, sizeof(router_addr))){                  
        close(udp_listenfd);                
        printf("Error: udp binding fails\n");                 
        return EXIT_FAILURE;			
    }

	//send request
	if (-1 == sendto(udp_listenfd, &request, sizeof (request), 0,  &ne_addr, ne_addrlen)){
		printf("Packet failed to send\n");
		return EXIT_FAILURE;
	}

    //receive request
	int len_rec;
    if (-1 == (len_rec = recvfrom(udp_listenfd, &response, sizeof(response), 0, &router_addr, &ne_addrlen))){	        
        printf("Packet failed to receive\n");	
        return EXIT_FAILURE;		
    }

	//initial the reponse
    ntoh_pkt_INIT_RESPONSE(&response);
    InitRoutingTbl(&response, router_id);
    printf("\n\n success in initialization\n\n");

    return 0;
}

	

