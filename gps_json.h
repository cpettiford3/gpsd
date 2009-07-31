/* gps_json.h - JSON handling for libgps and gpsd */

#include "json.h"

#define GPS_JSON_COMMAND_MAX	80
#define GPS_JSON_RESPONSE_MAX	1024

void json_tpv_dump(struct gps_data_t *, struct gps_fix_t *, char *, size_t);
void json_sky_dump(struct gps_data_t *, char *, size_t);
int json_tpv_read(const char *, struct gps_data_t *);
int json_sky_read(const char *, struct gps_data_t *);
int json_watch_read(int *, char *);
void json_watch_dump(int, char *, size_t);

#define NWATCHTYPES	5

struct watchmap_t {
    int mask;
    gnss_type class;
    char *string;
};
extern const struct watchmap_t watchmap[NWATCHTYPES];

/* gps_json.h ends here */