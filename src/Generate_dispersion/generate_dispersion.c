/* generate_dispersion.c

Similar to the mc_inversion program, this program is a simple forward calculator.
Given an earth structure model, it runs the dispersion calculator and returns expected group and phase velocity curves
Also requires an example dispersion curve so it knows which frequencies to calculate

*/

/*user defined header */
#include "mc_inversion_rp.h"

int main (int argc, char *argv[])
{
/* local variables declaration */
char vel_file_name[300], starting_model_file_name[300];
char outdir[50];
float latitude, longitude;
DISPERSION dispersion_input, initial_dispersion;
MODEL initial_model;

/* grab command line arguments */
     if(argc!=5) {
        fprintf(stderr, "USAGE: %s earth_model example_dispersion_curve latitude longitude\n", argv[0]);
        exit(1);
     }

// copy the argument to the parameter filename
//	strcpy(starting_model_file_name, argv[1]);
//	strcpy(vel_file_name, argv[2]);
	sscanf(argv[1],"%[A-z,0-9,.,_,/,-]",starting_model_file_name);
	sscanf(argv[2],"%[A-z,0-9,.,_,/,-]",vel_file_name);
	sscanf(argv[3],"%f",&latitude);
	sscanf(argv[4],"%f",&longitude);

//open the input data file and keep it in the datastream
	dispersion_input = read_vel_file(vel_file_name);

//initialize the model
	initial_model = read_model(starting_model_file_name);
	initial_dispersion = calculate_dispersion(&initial_model, &dispersion_input);

//make the outdir just be here
	sprintf(outdir, ".");

//output the dispersion
	output_dispersion(outdir, latitude, longitude,&initial_dispersion);	

	return 0;
}
