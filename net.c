#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libperfstat.h>
#include "opentsdbc.h"

static struct timeval tv;

int64_t last_poll_ts = 0;


perfstat_netinterface_total_t net1;

void net_collect(char * tags) {
   int i, ret, tot;
   perfstat_netinterface_total_t net2;
   char sent[16] = "";
   char recv[16] = "";
   gettimeofday(&tv, NULL );

   if (last_poll_ts == 0) {
        ret = perfstat_netinterface_total(NULL, &net1, sizeof(perfstat_netinterface_total_t), 1);
	if ( ret > 0 ) {
		last_poll_ts = tv.tv_sec;	
	}
	return;
   }

   gettimeofday(&tv, NULL );
   ret = perfstat_netinterface_total(NULL, &net2, sizeof(perfstat_netinterface_total_t), 1);
   				 
   if (ret > 0) {

       int elapsed_time = tv.tv_sec - last_poll_ts;
       last_poll_ts = tv.tv_sec;
       /* printf("\nTotal Statistics for %d networks\n", net2.number); */

       sprintf(sent, "%8llu", (net2.obytes - net1.obytes) / 1024 / (long) elapsed_time); 
       sprintf(recv, "%8llu", (net2.ibytes - net1.ibytes) / 1024 / (long) elapsed_time);
       
       sendCollectedData(tv.tv_sec, "net.recv", recv, tags);
       sendCollectedData(tv.tv_sec, "net.sent", sent, tags);
       
       memcpy(&net1, &net2, sizeof(perfstat_netinterface_total_t) );
  }
}

