#include "ne.h"
#include "router.h"


/* ----- GLOBAL VARIABLES ----- */
struct route_entry routingTable[MAX_ROUTERS];
int NumRoutes;
		// NumRoutes = own routing table number count
		// no_routes neighbor thinks it has no,routes

int check_onPath(struct route_entry , int );
void update(struct route_entry *, struct pkt_RT_UPDATE* , int ,int );
////////////////////////////////////////////////////////////////
void InitRoutingTbl (struct pkt_INIT_RESPONSE *InitResponse, int myID){
	/* ----- YOUR CODE HERE ----- */
	NumRoutes = 0;int i = 0;
	//construct initail routing table
	for (;i < InitResponse->no_nbr; i++){
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
}


////////////////////////////////////////////////////////////////
int UpdateRoutes(struct pkt_RT_UPDATE *RecvdUpdatePacket, int costToNbr, int myID){
	/* ----- YOUR CODE HERE ----- */
	int updated = 0;	// Program returns 1 if table is changed at all
	int i,j, x, update_cost, found_entry;
	int on_path = 0;		//am I on the path of sender 
	struct route_entry sender_entry;		//store the current entry during iteration
	//struct route_entry routingTable[j];
	for (i = 0; i < RecvdUpdatePacket->no_routes; i++){		//iterate sender's entries
		sender_entry = RecvdUpdatePacket->route[i];		//temp store current entry
		found_entry = 0;			//new entry iterated, reset found_entry
		update_cost = (costToNbr + sender_entry.cost) >= INFINITY 
			? INFINITY : costToNbr + sender_entry.cost;	// new cost = d(x,y) + d(y,z); (must be less than infinity)	

		for (j = 0; j < NumRoutes; j++ ){				// Search for matching entry in own table
			//routingTable[j] = routingTable[j];								// search in own table
			if (routingTable[j].dest_id == sender_entry.dest_id){			//if found entry in own table
				found_entry = 1;
				on_path = 0;	// if j in my table match i, then reset on_path and check if j is on i's path
				on_path = check_onPath(sender_entry, myID);

				//force update rule: update if sender IS my next hop
				//path vector rule: only update if I am not on sender's path AND sender has lower cost path
				if ((routingTable[j].next_hop == RecvdUpdatePacket->sender_id && (!on_path || update_cost == INFINITY)) || 
									((!on_path) && (update_cost < routingTable[j].cost))){
					
					if (sender_entry.path_len >= MAX_PATH_LEN){
						routingTable[j].cost = INFINITY;
					}
					else{//cost == INFINITY but path does not exceed 10 is included here
						update(&routingTable[j], RecvdUpdatePacket, i, update_cost);
					}
					updated = 1;
				}
				
			}
		}

		// If entry not found in my routing table, I need to add it
		if (!found_entry){
			//printf("Entry not found, adding entry to %d\n",routingTable[j].dest_id);
			routingTable[j].dest_id = RecvdUpdatePacket->route[i].dest_id;
			routingTable[j].next_hop = RecvdUpdatePacket->sender_id;
			routingTable[j].cost = update_cost;
			routingTable[j].path_len = sender_entry.path_len +1;
			routingTable[j].path[0] = myID;						//first position is myself

			for (x = 0; x < RecvdUpdatePacket->route[x].path_len; x++){
				routingTable[j].path[x+1] = sender_entry.path[x];				//copy entire path from position 1
			}
			NumRoutes++; updated = 1;
		}
	}

	return updated;
}

int check_onPath(struct route_entry sender, int my_id){
	int i = 0 ;
	for (; i < sender.path_len; i++){
		if (sender.path[i] == my_id){
			return 1;			// I (j) am on sender's path to dest_id
		}
	}
	return 0;
}

void update(struct route_entry *own_table, struct pkt_RT_UPDATE* sender_update, int i,int newCost){
	own_table->path[0] = sender_update->dest_id;
	int k ;
	for (k = 0; k <  own_table->path_len; k++){
		own_table->path[k+1] = sender_update->route[i].path[k];
	}
	own_table->path_len = sender_update->route[i].path_len + 1;
	own_table->cost = newCost;
	own_table->next_hop = sender_update->sender_id;
}

////////////////////////////////////////////////////////////////
void ConvertTabletoPkt(struct pkt_RT_UPDATE *UpdatePacketToSend, int myID){
	/* ----- YOUR CODE HERE ----- */
	UpdatePacketToSend->sender_id = myID;
	UpdatePacketToSend->no_routes = NumRoutes;
	UpdatePacketToSend->dest_id = routingTable[myID].next_hop;

	int i = 0;
	for (; i < NumRoutes; i++){
		UpdatePacketToSend->route[i] = routingTable[i];
	}
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
	int i = 0;
    for (; i < NumRoutes; i++){
	  if (routingTable[i].next_hop == DeadNbr){
	    routingTable[i].cost = INFINITY;
	  }
	}
}
