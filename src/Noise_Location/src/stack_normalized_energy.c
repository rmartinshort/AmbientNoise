/*
stack_percentile_location.c
read a list of noise source diagrams (should be in chronological order - or at least outputs the cumulatives in given order)
sums the energy along each azimuth
sorts those azimuths and makes a percentile as a function of azimuth
outputs a new file with the cumulative percentile as a function of azimuth for each month NSD
*/


#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#ifdef HAVE_MALLOC_H
#include<malloc.h>
#endif

#define PI 3.14159265358979311600


int compute_nodes(float max, float min, float inc);
void smooth_1d_array(float *arr_in, float *arr_out, int npts, int n_smooth);


void smooth_1d_array(float *arr_in, float *arr_out, int npts, int n_smooth) {
	int i, j, start, end, count;
	float sum;
	if (n_smooth <= 0 || n_smooth >= npts) {
		return;
	}

	//odd
	if (n_smooth % 2 == 1) {
		for (i=0; i<npts; i++) {
			sum = 0.0;
			count = 0;
			if (i-floor(n_smooth/2) < 0) {
				start = 0;
			} else {
				start = i-floor(n_smooth/2);
			}
			if (i+floor(n_smooth/2) > npts-1) {
				end = npts-1;
			} else {
				end = i+floor(n_smooth/2);
			}
			for (j=start; j<=end; j++) {
				sum = sum + arr_in[j];
				count = count + 1;
			}
			arr_out[i] = sum / (float) count;
		}
	} else {
		for (i=0; i<npts; i++) {
			sum = 0.0;
			count = 0;
			if (i-floor(n_smooth/2) < 0) {
				start = 0;
			} else {
				start = i-floor(n_smooth/2);
			}
			if (i+floor(n_smooth/2)-1 > npts-1) {
				end = npts-1;
			} else {
				end = i+floor(n_smooth/2)-1;
			}
			for (j=start; j<=end; j++) {
				sum = sum + (arr_in[j] + arr_in[j+1]) / 2.0;
				count = count + 1;
			}
			arr_out[i] = sum / (float) count;
		  }
	}
	return;
}
//END



int compute_nodes(float max, float min, float inc) {
	return floor((max-min)/inc+1.5);
}


int main (int argc, char *argv[]) {
	/* local main variables */
	int l, xNode, yNode, x, y,i;
	int nAzimuth;
	int azi, nsmooth = 1;
	int *hits;
	float xMin, xMax, xInc;
	float yMin, yMax, yInc;
	float xTmp, yTmp, data;
	float minSpeed, maxSpeed;
	float maxPower;
	float azimuthBinSize;
	float azimuth;
	float *sorting, *cumulative;
	char buff[100], listInputDiagramFileName[100], outputFileName[100];
	char paramFileName[100], inputDiagramFileName[100];
	FILE *paramFile, *listInputDiagramFile, *inputDiagramFile, *outputFile;
	
	/* check usage */
	if (argc != 2) {
		fprintf(stderr,"Usage: %s parameter_file\n",argv[0]);
		fprintf(stderr,"parameter_file format:\n");
		fprintf(stderr,"list_of_input_diagrams_FileName\n");
		fprintf(stderr,"minimum_x maximum_x increment_x\n");
		fprintf(stderr,"minimum_y maximum_y increment_y\n");
		fprintf(stderr,"minimum_speed_to_assess maximum_speed_to_assess\n");
		fprintf(stderr,"azimuth_bin_size\n");
		exit(1);
	}
	if (!(strcmp(argv[1],"-h"))) {
                fprintf(stderr,"Usage: %s parameter_file\n",argv[0]);
                fprintf(stderr,"parameter_file format:\n");
                fprintf(stderr,"list_of_input_diagrams_FileName\n");
                fprintf(stderr,"minimum_x maximum_x increment_x\n");
                fprintf(stderr,"minimum_y maximum_y increment_y\n");
		fprintf(stderr,"minimum_speed_to_assess maximum_speed_to_assess\n");
		fprintf(stderr,"azimuth_bin_size\n");
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
			sscanf(buff,"%s",listInputDiagramFileName);
			l++;
		} else if (l == 1) {
			sscanf(buff,"%f %f %f",&xMin, &xMax, &xInc);
			l++;
		} else if (l == 2) {
			sscanf(buff,"%f %f %f",&yMin, &yMax, &yInc);
			l++;
		} else if (l == 3) {
			sscanf(buff,"%f %f",&minSpeed, &maxSpeed);
			l++;
		} else if (l == 4) {
                        sscanf(buff,"%f",&azimuthBinSize);
                        l++;
                }

	}

	/* close the parameter file */
	fclose(paramFile);

	/* compute the nodes in x and y */
	xNode = compute_nodes(xMax, xMin, xInc);
	yNode = compute_nodes(yMax, yMin, yInc);
	
	nAzimuth = floor(360.0 / azimuthBinSize);

	/* allocate memory */
	if ((sorting = malloc(sizeof(sorting) * nAzimuth)) == NULL) {
		fprintf(stderr,"Error allocating sorting\n");
		exit(1);
	}
	if ((cumulative = malloc(sizeof(cumulative) * nAzimuth)) == NULL) {
		fprintf(stderr,"Error allocating cumulative\n");
		exit(1);
	}
	if ((hits = malloc(sizeof(hits) * nAzimuth)) == NULL) {
		fprintf(stderr,"Error allocating hits\n");
		exit(1);
	}	

	for (l=0; l<nAzimuth; l++) {
		cumulative[l] = 0.0;
	}

	/* try to read the input list of diagrams */
	if (( listInputDiagramFile = fopen(listInputDiagramFileName,"r")) == NULL) {
		fprintf(stderr,"Error opening file %s\n",listInputDiagramFileName);
		exit(1);
	}

	while (fgets(buff,100,listInputDiagramFile)) {
		sscanf(buff,"%s",inputDiagramFileName);
		if (( inputDiagramFile = fopen(inputDiagramFileName,"r")) == NULL) {
			fprintf(stderr,"Error, could not read %s\n",inputDiagramFileName);
			exit(1);
		}

		/* initialize the sorting diagram */
		for (l=0; l<nAzimuth; l++) {
			sorting[l] = 0.0;
			hits[l] = 0;
		}

		/* read the noise diagrams */
		for (x=0; x<xNode; x++) {
			for (y=0; y<yNode; y++) {
				fscanf(inputDiagramFile,"%f %f %f",&xTmp, &yTmp, &data);
				/* copy into sorting array */
				if (sqrtf( (xTmp * xTmp) + (yTmp * yTmp)) <= 1.0/minSpeed && sqrtf( (xTmp * xTmp) + (yTmp * yTmp)) >= 1.0/maxSpeed) {  /* checks the speed */
					azimuth =fmod(atan2(xTmp,yTmp) * 180 / PI+360.0, 360.0);
					for (i=0; i<nAzimuth; i++) {
						if (i < nAzimuth-1) {
							if (azimuth >= i*azimuthBinSize && azimuth<=(i+1) * azimuthBinSize) {
								azi = i;
								break;
							}
						} else {
								azi = nAzimuth-1;
								break;
						}
					}
					sorting[azi] = sorting[azi] + data;
					hits[azi]++;
				}
			}
		}
	
		/* we now have total power between min and max speed along each azimuth...so we normalize to the maximum and add to the cumulative array */
		maxPower = 0.0;
		for (l=0; l<nAzimuth; l++) {
			if (hits[l] > 0) {
				if (sorting[l]/(float)hits[l] > maxPower) {
					maxPower = sorting[l]/(float) hits[l];
				}
			}
		}

		/* add up so we have cumulative */	
		for (l=0; l<nAzimuth; l++) {
			if (hits[l] > 0) {
				sorting[l] = sorting[l] / (float) hits[l] / maxPower;
			} else {
				sorting[l] = 0.0;
			}
			cumulative[l] = cumulative[l] + sorting[l];
		}
		
		/* smooth the raw and added up arrays */
		smooth_1d_array(sorting,sorting,nAzimuth,nsmooth);
		smooth_1d_array(cumulative,cumulative,nAzimuth,nsmooth);

		/* output the cumulative and the sorting with the azimuth */
		sprintf(outputFileName,"%s_azi",inputDiagramFileName);
		if ((outputFile = fopen(outputFileName,"w")) == NULL) {
			fprintf(stderr,"FAIL: unable to write to file %s\n",outputFileName);
			exit(1);
		}
		for (l=0; l<nAzimuth; l++) {
			fprintf(outputFile," %3.2f %3.8f %3.8f\n",l*azimuthBinSize+azimuthBinSize/2,cumulative[l], sorting[l]);
		}
		fclose(outputFile);
	}
	
	/* done with the list file, so close it*/
	fclose(listInputDiagramFile);

	/* free up memory */
	free(sorting);
	free(cumulative);

	return 0;
}


