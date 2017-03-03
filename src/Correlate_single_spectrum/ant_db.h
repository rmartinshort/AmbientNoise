#define MAX_STATIONS 1000
#define MAX_EVENTS 32

typedef struct EVENT {
        int year;
        int month;
        int day;
        int jday;
} EVENT;

typedef struct STATION {
        char name[7];
        char component[4];
        char network[3];
        int on_off[MAX_EVENTS];
} STATION;

typedef struct ANT_DB {
	char base_directory[100];
	int npts_spectrum;
	int npts_time_series;
        int n_events;
        int n_stations;
        STATION station_table[MAX_STATIONS];
        EVENT event_table[MAX_EVENTS];
} ANT_DB;

