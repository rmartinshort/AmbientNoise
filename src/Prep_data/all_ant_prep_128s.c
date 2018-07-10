/*
all_ant_prep.c

given a list of lists which each one of the second lists has all the sac files from a given station/component to merge, it then merges those sac files and prepares them for the cross correlation by removing a mean and linear trend, tapering, removing the instrument response, decimating if necessary, filtering, removing earthquakes, and whitens the spectrum returning formatted data files with the real and imaginary components.

subroutines used:
read_sac_d
write_sac_d
initialize_spectrum
compute_mean_from_array
find_number_of_blanks
fill_gap
append_array
find_and_remove_excess_samples
prep_signal
fftw_forward
generate_instrument_response
decon_response_function
flt4
fftw_backward
decimate_signal
filter_and_smooth
whiten_rp

This program is already long enough without subroutines, so subroutines in lib_prep.c and headers in lib_prep.h.

*/

#include "lib_prep.h" 

int main(int argc, char *argv[])
{
	/*   hardwired bits of code */
	// - this is a storage directory where I keep response files
//	static char resp_dir[100] = "/data/isis/rwporrit/station_responses";
	
	//these parameters are used in the response removal and filtering
	static char filter_type[3] = "BP";
	static int npow = 1, npasses = 2, order = 2, width = 128;
	//Currently setup for 50-150 seconds
	static double f1 = 1.0/170.0, f2 = 1.0/160.0, f3 = 1.0/150.0, f4=1.0/5.0, f5 = 1.0 / 4.0, f6 = 1.0/3.0;
	//frequency max and min's for the earthquake removal
	static double fmin = .02, fmax = .0667;
	//finally a parameter for the standard time step
	//desample to this time step
	static double std_dt = 1.0;
	


	/* local variables */
	int i=0, npts=0, n_blanks=0;
	int npts_cut = std_dt * 24 * 60 * 60; //number of days in the time series for 1 day at std_dt
	int jday;
	double delta=1.0, delta_omega=1.0, delta_original=1.0;
	double temp_start_y, temp_end_y, temp_mean, temp_mean_1, temp_mean_2;
	double *signal, *temp, *interp_array, *temp_re, *temp_im;
	double *cut_rec;
	double phaseShiftDt, microSecondShift;
	char locid[4], originalChannelName[4];
	char temp_channel[2]=" ", year[10];
	char list_file_name[100], buff[300], station[7],channel[4], network[3], buff_list[300], ff_name[300];
	char tmpStation[7], tmpChannel[4], tmpNetwork[3], tmpLocid[4];
	char sac_file_name[300], real_comp_file_name[50], imag_comp_file_name[50];
	char pzFileName[100];
//	clock_t c0, c1, c2, c3;
	FILE *list_file, *temp_file;
	SAC_HD sac_header, sac_header_temp, sac_header_out;
	SPECTRUM signal_spectrum, response_spectrum;

	/* check usage */
	if (argc != 2) {
                fprintf(stderr,"Usage: %s [list_file] \n",argv[0]);
		fprintf(stderr,"where list_file is a list of of lists sac files to merge and process\n");
		fprintf(stderr,"the output is a real and imaginary component of each station and channel\n");
                exit(1);
        }

	/* allocate memory */
	signal = malloc(sizeof(*signal) * NPTS_SAC_FILE_MAX);
	temp = malloc(sizeof(*temp) * NPTS_SAC_FILE_MAX);
	interp_array = malloc(sizeof(*interp_array) * NPTS_SAC_FILE_MAX);
	cut_rec = malloc(sizeof(*cut_rec) * npts_cut);
	temp_re = malloc(sizeof(*temp_re) * (NPTS_SAC_FILE_MAX/2 + 1));
	temp_im = malloc(sizeof(*temp_im) * (NPTS_SAC_FILE_MAX/2 + 1));
	initialize_spectrum(&signal_spectrum,NPTS_SAC_FILE_MAX);
	initialize_spectrum(&response_spectrum,NPTS_SAC_FILE_MAX);	

	/* copy the argument to a local variable */
	strcpy(list_file_name,argv[1]);

	/* try to open the list file */
	if (! (list_file = fopen(list_file_name,"r"))) {
                printf("Error: file %s not found\n", list_file_name);
                exit(1);
        }

	/* read the list of lists */
	while(fgets(buff_list,300,list_file)) {
        	sscanf(buff_list,"%s %s",ff_name, pzFileName);
	        if (! (temp_file = fopen(ff_name,"r"))) {
                    printf("Error: file %s not found\n", ff_name);
                    exit(1);
                }

	/* loop over each file */
	i=0;
	while (fgets(buff,300,temp_file)) {
		if (i == 0) {
			//initialize
			sscanf(buff,"%s",sac_file_name);
			if (!read_sac_d(sac_file_name,signal,&(sac_header),NPTS_SAC_FILE_MAX)) {
				fprintf(stderr,"file %s not found\n",sac_file_name);
				exit(1);
			}
			//copy some bits from the sac header
			delta = sac_header.delta;
			sscanf(sac_header.kstnm,"%[A-Z,a-z,0-9]", station);
			sscanf(sac_header.kcmpnm,"%[A-Z,a-z,0-9]", channel);
			sscanf(sac_header.knetwk,"%[A-Z,a-z,0-9]", network);
//			sprintf(locid,"%c%c%c%c",sac_header.khole[0], sac_header.khole[1], sac_header.khole[2], sac_header.khole[3]);
//			sprintf(locid,"*");
			npts = sac_header.npts;
			delta_original = delta;
			sprintf(originalChannelName,"%s",channel);

			//increment the counter
			i++;
		} else {
			//read the next file
			sscanf(buff,"%s",sac_file_name);
	//		printf("Reading: %s\n",sac_file_name);
                        if (!read_sac_d(sac_file_name,temp,&(sac_header_temp),NPTS_SAC_FILE_MAX)) {
                                fprintf(stderr,"file %s not found\n",sac_file_name);
                                exit(1);
                        }
			
			//check the station, location, network, and channel are consistent
			sscanf(sac_header_temp.kstnm,"%[A-Z,a-z,0-9]", tmpStation);
                        sscanf(sac_header_temp.kcmpnm,"%[A-Z,a-z,0-9]", tmpChannel);
                        sscanf(sac_header_temp.knetwk,"%[A-Z,a-z,0-9]", tmpNetwork);
//khole does not contain location code. There is no sac header for it - which is very frustrating
//                      sscanf(sac_header_temp.khole,"%[A-Z,a-z,0-9]", tmpLocid);
//			sprintf(tmpLocid,"%c%c%c%c",sac_header_temp.khole[0], sac_header_temp.khole[1], sac_header_temp.khole[2], sac_header_temp.khole[3]);
//			sprintf(tmpLocid,"*");
			if (strcmp(station, tmpStation)) {
				fprintf(stderr,"Warning: station %s changed to %s.\n",station, tmpStation);
				sprintf(sac_header.kstnm,"%s",tmpStation);
				sprintf(station,"%s",tmpStation);
			}
			if (strcmp(channel, tmpChannel)) {
                                fprintf(stderr,"Warning: channel %s changed to %s.\n",channel, tmpChannel);
				sprintf(sac_header.kcmpnm,"%s",tmpChannel);
                                sprintf(channel,"%s",tmpChannel);
                        }
			if (strcmp(network, tmpNetwork)) {
                                fprintf(stderr,"Warning: network %s changed to %s.\n",network, tmpNetwork);
                                sprintf(sac_header.knetwk,"%s",tmpNetwork);
                                sprintf(network,"%s",tmpNetwork);
                        }
/*
			if (strcmp(locid, tmpLocid)) {
                                fprintf(stderr,"Warning: locid %s changed to %s.\n",locid, tmpLocid);
                                sprintf(sac_header.khole,"%s",tmpLocid);
                                sprintf(locid,"%s",tmpLocid);
                        }
*/
			//find the mean of the total signal.
			temp_mean_1 = compute_mean_from_array(signal,npts);
			temp_mean_2 = compute_mean_from_array(temp,sac_header_temp.npts);
			temp_mean = (temp_mean_1 + temp_mean_2) / 2.0;

			//find the number of missing samples between signal and temp
			n_blanks = find_number_of_blanks(&sac_header,&sac_header_temp);		
			
			//for the linear fit we just need the amplitude values at the ends of the array
			temp_start_y = signal[npts-1];
			temp_end_y = temp[0];

			//compute a linear array which is a straight line between the two files
			fill_gap(interp_array,temp_start_y, temp_end_y,temp_mean,n_blanks);

			//append the linear array
			append_array(signal,npts,interp_array,n_blanks);

			//update the npts
			npts = npts + n_blanks;
			//append the temp array (new file read in)
			append_array(signal,npts,temp,sac_header_temp.npts);

			//again update the npts to account for the temp file			
			npts = npts + sac_header_temp.npts;
			//update the sac header end time
			sac_header.e = sac_header.b + npts;
		
			//update the sac header for the new npts variable
			sac_header.npts = npts;

			//increment the counter
			i++;	
		}
	}

	fclose(temp_file);

	//update information for the merged sac file
	sac_header_out = sac_header;
	sac_header_out.npts = npts;

	//format an output name
//	sprintf(sac_file_name,"%s_%s_%s.SAC",network,station,channel);
	//write out a merged sac file name
//	write_sac_d(sac_file_name,signal,&(sac_header_out));
	
	/* Now that the files are merged, we apply the standard preparation as in prep_data_for_cor.c but with the addition of decimation to 1sps after removing the response if necessary */
	
	//from the updated sac header, we get the year and jday need for evalresp
	sprintf(year,"%d,%d",sac_header.nzyear,sac_header.nzjday);

	//remove mean, trend, and apply 10%cosine taper
	prep_signal(signal,signal,npts);

	//fft to prepare for response removal
	signal_spectrum.N = npts / 2 + 1;
	signal_spectrum.log_N = floor(log_n((npts/2+1),2)+0.5);

	fftw_forward(signal,&signal_spectrum,npts);

	
	//remove response
                //step 1: generate the spectrum
		response_spectrum.N = npts/2+1;
		response_spectrum.log_N = signal_spectrum.log_N;
		//define the step in frequency, delta_omega
		delta_omega = 1.0 / (npts * delta);
		//generate_instrument_response(&response_spectrum,resp_dir, station, channel, locid, network, year, delta, npts);
                generate_pz_response_spectrum(&response_spectrum, pzFileName, delta_omega);
		//step 2: deconvolve
	        decon_response_function(&signal_spectrum, &response_spectrum);

		
		//first fitler for the instrument
//		flt4(&signal_spectrum,&signal_spectrum,delta_omega,npow,f1,f2,f5,f6);

//		fftw_backward(&signal_spectrum, interp_array);
//		write_sac_d("temp.rp.sac",interp_array,&(sac_header));	

		//second filter for a bit of a multi-taper effect (maybe - its what cub does)
		flt4(&signal_spectrum,&signal_spectrum, delta_omega, npow, f2, f3, f4, f5);		
		
	//finally before returning temporarily to the time domain, we shift so the record starts at an even second
	phaseShiftDt = (double) sac_header.delta;
	microSecondShift = -1.0 * (double) sac_header.nzmsec / 1000.0;
	phase_shift(microSecondShift, phaseShiftDt, npts, &signal_spectrum);
	sac_header.nzmsec = 0;
	
	// return to the time domain for decimation (if necessary) and then removing earthquakes
	fftw_backward(&signal_spectrum,temp);

	//check if we need to decimate and do if necessary
	if (delta < std_dt) {
		decimate_signal(temp, temp, npts, delta, std_dt);
		sac_header.delta = std_dt;
		delta = std_dt;
		sac_header.npts = (int) floor((double) npts * delta_original / std_dt + 0.5); 
	}

	//re-initialize signal_spectrum
	for (i=0; i<signal_spectrum.N; i++) {
		signal_spectrum.re[i] = 0.0;
		signal_spectrum.im[i] = 0.0;
	}

	//cut off the beginning and end so the start is exactly 00:00:00.0000 and the end is exactly 23:59:59.0 of the day.
	if (sac_header.nzhour > 20) {
		jday = sac_header.nzjday + 1;
	} else {
		jday = sac_header.nzjday;
	}
	normalize_record_length(temp,cut_rec,&sac_header,npts_cut,jday);
//	cut_record(temp,cut_rec,&sac_header, npts_cut);   //this option goes from 00:30:00 to 23:30:00

	//the number of points is reset by cut_record
	npts = sac_header.npts;
	signal_spectrum.N = npts / 2 + 1;
	delta_omega = 1.0 / ( npts * delta);

	//temporary write statement
//	write_sac_d("temp.sac",temp,&(sac_header));

	//this effectively removes earthquakes
//	filter_and_smooth(temp, temp, npts, order, filter_type, fmin, fmax, delta, npasses,width);
	filter_and_smooth(cut_rec, cut_rec, npts, order, filter_type, fmin, fmax, delta, npasses, width);

	//write_sac_d("temp.sac",temp,&(sac_header));

	//return to the frequency domain and whiten
//	fftw_forward(temp,&signal_spectrum,npts);
	fftw_forward(cut_rec, &signal_spectrum, npts);
	whiten_rp(&signal_spectrum,&signal_spectrum,npts,npow,delta,f2,f3,f4,f5);
	
//	fftw_backward(&signal_spectrum,temp);
//	write_sac_d("temp.sac",temp,&(sac_header));

	//fill some user variables with information from the timeseries
	sac_header.user1 = npts;
	sac_header.user2 = delta;
	sac_header.user3 = delta_original;
	sac_header.npts = signal_spectrum.N;
	sac_header.delta = delta_omega;
	
	//finally use the spectrum to create final arrays which to write out
	//fill the new arrays
	for (i=0; i<signal_spectrum.N; i++) {
		temp_re[i] = signal_spectrum.re[i];
		temp_im[i] = signal_spectrum.im[i];
	}

	//the channel name will vary based on the std_dt given
	//These should not change because this is a standard convention!
	if (std_dt == 1.0) {
		sprintf(temp_channel,"L");
	} else if (std_dt < 1.0 && std_dt >= 0.02) {
		sprintf(temp_channel,"B");
	} else if (std_dt < 0.02) {
		sprintf(temp_channel,"H");
	} else if (std_dt > 1.0 && std_dt <= 100.0) {
		sprintf(temp_channel,"V");
	} else if (std_dt > 100.0) {
		sprintf(temp_channel,"U");
	}

	//printf("cmpinc: %f cmpaz: %f\n",sac_header.cmpinc, sac_header.cmpaz);

	//check if vertical, north, or east
	if (sac_header.cmpinc == 0) {
		sprintf(channel,"%sHZ",temp_channel);
	} else if (sac_header.cmpaz == 0) {
		sprintf(channel,"%sHN",temp_channel);
	} else if (sac_header.cmpinc == -12345 || sac_header.cmpaz == -12345) {
	        sprintf(channel,"%sHZ",temp_channel);
		sac_header.cmpinc = 0;
		sac_header.cmpaz = 0;
		fprintf(stderr,"Warning: missing component inclination and/or azimuth. Setting to vertical!\n");
	}  else {
		sprintf(channel,"%sHE",temp_channel);
	}

//	found some oddities with the ELW and WISH metadata
	if (!(strcmp(station,"ELW"))) {
		if (sac_header.cmpinc == 90) {
	            sprintf(channel,"%sHZ",temp_channel);
		    sac_header.cmpinc = 0.0;
    	        } else if (sac_header.cmpaz == 0) {
            	    sprintf(channel,"%sHN",temp_channel);
	        } else {
         	    sprintf(channel,"%sHE",temp_channel);
	        }
        }
	if (!(strcmp(station,"WISH"))) {
		if (sac_header.cmpaz == 90.0) {
                	sac_header.cmpaz = 0.0;
		}
        }

	//print the channel name into the header
	sprintf(sac_header.kcmpnm,"%s",channel);

	//prepare some output filenames - in the event of two location ID's, the earlier one is overwritten
        sprintf(real_comp_file_name,"%s_%s_%s.SAC_re",network,station,channel);
        sprintf(imag_comp_file_name,"%s_%s_%s.SAC_im",network,station,channel);
	
	//write the new sac files
        write_sac_d(real_comp_file_name,temp_re,&(sac_header));
        write_sac_d(imag_comp_file_name,temp_im,&(sac_header));
	
	//get_amplitude_spectrum(&signal_spectrum,temp_re);
	//write_sac_d("temp.am",temp_re,&(sac_header));


	printf("Done with net %s station %s channel %s (Original: %s) location ID -- day %d\n",network,station,channel,originalChannelName, sac_header.nzjday);

	}

	fclose(list_file);
// the OS does these free statements and leaving them in causes a crash at the end of executuion
//	free(signal);
//	free(temp);
//	free(interp_array);
//	free(cut_rec);
//	free_spectrum(&signal_spectrum);
//	free_spectrum(&response_spectrum);
//	free(temp_re);
//	free(temp_im);		



	return 0;
}


