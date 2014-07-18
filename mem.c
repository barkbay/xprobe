#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <libperfstat.h>
#include "opentsdbc.h"

#define do(xxx) printf( "%20s=%lld\n", # xxx, mem.xxx);
#define MB_SIZE 1024 * 1024
#define PAGE_SIZE_4KB 4096

static char real_inuse[8] = "";
char real_inuse_old[8] = "";
int64_t real_inuse_ts = 0;

static char real_free[8]= "";
char real_free_old[8] = "";
int64_t real_free_ts = 0;

static char real_pinned[8];

static char pgsp_used[8] = "";
char pgsp_used_old[8] = "";
int64_t pgsp_used_ts = 0;


static struct timeval tv;

/* Gather and display lpar utilization metrics */
void collect_mem_info(char * tags) {
	printf("MEM\n");
   int retcode;
   perfstat_memory_total_t mem;
   gettimeofday(&tv, NULL );
   retcode = perfstat_memory_total(NULL,&mem,sizeof(perfstat_memory_total_t),1);

   /* return code is number of structures returned */
   if(retcode != 1) {
       printf("perfstat_memort_total returned=%d errono=%d sizeof(perfstat_memory_total_t)=%d\n",retcode,errno,sizeof(perfstat_memory_total_t));
   } else {
       int64_t now = tv.tv_sec;     
       sprintf(real_inuse, "%8.0f", (double) (mem.real_inuse * PAGE_SIZE_4KB) / (double) (MB_SIZE));
       if (sendCollectedData2(now, "mem.used", real_inuse, tags, real_inuse_old, real_inuse_ts) == 1) {
	   real_inuse_ts = now;
           strcpy(real_inuse_old, real_inuse);
       }
       
       sprintf(real_free, "%8.0f", (double) (mem.real_free * PAGE_SIZE_4KB) / (double) (MB_SIZE));
       if (sendCollectedData2(tv.tv_sec, "mem.free", real_free, tags, real_free_old, real_free_ts) == 1) {
	    real_free_ts = now;
	    strcpy(real_free_old, real_free);
       }
       
       sprintf(pgsp_used, "%8.0f", (double) ( (mem.pgsp_total - mem.pgsp_free) * PAGE_SIZE_4KB) / (double) (MB_SIZE));
       if (sendCollectedData2(tv.tv_sec, "mem.paging.used", pgsp_used, tags, pgsp_used_old, pgsp_used_ts)) {
            pgsp_used_ts = now;
	    strcpy(pgsp_used_old, pgsp_used);
       }	
   }
}

