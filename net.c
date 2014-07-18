#include <stdio.h>
#include <stdlib.h>
#include <libperfstat.h>
#include "opentsdbc.h"

char * sin = "direction=in iface=";
char * sout = "direction=out iface=";

char *mktag(char * orig, char * sens, char * ifname) {

	int tagsize = strlen(orig) + 1 + strlen(sens) + strlen(ifname);
	char* result = malloc(tagsize);
	sprintf(result, "%s %s%s", orig, sens, ifname);
	return result;
}

struct timeval tv2;

void net_collect(char * tags) {

	gettimeofday(&tv2, NULL);

	char data[16] = "";

	perfstat_id_t name;
	perfstat_netinterface_t *ub;
	int nnetinterface, i;
	nnetinterface = perfstat_netinterface(NULL, NULL,
			sizeof(perfstat_netinterface_t), 0);

	ub = malloc(sizeof(perfstat_netinterface_t) * nnetinterface);



	strcpy(name.name, "");
	if (perfstat_netinterface(&name, ub, sizeof(perfstat_netinterface_t),
			nnetinterface) >= 0) {
		for (i = 0; i < nnetinterface; i++) {

			/* IN */
			char * tagsin = mktag(tags, sin, ub[i].name);
			/* bytes */
			sprintf(data, "%8llu", ub[i].ibytes);
			sendCollectedData(tv2.tv_sec, "proc.net.bytes", data, tagsin);
			/* errors */
			 sprintf(data, "%8llu", ub[i].ierrors);
			 sendCollectedData(tv2.tv_sec, "proc.net.errs", data, tagsin);
			free(tagsin);

			/* OUT */
			char * tagsout = mktag(tags, sout, ub[i].name);
			/* bytes */
			sprintf(data, "%8llu", ub[i].obytes);
			sendCollectedData(tv2.tv_sec, "proc.net.bytes", data, tagsout);
			/* errors */
			sprintf(data, "%8llu", ub[i].oerrors);
			sendCollectedData(tv2.tv_sec, "proc.net.errs", data, tagsout);
			free(tagsout);
		}
	}
	free(ub);
}
