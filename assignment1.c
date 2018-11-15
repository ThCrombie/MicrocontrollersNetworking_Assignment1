#include "contiki.h"
#include "net/rime/rime.h"
#include "random.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "dev/sht11/sht11-sensor.h" // sensor library
#include <stdio.h>
/*---------------------------------------------------------------------------*/
PROCESS(example_broadcast_process, "Assignment 1");
AUTOSTART_PROCESSES(&example_broadcast_process);
/*---------------------------------------------------------------------------*/
// for the sensinodes to send to the node with RIME ID 1
static void
recv_uc(struct unicast_conn *c, const linkaddr_t *from)
{
  // the 'sink' (rime id 1) should be the only one to recieve this.
  printf("unicast message received from %d.%d: '%s'\n",
	 from->u8[0], from->u8[1], (char *)packetbuf_dataptr());
}
static const struct unicast_callbacks unicast_callbacks = {recv_uc};
static struct unicast_conn uc;

static void
broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
  printf("broadcast message received from %d.%d: '%s'\n",
         from->u8[0], from->u8[1], (char *)packetbuf_dataptr());
  
  unicast_open(&uc, 146, &unicast_callbacks);
  linkaddr_t addr;
  // code to convert the heat data (int) into string
  static int temp;
  static int counter;
  static int avg;
  char stringValue[50];
  SENSORS_ACTIVATE(sht11_sensor);
  temp = sht11_sensor.value(SHT11_SENSOR_TEMP);
  counter = counter + 1;
  avg = temp / counter;
  sprintf(stringValue, "%d", avg);
  packetbuf_copyfrom(stringValue, 51); 

  //packetbuf_copyfrom("How r u", 8);
  
  // this addr means it will only send data back to the the 'sink'
  addr.u8[0] = 1;
  addr.u8[1] = 0;
  if(!linkaddr_cmp(&addr, &linkaddr_node_addr)) {
      unicast_send(&uc, &addr);
  }
  SENSORS_DEACTIVATE(sht11_sensor);
  temp = 0;
  counter = 0;
  avg = 0;

}
static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
static struct broadcast_conn broadcast;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_broadcast_process, ev, data)
{
  static struct etimer et;

  PROCESS_EXITHANDLER(broadcast_close(&broadcast);)

  PROCESS_BEGIN();

  broadcast_open(&broadcast, 129, &broadcast_call);

  while(1) {

    /* Delay 2-4 seconds */
    etimer_set(&et, CLOCK_SECOND * 4 + random_rand() % (CLOCK_SECOND * 4));

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    packetbuf_copyfrom("Requesting data", 17);
    broadcast_send(&broadcast);
  }

  PROCESS_END();
}
