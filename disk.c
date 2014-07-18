#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libperfstat.h>
#include "opentsdbc.h"

static struct timeval tv;

void disk_collect(char * tags) {
	int i, ret, tot;
	char read_sectors[16] = "";
	char write_sectors[16] = "";
	perfstat_disk_total_t data2;

	gettimeofday(&tv, NULL);

	ret = perfstat_disk_total(NULL, &data2, sizeof(perfstat_disk_total_t), 1);

	if (ret > 0) {

		sprintf(read_sectors, "%8llu", data2.rblks);
		sprintf(read_sectors, "%8llu", data2.wblks);

		sendCollectedData(tv.tv_sec, "iostat.part.read_sectors", read_sectors, tags);
		sendCollectedData(tv.tv_sec, "iostat.part.write_sectors", read_sectors, tags);

	} else {
		printf("perfstat_disk_total returns %d", ret);
	}
}
