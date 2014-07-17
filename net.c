#include <stdio.h>
#include <stdlib.h>
#include <libperfstat.h>
#include "opentsdbc.h"

static struct timeval tv;

char * in = " direction=in";
char * out = " direction=out";
char * sifname = " ifname=";

char *mktag(char * orig, char * sens, char * ifname) {

	int tagsize = strlen(orig) + strlen(sens) + 1 + strlen(sifname) + strlen(ifname);
	char* result = malloc(tagsize);
	sprintf(result, "%s %d %s %s\n", orig, sens, sifname, ifname);
	return result;
}

void net_collect(char * tags) {
	char sent[16] = "";
	char recv[16] = "";

	perfstat_id_t name;
	perfstat_netinterface_t *ub;
	int nnetinterface, i;
	nnetinterface = perfstat_netinterface(NULL, NULL,
			sizeof(perfstat_netinterface_t), 0);
	ub = malloc(sizeof(perfstat_netinterface_t) * nnetinterface);
	strcpy(name.name, "");
	if (perfstat_netinterface(&name, ub, sizeof(perfstat_netinterface_t),
			nnetinterface) >= 0)
		for (i = 0; i < nnetinterface; i++) {
			printf("name : %s\n", ub[i].name);
			printf("\tdescription: %s\n", ub[i].description);
			printf("\ttype : %u\n", ub[i].type);
			printf("\tmtu : %llu\n", ub[i].mtu);
			printf("\tipackets : %llu\n", ub[i].ipackets);
			printf("\tibytes : %llu\n", ub[i].ibytes);
			printf("\tierrors : %llu\n", ub[i].ierrors);
			printf("\topackets : %llu\n", ub[i].opackets);
			printf("\tobytes : %llu\n", ub[i].obytes);
			printf("\toerrors : %llu\n", ub[i].oerrors);
			printf("\tcollisions : %llu\n", ub[i].collisions);

			sprintf(recv, "%8llu", ub[i].ibytes);
			sprintf(sent, "%8llu", ub[i].obytes);

			char * tagsin = mktags(tags, in, ub[i].name);
			char * tagsout = mktags(tags, out, ub[i].name);

			sendCollectedData(tv.tv_sec, "proc.net.bytes", recv, tagsin);
			sendCollectedData(tv.tv_sec, "proc.net.bytes", sent, tagsout);

			free(tagsin);
			free(tagsout);
		}
}
