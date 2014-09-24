#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <libperfstat.h>
#include "opentsdbc.h"
#include "mem.h"

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

       sprintf(real_inuse, "%8.0f", (double) (mem.real_inuse * PAGE_SIZE_4KB));
       if (sendCollectedData2(now, "proc.meminfo.lpar.used", real_inuse, tags, real_inuse_old, real_inuse_ts) == 1) {
	         real_inuse_ts = now;
           strcpy(real_inuse_old, real_inuse);
       }
       
       sprintf(real_free, "%8.0f", (double) (mem.real_free * PAGE_SIZE_4KB));
       if (sendCollectedData2(tv.tv_sec, "proc.meminfo.lpar.free", real_free, tags, real_free_old, real_free_ts) == 1) {
      	    real_free_ts = now;
      	    strcpy(real_free_old, real_free);
       }

       sprintf(real_pinned, "%8.0f", (double) (mem.real_pinned * PAGE_SIZE_4KB));
       if (sendCollectedData2(tv.tv_sec, "proc.meminfo.lpar.pinned", real_pinned, tags, real_pinned_old, real_pinned_ts) == 1) {
            real_pinned_ts = now;
            strcpy(real_pinned_old, real_pinned);
       }

       sprintf(pgins, "%8.0f", (double) ( (mem.pgins) * PAGE_SIZE_4KB));
       if (sendCollectedData2(tv.tv_sec, "proc.meminfo.lpar.in", pgins, tags, pgins_old, pgins_ts)) {
            pgins_ts = now;
            strcpy(pgins_old, pgins);
       }
       sprintf(pgouts, "%8.0f", (double) ( (mem.pgouts) * PAGE_SIZE_4KB));
       if (sendCollectedData2(tv.tv_sec, "proc.meminfo.lpar.out", pgouts, tags, pgouts_old, pgouts_ts)) {
            pgouts_ts = now;
            strcpy(pgins_old, pgouts);
       }
       
       sprintf(pgsp_used, "%8.0f", (double) ( (mem.pgsp_total - mem.pgsp_free) * PAGE_SIZE_4KB));
       if (sendCollectedData2(tv.tv_sec, "proc.meminfo.lpar.paging.used", pgsp_used, tags, pgsp_used_old, pgsp_used_ts)) {
            pgsp_used_ts = now;
	          strcpy(pgsp_used_old, pgsp_used);
       }

       sprintf(pgsp_free, "%8.0f", (double) ( (mem.pgsp_free) * PAGE_SIZE_4KB));
       if (sendCollectedData2(tv.tv_sec, "proc.meminfo.lpar.paging.free", pgsp_free, tags, pgsp_free_old, pgsp_free_ts)) {
            pgsp_free_ts = now;
            strcpy(pgsp_free_old, pgsp_free);
       }

       sprintf(pgsp_rsvd, "%8.0f", (double) ( (mem.pgsp_rsvd) * PAGE_SIZE_4KB));
       if (sendCollectedData2(tv.tv_sec, "proc.meminfo.lpar.paging.rsvd", pgsp_rsvd, tags, pgsp_rsvd_old, pgsp_rsvd_ts)) {
            pgsp_rsvd_ts = now;
            strcpy(pgsp_rsvd_old, pgsp_rsvd);
       }

       sprintf(pgspins, "%8.0f", (double) ( (mem.pgspins) * PAGE_SIZE_4KB));
       if (sendCollectedData2(tv.tv_sec, "proc.meminfo.lpar.paging.in", pgspins, tags, pgspins_old, pgspins_ts)) {
            pgspins_ts = now;
            strcpy(pgspins_old, pgspins);
       }
       sprintf(pgspouts, "%8.0f", (double) ( (mem.pgspouts) * PAGE_SIZE_4KB));
       if (sendCollectedData2(tv.tv_sec, "proc.meminfo.lpar.paging.out", pgspouts, tags, pgspouts_old, pgspouts_ts)) {
            pgspouts_ts = now;
            strcpy(pgspouts_old, pgspouts);
       }

   }
}

