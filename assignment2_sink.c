// R00155348
// program for the sink mote
#include "contiki.h"
#include "net/rime/rime.h"
#include "random.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "dev/sht11/sht11-sensor.h" // sensor library
#include <stdio.h>
/*
SEQUENCE NUMBER IS DEPENDANT ON THE ROOT:
ROOT DOES +1 TO SEQUENCE NUMBER EACH TIME - NODES DONT
*/
PROCESS(sink, "Sink");
AUTOSTART_PROCESSES(&sink);

// doesnt need code to recieve anything, this is the root node

/* struct containing node data */
struct rootInfo {
	int hops;
	int sequence_nr;
};
static void
broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
  printf("broadcast message received from %d.%d: '%s'\n",
         from->u8[0], from->u8[1], (char *)packetbuf_dataptr());
}
static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
static struct broadcast_conn broadcast;

PROCESS_THREAD(sink, ev, data){
	// root's struct
	struct rootInfo root;
	root.hops = 0;
	root.sequence_nr = 1; // add + 1 to the nodes once they recieve this? how to send?

	// occasionally send out a broadcast to remind the other nodes
	
	static struct etimer et;
	PROCESS_EXITHANDLER(broadcast_close(&broadcast));
	PROCESS_BEGIN();
	broadcast_open(&broadcast, 129, &broadcast_call);
	struct rootInfo *rootPtr;
	rootPtr = &root;
	while(1){
		etimer_set(&et, CLOCK_SECOND * 10);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
		packetbuf_copyfrom(rootPtr, sizeof(root)+1); 
	// error in compiler with the packetbuffer
	// thing being sent needs to be a const void pointer

		root.sequence_nr = root.sequence_nr + 1;
		broadcast_send(&broadcast);
	}
	PROCESS_END();
};
