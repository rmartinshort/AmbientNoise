/*
 *  lib_select_dispersion.c
 *  
 *
 *  Created by Rob Porritt on 11/8/10.
 *  Copyright 2010 UC Berkeley. All rights reserved.
 *
 */

#include "select_dispersion.h"

void compute_disp_snr(SPECTRUM *spect, DISPERSION *ftan, float *snr, METADATA *met, int npts, float delta, float min, float max, float permin, float permax, float perinc,
					  SPECTRUM *spect_filt, DISPERSION *snrDispersion, 
					  fftw_complex *ff_spect, double *temp, fftw_plan plan_backward) {
	/* local variables */
	int i, n, j, n_window;
	int b_signal, e_signal, b_noise_lead, b_noise_trail, e_noise_lead, e_noise_trail;
	int count=0, n_noise = 0;
	int npow = 1, is_same_flag=0;
	int nper;
	float ave_lead=0.0, ave_trail=0.0, max_sig=0.0, sum=0.0, ave_noise = 1.0;
	float window_length, begin_signal_window, end_signal_window;
	float min_velocity, max_velocity;
	float distance = met->dist;
	float f1, f2, f3, f4, delta_omega;
	VELOCITIES snrVelocities;
	
	/* initialize the spectrum output from the filter routine */
	//spect_filt.N = spect->N;
	
	/*
	spect_filt.re = malloc(sizeof(*spect_filt.re) * spect_filt.N);
	spect_filt.im = malloc(sizeof(*spect_filt.im) * spect_filt.N);
	snrDispersion.per = malloc(sizeof(*snrDispersion.per) * ftan->N);
	snrDispersion.ph = malloc(sizeof(*snrDispersion.ph) * ftan->N);
	snrDispersion.gp = malloc(sizeof(*snrDispersion.gp) * ftan->N);
	*/
	 
	//snrDispersion.N = ftan->N;

	/* stuff for fftw */
	
	//ff_spect = fftw_malloc(sizeof(fftw_complex) * spect->N);
	n = (spect->N - 1) * 2+1;
	//temp = fftw_malloc(sizeof(*temp) * n);
	//plan_backward = fftw_plan_dft_c2r_1d(n,ff_spect,temp,FFTW_ESTIMATE);
	
	/* find the min and max velocities */
	min_velocity = min;
	max_velocity = max;
	//      printf("%f %f\n",min_velocity, max_velocity);
	if (max_velocity <= 0.0 || max_velocity > 10.0 || max_velocity < min_velocity) {
		max_velocity = 4.5;
	}
	if (min_velocity <= 0.0 || min_velocity > max_velocity || min_velocity > 10) {
		min_velocity = 1.5;
	}
	if (max_velocity <= 0.0 || max_velocity > 10.0 || max_velocity < min_velocity) {
		max_velocity = 4.5;
	}
	if (min_velocity <= 0.0 || min_velocity > max_velocity || min_velocity > 10) {
		min_velocity = 1.5;
	}
	
	/* define the signal window from the min and max read in group and phase velocities */
	if (distance < 1.0) {
		//              distance = 1.0;
		//              max_velocity = 4.0;
		//              min_velocity = 1.5;
		is_same_flag = 1;
	} else {
		is_same_flag = 0;
	}
	if (is_same_flag == 0) {
               // begin_signal_window = distance / max_velocity - disp->per[disp->N-1];
               // end_signal_window = distance / min_velocity + 2.0 * disp->per[disp->N-1];

		begin_signal_window = distance / max_velocity - ftan->per[ftan->N-1];
		end_signal_window = distance / min_velocity + 2.0 * ftan->per[ftan->N-1];
		if (begin_signal_window <= 0.0) {
			begin_signal_window = 0.0;
		}
		if (end_signal_window * delta > (float) npts) {
			end_signal_window = (float) npts / delta;
		}
		window_length = end_signal_window - begin_signal_window;
		n_window = window_length * delta;
		/*---schematic---
		 \0xxxxxlllllllllxxxxsssssssxxxxxxtttttttxxxxxxxxxx\0
		 each above character is part of the signal later returned in temp. The "x" characters are unused in snr, "l" are the leading noise window (compute average value), "s" represents signal (compute max), "t" are the trailing noise window, and "\0" are the terminal points. The location of the signal varies as above and the others have to cope with that while staying away from the terminals. Ideally each noise window is the same length as the signal window.
		 */
		b_signal = (int) floor(begin_signal_window * delta+0.5);
		e_signal = b_signal + n_window;
		
		//check that we're too close to the edge to have a leading window
		if (n_window + 10 > b_signal) {
//			printf("Stations (%s and %s) are too close (%f km) for a leading SNR window...\n",met->sta1, met->sta2,distance);
			b_noise_lead = -1;
			e_noise_lead = -1;
		} else {
			b_noise_lead = 10;
			e_noise_lead = b_noise_lead + n_window;
		}
		
		//check that we're too close to the edge to have a trailing window
		if (npts - (e_signal + n_window * 2) < 0) {
//			printf("Stations (%s and %s) are too far (%f km) for a trailing SNR window...\n",met->sta1, met->sta2, distance);
			b_noise_trail = -1;
			e_noise_trail = -1;
		} else {
			b_noise_trail = e_signal + n_window;
			e_noise_trail = b_noise_trail + n_window;
		}
		
		/* primary loop */
		for (i=0; i<ftan->N; i++) {
			//check the period and make sure we don't go beyond the limits of the array
			if (i != 0 && i != ftan->N-1) {
				f1 = 1.0 / (ftan->per[i+1] + 5.0);
				f2 = 1.0 / ftan->per[i+1];
				f3 = 1.0 / ftan->per[i-1];
				f4 = 1.0 / (ftan->per[i-1] - 5.0);
			} else if (i == 0) {
				f4 = 1.0 / (ftan->per[i] - 5.0);
				f3 = 1.0 / (ftan->per[i]- 3.0);
				f2 = 1.0 / ftan->per[i+1];
				f1 = 1.0 / (ftan->per[i+1] + 5.0);
			} else if (i == ftan->N-1) {
				f4 = 1.0 / (ftan->per[i-1]  - 5.0);
				f3 = 1.0 / ftan->per[i-1];
				f2 = 1.0 / (ftan->per[i] + 3.0);
				f1 = 1.0 / (ftan->per[i] + 5.0);
			}
			
			//printf("%d: %f %f %f %f  =  %f %f %f %f\n",i,f1,f2,f3,f4,1./f1,1./f2,1./f3,1./f4);
			
			//compute the difference in frequencies in the spectrum
			delta_omega = 1.0 / ((float) npts * delta);
			
			//filter the seismogram
			filter4(spect, spect_filt, delta_omega, npow, f1, f2, f3, f4);
			
			//drop the filtered seismogram into the array for fftw
			for (j=0; j<spect->N; j++) {
				ff_spect[j][0] = spect_filt->re[j];
				ff_spect[j][1] = spect_filt->im[j];
			}
			
			//do the ifft
			fftw_execute(plan_backward);
			for (j=0; j<n; j++) {
				temp[j] = abs(temp[j]) / (double) n;
			}
			
			//a counter for the noise computation
			n_noise = 0;
			
			//if there is a leading window, compute average
			sum = 0.0;
			count = 0;
			if (b_noise_lead > 0) {
				for (j=b_noise_lead; j<=e_noise_lead; j++) {
					sum = sum + (float) (temp[j] * temp[j]);
					count++;
				}
				ave_lead = sqrtf( sum / (float) count );
				n_noise++;
			}
			//if there is a trailing window, compute average
			sum = 0.0;
			count = 0;
			if (b_noise_trail > 0) {
				for (j=b_noise_trail; j<=e_noise_trail; j++) {
					sum = sum + (float) (temp[j] * temp[j]);
					count++;
				}
				ave_trail = sqrtf( sum / (float) count);
				n_noise++;
			}
			//compute max in the signal window
			max_sig = 0.0;
			for (j=b_signal; j<=e_signal; j++) {
				if ((float) temp[j] > max_sig) {
					max_sig = (float) temp[j];
				}
			}
			//compute average noise
			if (n_noise > 0) {
				ave_noise = (ave_lead + ave_trail) / (float) n_noise;
			} else {
				ave_noise = 1.0;
			}
			//if snr > thresshold, keep the measurement, else put in a nan value of -1
			//if ((max_sig / ave_noise) < thresshold || ave_noise <= 0.00000000) {
			//	disp->gp[i] = NAN_VALUE;
			//	disp->ph[i] = NAN_VALUE;
			//}
			//              printf("per[%d]: %f, u: %f c: %f max_sig: %f, ave_noise: %f, ave_trail: %f snr: %f\n",i,disp->per[i],disp->gp[i], disp->ph[i], max_sig, ave_noise,ave_trail, max_sig/ave_noise);
			if (ave_noise > 0.0 || ave_noise < 0.0) {
				snrDispersion->per[i] = ftan->per[i];
				snrDispersion->ph[i] = max_sig / ave_noise;
			} else {
				snrDispersion->per[i] = ftan->per[i];
				snrDispersion->ph[i] = 0.00001;
			}
		}
	}  else {
		printf("stations %s and %s are colocated [distance = %f]\n",met->sta1, met->sta2, distance);
		for (i=0; i<ftan->N; i++) {
		//	disp->gp[i] = NAN_VALUE;
		//	disp->ph[i] = NAN_VALUE;
			snrDispersion->per[i] = ftan->per[i];
			snrDispersion->ph[i] = 0.00001;
		}
	}
	interpolate_dispersion(snrDispersion, &snrVelocities, permin, permax, perinc);
	nper = floor((permax - permin) / perinc + 1.5);
	for (i=0; i<nper; i++) {
		snr[i] = snrVelocities.ph[i];
	}

	/*
	free(snrDispersion.per);
	free(snrDispersion.ph);
	free(snrDispersion.gp);
	free(spect_filt.re);
	free(spect_filt.im);
	fftw_free(temp);
	fftw_free(ff_spect);
	fftw_destroy_plan(plan_backward);
	*/
	
	return;
}


//"Tapering subroutine itself"///
void filter4(SPECTRUM *spect, SPECTRUM *spect_out, float delta_omega, int npow, float f1, float f2, float f3, float f4) {
        int i, j;
        float d1, d2, f, ss, s=0.0;

        for (i=0; i<spect->N; i++) {
                f = i * delta_omega;
                if (f <= f1) {
                        s = 0.0;
                } else if ( f <= f2) {
                        d1 = PI / (f2 - f1);
                        ss = 1.0;
                        for (j=1; j<=npow; j++) {
                                ss = ss * (1 - cos(d1 * (f1 -f))) / 2.0;
                        }
                        s = ss;
                } else if (f <= f3) {
                        s = 1.0;
                } else if (f <= f4) {
                        d2 = PI / (f4 - f3);
                        ss = 1.0;
                        for (j=1; j<=npow; j++) {
                                ss = ss * (1+cos(d2 * (f3 - f))) / 2.0;
                        }
                        s = ss;
                }
                spect_out->re[i] = spect->re[i] * s;
                spect_out->im[i] = spect->im[i] * s;
        }

        return;
}
////END



/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int read_sac (char *fname, float *sig, SAC_HD *SHD, int nmax)
/*----------------------------------------------------------------------------
 ----------------------------------------------------------------------------*/
{
	FILE *fsac;
	/*..........................................................................*/
	fsac = fopen(fname, "rb");
	if ( !fsac )
	{
		//fclose (fsac);
		return 0;
	}
	
	if ( !SHD ) SHD = &SAC_HEADER;
	
	fread(SHD,sizeof(SAC_HD),1,fsac);
	
	if ( SHD->npts > nmax )
	{
		fprintf(stderr,"ATTENTION !!! in the file %s the number of points is limited to %d\n",fname,nmax);
		
		SHD->npts = nmax;
	}
	
	fread(sig,sizeof(float),(int)(SHD->npts),fsac);
	
	fclose (fsac);
	
	/*-------------  calcule de t0  ----------------*/
	{
        int eh, em ,i;
        float fes;
        char koo[9];
		
        for ( i = 0; i < 8; i++ ) koo[i] = SHD->ko[i];
        koo[8] = '\0';
		
        SHD->o = SHD->b + SHD->nzhour*3600. + SHD->nzmin*60 +
		SHD->nzsec + SHD->nzmsec*.001;
		
        sscanf(koo,"%d%*[^0123456789]%d%*[^.0123456789]%g",&eh,&em,&fes);
		
        SHD->o  -= (eh*3600. + em*60. + fes);
		/*-------------------------------------------*/}
	return 1;
}
//////END



//checks the signal to noise ratio and only returns the values with snr greater than thresshold
void compute_snr(SPECTRUM *spect, VELOCITIES *ftan, float *snr, METADATA *met, int npts, float delta, float min, float max, float permin, float perinc) {
	/* local variables */
	int i, n, j, n_window;
	int b_signal, e_signal, b_noise_lead, b_noise_trail, e_noise_lead, e_noise_trail;
	int count=0, n_noise = 0;
	int npow = 1, is_same_flag=0;;
	float ave_lead=0.0, ave_trail=0.0, max_sig=0.0, sum=0.0, ave_noise = 1.0;
	float window_length, begin_signal_window, end_signal_window;
	float min_velocity, max_velocity;
	float distance = met->dist;
	float f1, f2, f3, f4, delta_omega;
	SPECTRUM spect_filt;
	fftw_plan plan_backward;
	fftw_complex *ff_spect;
	double *temp;
	
	/* initialize the spectrum output from the filter routine */
	spect_filt.N = spect->N;
	spect_filt.re = malloc(sizeof(spect_filt.re) * spect_filt.N);
	spect_filt.im = malloc(sizeof(spect_filt.im) * spect_filt.N);
	
	/* stuff for fftw */
	ff_spect = fftw_malloc(sizeof(fftw_complex) * spect->N);
	n = (spect->N - 1) * 2+1;
	temp = fftw_malloc(sizeof(*temp) * n);
	plan_backward = fftw_plan_dft_c2r_1d(n,ff_spect,temp,FFTW_MEASURE);
	
	/* find the min and max velocities */
	min_velocity = min;
	max_velocity = max;
	//      printf("%f %f\n",min_velocity, max_velocity);
	if (max_velocity <= 0.0 || max_velocity > 10.0 || max_velocity < min_velocity) {
		max_velocity = 4.5;
	}
	if (min_velocity <= 0.0 || min_velocity > max_velocity || min_velocity > 10) {
		min_velocity = 1.5;
	}
	if (max_velocity <= 0.0 || max_velocity > 10.0 || max_velocity < min_velocity) {
		max_velocity = 4.5;
	}
	if (min_velocity <= 0.0 || min_velocity > max_velocity || min_velocity > 10) {
		min_velocity = 1.5;
	}
	
	/* define the signal window from the min and max read in group and phase velocities */
	if (distance < 1.0) {
		//              distance = 1.0;
		//              max_velocity = 4.0;
		//              min_velocity = 1.5;
		is_same_flag = 1;
	} else {
		is_same_flag = 0;
	}
	if (is_same_flag == 0) {
               // begin_signal_window = distance / max_velocity - disp->per[disp->N-1];
               // end_signal_window = distance / min_velocity + 2.0 * disp->per[disp->N-1];

		begin_signal_window = distance / max_velocity - (permin + perinc * (ftan->N-1));
		end_signal_window = distance / min_velocity + 2.0 * (permin + perinc * (ftan->N-1));
		if (begin_signal_window <= 0.0) {
			begin_signal_window = 0.0;
		}
		if (end_signal_window * delta > (float) npts) {
			end_signal_window = (float) npts / delta;
		}
		window_length = end_signal_window - begin_signal_window;
		n_window = window_length * delta;
		/*---schematic---
		 \0xxxxxlllllllllxxxxsssssssxxxxxxtttttttxxxxxxxxxx\0
		 each above character is part of the signal later returned in temp. The "x" characters are unused in snr, "l" are the leading noise window (compute average value), "s" represents signal (compute max), "t" are the trailing noise window, and "\0" are the terminal points. The location of the signal varies as above and the others have to cope with that while staying away from the terminals. Ideally each noise window is the same length as the signal window.
		 */
		b_signal = (int) floor(begin_signal_window * delta+0.5);
		e_signal = b_signal + n_window;
		
		//check that we're too close to the edge to have a leading window
		if (n_window + 10 > b_signal) {
			printf("Stations (%s and %s) are too close (%f km) for a leading SNR window...\n",met->sta1, met->sta2,distance);
			b_noise_lead = -1;
			e_noise_lead = -1;
		} else {
			b_noise_lead = 10;
			e_noise_lead = b_noise_lead + n_window;
		}
		
		//check that we're too close to the edge to have a trailing window
		if (npts - (e_signal + n_window * 2) < 0) {
			printf("Stations (%s and %s) are too far (%f km) for a trailing SNR window...\n",met->sta1, met->sta2, distance);
			b_noise_trail = -1;
			e_noise_trail = -1;
		} else {
			b_noise_trail = e_signal + n_window;
			e_noise_trail = b_noise_trail + n_window;
		}
		
		/* primary loop */
		for (i=0; i<ftan->N; i++) {
			//check the period and make sure we don't go beyond the limits of the array
			if (i != 0 && i != ftan->N-1) {
				f1 = 1.0 / ( (permin + (perinc * (i+1))) + 5.0);
				f2 = 1.0 / (permin + (perinc * (i+1)));
				f3 = 1.0 / (permin + (perinc * (i-1)));
				f4 = 1.0 / ((permin + (perinc * (i-1))) - 5.0);
			} else if (i == 0) {
				f4 = 1.0 / ((permin + (perinc * i)) - 5.0);
				f3 = 1.0 / ((permin + (perinc * i)) - 3.0);
				f2 = 1.0 / (permin + (perinc * (i+1)));
				f1 = 1.0 / ((permin + (perinc * (i+1))) + 5.0);
			} else if (i == ftan->N-1) {
				f4 = 1.0 / ((permin + (perinc * (i-1)))  - 5.0);
				f3 = 1.0 / (permin + (perinc * (i-1)));
				f2 = 1.0 / ((permin + (perinc * i)) + 3.0);
				f1 = 1.0 / ((permin + (perinc * i)) + 5.0);
			}
			
			//printf("%d: %f %f %f %f  =  %f %f %f %f\n",i,f1,f2,f3,f4,1./f1,1./f2,1./f3,1./f4);
			
			//compute the difference in frequencies in the spectrum
			delta_omega = 1.0 / ((float) npts * delta);
			
			//filter the seismogram
			filter4(spect, &spect_filt, delta_omega, npow, f1, f2, f3, f4);
			
			//drop the filtered seismogram into the array for fftw
			for (j=0; j<spect->N; j++) {
				ff_spect[j][0] = spect_filt.re[j];
				ff_spect[j][1] = spect_filt.im[j];
			}
			
			//do the ifft
			fftw_execute(plan_backward);
			for (j=0; j<n; j++) {
				temp[j] = abs(temp[j]) / (double) n;
			}
			
			//a counter for the noise computation
			n_noise = 0;
			
			//if there is a leading window, compute average
			sum = 0.0;
			count = 0;
			if (b_noise_lead > 0) {
				for (j=b_noise_lead; j<=e_noise_lead; j++) {
					sum = sum + (float) (temp[j] * temp[j]);
					count++;
				}
				ave_lead = sqrtf( sum / (float) count );
				n_noise++;
			}
			//if there is a trailing window, compute average
			sum = 0.0;
			count = 0;
			if (b_noise_trail > 0) {
				for (j=b_noise_trail; j<=e_noise_trail; j++) {
					sum = sum + (float) (temp[j] * temp[j]);
					count++;
				}
				ave_trail = sqrtf( sum / (float) count);
				n_noise++;
			}
			//compute max in the signal window
			max_sig = 0.0;
			for (j=b_signal; j<=e_signal; j++) {
				if ((float) temp[j] > max_sig) {
					max_sig = (float) temp[j];
				}
			}
			//compute average noise
			if (n_noise > 0) {
				ave_noise = (ave_lead + ave_trail) / (float) n_noise;
			} else {
				ave_noise = 1.0;
			}
			//if snr > thresshold, keep the measurement, else put in a nan value of -1
			//if ((max_sig / ave_noise) < thresshold || ave_noise <= 0.00000000) {
			//	disp->gp[i] = NAN_VALUE;
			//	disp->ph[i] = NAN_VALUE;
			//}
			//              printf("per[%d]: %f, u: %f c: %f max_sig: %f, ave_noise: %f, ave_trail: %f snr: %f\n",i,disp->per[i],disp->gp[i], disp->ph[i], max_sig, ave_noise,ave_trail, max_sig/ave_noise);
			if (ave_noise > 0.0) {
				snr[i] = max_sig / ave_noise;
			} else {
				snr[i] = .00001;
			}
		}
	}  else {
		printf("stations %s and %s are colocated [distance = %f]\n",met->sta1, met->sta2, distance);
		for (i=0; i<ftan->N; i++) {
		//	disp->gp[i] = NAN_VALUE;
		//	disp->ph[i] = NAN_VALUE;
			snr[i] = .00001;
		}
	}
	free(spect_filt.re);
	free(spect_filt.im);
	fftw_free(temp);
	fftw_free(ff_spect);
	fftw_destroy_plan(plan_backward);
	
	return;
}
//END




//reads the ftan file
int read_ftan(char *fname, DISPERSION *disp) {
	int i,index;
	float central_period, amp, snr, tmp_period, dumgp;
	char buff[100], isnan[4];
	FILE *ff;
	
	if (! (ff = fopen(fname,"r"))) {
//		printf("Error: read_ftan: file %s not found\n",fname);
		return 0;  //if this returns 0, make sure to fill the dispersion properly
	}
	
	
	i=0;
	while (fgets(buff,100,ff)) {
		//check for nan's
		sscanf(buff,"%d %f %f %s",&index, &central_period, &tmp_period, isnan);
		if (strcmp(isnan,"nan")) {
			sscanf(buff,"%d %f %f %f %f %f %f\n",&index, &central_period, &disp->per[i], &dumgp, &disp->ph[i], &amp, &snr);
		} else {
			printf("Error, ftan file has nan's\n");
			return 0;
		}
		i++;
	}
	
	disp->N = i;
	
	fclose(ff);
	
	return 1;
}
//END

int compute_nodes(float min, float max, float inc) {
	return floor((max - min) / inc + 1.5);
}

//Fill the metadata from the header
void fill_metadata(SAC_HD *sac_header, METADATA *meta) {
	meta->lat1 = sac_header->stla;
	meta->lon1 = sac_header->stlo;
	meta->lat2 = sac_header->evla;
	meta->lon2 = sac_header->evlo;
	meta->dist = sac_header->dist;
	meta->azimuth = sac_header->az;
	meta->ndays = sac_header->user0;
	sscanf(sac_header->kstnm,"%s",meta->sta1);
	sscanf(sac_header->kevnm,"%s",meta->sta2);
	
	return;
}
//END

//interpolate the dispersion data to a standard period array
void interpolate_dispersion(DISPERSION *disp, VELOCITIES *vel_out, float permin, float permax, float perinc) {
	int i, n_per, j=0, k;
	float per, dx, dc;
	float temp_vel = 0.0;
	n_per = compute_nodes(permin, permax, perinc);
	
	for (i=0; i<n_per; i++) {
		/* the current period we're interpolating to */
		per = permin + i * perinc;
		
		/* possibly used later */
		temp_vel = 0.0;
		
		/* to prevent extrapolation */
		if (per < disp->per[0] || per > disp->per[disp->N-1]) {
			vel_out->ph[i] = NAN_VALUE;
		} else {
			/* which element of per should we use? */
			while (per > disp->per[j]) {
				j++;
			}
			/* find the change in period */
			if (j > 0) {
				dx = disp->per[j] - disp->per[j-1];
			} else {
				dx = disp->per[j];
			}
			/* do the linear interpolation for phase */
			if (j>0 && disp->ph[j] != NAN_VALUE && disp->ph[j-1] != NAN_VALUE) {
				dc = disp->ph[j] - disp->ph[j-1];
				vel_out->ph[i] = (dc / dx) * ( per - disp->per[j-1] ) + disp->ph[j-1];
			} else if (j>0 && disp->ph[j] != NAN_VALUE) {
				/* the previous value is undefined - only want the nearest periods*/
				if (per + perinc > disp->per[j]) {
					temp_vel = disp->ph[j];
					/* find most recent previous velocity */
					for (k=j; k>=0; k--) {
						if (disp->ph[k] != NAN_VALUE) {
							temp_vel = disp->ph[k];
							break;
						}
					}
					dc = temp_vel - disp->ph[j];
					vel_out->ph[i] = (dc / dx) * ( per - disp->per[j]) + disp->ph[j];
				} else {
					vel_out->ph[i] = NAN_VALUE;
				}
			} else if (j>0 && disp->ph[j-1] != NAN_VALUE) {
				/* the next value is undefined */
				if (per - perinc < disp->per[j-1]) {
					temp_vel = disp->ph[j-1];
					/* find the next velocity */
					for (k=j; k<disp->N; k++) {
						if (disp->ph[k] != NAN_VALUE) {
							temp_vel = disp->ph[k];
							break;
						}
					}
					dc = temp_vel - disp->ph[j-1];
					vel_out->ph[i] = (dc / dx) * (per - disp->per[j-1] ) + disp->ph[j-1];
				} else {
					vel_out->ph[i] = NAN_VALUE;
				}
			} else {
				vel_out->ph[i] = NAN_VALUE;
			}
		}
	}
	
	return;
}
//END

//find the minimum velocity in the dispersion profile (group velocities removed version)
float find_minimum_dispersion(DISPERSION *disp) {
	int i;
	float min_disp = 100.0;
	for (i=0; i<disp->N; i++) {
		if (min_disp > disp->ph[i] && disp->ph[i] > MIN_VEL) {
			min_disp = disp->ph[i];
		}
	}
	
	return min_disp;
}
//END


//find the maximum velocity in the dispersion profile (group velocities removed )
float find_maximum_dispersion(DISPERSION *disp) {
	int i;
	float max_disp = 0.0;
	for (i=0; i<disp->N; i++) {
		if (max_disp < disp->ph[i] && disp->ph[i] < MAX_VEL) {
			max_disp = disp->ph[i];
		}
	}
	
	return max_disp;
}
//END





