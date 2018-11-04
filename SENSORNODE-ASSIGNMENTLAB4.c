/*
	Thomas Crombie, R00155348
	
	Sensor nodes send the average of X amount
	of samples back to the Collector.
	> Sensor nodes wait until asked by the Collector

*/

#include "contiki.h"
#include "net/rime/rime.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include <stdio.h>


PROCESS_(sensor_node, "Sensor node");
AUTOSTART_PROCESS(&sensor_node);
static void receive_unicast(struct unicast_conn *c, const linkaddr_t *from){
	printf("unicast message recieved from %d.%d\n",
	from->u8[0], from->[1]);
}
static const struct unicast_callbacks unicast_callbacks 
={recieve_unicast};
static struct unicast_conn uc;

PROCESS_THREAD(sensor_node, ev, data){
PROCESS_EXITHANDLER(unicast_close(&uc));

PROCESS_BEGIN();
unicast_open(&uc, 146, &unicast_callbacks);

while(1){
	static struct etime et:
	linkaddr_t addr;
	etimer_set(&et, CLOCK SECOND);
	PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
	
	packetbuf_copyfrom("HELLO", 5);
	addr.u8[0] = 1;
	addr.u8[1] = 0;
	if(!linkaddr_cmp(&addr, &linkaddr_node_addr)){
		unicast_send(&uc, &addr);
	}
}
PROCESS_END();

}
