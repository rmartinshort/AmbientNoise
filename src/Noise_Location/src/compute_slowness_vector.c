/*
compute_slowness_vector.c
give a velocity, compute 360 points for the vector slowness
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define PI 3.14159265358979311600

int main (int argc, char *argv[]) {
	int i;
	double velocity;

	if (argc != 2) {
		fprintf(stderr,"Usage: %s velocity\n",argv[0]);
		fprintf(stderr,"returns 360 vector slowness (east, north) to standard out\n");
		exit(1);
	}

	if (! (strcmp(argv[1],"-h"))) {
		fprintf(stderr,"Usage: %s velocity\n",argv[0]);
                fprintf(stderr,"returns 360 vector slowness (east, north) to standard out\n");
                exit(1);
        }

	sscanf(argv[1],"%lf", &velocity);

	for (i=0; i<360; i++) {
		fprintf(stdout,"%lf %lf\n", cos(i*PI/180.0) / velocity, sin(i*PI/180.0) / velocity);
	}

	return 0;
}

