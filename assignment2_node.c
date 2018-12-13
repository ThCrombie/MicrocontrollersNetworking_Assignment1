// R00155348
// program for the nodes

#include "contiki.h"
#include "net/rime/rime.h"
#include "random.h"
#include "dev/button-sensor.h"
//#include "dev/leds.h"
//#include "dev/sht11/sht11-sensor.h" // sensor library
#include <stdio.h>

PROCESS(node, "Node");
AUTOSTART_PROCESS(&node);

struct nodeInfo {
	int hops;
	int sequence_nr;
}
struct nodeInfo node;
// code for recieving a broadcast (for the nodes 1 hop away from sink/root
static void broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from){
	printf("Message recieved from %d.%d: '%s'\n", from->u8[0], from->[1], (char *)packetbuf_dataptr()));

// packetbuf_dataptr() = nodeinfo

}

// code for recieving a unicast (from another leaf/node)
static void
recv_uc(struct unicast_conn *c, const linkaddr_t *from){
  printf("unicast message received from %d.%d\n",
	 from->u8[0], from->u8[1]);
}
static const struct unicast_callbacks unicast_callbacks = {recv_uc};
static struct unicast_conn uc;

// main thread:
// sends data recieved to the next node
// addressing ?
PROCESS_THREAD(node, ev, data){
	PROCESS_EXITHANDLER(unicast_close(&uc));
	PROCESS_BEGIN();
	unicast_open(&uc, 146, &unicast_callbacks);
	
	while(1){
		// send this node's hop and sequence count over to the next node	
		linkaddr_t addr;
		etimer_set(&et, CLOCK_SECOND);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
	// this needs to send the node's structs
		packetbuf_copyfrom("Hello", 5);
		addr.u8[0] = 1;
		addr.u8[1] = 0;
		if(!linkaddr_cmp(&addr, &linkaddr_node_addr)) {
			unicast_send(&uc, &addr);
		}
	}
	PROCESS_END(); 
}
