/****************************************************************************

NAME
   libgps_shm.c - reasder access to shared-memory export

DESCRIPTION
   This is a very lightweight alternative to JSON-over-sockets.  Clients
won't be able to filter by device, and won't get device activation/deactivation
notifications.  But both client and daemon will avoid all the marshalling and 
unmarshalling overhead.

PERMISSIONS
   This file is Copyright (c) 2010 by the GPSD project
   BSD terms apply: see the file COPYING in the distribution root for details.

***************************************************************************/
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "gpsd.h"

#ifdef SHM_EXPORT_ENABLE

int gps_shm_open(/*@out@*/struct gps_data_t *gpsdata)
/* open a shared-memory connection to the daemon */
{
    int shmid;

    gpsdata->privdata = NULL;
    shmid = shmget((key_t)GPSD_KEY, sizeof(struct gps_data_t), 0);
    if (shmid == -1) {
	/* daemon isn't running or failed to create shared segment */
	return -1;
    } 
    gpsdata->privdata = shmat(shmid, 0, 0);
    if ((int)(long)gpsdata->privdata == -1) {
	/* attach failed for sume unknown reason */
	return -2;
    }
    return 0;
}

int gps_shm_read(struct gps_data_t *gpsdata)
/* read an update from the shared-memory segment */
{
    if (gpsdata->privdata == NULL)
	return -1;
    else
    {
	int before, after;
	struct shmexport_t *shared = (struct shmexport_t *)gpsdata->privdata;

	before = shared->bookend1;
	/*
	 * The bookend-consistency technique wants this to be a forward copy.
	 * But it is not guaranteed, that memcpy() works this way, and some compilers on some platforms
	 * are known to implement it as a reverse copy.  Notably GCC does
	 * this on x64 Atom. 
	 *
	 * The safest thing to do here would be to use a naive C implementation
	 * of forward byte copy, but if we did that we'd be sacrificing the 
	 * superior performance of the asm optimization GCC does for 
	 */
	(void)memcpy((void *)gpsdata, 
		     (void *)&shared->gpsdata, 
		     sizeof(struct gps_data_t));
	after = shared->bookend2;
	/*@i1@*/gpsdata->privdata = shared;
	return (before == after) ? 0 : -1;
    }
}

void gps_shm_close(struct gps_data_t *gpsdata)
{
    if (gpsdata->privdata != NULL)
	(void)shmdt((const void *)gpsdata->privdata);
}

#endif /* SHM_EXPORT_ENABLE */

/* end */
