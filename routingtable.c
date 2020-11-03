#include "ne.h"
#include "router.h"


/* ----- GLOBAL VARIABLES ----- */
struct route_entry routingTable[MAX_ROUTERS];
int NumRoutes;


////////////////////////////////////////////////////////////////
void InitRoutingTbl (struct pkt_INIT_RESPONSE *InitResponse, int myID){
	/* ----- YOUR CODE HERE ----- */
	NumRoutes = 0;
	//construct initail routing table
	for (int i = 0;i < InitResponse->no_nbr; i++){
	  routingTable[i].dest_id = InitResponse->nbrcost[i].nbr;
	  routingTable[i].next_hop = InitResponse->nbrcost[i].nbr;
	  routingTable[i].cost = InitResponse->nbrcost[i].cost;
	  routingTable[i].path[0] = myID;
	  routingTable[i].path[1] = InitResponse->nbrcost[i].nbr;
	  routingTable[i].path_len = 2;

	  NumRoutes += 1;
	}

	routingTable[i].dest_id = myID;
	routingTable[i].next_hop = myID;
	routingTable[i].cost = 0;
	routingTable[i].path_len = 1;
	routingTable[i].path[0] = myID;
	NumRoutes += 1;
	return;
}


////////////////////////////////////////////////////////////////
int UpdateRoutes(struct pkt_RT_UPDATE *RecvdUpdatePacket, int costToNbr, int myID){
	/* ----- YOUR CODE HERE ----- */
	
}


////////////////////////////////////////////////////////////////
void ConvertTabletoPkt(struct pkt_RT_UPDATE *UpdatePacketToSend, int myID){
	/* ----- YOUR CODE HERE ----- */
	UpdatePacketToSend->sender_id = myID;
	UpdatePacketToSend->no_routes = NumRoutes;
	UpdatePacketToSend->dest_id = routingTable[myID].next_hop;

	for (int i = 0; i < NumRoutes; i++){
	  UpdatePacketToSend->route[i] = routingTable[i];
	}
	return;
}


////////////////////////////////////////////////////////////////
//It is highly recommended that you do not change this function!
void PrintRoutes (FILE* Logfile, int myID){
	/* ----- PRINT ALL ROUTES TO LOG FILE ----- */
	int i;
	int j;
	for(i = 0; i < NumRoutes; i++){
		fprintf(Logfile, "<R%d -> R%d> Path: R%d", myID, routingTable[i].dest_id, myID);

		/* ----- PRINT PATH VECTOR ----- */
		for(j = 1; j < routingTable[i].path_len; j++){
			fprintf(Logfile, " -> R%d", routingTable[i].path[j]);	
		}
		fprintf(Logfile, ", Cost: %d\n", routingTable[i].cost);
	}
	fprintf(Logfile, "\n");
	fflush(Logfile);
}


////////////////////////////////////////////////////////////////
void UninstallRoutesOnNbrDeath(int DeadNbr){
	/* ----- YOUR CODE HERE ----- */
    for (int i = 0; i < NumRoutes; i++){
	  if (routingTable[i].next_hop == DeadNbr){
	    routingTable[i].cost = INFINITY;
	  }
	}
	return;
}