/*
 *  select_dispersion_snr_and_dist.c
 *  
 *
 *  Created by Rob Porritt on 4/28/11.
 *  Copyright 2011 UC Berkeley. All rights reserved.
 *
 */

#include "select_dispersion.h"


int main (int argc, char *argv[]) {
	
	/* local main variables */
	unsigned int i, l, n, nPer, skip_flag, lag; 
	unsigned int nPerRead, nCor, lp, nCorRead=0;
	
	float minPer, maxPer, perInc, per;
	float distanceThresshold, snrThresshold;
	float minVel, maxVel, tmpMinPer=0.0, tmpMaxPer=0.0;
	float *snrArray;
	float *signal_f;
	
	double *signal;
	
	char paramFileName[100], buff[100];
	char sacListFileName[100];
	char sacFileName[100], ftanFileName[130];
	char phOutName[100];
	char outputDirectory[100];
	
	FILE *sacListFile, *paramFile, *phOutFile;
	
	
	/* local custom types */
	DISPERSION tempDispersion;
	SPECTRUM tempSpectrum;
	METADATA meta;
	METADATA *allMet;
//	VELOCITIES allVel[MAX_COR];
	VELOCITIES *allVel;
	VELOCITIES ftanArray;
	SAC_HD header;
	
	/* fftw variables */
	fftw_plan plan_forward, plan_backward;
	fftw_complex *tempFftwComplex;
	
	/* for the snr routine */
	SPECTRUM snr_spect_filt;
	DISPERSION snrDispersion;
	fftw_complex *snr_ff_spect;
	double *snr_temp;
	
	
	/* check usage */
	if (argc != 2) {
		fprintf(stderr,"Usage: %s parameter_file\n",argv[0]);
		fprintf(stderr,"sacFileList\n");
		fprintf(stderr,"nPerFTAN lagTimeInCors\n");
		fprintf(stderr,"minPeriod maxPerid periodIncrement\n");
		fprintf(stderr,"outputDirectory\n");
		exit(1);
	}
	if (! strcmp(argv[1],"-h")) {
		fprintf(stderr,"Usage: %s parameter_file\n",argv[0]);
		fprintf(stderr,"sacFileList\n");
		fprintf(stderr,"nPerFTAN lagTimeInCors\n");
		fprintf(stderr,"minPeriod maxPerid periodIncrement\n");
		fprintf(stderr,"outputDirectory\n");
		exit(1);
	}
	
	/* copy the parameter file to a local variable and open */
	sscanf(argv[1],"%s",paramFileName);
	if (( paramFile = fopen(paramFileName,"r")) == NULL) {
		fprintf(stderr,"Error, file %s not found\n",paramFileName);
		exit(1);
	}
	
	/* read parameter file */
	l=0;
	while (fgets(buff,100,paramFile)) {
		if (l == 0) {
			sscanf(buff,"%s",sacListFileName);
			l++;
		} else if (l == 1) {
			sscanf(buff,"%d %d", &nPerRead, &lag);
			l++;
		} else if (l == 2) {
			sscanf(buff,"%f %f %f",&minPer, &maxPer, &perInc);
			l++;
		} else if (l == 3) {
			sscanf(buff,"%s",outputDirectory);
			l++;
		}
	}
	
	/* close up the parameter file */
	fclose(paramFile);
	
	/* compute nodes */
	nPer = compute_nodes(minPer, maxPer, perInc);
	tmpMinPer = minPer;
	tmpMaxPer = maxPer;
	
	/* allocate memory */
	if ((snrArray = malloc(sizeof(*snrArray) * nPer)) == NULL) {
		fprintf(stderr,"Error allocating snrArray\n");
		exit(1);
	}
	if ((tempDispersion.per = malloc(sizeof(*tempDispersion.per) * nPerRead)) == NULL) {
		fprintf(stderr,"Error allocating tempDispersion.per\n");
		exit(1);
	}
	if ((tempDispersion.ph = malloc(sizeof(*tempDispersion.ph) * nPerRead)) == NULL) {
		fprintf(stderr,"Error allocating tempDispersion.ph\n");
		exit(1);
	}
	if ((signal_f = malloc(sizeof(*signal_f) * lag)) == NULL) {
		fprintf(stderr,"Error allocating signal_f\n");
		exit(1);
	}
	if ((signal = fftw_malloc(sizeof(*signal) * lag)) == NULL) {
		fprintf(stderr,"Error allocating signal\n");
		exit(1);
	}
	if ((tempFftwComplex = fftw_malloc(sizeof(*tempFftwComplex) * lag)) == NULL) {
		fprintf(stderr,"Error allocating fftw complex\n");
		exit(1);
	}
	if ((tempSpectrum.re = malloc(sizeof(*tempSpectrum.re) * lag)) == NULL) {
		fprintf(stderr,"Error allocating tempSpectrum.re\n");
		exit(1);
	}
	if ((tempSpectrum.im = malloc(sizeof(*tempSpectrum.im) * lag)) == NULL) {
		fprintf(stderr,"Error allocating tempSpectrum.im\n");
		exit(1);
	}
	if ((snr_spect_filt.re = malloc(sizeof(*snr_spect_filt.re) * lag)) == NULL) {
		fprintf(stderr,"Error allocating snr_spect_filt.re\n");
		exit(1);
	}
	if ((snr_spect_filt.im = malloc(sizeof(*snr_spect_filt.im) * lag)) == NULL) {
		fprintf(stderr,"Error allocating snr_spect_filt.im\n");
		exit(1);
	}
	if ((snrDispersion.per = malloc(sizeof(*snrDispersion.per) * nPerRead)) == NULL) {
		fprintf(stderr,"Error allocating snrDispersion.per\n");
		exit(1);
	}
	if ((snrDispersion.ph = malloc(sizeof(*snrDispersion.ph) * nPerRead)) == NULL) {
		fprintf(stderr,"Error allocating snrDispersion.ph\n");
		exit(1);
	}
	if ((snr_temp = fftw_malloc(sizeof(*snr_temp) * lag)) == NULL) {
		fprintf(stderr,"Error allocating snr_temp\n");
		exit(1);
	}
	if ((snr_ff_spect = fftw_malloc(sizeof(*snr_ff_spect) * lag)) == NULL) {
		fprintf(stderr,"Error allocating snr_ff_spect\n");
		exit(1);
	}
	
	
	/* lots of allocation, so print a message its all allocated [many arrays, all should be small] */
	printf("Memory allocated\n");
	
	tempDispersion.N = nPerRead;
	ftanArray.N = nPer;
	snrDispersion.N = nPerRead;
	snr_spect_filt.N = (lag/2+1);
	
	
	/* create the fftw plans */
	plan_forward = fftw_plan_dft_r2c_1d ( lag, signal, tempFftwComplex, FFTW_MEASURE );
	n = lag;
	plan_backward = fftw_plan_dft_c2r_1d (n, snr_ff_spect, snr_temp, FFTW_MEASURE);
	
	printf("FFT plans ready\n");
	/* read list of sac files */
	if ((sacListFile = fopen(sacListFileName,"r")) == NULL) {
		fprintf(stderr,"Error opening %s.\n",sacListFileName);
		exit(1);
	}
	
	//little trick to read the list file first and figure out the memory needed for the big arrays
	while(fgets(buff,100,sacListFile)) {
		nCorRead++;
	}
	rewind(sacListFile);

	if ((allMet = malloc(sizeof(*allMet) * nCorRead)) == NULL) {
		fprintf(stderr,"Error allocating allMet\n");
		exit(1);
	}

	if ((allVel = malloc(sizeof(*allVel) * nCorRead)) == NULL) {
		fprintf(stderr,"Error allocating allVel");
		exit(1);
	}

	/* loop over each sac file */
	l=0;
	while (fgets(buff,100,sacListFile)) {
		sscanf(buff,"%s",sacFileName);
		if ( read_sac(sacFileName, signal_f, &(header), lag) == 0) {
			//printf("Error, file %s not found\n",sacFileName);
			skip_flag = 1;
			for (i=0; i<nPer; i++) {
				allVel[l].ph[i] = NAN_VALUE;
			}
			l++;
		} else {
			for (i=0; i<lag; i++) {
				signal[i] = (double) signal_f[i];
			}
			skip_flag = 0;
			fill_metadata(&header,&meta);
			
			allMet[l].lat1 = meta.lat1;
			allMet[l].lon1 = meta.lon1;
			allMet[l].lat2 = meta.lat2;
			allMet[l].lon2 = meta.lon2;
			allMet[l].dist = meta.dist;
			allMet[l].azimuth = meta.azimuth;
			allMet[l].ndays = meta.ndays;
			strcpy(allMet[l].sta1, meta.sta1);
			strcpy(allMet[l].sta2, meta.sta2);
		}
		
		if (skip_flag == 0) {
			/* read FTAN */
			sprintf(ftanFileName,"%s_2_DISP.1",sacFileName);
			if (read_ftan_gp(ftanFileName,&tempDispersion) == 0) {
				skip_flag = 1;
				for (i=0; i<nPer; i++) {
					allVel[l].ph[i] = NAN_VALUE;
				}
				l++;
			} else {
				skip_flag = 0;
			}
			if (skip_flag == 0) {
				
				/* get the min and max velocity */
				minVel = find_minimum_dispersion(&tempDispersion);
				maxVel = find_maximum_dispersion(&tempDispersion);
				
				/* interpolate FTAN */
				interpolate_dispersion(&tempDispersion, &ftanArray, minPer, maxPer, perInc);
				
				/* compute the fft */
				fftw_execute(plan_forward);
				
				/* load the output from the fft into a spectrum format */
				tempSpectrum.N = lag / 2 + 1;
				for (i=0; i<tempSpectrum.N; i++) {
					tempSpectrum.re[i] = tempFftwComplex[i][0];
					tempSpectrum.im[i] = tempFftwComplex[i][1];
				}
				
				/* compute SNR */
				compute_disp_snr(&tempSpectrum, &tempDispersion, snrArray, &meta, header.npts, header.delta, minVel, maxVel, minPer, maxPer, perInc,
								 &snr_spect_filt, &snrDispersion,
								 snr_ff_spect, snr_temp, plan_backward);
				
				//select
				for (i=0; i<nPer; i++) {
					snrThresshold = SNR_MIN_MID;
					distanceThresshold = DIST_MIN_MID;
					snrThresshold = 15; //was 10, then 20, then 15
					distanceThresshold = 3.0; // was 3
					printf("%f %f %f %f %f %f %f\n",snrArray[i], snrThresshold, meta.dist, distanceThresshold, (ftanArray.ph[i] * (minPer+i*perInc)),tmpMinPer, tmpMaxPer);
					
					if (snrArray[i] >= snrThresshold && meta.dist >= (distanceThresshold * (ftanArray.ph[i] * (minPer+i*perInc))) && (minPer + i * perInc) >= tmpMinPer && (minPer+i*perInc) <= tmpMaxPer) {
						allVel[l].ph[i] = ftanArray.ph[i];
					} else {
						allVel[l].ph[i] = NAN_VALUE;
					}
				}
				printf("Done with %s (%s and %s)\n",sacFileName,meta.sta1, meta.sta2);
				l++;
			} //second skip flag
		} //first skip flag
	}//each sac file
	/* end over each sac file */
	fclose(sacListFile);
	
	/* save the number read */
	nCor = l;
	printf("Finished checking %d correlation files. Preparing output in %s.\n",nCor, outputDirectory);
	
	/* loop over output periods */
	for (i=0; i<nPer; i++) {
		per = i * perInc + minPer;
		//sprintf(gpOutName,"%s/data%0.0fs_gp.txt",outputDirectory,per);
		sprintf(phOutName,"%s/data%0.0fs_gp.txt",outputDirectory,per);
		/* open output data file */
		//if ((gpOutFile = fopen(gpOutName,"w")) == NULL) {
		//	printf("Error writing file %s\n",gpOutName);
		//	exit(1);
		//}
		if ((phOutFile = fopen(phOutName,"w")) == NULL) {
			printf("Error writing file %s\n", phOutName);
			exit(1);
		}
		
		/* print to it */
		//lg = 0;
		lp = 0;
		for (l=0; l<nCor; l++) {
			if (allVel[l].ph[i] > 0.0) {
				lp++;
				fprintf(phOutFile," %d %f %f %f %f %f 0.000 1 %s %s %f %f %f\n",lp, allMet[l].lat1, allMet[l].lon1, allMet[l].lat2, allMet[l].lon2, allVel[l].ph[i],allMet[l].sta1, allMet[l].sta2,allMet[l].dist, allMet[l].azimuth, allMet[l].ndays);
		//		if (allVel[l].gp[i] > 0.0 && allVel[l].gp[i] < allVel[l].ph[i]) {
		//			lg++;
		//			fprintf(gpOutFile," %d %f %f %f %f %f 0.000 1 %s %s %f %f %f\n",lg, allMet[l].lat1, allMet[l].lon1, allMet[l].lat2, allMet[l].lon2, allVel[l].gp[i],allMet[l].sta1, allMet[l].sta2,allMet[l].dist, allMet[l].azimuth, allMet[l].ndays);
		//		}
			}
		}
		/* close file */
		fclose(phOutFile);
		//fclose(gpOutFile);
		printf("Finished writing %s with %d measurements\n",phOutName,lp);
		/* end loop */
	}
	
	/* clean up */
	/*
	 fftw_destroy_plan(plan_forward);
	 free(signal_f);
	 fftw_free(signal);
	 free(snrArray);
	 free(tempDispersion.per);
	 free(tempDispersion.ph);
	 free(tempDispersion.gp);
	 free(grade);
	 fftw_free(tempFftwComplex);
	 free(tempSpectrum.re);
	 free(tempSpectrum.im);
	 */	
	return 0;
}


