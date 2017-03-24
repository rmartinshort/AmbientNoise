/* interp_ftan.c
a program to interpolate ftan output
Only for use on the _2_DISP.1 files
*/

/* standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif
#define N_POINTS_FTAN 40
#define N_POINTS_OUT_MAX 100

int main(int argc, char *argv[])
{

	/* local variables */
	float *central_period, *apparent_period, *group_velocity, *phase_velocity;
	float *discrimination_function, *signal_to_noise_ratio;
	float *central_period_out, *apparent_period_out, *group_velocity_out, *phase_velocity_out;
	float *discrimination_function_out, *signal_to_noise_ratio_out;
	int *n_period_input_array;
	char ftan_filename[300], buff[300], out_filename[300];
	FILE *ftan_file, *out_file;
	int i=0, ii, jj=0, k, kk, l;
	float min_apparent_period, max_apparent_period, min_central_period, max_central_period;
	float temp_period_first, temp_period_second, temp_group_velocity_first, temp_group_velocity_second;
	float temp_phase_velocity_first, temp_phase_velocity_second, temp_discrimination_function_first, temp_discrimination_function_second;
	float temp_signal_to_noise_ratio_second, temp_signal_to_noise_ratio_first;

	/* check and copy command line inputs */
	if (argc!=2) {
	   fprintf(stderr, "Usage: %s [ftan_file]\n", argv[0]);
	   exit(1);
	}
	
	strcpy(ftan_filename,argv[1]);

	/* open the ftan file */
	if (! (ftan_file = fopen(ftan_filename,"r"))) {
		printf("Error: file %s not found\n", ftan_filename);
		exit(1);
	}

	/* allocate memory for the input arrays */
	central_period = (float *) malloc(N_POINTS_FTAN * sizeof(float));
	apparent_period = (float *) malloc(N_POINTS_FTAN * sizeof(float));
	group_velocity = (float *) malloc(N_POINTS_FTAN * sizeof(float));
	phase_velocity = (float *) malloc(N_POINTS_FTAN * sizeof(float));
	discrimination_function = (float *) malloc(N_POINTS_FTAN * sizeof(float));
	signal_to_noise_ratio = (float *) malloc(N_POINTS_FTAN * sizeof(float));
	n_period_input_array = (int *) malloc(N_POINTS_FTAN * sizeof(int));

	/* scan the ftan file */
	while (fgets(buff,300,ftan_file)!=NULL) {
		sscanf(buff,"  %d %f %f %f %f %f %f",&n_period_input_array[i], &central_period[i], &apparent_period[i], &group_velocity[i], &phase_velocity[i], &discrimination_function[i], &signal_to_noise_ratio[i]);
		i++;
	}

	/* close the ftan file */
	fclose(ftan_file);

	/* create the empty arrays for output */
	central_period_out = (float *) malloc(N_POINTS_OUT_MAX * sizeof(float));
	apparent_period_out = (float *) malloc(N_POINTS_OUT_MAX * sizeof(float));
	group_velocity_out = (float *) malloc(N_POINTS_OUT_MAX * sizeof(float));
	phase_velocity_out = (float *) malloc(N_POINTS_OUT_MAX * sizeof(float));
	discrimination_function_out = (float *) malloc(N_POINTS_OUT_MAX * sizeof(float));
	signal_to_noise_ratio_out = (float *) malloc(N_POINTS_OUT_MAX * sizeof(float));

	/* create the interpolated onto period array */

	//get the maximum period read
	max_apparent_period = apparent_period[i-1]; 
	max_central_period = central_period[i-1];
	//get the minimum period read
	min_apparent_period = apparent_period[0];
	min_central_period = central_period[0];

	for (ii = (int) floor(min_apparent_period)-1; ii<= (int) ceil(max_apparent_period)+1; ii++) {
		apparent_period_out[jj] = ii;
		jj++;
	}

	jj=0, ii=0;

	for (ii = (int) floor(min_central_period)-1; ii<= (int) ceil(max_central_period)+1; ii++) {
		central_period_out[jj] = ii;
		jj++;
	}

	/* interpolate the various values */
	for (k = 0; k<jj; k++) {
		for (kk = 0; kk < i-1; kk++) {
			if (central_period_out[k] >= central_period[kk] && central_period_out[k] < central_period[kk+1]) {
				temp_period_first = apparent_period[kk];
				temp_period_second = apparent_period[kk+1];
				temp_group_velocity_first = group_velocity[kk];
				temp_group_velocity_second = group_velocity[kk+1];
				temp_phase_velocity_first = phase_velocity[kk];
				temp_phase_velocity_second = phase_velocity[kk+1];
				temp_discrimination_function_first = discrimination_function[kk];
				temp_discrimination_function_second = discrimination_function[kk+1];
				temp_signal_to_noise_ratio_first = signal_to_noise_ratio[kk];
				temp_signal_to_noise_ratio_second = signal_to_noise_ratio[kk+1];
			}
		}
		group_velocity_out[k] = temp_group_velocity_first + (temp_group_velocity_second - temp_group_velocity_first) / (temp_period_second - temp_period_first) * (central_period_out[k] - temp_period_first);
		phase_velocity_out[k] = temp_phase_velocity_first + (temp_phase_velocity_second - temp_phase_velocity_first) / (temp_period_second - temp_period_first) * (central_period_out[k] - temp_period_first);
		discrimination_function_out[k] = temp_discrimination_function_first + (temp_discrimination_function_second - temp_discrimination_function_first) / (temp_period_second - temp_period_first) * (central_period_out[k] - temp_period_first);
		signal_to_noise_ratio_out[k] = temp_signal_to_noise_ratio_first + (temp_signal_to_noise_ratio_second - temp_signal_to_noise_ratio_first) / (temp_period_second - temp_period_first) * (central_period_out[k] - temp_period_first);
	}

	/* open an output file */
	strcpy(out_filename,ftan_filename);
	strcat(out_filename,"_interp");
	if (! (out_file = fopen(out_filename,"w"))) {
		printf("Error: %s file cannot be written\n", out_filename);
		exit(1);
	}

	/* write to the output file */
	for (l=0; l<jj; l++) {
		fprintf(out_file,"  %d    %4.4f    %4.4f    %4.4f    %4.4f    %4.4f    %4.4f\n", l,central_period_out[l], apparent_period_out[l],group_velocity_out[l],phase_velocity_out[l],discrimination_function_out[l],signal_to_noise_ratio_out[l]);
	}

	/* close the file */
	fclose(out_file);

	/* free the memory */
	free(n_period_input_array);
	free(central_period);
	free(apparent_period);
	free(group_velocity);
	free(phase_velocity);
	free(discrimination_function);
	free(signal_to_noise_ratio);
	free(central_period_out);
	free(apparent_period_out);
	free(group_velocity_out);
	free(phase_velocity_out);
	free(discrimination_function_out);
	free(signal_to_noise_ratio_out);

	return 0;
}

