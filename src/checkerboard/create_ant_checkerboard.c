/*

create_ant_checkerboard.c

uses a set of parameters similar to the barmin itomo tomography code and a data file with path information to create a checkerboard map and synthetic data to be used for the inversion

takes a period flag which determines the width of the rays to simulate frequency dependence (use 0 for "thin" rays). Actually, this might as well be a double - no need to skimp on memory

parameter file format:
raypath_file (as per the barmin tomography code)
latmin latmax latinc     
longmin longmax longinc  
min_velocity(%) max_velocity(%) average_velocity
period_flag
output_data_file_name (for input to tomography)
output_checkboard_file_name  (for comparison plotting)

The actual checkerboard output will consist of blocks each defined by 17 points. 8 around the edges with 0 values, 1 in the center with the node's value, and 8 a perturbation into the cell from the edge with the node's value. Not sure this is needed. Defined, but commented out. 

diagram of a cell:
    0   0   0
     X  X  X
    0X  Z  X0
     X  X  X
    0   0   0
where Z is the value at the node's location
0 is a half node distance away
and X is an EDGE_PERTURBATION away from the 0's
Z and X both represent the value of the node and 0's are the average background velocity

Along a raypath, we define a field of points and calculate the velocity each point feels. Summing those up results in the total time the ray takes to travel.
  uses the ray tracer library for ray computation


*/
#include "libraytracerrp.h"
#define EDGE_PERTURBATION 0.001


int main (int argc, char *argv[]) {

	/* local variables */
	unsigned int xNode, yNode, x, y, l;
	unsigned int nRead;
	int xFlag, yFlag, plusOrMinusFlag, dum2;
	unsigned int nPerpendicularMeasurements = 10;
	unsigned int nParallelMeasurements = 100;
	double latMin, latMax, latInc;
	double longMin, longMax, longInc;
	double minVelPer, maxVelPer, aveVel, minVel, maxVel;
	double period, width;
	double dataLat1, dataLong1, dataLat2, dataLong2;
	double dum1, widthDeg;
	double invel, tmpVel;
	double **checkerBoardData;
	char paramFileName[200], buff[200], rayPathFileName[200];
	char outputRayPathFileName[200], outputCheckerBoardFileName[200];
	char sta1[6], sta2[6];
	FILE *paramFile, *rayPathFile, *outputRayPathFile, *outputCheckerBoardFile;
//	FILE *rectangleFile;

	/* check usage */
	if (argc != 2) {
		fprintf(stderr,"Usage: %s parameterfile\n",argv[0]);
		fprintf(stderr,"Parameterfile format:\n");
		fprintf(stderr,"raypathFileName\n");
		fprintf(stderr,"latitude_min latitude_max latitude_inc\n");
		fprintf(stderr,"longitude_min longitude_max longitude_inc\n");
		fprintf(stderr,"min_velocity(%%) max_velocity(%%) average_velocity(km/s)\n");
		fprintf(stderr,"period\n");
		fprintf(stderr,"outputRayPathFileName\n");
		fprintf(stderr,"outputCheckerBoardFileName\n");
		exit(1);
	}

	/* if the first argument is just -h */
	if (!strcmp(argv[1], "-h")) {
                fprintf(stderr,"Usage: %s parameterfile\n",argv[0]);
                fprintf(stderr,"Parameterfile format:\n");
                fprintf(stderr,"raypathFileName\n");
                fprintf(stderr,"latitude_min latitude_max latitude_inc\n");
                fprintf(stderr,"longitude_min longitude_max longitude_inc\n");
                fprintf(stderr,"min_velocity(%%) max_velocity(%%) average_velocity(km/s)\n");
                fprintf(stderr,"period\n");
                fprintf(stderr,"outputRayPathFileName\n");
                fprintf(stderr,"outputCheckerBoardFileName\n");
    		exit(1);
	}

	/* copy the argument */
	sscanf(argv[1],"%s",paramFileName);

	/* try to open the parameter file */
	if (( paramFile = fopen(paramFileName,"r")) == NULL) {
		fprintf(stderr,"Error, file %s not found\n",paramFileName);
		exit(1);
	}
	
	/* read the parameter file */
	l=0;
	while (fgets(buff,200,paramFile)) {
		if (l == 0) {
			sscanf(buff,"%s",rayPathFileName);
			l++;
		} else if (l == 1) {
			sscanf(buff,"%lf %lf %lf",&latMin, &latMax, &latInc);
			l++;
		} else if (l == 2) {
			sscanf(buff,"%lf %lf %lf", &longMin, &longMax, &longInc);
			l++;
		} else if (l == 3) {
			sscanf(buff,"%lf %lf %lf", &minVelPer, &maxVelPer, &aveVel);
			l++;
		} else if (l == 4) {
			sscanf(buff,"%lf", &period);
			l++;
		} else if (l == 5) {
			sscanf(buff,"%s",outputRayPathFileName);
			l++;
		} else if (l == 6) {
			sscanf(buff,"%s",outputCheckerBoardFileName);
			l++;
		}
	}

	/* close the parameter file */
	fclose(paramFile);

	/* compute nodes */
	xNode = compute_nodes(longMax, longMin, longInc);
	yNode = compute_nodes(latMax,  latMin,  latInc);

	/* min/max vel in the main checkerboards */
	minVel = aveVel + (minVelPer / 100.0) * aveVel;
	maxVel = aveVel + (maxVelPer / 100.0) * aveVel;

	/* width of the ray */
	if (period > 0.0) {
		width = aveVel * period;
	} else {
		width = 1.0;
	}
	widthDeg = width / 111.321;
	
	/* allocate memory for the checkerboard (not strictly necessary, but may make for easier/faster computations later)*/
	if (( checkerBoardData = malloc(sizeof(checkerBoardData) * xNode)) == NULL) {
		fprintf(stderr,"Error, could not allocate checkerBoardData\n");
		exit(1);
	}
	for (x=0; x<xNode; x++) {
		if (( checkerBoardData[x] = malloc(sizeof(checkerBoardData) * yNode)) == NULL) {
         	       fprintf(stderr,"Error, could not allocate checkerBoardData %d\n",x);
         	       exit(1);
        	}
	}

	/* try to open the output checkerboard file */
	if (( outputCheckerBoardFile = fopen(outputCheckerBoardFileName,"w")) == NULL) {
		fprintf(stderr,"Error opening %s to write\n",outputCheckerBoardFileName);
		exit(1);
	}

	/* create the checkerboard */
	xFlag = -1;
	yFlag = 1;
	for (x=0; x<xNode; x++) {
		if (xFlag == -1) {
			xFlag = 1;
		} else {
			xFlag = -1;
		}
		for (y=0; y<yNode; y++) {
			if (yFlag == -1) {
				yFlag = 1;
			} else {
				yFlag = -1;
			}
			if (y == 0 && x != 0) {
				if (checkerBoardData[x-1][0] == maxVel) {
					yFlag = -1 * xFlag;
				} else {
					yFlag = xFlag;
				}
			}

			plusOrMinusFlag = yFlag * xFlag;
			/* dump checkerboard data to file */
			//main node
			if (plusOrMinusFlag == 1) {
				checkerBoardData[x][y] = maxVel;
				fprintf(outputCheckerBoardFile," %3.3f %3.3f %3.8f \n",x*longInc + longMin, y*latInc + latMin,maxVel);

/*
				// 8 averages along the sides
				fprintf(outputCheckerBoardFile," %3.3f %3.3f %3.8f \n",x*longInc + longMin-longInc/2.0, y*latInc + latMin-latInc/2.0,aveVel);
				fprintf(outputCheckerBoardFile," %3.3f %3.3f %3.8f \n",x*longInc + longMin, y*latInc + latMin-latInc/2.0,aveVel);
				fprintf(outputCheckerBoardFile," %3.3f %3.3f %3.8f \n",x*longInc + longMin+longInc/2.0, y*latInc + latMin-latInc/2.0,aveVel);

				fprintf(outputCheckerBoardFile," %3.3f %3.3f %3.8f \n",x*longInc + longMin-longInc/2.0, y*latInc + latMin,aveVel);
				fprintf(outputCheckerBoardFile," %3.3f %3.3f %3.8f \n",x*longInc + longMin+longInc/2.0, y*latInc + latMin,aveVel);

                                fprintf(outputCheckerBoardFile," %3.3f %3.3f %3.8f \n",x*longInc + longMin-longInc/2.0, y*latInc + latMin+latInc/2.0,aveVel);
                                fprintf(outputCheckerBoardFile," %3.3f %3.3f %3.8f \n",x*longInc + longMin, y*latInc + latMin-latInc/2.0,aveVel);
                                fprintf(outputCheckerBoardFile," %3.3f %3.3f %3.8f \n",x*longInc + longMin+longInc/2.0, y*latInc + latMin+latInc/2.0,aveVel);

				// 8 values a small perturbation from the edge
				fprintf(outputCheckerBoardFile," %3.3f %3.3f %3.8f \n",x*longInc + longMin-longInc/2.0+EDGE_PERTURBATION, y*latInc + latMin-latInc/2.0+EDGE_PERTURBATION,maxVel);
                                fprintf(outputCheckerBoardFile," %3.3f %3.3f %3.8f \n",x*longInc + longMin, y*latInc + latMin-latInc/2.0+EDGE_PERTURBATION,maxVel);
                                fprintf(outputCheckerBoardFile," %3.3f %3.3f %3.8f \n",x*longInc + longMin+longInc/2.0-EDGE_PERTURBATION, y*latInc + latMin-latInc/2.0+EDGE_PERTURBATION,maxVel);

                                fprintf(outputCheckerBoardFile," %3.3f %3.3f %3.8f \n",x*longInc + longMin-longInc/2.0+EDGE_PERTURBATION, y*latInc + latMin,maxVel);
                                fprintf(outputCheckerBoardFile," %3.3f %3.3f %3.8f \n",x*longInc + longMin+longInc/2.0-EDGE_PERTURBATION, y*latInc + latMin,maxVel);

                                fprintf(outputCheckerBoardFile," %3.3f %3.3f %3.8f \n",x*longInc + longMin-longInc/2.0+EDGE_PERTURBATION, y*latInc + latMin+latInc/2.0-EDGE_PERTURBATION,maxVel);
                                fprintf(outputCheckerBoardFile," %3.3f %3.3f %3.8f \n",x*longInc + longMin, y*latInc + latMin-latInc/2.0,maxVel);
                                fprintf(outputCheckerBoardFile," %3.3f %3.3f %3.8f \n",x*longInc + longMin+longInc/2.0-EDGE_PERTURBATION, y*latInc + latMin+latInc/2.0-EDGE_PERTURBATION,maxVel);
*/
			} else {
				checkerBoardData[x][y] = minVel;
				fprintf(outputCheckerBoardFile," %3.3f %3.3f %3.8f \n",x*longInc + longMin, y*latInc + latMin,minVel);
/*
				// 8 averages along the sides
                                fprintf(outputCheckerBoardFile," %3.3f %3.3f %3.8f \n",x*longInc + longMin-longInc/2.0, y*latInc + latMin-latInc/2.0,aveVel);
                                fprintf(outputCheckerBoardFile," %3.3f %3.3f %3.8f \n",x*longInc + longMin, y*latInc + latMin-latInc/2.0,aveVel);
                                fprintf(outputCheckerBoardFile," %3.3f %3.3f %3.8f \n",x*longInc + longMin+longInc/2.0, y*latInc + latMin-latInc/2.0,aveVel);

                                fprintf(outputCheckerBoardFile," %3.3f %3.3f %3.8f \n",x*longInc + longMin-longInc/2.0, y*latInc + latMin,aveVel);
                                fprintf(outputCheckerBoardFile," %3.3f %3.3f %3.8f \n",x*longInc + longMin+longInc/2.0, y*latInc + latMin,aveVel);

                                fprintf(outputCheckerBoardFile," %3.3f %3.3f %3.8f \n",x*longInc + longMin-longInc/2.0, y*latInc + latMin+latInc/2.0,aveVel);
                                fprintf(outputCheckerBoardFile," %3.3f %3.3f %3.8f \n",x*longInc + longMin, y*latInc + latMin-latInc/2.0,aveVel);
                                fprintf(outputCheckerBoardFile," %3.3f %3.3f %3.8f \n",x*longInc + longMin+longInc/2.0, y*latInc + latMin+latInc/2.0,aveVel);

                                // 8 values a small perturbation from the edge
                                fprintf(outputCheckerBoardFile," %3.3f %3.3f %3.8f \n",x*longInc + longMin-longInc/2.0+EDGE_PERTURBATION, y*latInc + latMin-latInc/2.0+EDGE_PERTURBATION,minVel);
                                fprintf(outputCheckerBoardFile," %3.3f %3.3f %3.8f \n",x*longInc + longMin, y*latInc + latMin-latInc/2.0+EDGE_PERTURBATION,minVel);
                                fprintf(outputCheckerBoardFile," %3.3f %3.3f %3.8f \n",x*longInc + longMin+longInc/2.0-EDGE_PERTURBATION, y*latInc + latMin-latInc/2.0+EDGE_PERTURBATION,minVel);

                                fprintf(outputCheckerBoardFile," %3.3f %3.3f %3.8f \n",x*longInc + longMin-longInc/2.0+EDGE_PERTURBATION, y*latInc + latMin,minVel);
                                fprintf(outputCheckerBoardFile," %3.3f %3.3f %3.8f \n",x*longInc + longMin+longInc/2.0-EDGE_PERTURBATION, y*latInc + latMin,minVel);

                                fprintf(outputCheckerBoardFile," %3.3f %3.3f %3.8f \n",x*longInc + longMin-longInc/2.0+EDGE_PERTURBATION, y*latInc + latMin+latInc/2.0-EDGE_PERTURBATION,minVel);
                                fprintf(outputCheckerBoardFile," %3.3f %3.3f %3.8f \n",x*longInc + longMin, y*latInc + latMin-latInc/2.0+EDGE_PERTURBATION,minVel);
                                fprintf(outputCheckerBoardFile," %3.3f %3.3f %3.8f \n",x*longInc + longMin+longInc/2.0-EDGE_PERTURBATION, y*latInc + latMin+latInc/2.0-EDGE_PERTURBATION,minVel);
*/
			}
		}
	}

	/* close the checkerboard file */
	fclose(outputCheckerBoardFile);

	/* open the tomography input and output file */
	if (( rayPathFile = fopen(rayPathFileName,"r")) == NULL) {
		fprintf(stderr,"Error, file %s not found\n",rayPathFileName);
		exit(1);
	}

	if (( outputRayPathFile = fopen(outputRayPathFileName,"w")) == NULL) {
                fprintf(stderr,"Error, file %s not write-able\n",outputRayPathFileName);
                exit(1);
        }

//	if (( rectangleFile = fopen("rawCheckerBoardData.dat","w")) == NULL) {
//		fprintf(stderr,"Error, file rawCheckerBoardData.dat not writeable\n");
//		exit(1);
//	}

	/* read each line */
	printf("Beginning loop through raypath file for checkerboard raypath data\n");
//reference format:
//printf(ph_out," %d %lf %lf %lf %lf %lf 0.000 1 %s %s\n",lp,all_met[l].lat1, all_met[l].lon1, all_met[l].lat2, all_met[l].lon2, all_vel[l].ph[i], all_met[l].sta1, all_met[l].sta2);
	while (fgets(buff,200,rayPathFile)) {
		sscanf(buff," %d %lf %lf %lf %lf %lf %lf %d %s %s", &nRead,&dataLat1, &dataLong1, &dataLat2, &dataLong2, &invel, &dum1, &dum2, sta1, sta2);
		tmpVel = trace_ray(dataLat1, dataLong1, dataLat2, dataLong2, xNode, yNode, checkerBoardData, nParallelMeasurements, nPerpendicularMeasurements, latMin, latInc, longMin, longInc, widthDeg, aveVel);
		//dump to file
		fprintf(outputRayPathFile," %d %lf %lf %lf %lf %lf %lf %d %s %s\n", nRead, dataLat1, dataLong1, dataLat2, dataLong2, tmpVel, dum1, dum2, sta1, sta2);
	}

	printf("Checkerboard program done.\n");

//	printf("%lf %lf %lf\n",minVel, aveVel,maxVel);

	/* close input and output files */
	fclose(rayPathFile);
	fclose(outputRayPathFile);
//	fclose(rectangleFile);

	/* free memory */
	for (x=0; x<xNode; x++) {
		free(checkerBoardData[x]);
	}
	free(checkerBoardData);

	return 0;
}

