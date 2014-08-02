/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* bos53N src/bos/usr/ccs/lib/libperfstat/simplelparstat.c 1.2            */
/*                                                                        */
/* Licensed Materials - Property of IBM                                   */
/*                                                                        */
/* Restricted Materials of IBM                                            */
/*                                                                        */
/* COPYRIGHT International Business Machines Corp. 2007                   */
/* All Rights Reserved                                                    */
/*                                                                        */
/* US Government Users Restricted Rights - Use, duplication or            */
/* disclosure restricted by GSA ADP Schedule Contract with IBM Corp.      */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */

static char sccsid[] =
		"@(#)42        1.2  src/bos/usr/ccs/lib/libperfstat/simplelparstat.c, libperfstat, bos53N, n2007_38A8 9/17/07 05:13:28";

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libperfstat.h>
#include <sys/systemcfg.h>
#include "opentsdbc.h"
#include "uname.h"
#include "net.h"
#include "disk.h"

#define XINTFRAC	((double)(_system_configuration.Xint)/(double)(_system_configuration.Xfrac))
/* convert physical processor tics to seconds */
#define HTIC2SEC(x)	((double)x * XINTFRAC)/(double)1000000000.0

static int disp_util_header = 1;
static u_longlong_t last_time_base;
static u_longlong_t last_pcpu_user, last_pcpu_sys, last_pcpu_idle,
		last_pcpu_wait;
static u_longlong_t last_lcpu_user, last_lcpu_sys, last_lcpu_idle,
		last_lcpu_wait;
static u_longlong_t last_busy_donated, last_idle_donated;
static u_longlong_t last_busy_stolen, last_idle_stolen;
static u_longlong_t last_phint = 0, last_vcsw = 0, last_pit = 0;

static char hostname[1024];
static char * hostname_tag;

char machine_tag[32] = "";
char machine_id[16] = "";

static char * tags ;

static char * tag_idle;
static char * tag_user;
static char * tag_system;
static char * tag_wait;


static struct timeval tv;

static char pcpu_user[8];
static char pcpu_sys[8];
static char pcpu_wait[8];
static char pcpu_idle[8];
static char physc[8];
static char entc[8];

void display_lpar_util(void);

char *mktagcpu(char * orig, char * type) {
	int tagsize = strlen(orig) + 1 + strlen(type);
	char* result = malloc(tagsize);
	sprintf(result, "%s %s", orig, type);
	return result;
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		fprintf(stderr, "usage: %s <interval>\n", argv[0]);
		return 0;
	}

	/* Ingore SIGPIPE */
	struct sigaction act;
	
	char machine_tag[32] = "";

	act.sa_handler = SIG_IGN;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGPIPE, &act, NULL );

	/* Retrieve hostname */

	if (getHostname(hostname) > 0)
		exit(1);
	printf("hostname : %s\n", hostname);

	/* Create hostname tag */

	hostname_tag = malloc(5 + strlen(hostname));
	sprintf(hostname_tag, "host=%s", hostname);

        /* Retrieve the machine ID */
	get_machine_id(machine_id);
	sprintf(machine_tag, "machine=%s", machine_id);

	tags = malloc( strlen(machine_tag) + 1 + strlen(hostname_tag) );
	sprintf(tags, "%s %s", hostname_tag, machine_tag);

	tag_idle = mktagcpu(tags, "type=idle");
	tag_user = mktagcpu(tags, "type=user");
	tag_wait = mktagcpu(tags, "type=wait");
	tag_system = mktagcpu(tags, "type=system");

	printf("%s\n", tags);

	display_lpar_util();
	while (1) {
		net_collect(tags);
		disk_collect(tags);
		/*collect_mem_info(tags); */
		display_lpar_util();
		sleep(atoi(argv[1]));
	}
	return (0);
}

/* Save the current values for the next iteration */
void save_last_values(perfstat_cpu_total_t *cpustats,
		perfstat_partition_total_t *lparstats) {
	last_vcsw = lparstats->vol_virt_cswitch + lparstats->invol_virt_cswitch;
	last_time_base = lparstats->timebase_last;
	last_phint = lparstats->phantintrs;
	last_pit = lparstats->pool_idle_time;

	last_pcpu_user = lparstats->puser;
	last_pcpu_sys = lparstats->psys;
	last_pcpu_idle = lparstats->pidle;
	last_pcpu_wait = lparstats->pwait;

	last_lcpu_user = cpustats->user;
	last_lcpu_sys = cpustats->sys;
	last_lcpu_idle = cpustats->idle;
	last_lcpu_wait = cpustats->wait;

	last_busy_donated = lparstats->busy_donated_purr;
	last_idle_donated = lparstats->idle_donated_purr;

	last_busy_stolen = lparstats->busy_stolen_purr;
	last_idle_stolen = lparstats->idle_stolen_purr;
}

/* retrieve metrics using perfstat API */
void collect_metrics(perfstat_cpu_total_t *cpustats,
		perfstat_partition_total_t *lparstats) {
	gettimeofday(&tv, NULL );
	if (!perfstat_partition_total(NULL, lparstats,
			sizeof(perfstat_partition_total_t), 1)) {
		perror("perfstat_partition_total");
		exit(-1);
	}

	if (!perfstat_cpu_total(NULL, cpustats, sizeof(perfstat_cpu_total_t), 1)) {
		perror("perfstat_cpu_total");
		exit(-1);
	}
}

/* Gather and display lpar utilization metrics */
void display_lpar_util(void) {
	u_longlong_t delta_pcpu_user, delta_pcpu_sys, delta_pcpu_idle,
			delta_pcpu_wait;
	u_longlong_t delta_lcpu_user, delta_lcpu_sys, delta_lcpu_idle,
			delta_lcpu_wait;
	u_longlong_t delta_busy_stolen, delta_busy_donated, delta_idle_stolen,
			delta_idle_donated;
	u_longlong_t vcsw, lcputime, pcputime;
	u_longlong_t entitled_purr, unused_purr;
	u_longlong_t delta_purr, delta_time_base;
	double phys_proc_consumed, entitlement, percent_ent, delta_sec;
	perfstat_partition_total_t lparstats;
	perfstat_cpu_total_t cpustats;

	/* retrieve the metrics */
	collect_metrics(&cpustats, &lparstats);

	/* Print the header for utilization metrics (only once) */
	if (disp_util_header) {

		disp_util_header = 0;

		/* first iteration, we only read the data, print the header and save the data */
		save_last_values(&cpustats, &lparstats);
		return;
	}

	/* calculate physcial processor tics during the last interval in user, system, idle and wait mode  */
	delta_pcpu_user = lparstats.puser - last_pcpu_user;
	delta_pcpu_sys = lparstats.psys - last_pcpu_sys;
	delta_pcpu_idle = lparstats.pidle - last_pcpu_idle;
	delta_pcpu_wait = lparstats.pwait - last_pcpu_wait;

	/* calculate total physcial processor tics during the last interval */
	delta_purr = pcputime = delta_pcpu_user + delta_pcpu_sys + delta_pcpu_idle
			+ delta_pcpu_wait;

	/* calculate clock tics during the last interval in user, system, idle and wait mode */
	delta_lcpu_user = cpustats.user - last_lcpu_user;
	delta_lcpu_sys = cpustats.sys - last_lcpu_sys;
	delta_lcpu_idle = cpustats.idle - last_lcpu_idle;
	delta_lcpu_wait = cpustats.wait - last_lcpu_wait;

	/* calculate total clock tics during the last interval */
	lcputime = delta_lcpu_user + delta_lcpu_sys + delta_lcpu_idle
			+ delta_lcpu_wait;

	/* calculate entitlement for this partition - entitled physical processors for this partition */
	entitlement = (double) lparstats.entitled_proc_capacity / 100.0;

	/* calculate delta time in terms of physical processor tics */
	delta_time_base = lparstats.timebase_last - last_time_base;

	if (lparstats.type.b.shared_enabled) { /* partition is a SPLPAR */
		/* calculate entitled physical processor tics for this partitions */
		entitled_purr = delta_time_base * entitlement;
		if (entitled_purr < delta_purr) { /* for uncapped SPLPAR */
			/* in case of uncapped SPLPAR, consider entitled physical processor tics or
			 * consumed physical processor tics, which ever is greater */
			entitled_purr = delta_purr;
		}
		/* calculate unused physical processor tics out of the entitled physical processor tics */
		unused_purr = entitled_purr - delta_purr;

		/* distributed unused physical processor tics amoung wait and idle proportionally to wait and idle in clock tics */
		delta_pcpu_wait += unused_purr
				* ((double) delta_lcpu_wait
						/ (double) (delta_lcpu_wait + delta_lcpu_idle));
		delta_pcpu_idle += unused_purr
				* ((double) delta_lcpu_idle
						/ (double) (delta_lcpu_wait + delta_lcpu_idle));

		/* far SPLPAR, consider the entitled physical processor tics as the actual delta physical processor tics */
		pcputime = entitled_purr;
	} else if (lparstats.type.b.donate_enabled) { /* if donation is enabled for this DLPAR */
		/* calculate busy stolen and idle stolen physical processor tics during the last interval */
		/* these physical processor tics are stolen from this partition by the hypervsior
		 * which will be used by wanting partitions */
		delta_busy_stolen = lparstats.busy_stolen_purr - last_busy_stolen;
		delta_idle_stolen = lparstats.idle_stolen_purr - last_idle_stolen;

		/* calculate busy donated and idle donated physical processor tics during the last interval */
		/* these physical processor tics are voluntarily donated by this partition to the hypervsior
		 * which will be used by wanting partitions */
		delta_busy_donated = lparstats.busy_donated_purr - last_busy_donated;
		delta_idle_donated = lparstats.idle_donated_purr - last_idle_donated;

		/* add busy donated and busy stolen to the kernel bucket, as cpu
		 * cycles were donated / stolen when this partition is busy */
		delta_pcpu_sys += delta_busy_donated;
		delta_pcpu_sys += delta_busy_stolen;

		/* distribute idle stolen to wait and idle proportionally to the logical wait and idle in clock tics, as
		 * cpu cycles were stolen when this partition is idle or in wait */
		delta_pcpu_wait += delta_idle_stolen
				* ((double) delta_lcpu_wait
						/ (double) (delta_lcpu_wait + delta_lcpu_idle));
		delta_pcpu_idle += delta_idle_stolen
				* ((double) delta_lcpu_idle
						/ (double) (delta_lcpu_wait + delta_lcpu_idle));

		/* distribute idle donated to wait and idle proportionally to the logical wait and idle in clock tics, as
		 * cpu cycles were donated when this partition is idle or in wait */
		delta_pcpu_wait += delta_idle_donated
				* ((double) delta_lcpu_wait
						/ (double) (delta_lcpu_wait + delta_lcpu_idle));
		delta_pcpu_idle += delta_idle_donated
				* ((double) delta_lcpu_idle
						/ (double) (delta_lcpu_wait + delta_lcpu_idle));

		/* add donated to the total physical processor tics for CPU usage calculation, as they were
		 * distributed to respective buckets accordingly */
		pcputime += (delta_idle_donated + delta_busy_donated);

		/* add stolen to the total physical processor tics for CPU usage calculation, as they were
		 * distributed to respective buckets accordingly */
		pcputime += (delta_idle_stolen + delta_busy_stolen);

	}

	/* Processor Utilization - Applies for both SPLPAR and DLPAR*/
	sprintf(pcpu_user, "%5.1f", (double) delta_pcpu_user * 100.0 / (double) pcputime);
	sendCollectedData(tv.tv_sec, "proc.stat.cpu.lpar", pcpu_user, tag_user);

	sprintf(pcpu_sys, "%5.1f ", (double) delta_pcpu_sys * 100.0 / (double) pcputime);
	sendCollectedData(tv.tv_sec, "proc.stat.cpu.lpar", pcpu_sys, tag_system);

	sprintf(pcpu_wait, "%6.1f ", (double) delta_pcpu_wait * 100.0 / (double) pcputime);
	sendCollectedData(tv.tv_sec, "proc.stat.cpu.lpar", pcpu_wait, tag_wait);

	sprintf(pcpu_idle, "%6.1f ", (double) delta_pcpu_idle * 100.0 / (double) pcputime);
	sendCollectedData(tv.tv_sec, "proc.stat.cpu.lpar", pcpu_idle, tag_idle);

	if (lparstats.type.b.shared_enabled) { /* print SPLPAR specific stats */
		/* Physical Processor Consumed by this partition */
		phys_proc_consumed = (double) delta_purr / (double) delta_time_base;
		sprintf(physc, "%5.2f ", (double) phys_proc_consumed);
		sendCollectedData(tv.tv_sec, "proc.stat.cpu.lpar.physc", physc, tags);

		/* Percentage of Entitlement Consumed - percentage of entitled physical processor tics consumed */
		percent_ent = (double) ((phys_proc_consumed / entitlement) * 100);
		sprintf(entc, "%5.1f ", percent_ent);
		sendCollectedData(tv.tv_sec, "proc.stat.cpu.lpar.entc", entc, tags);

	} else if (lparstats.type.b.donate_enabled) { /* print donation-enabled DLPAR specific stats */
		/* Physical Processor Consumed by this partition
		 * (excluding donated and stolen physical processor tics). */
		phys_proc_consumed = (double) delta_purr / (double) delta_time_base;
		sprintf(physc, "%5.2f ", (double) phys_proc_consumed);
		sendCollectedData(tv.tv_sec, "proc.stat.cpu.lpar.physc", physc, tags);
	}

	save_last_values(&cpustats, &lparstats);
}
