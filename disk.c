#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libperfstat.h>
#include "opentsdbc.h"

perfstat_disk_total_t data1;
static struct timeval tv;
static int64_t last_disk_poll_ts = 0;
char busy_old[16] = "";
int64_t busy_ts = 0;

void disk_collect(char * tags) {
   int i, ret, tot;
   char busy[16] = "";
   perfstat_disk_total_t data2;

   gettimeofday(&tv, NULL );

   if (last_disk_poll_ts == 0) {
   	ret = perfstat_disk_total(NULL, &data1, sizeof(perfstat_disk_total_t), 1);
        if ( ret > 0 ) {
                last_disk_poll_ts = tv.tv_sec;
        } else {
		 printf("perfstat_disk_total returns %d", ret);
        }
        return;
   }


   ret = perfstat_disk_total(NULL, &data2, sizeof(perfstat_disk_total_t), 1);
   				 
   if (ret > 0) {
	int elapsed_time = tv.tv_sec - last_disk_poll_ts;
	last_disk_poll_ts = tv.tv_sec;
      	sprintf(busy, "%8llu", (data2.time  - data1.time) / ( (long) elapsed_time) );
        if ( sendCollectedData2(tv.tv_sec, "disk.busy", busy, tags, busy_old, busy_ts) == 1) {
	   busy_ts = tv.tv_sec;
           strcpy(busy_old, busy);
        }
	memcpy(&data1, &data2, sizeof(perfstat_disk_total_t) );
   } else {
	printf("perfstat_disk_total returns %d", ret);
   }
}
