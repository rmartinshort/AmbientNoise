/* mc_inversion_rp.h

header file for mc_inversion_rp.c

includes key data structures and function prototypes

*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif

#define NPERIODS 100
#define NLAYERS 500
#define NMODELS 3000
#define NCONDITIONS 50
#define N_LATITUDE_NODE 200
#define N_LONGITUDE_NODE 200
#define N_PASSES 3

typedef struct dispersion
{
	int n_measurements;
	int n_phase_measurements;
	int n_group_measurements;
	float phase_period[NPERIODS], group_period[NPERIODS];
	float phase_velocity[NPERIODS], phase_error[NPERIODS];
	float group_velocity[NPERIODS], group_error[NPERIODS];
} DISPERSION;

typedef struct model
{
	float ave_error;
	int n_layers;
	float misfit;
	float depth[NLAYERS], compressional_velocity[NLAYERS], shear_velocity[NLAYERS], density[NLAYERS], error[NLAYERS];
} MODEL;

typedef struct model_family
{
	int n_models;
	float ave_error;
	MODEL models[NMODELS];
} MODEL_FAMILY;


typedef struct all_inmodels
{
	int n_models;
	int n_layers;
	int x_node, y_node, n_passes;
	MODEL input_model[N_LONGITUDE_NODE][N_LATITUDE_NODE][N_PASSES];
} ALL_INMODELS;


/*
structure describing condition on a model
 typ1, n1 - type and number of first value (type 0 - Vs, 1 - depth) val1
 typ2, n2 - type and number of second value (type 0 - Vs, 1 - depth) val2
 lim1, lim2 - limits for a condition < or >

 Typ - type of a condition:
       0 - val1 <= lim1
       1 - val1 >= lim1
       2 - lim1 <= val1 <= lim2
       3 - (val2-val1) >= lim1
       4 - (val2-val1) <= lim1
*/
typedef struct Condition
{
int Typ, typ1, n1, typ2, n2;
float lim1, lim2;
} COND;

typedef struct all_conditions
{
int Ncond;
COND cnd[NCONDITIONS];
} ALL_COND;

//external program to call the subroutine from disper.f
extern void ph_vel_ (int *idispl, int *idispr, int *nsph,
  int *mmaxin, float *din, float*ain, float *bin, float *rhoin,
  float *rat, int *iunit, float *qbinv, int *ifunc,
  int *kmax, int *mode, float *ddc, float *sone, int *igr, float *h,
  float  *t, int *iq, int *is, int *ie,
  double *c, double *cb);



/* function prototypes */
DISPERSION read_vel_file(char *dispersion_file_input_name);   //done 11/27/2009
ALL_COND read_conditions(char *conditions_file_input_name);   //done 11/27/2009
MODEL read_model(char *model_file_input_name);				  //done 11/27/2009
int verify_conditions(MODEL *model_variable, ALL_COND *conditions_variable);   //done 11_29_2009
DISPERSION calculate_dispersion(MODEL *model_variable, DISPERSION *dispersion);  //done 12_1_2009  !!!
float calculate_error_function(DISPERSION *dispersion_1, DISPERSION *dispersion_2);  //done 11_30_2009
MODEL perturb_model(MODEL *model_variable, double seed); //done 11_29_2009
MODEL_FAMILY *append_model(MODEL_FAMILY *model_family, MODEL new_model, float error); //done 11_29_2009
MODEL_FAMILY *find_top_percent_models(MODEL_FAMILY *input_family, MODEL_FAMILY *output_family, int percent_kept); //done 12_19_2009
MODEL calculate_mean_model(MODEL_FAMILY *model_family); //done 11_29_2009
ALL_INMODELS *append_input_models(ALL_INMODELS *all_input_models, MODEL model, int x, int y, int n);
ALL_INMODELS *smooth_all_input_models(ALL_INMODELS *all_input_models, int n_smoothing_nodes);
ALL_INMODELS *initialize_all_input_models(ALL_INMODELS *all_input_models, int x_node, int y_node, int n_passes);
MODEL get_smooth_model(ALL_INMODELS *all_input_models, int x, int y);
MODEL generate_model(MODEL *model, double seed);
MODEL compute_median_model(MODEL_FAMILY *model_family);

void flush_models(MODEL_FAMILY *model_family); //done 11_30_2009

void output_smooth_model(MODEL_FAMILY *model_family, char *output_directory, float latitude, float longitude, float max_depth, float depth_increment); //done 11_30_2009
//void output_layered_model(MODEL_FAMILY *model_family, char *output_directory, float latitude, float longitude);
void output_inmodel(MODEL_FAMILY *model_family, char *output_directory, float latitude, float longitude); //done 11_30_2009
void output_combo_model(MODEL_FAMILY *model_family, char *output_directory, float latitude, float longitude, float max_depth, float depth_increment, int moho_layer);  //done 11_30_2009
void output_inversion_parameters(char *output_directory, float latitude, float longitude, float initial_error, float average_error, float minimum_error, double time_to_invert_1, double time_to_invert_2, double time_to_invert_3, int n_fitting_models, float final_thresshold, int iterations); //done 11_30_20009
void output_minimum_misfit(char *output_directory, float latitude, float longitude, float minimum_misfit,DISPERSION *minimum_misfit_dispersion,MODEL *minimum_misfit_model);  //done 11_30_2009
void output_dispersion(char *output_directory, float latitude, float longitude, DISPERSION *dispersion);

//void release_memory_allocation();



