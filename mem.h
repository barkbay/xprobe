#ifndef __include_mem_h__
#define __include_mem_h__

#define PAGE_SIZE_4KB 4096

static char real_inuse[8] = "";
char real_inuse_old[8] = "";
int64_t real_inuse_ts = 0;

static char real_free[8]= "";
char real_free_old[8] = "";
int64_t real_free_ts = 0;

static char real_pinned[8]= "";
char real_pinned_old[8] = "";
int64_t real_pinned_ts = 0;

static char pgins[8] = "";
char pgins_old[8] = "";
int64_t pgins_ts = 0;

static char pgouts[8] = "";
char pgouts_old[8] = "";
int64_t pgouts_ts = 0;

static char pgsp_used[8] = "";
char pgsp_used_old[8] = "";
int64_t pgsp_used_ts = 0;

static char pgsp_free[8] = "";
char pgsp_free_old[8] = "";
int64_t pgsp_free_ts = 0;

static char pgsp_rsvd[8] = "";
char pgsp_rsvd_old[8] = "";
int64_t pgsp_rsvd_ts = 0;

static char pgspins[8] = "";
char pgspins_old[8] = "";
int64_t pgspins_ts = 0;

static char pgspouts[8] = "";
char pgspouts_old[8] = "";
int64_t pgspouts_ts = 0;

void collect_mem_info(char * tags);

#endif /* #ifndef __include_mem_h__ */