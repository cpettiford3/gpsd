#include <config.h>
#if DBUS_ENABLE==1

#include <gpsd_dbus.h>
#include <stdio.h>

static DBusConnection* connection = NULL;

/*
 * does what is required to initialize the dbus connection
 * this is rather pretty basic at this point, as we don't
 * receive commands via dbus
 *
 * returns 0 when everything is ok
 */
int initialize_dbus_conection (void) {
	DBusError	error;

	dbus_error_init (&error);
	connection = dbus_bus_get (DBUS_BUS_SYSTEM, &error);
	if (connection == NULL) {
		/* report error */
		return 1;
	}
	return 0;
}

/*
 * this sends the current fix data for this channel via dbus
 *
 *
 */
void send_dbus_fix (struct gps_device_t* channel) {
	struct gps_data_t*	gpsdata;
	struct gps_fix_t*	gpsfix;
	DBusMessage*		message;
	DBusMessageIter		iter;
	dbus_uint32_t		serial; /* we don't care about it, 
					   but we must collect it */
	
	/* if the connection is non existent, return without doing
	 * anything
	 */
	if (connection == NULL) return;

	gpsdata = &(channel->gpsdata);
	gpsfix = &(gpsdata->fix);

	message = dbus_message_new_signal (
			"/org/gpsd",
			"org.gpsd",
			"fix");
	dbus_message_iter_init (message, &iter);
	
	/*
	 * add the interesting information to the message
	 */

	dbus_message_iter_append_double (&iter, gpsfix->time);
	dbus_message_iter_append_int32 (&iter, gpsfix->mode);
	dbus_message_iter_append_double (&iter, gpsfix->ept);
	dbus_message_iter_append_double (&iter, gpsfix->latitude);
	dbus_message_iter_append_double (&iter, gpsfix->longitude);
	dbus_message_iter_append_double (&iter, gpsfix->eph);
	dbus_message_iter_append_double (&iter, gpsfix->altitude);
	dbus_message_iter_append_double (&iter, gpsfix->epv);
	dbus_message_iter_append_double (&iter, gpsfix->track);
	dbus_message_iter_append_double (&iter, gpsfix->epd);
	dbus_message_iter_append_double (&iter, gpsfix->speed);
	dbus_message_iter_append_double (&iter, gpsfix->eps);
	dbus_message_iter_append_double (&iter, gpsfix->climb);
	dbus_message_iter_append_double (&iter, gpsfix->epc);
	dbus_message_iter_append_double (&iter, gpsfix->separation);

	dbus_message_set_no_reply (message, TRUE);
	
	/* the message is complete 
	 * time to send it
	 */

	dbus_connection_send (connection, message, &serial);
	dbus_message_unref (message);
}

#endif
