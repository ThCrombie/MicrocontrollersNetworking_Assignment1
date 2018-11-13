/*
	Thomas Crombie, R00155348

	Sensor nodes send the average of X amount
	of samples back to the Collector.
	> Sensor nodes wait until asked by the Collector

*/

#include "contiki.h"
#include "net/rime/rime.h"
#include "dev/sht11/sht11-sensor.h"
#include "dev/button-sensor.h"

#include "dev/leds.h"

#include <stdio.h>

/*---------------------------------------------------------------------------*/
PROCESS(example_unicast_process, "Sensor nodes");
AUTOSTART_PROCESSES(&example_unicast_process);
/*---------------------------------------------------------------------------*/
static void
recv_uc(struct unicast_conn *c, const linkaddr_t *from)
{
  printf("unicast message received from %d.%d\n",
	 from->u8[0], from->u8[1]);
}
static const struct unicast_callbacks unicast_callbacks = {recv_uc};
static struct unicast_conn uc;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_unicast_process, ev, data)
{
  PROCESS_EXITHANDLER(unicast_close(&uc);)
    
  PROCESS_BEGIN();

  unicast_open(&uc, 146, &unicast_callbacks);
  
  static int temp;
  static int counter;
  static int avg;
  char stringValue[50];

  while(1) {
    static struct etimer et;
    linkaddr_t addr;
    
    //etimer_set(&et, CLOCK_SECOND);
    etimer_set(&et, CLOCK_SECOND * 30); // send data every 30 seconds back to the sink

    SENSORS_ACTIVATE(sht11_sensor);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
// code from lab 3: this data is sent to the sink (hopefully)
    temp = sht11_sensor.value(SHT11_SENSOR_TEMP);
    counter = counter + 1;
    avg = temp / counter;
    
    sprintf(stringValue, "%d", avg);
    
    //sends a string: packetbuf_copyfrom("Hello", 6);
    packetbuf_copyfrom("%s", stringValue); // sends the value as string (hopefully)
    addr.u8[0] = 1;
    addr.u8[1] = 0;
    if(!linkaddr_cmp(&addr, &linkaddr_node_addr)) {
      unicast_send(&uc, &addr);
    }
  temp = 0;
  counter = 0;

  }

  PROCESS_END();
}



