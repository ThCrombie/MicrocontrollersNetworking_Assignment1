/*
	Thomas Crombie, R00155348
	Collector code

	Collector must send a message to the sensor nodes
	to query their sensor values.
	> the collector must ask the nodes dor the data
	> nodes reply with their average of last X samples.
	

*/

#include "contiki.h"
#include "net/rime/rime.h"
#include "random.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include <stdio.h>


PROCESS(collector_process, "Collector node");
AUTOSTART_PROCESS(&collector_process);
static void broadcast_recieve(struct broadcast_conn *c, 
const linkaddr_t *from){
	printf("broadcast message recieved from %d.%d: '%s'\n", 
	from->u8[0], from->u8[1], (char *)packetbuf_dataptr());
}
static const struct broadcast_callbacks broadcast_call = 
{broadcast_recieve};
static struct broadcast_conn broadcast;
PROCESS_THREAD(collector_process, ev, data){
	static struct etimer et;
	PROCESS_EXITHANDLER(broadcast_close($broadcast);)
	PROCESS_BEGIN();
	boradcast_open(&broadcast,129,&broadcast_call);
	while(1){
		etimer_set(&et, CLOCK_SECOND * 10);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
		packetbuf_copyfrom("send samples please", 20);
		broadcast_send(&broadcast);
		printf("Querying nodes for samples\n");
		// IS THIS HOW TO COLLECT? CHECK COLLECT.C EXAMPLE
	}
	PROCESS_END();
}
