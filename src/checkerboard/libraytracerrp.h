#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif
#include <float.h>
#include <time.h>

#define EARTH_RADIUS 6371
#define PI 3.14159265358979311600
#define TODEG   57.29577950
#define TORAD   (1./TODEG)

typedef struct DISTAZ {
        double distance;
        double degrees;
        double azimuth;
} DISTAZ;


int compute_nodes(double max, double min, double inc);
double degree_dist(double lat1, double long1, double lat2, double long2);
double km_dist(double lat1, double long1, double lat2, double long2);
double compute_event_station_azimuth(double lat1, double long1, double lat2, double long2);
double powi(double b, long x);
DISTAZ distaz(double lat1, double long1, double lat2, double long2);
double recursive_mean(double new_value, double old_mean, int n);
double trace_ray(double lat1, double long1, double lat2, double long2,
		 int xNode, int yNode, double **dispersionData,
		 int nParallelMeasurements, int nPerpendicularMeasurements,
		 double latMin, double latInc, double longMin, double longInc,
		 double widthDeg, double aveVel);
