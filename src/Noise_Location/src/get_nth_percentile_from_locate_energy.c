/*
program to get the nth percentile values from a noise source diagram
parameter file format
inputDiagramFileName
min_x max_x x_inc
min_y max_y y_inc
percentile (95, 90, 80, etc...(floating point))
minimum speed
outputFileName
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#ifdef HAVE_MALLOC_H
#include<malloc.h>
#endif

#define PERCENTILE_CORRIDOR 2
#define PI 3.14159265358979311600

int float_cmp(const void *a, const void *b);
int compute_nodes(float max, float min, float inc);

int float_cmp(const void *a, const void *b) {
	const float *fa = (const float *) a;
	const float *fb = (const float *) b;
	return (int) *fa - *fb;
}

int compute_nodes(float max, float min, float inc) {
	return floor((max-min)/inc+1.5);
}


int main (int argc, char *argv[]) {
	/* local variables */
	int i, xNode, yNode, l, totalNodes, x, y;
	int nodeBottom, nodeTop;
	float xMin, xMax, xInc;
	float yMin, yMax, yInc;
	float percentile, percentileValueBottom, percentileValueTop;
	float xTmp, yTmp;
	float maxSpeed;
	float totalWeight=0.0, azimuthSum = 0.0;
	float **data, *sorting;
	char buff[100], inputDiagramFileName[100], outputFileName[100], paramFileName[100], azimuthFileName[100];
	FILE *paramFile, *inputDiagramFile, *outputFile, *azimuthFile;

	/* check the usage */
	if (argc != 2) {
		fprintf(stderr,"Usage: %s parameter_file\n",argv[0]);
		fprintf(stderr,"parameter_file format:\n");
		fprintf(stderr,"inputDiagramFileName\n");
		fprintf(stderr,"minimum_x maximum_x increment_x\n");
		fprintf(stderr,"minimum_y maximum_y increment_y\n");
		fprintf(stderr,"percentile_value_to_keep\n");
		fprintf(stderr,"maximum_speed_to_assess\n");
		fprintf(stderr,"outputFileName\n");
		exit(1);
	}
	if (!(strcmp(argv[1],"-h"))) {
                fprintf(stderr,"Usage: %s parameter_file\n",argv[0]);
                fprintf(stderr,"parameter_file format:\n");
                fprintf(stderr,"inputDiagramFileName\n");
                fprintf(stderr,"minimum_x maximum_x increment_x\n");
                fprintf(stderr,"minimum_y maximum_y increment_y\n");
                fprintf(stderr,"percentile_value_to_keep\n");
		fprintf(stderr,"maximum_speed_to_assess\n");
                fprintf(stderr,"outputFileName\n");
                exit(1);
        }

	/* copy the parameter filename */
	sscanf(argv[1],"%s",paramFileName);

	/* try to open the parameter file */
	if ( ( paramFile = fopen(paramFileName,"r")) == NULL) {
		fprintf(stderr,"Error trying to read file %s\n",paramFileName);
		exit(1);
	}

	/* read the parameter file */
	l=0;
	while (fgets(buff,100,paramFile)) {
		if (l == 0) {
			sscanf(buff,"%s",inputDiagramFileName);
			l++;
		} else if (l == 1) {
			sscanf(buff,"%f %f %f",&xMin, &xMax, &xInc);
			l++;
		} else if (l == 2) {
			sscanf(buff,"%f %f %f",&yMin, &yMax, &yInc);
			l++;
		} else if (l == 3) {
			sscanf(buff,"%f",&percentile);
			l++;
		} else if (l == 4) {
			sscanf(buff,"%f",&maxSpeed);
			l++;
		} else if (l == 5) {
			sscanf(buff,"%s",outputFileName);
			l++;
		}
	}

	/* close the parameter file */
	fclose(paramFile);

	/* compute the nodes in x and y */
	xNode = compute_nodes(xMax, xMin, xInc);
	yNode = compute_nodes(yMax, yMin, yInc);
	totalNodes = xNode * yNode;

	/* allocate memory for the data  */
	if ( ( data = malloc(sizeof(data) * xNode)) == NULL) {
		fprintf(stderr,"Error allocating data\n");
		exit(1);
	}
	if ( ( sorting = malloc(sizeof(sorting) * totalNodes)) == NULL) {
                fprintf(stderr,"Error allocating sorting\n");
                exit(1);
        }
	for (i=0; i<xNode; i++) {
		if ( ( data[i] = malloc(sizeof(data) * yNode)) == NULL) {
	                fprintf(stderr,"Error allocating data %d\n",i);
	                exit(1);
	        }
	}

	/* try to open the data file */
	if ( ( inputDiagramFile = fopen(inputDiagramFileName,"r")) == NULL) {
		fprintf(stderr,"Error, trying to open file %s\n",inputDiagramFileName);
		exit(1);
	}

	/* read the data file */
	i=0;
	for (x=0; x<xNode; x++) {
		for (y=0; y<yNode; y++) {
			fscanf(inputDiagramFile,"%f %f %f",&xTmp, &yTmp, &data[x][y]);
			/* copy into sorting array */
			if (sqrtf( (xTmp * xTmp) + (yTmp * yTmp)) >= 1.0/maxSpeed) {
				sorting[i] = data[x][y];
				i++;
			}
		}
	}

	/* close the data file */
	fclose(inputDiagramFile);

	/* sort the data */
	qsort(sorting,i,sizeof(float),float_cmp);

	/* get the percentile value */
	nodeTop = floor(i * (percentile + PERCENTILE_CORRIDOR/2) / 100.0 + .5);
	nodeBottom = floor(i * (percentile - PERCENTILE_CORRIDOR/2) / 100.0 + .5);
	/* avoid going out of bounds */
	if (nodeTop < totalNodes) {
		percentileValueTop = sorting[nodeTop];
	} else {
		percentileValueTop = sorting[totalNodes-1];
	}
	if (nodeBottom >= 0 ) {
		percentileValueBottom = sorting[nodeBottom];
	} else {
		percentileValueBottom = sorting[0];
	}

	/* open the output file */
	if ( ( outputFile = fopen(outputFileName,"w")) == NULL) {
		fprintf(stderr,"Error, could not write to %s\n",outputFileName);
		exit(1);
	}

	/* a file name which will be for the weighted mean azimuth */
	sprintf(azimuthFileName,"%s_azi",outputFileName);

	/* loop through the data, outputting if the value is greater than the percentileValueBottom, but less than percentileValueTop */
	//The idea is we want a sort of Nth percentile contour. If I were to output all values, they would overlap and be a messy figure
	for (x=0; x<xNode; x++) {
		for (y=0; y<yNode; y++) {

			if (data[x][y] >= percentileValueBottom && data[x][y] <= percentileValueTop && 1.0/maxSpeed <= sqrtf(((xMin + x * xInc) * (xMin + x * xInc))+((yMin + y * yInc) * (yMin + y * yInc))  ) ) {
				fprintf(outputFile,"%f %f %f\n",xMin + x * xInc, yMin + y * yInc, data[x][y]);
			}

			if (data[x][y] >= percentileValueBottom && 1.0/maxSpeed <= sqrtf(((xMin + x * xInc) * (xMin + x * xInc))+((yMin + y * yInc) * (yMin + y * yInc))  ) ) {
				//compute a weighted mean to determine the maximum likelihood azimuth
				azimuthSum = azimuthSum + atan2((yMin + y * yInc), (xMin + x*xInc)) * data[x][y];
				totalWeight = totalWeight + data[x][y];
			}
		}
	}

	if ((azimuthFile = fopen(azimuthFileName,"w")) == NULL) {
		fprintf(stderr,"PWNED\n");
		exit(1);
	}
	
	fprintf(azimuthFile,"%f\n",(azimuthSum / totalWeight) * 180 / PI);

	/* close up the output file */
	fclose(outputFile);

	/* message user its finished */
	fprintf(stdout,"Program %s finished. New file %s created.\n",argv[0],outputFileName);

	/* free up memory */
	free(sorting);
	for (i=0; i<xNode; i++) {
		free(data[i]);
	}
	free(data);
	
	return 0;
}

