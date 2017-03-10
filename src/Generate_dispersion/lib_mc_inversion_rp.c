/* lib_mc_inversion_rp.c

library functions for mc_inversion_rp
The function prototypes are defined in mc_inversion_rp.h

*/
#include "mc_inversion_rp.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif

///compute_median_error///


///compute_median_model///
MODEL compute_median_model(MODEL_FAMILY *model_family) {
//basically a bubble sort algorithm based on http://c-pgms.blogspot.com/2008/08/program-to-find-meanmedianand-mode_22.html
/*
ORIGINAL FUNCTION
float median_function(float a[],int n)
{
float temp;
int i,j;
for(i=0;i<n;i++)
for(j=i+1;j<n;j++)
{
if(a[i]>a[j])
{
temp=a[j];
a[j]=a[i];
a[i]=temp;
}
}
if(n%2==0)
return (a[n/2]+a[n/2-1])/2;
else
return a[n/2];
}
*/
MODEL temp;
MODEL median_model;
int i,j;
int n, out_index;
n = model_family->n_models;
for(i=0;i<n;i++) {
	for(j=i+1;j<n;j++) {
		if (model_family->models[i].misfit > model_family->models[j].misfit) {
			temp = model_family->models[j];
			model_family->models[j] = model_family->models[i];
			model_family->models[i] = temp;
		}
	}
}
out_index = floor(n/2+0.5);

median_model = model_family->models[out_index];


//my own typical algorithm to compute standard deviation. This time looking at variation from median in place of mean
int ii;
float diff[model_family->n_models], sum_error = 0.0, sum_ave_error = 0.0;
for (i=0; i<(model_family->models[0].n_layers); i++) {
 for (ii=0; ii<model_family->n_models; ii++) {
	diff[ii] = model_family->models[ii].shear_velocity[i] - median_model.shear_velocity[i];
	sum_error = sum_error + diff[ii] * diff[ii];
  }
  median_model.error[i] = sqrt(sum_error / model_family->n_models);
  sum_ave_error = sum_ave_error + median_model.error[i];
}

median_model.ave_error = sum_ave_error / model_family->models[0].n_layers;

return median_model;

}
////END////

//////read_vel_file
DISPERSION read_vel_file(char *dispersion_file_input_name) {
// local variables
FILE *dispersion_file;
DISPERSION dispersion_output;
char buff[100];
int l, dummy1, group_or_phase_flag, dummy2, temp_period;
int phase_index = 0, group_index = 0;
float temp_velocity, temp_error;

//open the dispersion file
		if (! (dispersion_file = fopen(dispersion_file_input_name,"r"))) {
                printf("Error: file %s not found\n", dispersion_file_input_name);
                exit(1);
        }
        
//read the dispersion file
l=0;
while(fgets(buff,100,dispersion_file)) {
	if (l != 0) {
		sscanf(buff,"%d %d %d %d %f %f\n",&dummy1, &group_or_phase_flag, &dummy2, &temp_period, &temp_velocity, &temp_error);
		//if dummy1 == 2 -> rayleigh. if dummy1 == 1 -> love. dummy2 is the mode branch. Ambient noise only returns 1 (fundamental mode)
//		printf("dummy1: %d, group_or_phase_flag: %d, dummy2: %d, temp_period: %d, temp_velocity: %3.5f, temp_error: %3.5f\n", dummy1, group_or_phase_flag, dummy2, temp_period, temp_velocity, temp_error);

		if (group_or_phase_flag == 1) {
			//phase velocity
			dispersion_output.phase_velocity[phase_index] = temp_velocity;
			dispersion_output.phase_error[phase_index] = temp_error*3.0;
			dispersion_output.phase_period[phase_index] = (float) temp_period * 1.0;
			phase_index++;
		} else {
			dispersion_output.group_velocity[group_index] = temp_velocity;
			dispersion_output.group_error[group_index] = temp_error*3.0;
			dispersion_output.group_period[group_index] = (float) temp_period * 1.0;
			group_index++;
		}
		l++;
	} else {
		l++;
	}
}
	dispersion_output.n_phase_measurements = phase_index;
	dispersion_output.n_group_measurements = group_index;
	
fclose(dispersion_file);

dispersion_output.n_measurements = group_index + phase_index;   

//printf("dispersion_output.group_period[3]: %3.1f\n", dispersion_output.group_period[3]);
//return the dispersion structure        
return dispersion_output;
}
//////////END////////////


///////read_conditions /////////
ALL_COND read_conditions(char *conditions_file_name) {
FILE *conditions_file;
ALL_COND conditions_output;
char buff[100];

		if (! (conditions_file = fopen(conditions_file_name,"r"))) {
                printf("Error: file %s not found\n", conditions_file_name);
                conditions_output.Ncond = 0;
                return conditions_output;
        }
conditions_output.Ncond = 0;

while(fgets(buff,100,conditions_file)) {
	sscanf(buff, "%d %d %d %d %d %f %f", &conditions_output.cnd[conditions_output.Ncond].Typ, &conditions_output.cnd[conditions_output.Ncond].typ1, &conditions_output.cnd[conditions_output.Ncond].n1, &conditions_output.cnd[conditions_output.Ncond].typ2, &conditions_output.cnd[conditions_output.Ncond].n2, &conditions_output.cnd[conditions_output.Ncond].lim1, &conditions_output.cnd[conditions_output.Ncond].lim2);
	conditions_output.Ncond++;
}

fclose(conditions_file);

return conditions_output;	
}
////////////  END //////


////////////  read_model    ////
MODEL read_model(char *model_file_name) {
FILE *model_file;
MODEL model_output;
int layer = 0;
char buff[100];
float sum_ave_error=0.0;
		
	if (! (model_file = fopen(model_file_name,"r"))) {
                printf("Error: file %s not found\n", model_file_name);
                exit(1);
        }
        
while(fgets(buff,100,model_file)) {
	sscanf(buff,"%f %f %f %f %f", &model_output.depth[layer], &model_output.compressional_velocity[layer], &model_output.shear_velocity[layer], &model_output.density[layer], &model_output.error[layer]);
        sum_ave_error = sum_ave_error + model_output.error[layer];
	layer++; 
}
model_output.n_layers = layer;
model_output.ave_error = sum_ave_error / layer;

fclose(model_file);



return model_output;
}       
/////// END ///////////


///// verify_conditions ////
int verify_conditions(MODEL *model, ALL_COND *conditions) {
int i;
float val1, val2;
COND *temp_condition;

//loop over each condition
for (i=0; i<conditions->Ncond;i++) {
//loop over each layer
	temp_condition = &(conditions->cnd[i]);
	val1 = 999999.0;
	val2 = -999999.0;
	//check the condition
	if (temp_condition->n1 < model->n_layers && temp_condition->typ1 == 0) {
		val1 = model->shear_velocity[temp_condition->n1];
	}
	if (temp_condition->n1 < model->n_layers && temp_condition->typ1 == 1) {
		val1 = model->depth[temp_condition->n1];
	}
	if (temp_condition->n1 < model->n_layers && temp_condition->typ2 == 0) {
		val2 = model->shear_velocity[temp_condition->n1];
	}
	if (temp_condition->n1 < model->n_layers && temp_condition->typ2 == 1) {
		val2 = model->depth[temp_condition->n1];
	}
	//printf("val1: %3.5f, val2: %3.5f\n", model->thickness[temp_condition->n1], model->shear_velocity[temp_condition->n1]);
 	if ( (temp_condition->Typ == 0) && (val1 > temp_condition->lim1) ) {
// 		printf("0 val1: %f, val2: %f\n", val1, val2);
	 	 return 0;
	 }
	 if ( (temp_condition->Typ == 1) && (val1 < temp_condition->lim1) ) {
// 		printf("1 val1: %f, val2: %f\n", val1, temp_condition->lim1);
 		return 0;
	 }
 	if ( (temp_condition->Typ == 2) && (val1 < temp_condition->lim1 || val1 > temp_condition->lim2) ) {
// 		printf("2 val1: %f, val2: %f\n", val1, val2);
 		return 0;
	 }
 	if ( (temp_condition->Typ == 3) && ((val2-val1) < temp_condition->lim1) ) {
// 		printf("3 val1: %f, val2: %f\n", val1, val2);
 		return 0;
 	}
	if ( (temp_condition->Typ == 4) && ((val2-val1) > temp_condition->lim1) ) {
//		printf("4 val1: %f, val2: %f\n", val1, val2);
 		return 0;
 	}		
}
	return 1;
}
///END////
////////// generate_model/////
MODEL generate_model(MODEL *model, double seed) {
int i;
MODEL new_model;
int moho_layer = 6;
float depth_remaining = 0.0;
float vp_vs_ratio, density_vs_ratio;

new_model.n_layers = model->n_layers;
new_model.ave_error = model->ave_error;
srand48(seed);

//generates the velocity of each layer
for (i=0; i<new_model.n_layers; i++) {
  vp_vs_ratio = model->compressional_velocity[i] / model->shear_velocity[i];
  density_vs_ratio = model->density[i] / model->shear_velocity[i];
  if (i == 0) {
    new_model.shear_velocity[i] = (3.5 - 1.2) * drand48() + 1.2;
    new_model.compressional_velocity[i] = new_model.shear_velocity[i] * vp_vs_ratio;
    new_model.density[i] = new_model.shear_velocity[i] * density_vs_ratio;
    new_model.depth[i] = 0.0;
  } else if (i < moho_layer) {
    new_model.shear_velocity[i] = (4.0 - new_model.shear_velocity[i-1]) * drand48() + new_model.shear_velocity[i-1];
    new_model.compressional_velocity[i] = new_model.shear_velocity[i] * vp_vs_ratio;
    new_model.density[i] = new_model.shear_velocity[i] * density_vs_ratio;
  } else if (i == new_model.n_layers - 1) {
    new_model.shear_velocity[i] = 4.5;
    new_model.compressional_velocity[i] = new_model.shear_velocity[i] * vp_vs_ratio;
    new_model.density[i] = new_model.shear_velocity[i] * density_vs_ratio;
  } else {
    new_model.shear_velocity[i] = (4.7 - 4.1) * drand48() + 4.1;
    new_model.compressional_velocity[i] = new_model.shear_velocity[i] * vp_vs_ratio;
    new_model.density[i] = new_model.shear_velocity[i] * density_vs_ratio;
  }
}
//now we generate a value for the moho
new_model.depth[moho_layer] = 25.0 * drand48()+10.0;
depth_remaining = new_model.depth[moho_layer];

//now the sum thickness of the crustal layers (i=0 to i=5) must equal the moho layer value and each layer in the mantle is a relatively constant value after that
for (i=1; i<new_model.n_layers; i++) {
  if (i != moho_layer) {
   //crust
    if (i < moho_layer) {
      new_model.depth[i] = new_model.depth[moho_layer] / 6.0 * drand48() + new_model.depth[i-1];
      depth_remaining = depth_remaining - new_model.depth[i];
   //mantle
    } else if (i == new_model.n_layers - 1) {
      new_model.depth[i] = 400.0;
    } else {
      new_model.depth[i] = new_model.depth[i-1] + 5.0 * drand48() + 5.0;
    }   
  }
}

//for (i=0; i<(new_model.n_layers); i++) {
//  printf("%3.2f %3.5f %3.5f %3.5f %3.5f\n", new_model.depth[i], new_model.compressional_velocity[i], new_model.shear_velocity[i], new_model.density[i], new_model.error[i]);
//}


return new_model;
}
////END/////

////////// perturb_model //////////
/// version 2///
MODEL perturb_model(MODEL *model, double seed) {
//combination of generate_model and perturb_model_v1
//choose vel_max, vel_min, depth_min, depth_max based on either conditions and/or error bounds
int i, moho_layer = 6;
MODEL new_model;
float vel_min = 0.0, vel_max = 0.0, depth_min = 0.0, depth_max = 0.0;
float perturbation, vp_vs_ratio, density_vs_ratio;
float moho_depth_perturbation, depth_remaining;
float temp=0.0, temp2=0.0;

srand48(seed);
new_model.n_layers = model->n_layers;
new_model.ave_error = model->ave_error;

for (i=0; i<new_model.n_layers; i++) {
 if (i == 0) {
   //surface sedimentary layer
   vp_vs_ratio = model->compressional_velocity[i] / model->shear_velocity[i];
   density_vs_ratio = model->density[i] / model->shear_velocity[i];
   temp = model->shear_velocity[i] - model->error[i];
   if (temp > 1.2) {
     vel_min = temp;
   } else {
     vel_min = 1.2;
   }
   temp = model->shear_velocity[i]+model->error[i];
   if (temp > 3.5) {
     vel_max = 3.5;
   } else {
     vel_max = temp;
   }
   depth_min = 0.0;
   depth_max = 0.0;
   new_model.depth[i] = 0.0;
   new_model.shear_velocity[i] = (vel_max - vel_min) * drand48() + vel_min;
   new_model.compressional_velocity[i] = new_model.shear_velocity[i] * vp_vs_ratio;
   new_model.density[i] = new_model.shear_velocity[i] * density_vs_ratio;
   } else if (i == new_model.n_layers - 1) {
   //deep mantle layer
   vp_vs_ratio = model->compressional_velocity[i] / model->shear_velocity[i];
   density_vs_ratio = model->density[i] / model->shear_velocity[i];
   temp = model->shear_velocity[i]-model->error[i];
   if ( temp > 4.48) {
     vel_min = temp;
   } else {
     vel_min = 4.48;
   }
   temp = model->shear_velocity[i]+model->error[i];
   if (temp > 4.52) {
     vel_max = 4.52;
   } else {
     vel_max = temp;
   }
   depth_min = 400.0;
   depth_max = 400.0;
   new_model.depth[i] = 400.0;
   new_model.shear_velocity[i] = (vel_max - vel_min) * drand48() + vel_min;
   new_model.compressional_velocity[i] = new_model.shear_velocity[i] * vp_vs_ratio;
   new_model.density[i] = new_model.shear_velocity[i] * density_vs_ratio;
 } else if (i < moho_layer)  {
   //crust
   vp_vs_ratio = model->compressional_velocity[i] / model->shear_velocity[i];
   density_vs_ratio = model->density[i] / model->shear_velocity[i];
   temp = model->shear_velocity[i]-model->error[i];
   temp2 = new_model.shear_velocity[i-1];
   if (temp > temp2) {
     vel_min = temp;
   } else {
     vel_min = temp2;
   }
   temp = model->shear_velocity[i]+model->error[i];
   if (temp > 4.0) {
     vel_max = 4.0;
   } else {
     vel_max = temp;
   }   
   new_model.shear_velocity[i] = (vel_max - vel_min) * drand48() + vel_min;
   new_model.compressional_velocity[i] = new_model.shear_velocity[i] * vp_vs_ratio;
   new_model.density[i] = new_model.shear_velocity[i] * density_vs_ratio;

 } else {
   //typical mantle
   vp_vs_ratio = model->compressional_velocity[i] / model->shear_velocity[i];
   density_vs_ratio = model->density[i] / model->shear_velocity[i];
   temp = model->shear_velocity[i]-model->error[i];
   if (temp > 4.1) {
     vel_min = temp;
   } else {
     vel_min = 4.1;
   }
   temp = model->shear_velocity[i]+model->error[i];
   if (temp > 4.7) {
     vel_max = 4.7;
   } else {
     vel_max = temp;
   } 
   new_model.shear_velocity[i] = (vel_max - vel_min) * drand48() + vel_min;
   new_model.compressional_velocity[i] = new_model.shear_velocity[i] * vp_vs_ratio;
   new_model.density[i] = new_model.shear_velocity[i] * density_vs_ratio;

 }
}
//redefine the moho layer depth
if (drand48() < .5) {
  moho_depth_perturbation = drand48() * -5.0;
} else {
  moho_depth_perturbation = drand48() * 5.0;
}
new_model.depth[moho_layer] = model->depth[moho_layer] + moho_depth_perturbation;
depth_remaining = new_model.depth[moho_layer];

//now the sum thickness of the crustal layers (i=0 to i=5) must equal the moho layer value and each layer in the mantle is a relatively constant value after that
for (i=1; i<new_model.n_layers-1; i++) {
  if (i != moho_layer) {
   //crust
    if (i < moho_layer) {
      new_model.depth[i] = new_model.depth[moho_layer] / 6.0 * drand48() + new_model.depth[i-1];
      depth_remaining = depth_remaining - new_model.depth[i];
   //mantle
    } else {
      new_model.depth[i] = new_model.depth[i-1] + 5.0 * drand48() + 5.0;
    }
  }
}

//for (i=0; i<(new_model.n_layers); i++) {
//  printf("%3.2f %3.5f %3.5f %3.5f %3.5f\n", new_model.depth[i], new_model.compressional_velocity[i], new_model.shear_velocity[i], new_model.density[i], new_model.error[i]);
//}

return new_model;
} 

/// version 1///
/*
MODEL perturb_model(MODEL *model, double seed) {
int i;
MODEL new_model;
float perturbation, sign;
float vp_vs_ratio, density_vs_ratio;
//time_t t1;

//seed the random variable
//(void) time(&t1);
srand48((double) seed);

for (i=0; i<model->n_layers; i++) {
	if (drand48() < .5) {
		sign = -1.0;
	} else {
		sign = 1.0;
	}
	perturbation = drand48() * sign * model->error[i];
//printf("perturbation: %3.5f\n", perturbation);
	vp_vs_ratio = model->compressional_velocity[i] / model->shear_velocity[i];
	density_vs_ratio = model->density[i] / model->shear_velocity[i];
	new_model.shear_velocity[i] = model->shear_velocity[i] + perturbation;
	new_model.compressional_velocity[i] = new_model.shear_velocity[i] * vp_vs_ratio;
	if (i == 0) {
		new_model.depth[i] = fabs(model->depth[i] + (perturbation / model->error[i]) * model->depth[i] / 10);	
	}  else {
		new_model.depth[i] = model->depth[i] + (perturbation / model->error[i]) * (model->depth[i] - model->depth[i-1]);
		if (new_model.depth[i] < new_model.depth[i-1]) {
			new_model.depth[i] = new_model.depth[i-1] + .1;
		}
	}
	new_model.density[i] = new_model.shear_velocity[i] * density_vs_ratio;
	new_model.error[i] = model->error[i];
}
new_model.n_layers = model->n_layers;
//for (i=0; i<(new_model.n_layers); i++) {
//  printf("%3.2f %3.5f %3.5f %3.5f %3.5f\n", new_model.depth[i], new_model.compressional_velocity[i], new_model.shear_velocity[i], new_model.density[i], new_model.error[i]);
//}


return new_model;	
}
*/
///  END ///

/// append_model ///
MODEL_FAMILY *append_model(MODEL_FAMILY *model_family, MODEL new_model, float error) {

//printf("model_family.n_models: %d\n", model_family->n_models);
	float sum;

	model_family->models[model_family->n_models] = new_model;
	model_family->n_models = model_family->n_models + 1;
	
	if (model_family->n_models == 1) {
		model_family->ave_error = error;
	} else {
		sum = model_family->ave_error * (model_family->n_models-1);
		sum = sum + error;
		model_family->ave_error = sum / model_family->n_models;
	}

//printf("appending model number %d.\n", model_family->n_models);
return model_family;	
}
///  END ///


///////find_top_percent_models
MODEL_FAMILY *find_top_percent_models(MODEL_FAMILY *input_family, MODEL_FAMILY *output_family, int percent_kept)
{
	
	//This version doesn't actually use the percent_kept value...
	int i, j, n, out_index;
	float temp;
	float median_misfit;
	float sum = 0.0, ave_misfit = 0.0; 
	n=input_family->n_models;
	
	//first loop: compute the median misfit
/*	for (i=0; i<n; i++) {
		for (j=i+1; j<n; j++) {
			if (input_family->models[i].misfit > input_family->models[j].misfit) {
				temp = input_family->models[j].misfit;
				input_family->models[j].misfit = input_family->models[i].misfit;
				input_family->models[i].misfit = temp;
			}
		}
	}
	out_index = floor(n/2+0.5);
	median_misfit = input_family->models[out_index].misfit;
*/
	//first loop: compute the mean misfit
	for (i=0; i<n; i++) {
		sum = sum + input_family->models[i].misfit;
	}
	ave_misfit = sum / n;
	
	//second loop: if the misfit is less than the average, append it
	for (i=0; i<input_family->n_models; i++) {
//		if (input_family->models[i].misfit <= median_misfit) {
		if (input_family->models[i].misfit < ave_misfit) {
			output_family = append_model(output_family,input_family->models[i],input_family->models[i].misfit);
		}
	}
	printf("n_models_kept: %d\n",output_family->n_models);
	
return output_family;	
}


/// END ///

///  calculate_mean_model ///
MODEL calculate_mean_model(MODEL_FAMILY *model_family) {
int i, ii;
float sum_depth, sum_compressional_velocity, sum_shear_velocity, sum_density, sum_error;
MODEL new_model;
float diff[model_family->n_models];
float sum_ave_error=0.0;

new_model.n_layers = model_family->models[0].n_layers;

for (i=0; i<(model_family->models[0].n_layers); i++) {
  sum_depth = 0.0;
  sum_compressional_velocity = 0.0;
  sum_shear_velocity = 0.0;
  sum_density = 0.0;
  sum_error = 0.0;
  for (ii=0; ii<model_family->n_models; ii++) {
  	sum_depth = sum_depth + model_family->models[ii].depth[i];
  	sum_compressional_velocity = sum_compressional_velocity + model_family->models[ii].compressional_velocity[i];
  	sum_shear_velocity = sum_shear_velocity + model_family->models[ii].shear_velocity[i];
  	sum_density = sum_density + model_family->models[ii].density[i];
 	//sum_error = sum_error + model_family->models[ii].error[i];
//instead should calculate the standard deviation of the model family
  }
  new_model.depth[i] = (sum_depth / (float) model_family->n_models);
  new_model.compressional_velocity[i] = sum_compressional_velocity / (float) model_family->n_models;
  new_model.shear_velocity[i] = sum_shear_velocity / (float) model_family->n_models;
  new_model.density[i] = sum_density / (float) model_family->n_models;
//  new_model.error[i] = sum_error / (float) model_family->n_models;	
  for (ii=0; ii<model_family->n_models; ii++) {
	diff[ii] = model_family->models[ii].shear_velocity[i] - new_model.shear_velocity[i];
	sum_error = sum_error + diff[ii] * diff[ii];
  }
  new_model.error[i] = sqrt(sum_error / model_family->n_models);
  sum_ave_error = sum_ave_error + new_model.error[i];
}

new_model.ave_error = sum_ave_error / model_family->models[0].n_layers;

//for (i=0; i<(new_model.n_layers); i++) {
//  printf("%3.2f %3.5f %3.5f %3.5f %3.5f\n", new_model.depth[i], new_model.compressional_velocity[i], new_model.shear_velocity[i], new_model.density[i], new_model.error[i]);
//}

return new_model;
}
//// END/////

/// flush_models ////
void flush_models(MODEL_FAMILY *model_family)
{
	int i, ii;
	
	for (i=0; i<model_family->n_models; i++) {
		for (ii = 0; ii<model_family->models[i].n_layers; ii++) {
			model_family->models[i].depth[ii] = 0.0;
			model_family->models[i].compressional_velocity[ii] = 0.0;
			model_family->models[i].shear_velocity[ii] = 0.0;
			model_family->models[i].density[ii] = 0.0;
			model_family->models[i].error[ii] = 0.0;
		}
	}
	model_family->n_models = 0;
        model_family->ave_error = 0.0;	
return;
}
//// END ////


/// output_smooth_model ////
void output_smooth_model(MODEL_FAMILY *model_family, char *output_dir, float latitude, float longitude, float max_depth, float depth_increment)
{
	
	int i, ii, iii;
	int n_depths;
	float *depth, *shear_velocity, *error;
	float sum_shear_velocity, sum_error;
	char output_file_name[300];
	FILE *output_file;
	
	sprintf(output_file_name,"%s/%3.1f_%3.1f_smooth_model.dat",output_dir,latitude,longitude);
	if (! (output_file = fopen(output_file_name, "w"))) {
		printf("file %s could not be opened for writing.\n", output_file_name);
		return;
	}
			
	n_depths = ceil(max_depth / depth_increment - .5) + 1;
	depth = (float *) malloc(n_depths * sizeof(float));
	shear_velocity = (float *) malloc(n_depths * sizeof(float));
	error = (float *) malloc(n_depths * sizeof(float));
	
	for (i=0; i<n_depths; i++) {
		depth[i] = i * depth_increment;
		sum_shear_velocity = 0.0;
		sum_error = 0.0;
		for (ii=0; ii<model_family->n_models; ii++) {
			for (iii=0; iii<model_family->models[0].n_layers-1; iii++) {
				if (depth[i] >= model_family->models[ii].depth[iii] && depth[i] < model_family->models[ii].depth[iii+1]) 
				{
					sum_shear_velocity = sum_shear_velocity + model_family->models[ii].shear_velocity[iii];
					sum_error = sum_error + model_family->models[ii].error[iii];
				}
			}
		}
		fprintf(output_file,"%3.2f %3.5f %3.5f\n", depth[i], sum_shear_velocity / model_family->n_models, sum_error / model_family->n_models);
	}
	fclose(output_file);
	free(depth);
	free(shear_velocity);
	free(error);
	return;
}
////END/////

//output_inmodel///
void output_inmodel(MODEL_FAMILY *model_family, char *output_directory, float latitude, float longitude)
{
	int i;
	MODEL out_model;
	FILE *output_file;
	char output_file_name[300];
	
	sprintf(output_file_name,"%s/%3.1f_%3.1f_inmodel",output_directory,latitude,longitude);
	if (! (output_file = fopen(output_file_name, "w"))) {
		printf("file %s could not be opened for writing.\n", output_file_name);
		return;
	}
	
	out_model = calculate_mean_model(model_family);
	for (i=0; i<out_model.n_layers; i++) {
		fprintf(output_file, "%3.2f %3.5f %3.5f %3.5f %3.5f\n",out_model.depth[i], out_model.compressional_velocity[i], out_model.shear_velocity[i], out_model.density[i], out_model.error[i]);
	}
	fclose(output_file);

return;	
}
/// END ///

void output_combo_model(MODEL_FAMILY *model_family, char *output_directory, float latitude, float longitude, float max_depth, float depth_increment, int moho_layer)
{
	//outputs the crustal model on the top as flat layers until it reaches the moho_layer index. Then uses the same method as output_smooth_model to output a shear velocity each depth increment
	int i, ii, iii;
	int n_depths, mantle_flag = 0;
	float *depth, *shear_velocity, *error;
	float sum_shear_velocity, sum_error, moho_depth;
	char output_file_name[300];
	FILE *output_file;
	MODEL out_model;
	
	sprintf(output_file_name,"%s/%3.1f_%3.1f_combo_model.dat",output_directory,latitude,longitude);
	if (! (output_file = fopen(output_file_name, "w"))) {
		printf("file %s could not be opened for writing.\n", output_file_name);
		return;
	}
	
	out_model = calculate_mean_model(model_family);
	moho_depth = out_model.depth[moho_layer];
			
	n_depths = ceil(max_depth / depth_increment - .5) + 1;
	depth = (float *) malloc(n_depths * sizeof(float));
	shear_velocity = (float *) malloc(n_depths * sizeof(float));
	error = (float *) malloc(n_depths * sizeof(float));
	
	for (i=0; i<n_depths; i++) {
		depth[i] = i * depth_increment;
		if (depth[i] < moho_depth) {
			for (ii=0; ii<moho_layer; ii++) {
				if (depth[i] >= out_model.depth[ii] && depth[i] < out_model.depth[ii+1]) {
					fprintf(output_file, "%3.2f %3.5f %3.5f\n", depth[i], out_model.shear_velocity[ii], out_model.error[ii]);
				}
			}
		} else {
		sum_shear_velocity = 0.0;
		sum_error = 0.0;
		for (ii=0; ii<model_family->n_models; ii++) {
			for (iii=0; iii<model_family->models[0].n_layers-1; iii++) {
				if (depth[i] >= model_family->models[ii].depth[iii] && depth[i] < model_family->models[ii].depth[iii+1]) 
				{
					sum_shear_velocity = sum_shear_velocity + model_family->models[ii].shear_velocity[iii];
					sum_error = sum_error + model_family->models[ii].error[iii];
				}
			}
		}
		fprintf(output_file,"%3.2f %3.5f %3.5f\n", depth[i], sum_shear_velocity / model_family->n_models, sum_error / model_family->n_models);
		}
	}
	fclose(output_file);
        free(depth);
        free(shear_velocity);
        free(error);
	return;	
}
////END////

///output_inversion_parameters /////
void output_inversion_parameters(char *output_directory, float latitude, float longitude, float initial_error, float average_error, float minimum_error, double time_to_invert_1, double time_to_invert_2, double time_to_invert_3, int n_fitting_models, float final_thresshold, int iterations) 
{
	FILE *output_file;
	char output_file_name[300];

	sprintf(output_file_name,"%s/%3.1f_%3.1f_mc_inversion_parameters",output_directory,latitude,longitude);

	if (! (output_file = fopen(output_file_name, "w"))) {
		printf("file %s could not be opened for writing.\n", output_file_name);
		return;
	}	

	fprintf(output_file,"%3.5f\tinitial_misfit\n",initial_error);
	fprintf(output_file,"%3.5f\taverage_misfit\n",average_error);
	fprintf(output_file,"%3.5f\tminimum_misfit\n",minimum_error);
	fprintf(output_file,"%3.5lf\ttime_to_invert_1\n",time_to_invert_1);
	fprintf(output_file,"%3.5lf\ttime_to_invert_2\n",time_to_invert_2);
	fprintf(output_file,"%3.5lf\ttime_to_invert_3\n",time_to_invert_3);
	fprintf(output_file,"%d\tnumber_of_fitting_models\n",n_fitting_models);
	fprintf(output_file,"%3.2f\tfinal_thresshold\n",final_thresshold);
	fprintf(output_file,"%d\titerations\n",iterations);
	
	fclose(output_file);


return;	
}
////END////


////output_minimum_misfit////
void output_minimum_misfit(char *output_directory, float latitude, float longitude, float minimum_misfit,DISPERSION *minimum_misfit_dispersion,MODEL *minimum_misfit_model)
{
	
	FILE *output_file;
	char output_file_name[300];
	int i;

	sprintf(output_file_name,"%s/%3.1f_%3.1f_%3.3f_minimum_misfit_dispersion",output_directory,latitude,longitude, minimum_misfit);

	if (! (output_file = fopen(output_file_name, "w"))) {
		printf("file %s could not be opened for writing.\n", output_file_name);
		return;
	}	
	for (i=0; i<minimum_misfit_dispersion->n_phase_measurements; i++) {
		fprintf(output_file,"2 1 1 %3.2f %3.5f %3.5f\n",minimum_misfit_dispersion->phase_period[i], minimum_misfit_dispersion->phase_velocity[i], minimum_misfit_dispersion->phase_error[i]);
	}
	for (i=0; i<minimum_misfit_dispersion->n_group_measurements; i++) {
		fprintf(output_file,"2 2 1 %3.2f %3.5f %3.5f\n",minimum_misfit_dispersion->group_period[i], minimum_misfit_dispersion->group_velocity[i], minimum_misfit_dispersion->group_error[i]);
	}
	
	fclose(output_file);
	
	
	sprintf(output_file_name,"%s/%3.1f_%3.1f_%3.3f_minimum_misfit_model",output_directory,latitude,longitude, minimum_misfit);

	if (! (output_file = fopen(output_file_name, "w"))) {
		printf("file %s could not be opened for writing.\n", output_file_name);
		return;
	}	
	for (i=0; i<minimum_misfit_model->n_layers; i++) {
		fprintf(output_file, "%3.2f %3.5f %3.5f %3.5f %3.5f\n",minimum_misfit_model->depth[i], minimum_misfit_model->compressional_velocity[i], minimum_misfit_model->shear_velocity[i], minimum_misfit_model->density[i], minimum_misfit_model->error[i]);
	}
	
		
	fclose(output_file);
	
	return;
}
///END///

//// calculate_error_function ///
float calculate_error_function(DISPERSION *dispersion1, DISPERSION *dispersion2)
{
	float diff, error;
	int i;
/*	
	for (i=0; i<dispersion1->n_phase_measurements; i++) {
		diff = fabs(dispersion1->phase_velocity[i] - dispersion2->phase_velocity[i]) / dispersion2->phase_velocity[i];
		if (diff > (dispersion2->phase_error[i] / dispersion2->phase_velocity[i])) {
			error = error + (diff - (dispersion2->phase_error[i] / dispersion2->phase_velocity[i]));
		}
	}
	for (i=0; i<dispersion1->n_group_measurements; i++) {
		diff = fabs(dispersion1->group_velocity[i] - dispersion2->group_velocity[i]) / dispersion2->group_velocity[i];
		if (diff > (dispersion2->group_error[i] / dispersion2->group_velocity[i])) {
			error = error + (diff - ( dispersion2->group_error[i] / dispersion2->group_velocity[i]));
		}
	}
*/	
	for (i=0; i<dispersion1->n_phase_measurements; i++) {
		diff = fabs(dispersion1->phase_velocity[i] - dispersion2->phase_velocity[i]);
		if (diff > (dispersion2->phase_error[i])) {
			error = error + (diff - (dispersion2->phase_error[i]));
		}
	}
	for (i=0; i<dispersion1->n_group_measurements; i++) {
		diff = fabs(dispersion1->group_velocity[i] - dispersion2->group_velocity[i]);
		if (diff > (dispersion2->group_error[i])) {
			error = error + (diff - ( dispersion2->group_error[i]));
		}
	}

//	return error / (float) dispersion1->n_measurements;
	return error;
}
///END///

//calculate_dispersion////
DISPERSION calculate_dispersion(MODEL *model, DISPERSION *dispersion)
{
	DISPERSION output_dispersion;
	float shear_velocity_q_inverse[model->n_layers], poisson_ratio[model->n_layers], thickness[model->n_layers];
	int i, j;
	int spherical_correction_flag = 0, love_or_rayleigh = 2, group_flag, iq=1, is=1; 
	int n_love_calculated = 0, n_rayleigh_calculated;
	int unit_flag = 0, n_velocities, mode = 1, n_measurements;
	float temp_q, ddc = 0.01, h = 0.001, sone = 0.0;
	double c[dispersion->n_phase_measurements], cb[dispersion->n_phase_measurements], u[dispersion->n_group_measurements], ub[dispersion->n_group_measurements];
	//Translation:
	//NCOU = n_layers
	//NMES = n_measurements	
	
	//some preliminary calculations
	//Poisson's ratio and thickness:
	for ( j = 0; j < model->n_layers; j++ ) {
   		temp_q = model->shear_velocity[j]*model->shear_velocity[j]/model->compressional_velocity[j]/model->compressional_velocity[j];
     	poisson_ratio[j] = (1.-2.*temp_q)/(2.-2.*temp_q);
     	shear_velocity_q_inverse[j] = .001;
     	if (j != model->n_layers - 1) {
     		thickness[j] = model->depth[j+1] - model->depth[j];
     	} else {
     		thickness[j] = 1000;
     	}
   	}

	group_flag = 0;
	ph_vel_ (&n_love_calculated, &dispersion->n_phase_measurements, &spherical_correction_flag,
    		&(model->n_layers), thickness, model->compressional_velocity, model->shear_velocity, model->density, poisson_ratio, &unit_flag,
    		shear_velocity_q_inverse, &love_or_rayleigh,
    		&(dispersion->n_phase_measurements), &mode, &ddc, &sone, &group_flag, &h,
    		&(dispersion->phase_period[0]), &iq, &is, &(dispersion->n_phase_measurements), c, cb);
    	
	for (i=0; i<dispersion->n_phase_measurements;i++) {
                output_dispersion.phase_velocity[i] = (float)c[i];
//                printf("dispersion->phase_period[%d]: %3.2f\n",i, dispersion->phase_period[i]);
   	 	output_dispersion.phase_period[i] = dispersion->phase_period[i];
    		output_dispersion.phase_error[i] = 0.0;
	}
	
	group_flag = 1;
	ph_vel_ (&n_love_calculated, &dispersion->n_group_measurements, &spherical_correction_flag,
    		&(model->n_layers), thickness, model->compressional_velocity, model->shear_velocity, model->density, poisson_ratio, &unit_flag,
    		shear_velocity_q_inverse, &love_or_rayleigh,
    		&(dispersion->n_group_measurements), &mode, &ddc, &sone, &group_flag, &h,
    		&(dispersion->group_period[0]), &iq, &is, &(dispersion->n_group_measurements), u, ub);

	for (i=0; i<dispersion->n_group_measurements; i++) {
		output_dispersion.group_velocity[i] = (float)( 1./(1./u[i] + (1./u[i]-1./ub[i])/2./(double) h) );
		output_dispersion.group_period[i] = dispersion->group_period[i];
    		output_dispersion.group_error[i] = 0.0;
	}

output_dispersion.n_measurements = dispersion->n_measurements;
output_dispersion.n_phase_measurements = dispersion->n_phase_measurements;
output_dispersion.n_group_measurements = dispersion->n_group_measurements;


return output_dispersion;	
}
///END////

///output_dispersion////
void output_dispersion(char *output_directory, float latitude, float longitude, DISPERSION *dispersion) {
	FILE *output_file;
        char output_file_name[300];
        int i;

        sprintf(output_file_name,"%s/%3.1f_%3.1f_calculated_dispersion",output_directory,latitude,longitude);

        if (! (output_file = fopen(output_file_name, "w"))) {
                printf("file %s could not be opened for writing.\n", output_file_name);
                return;
        }

 	for (i=0; i<dispersion->n_phase_measurements; i++) {
                fprintf(output_file,"2 1 1 %3.2f %3.5f %3.5f\n",dispersion->phase_period[i], dispersion->phase_velocity[i], dispersion->phase_error[i]);
        }
        for (i=0; i<dispersion->n_group_measurements; i++) {
                fprintf(output_file,"2 2 1 %3.2f %3.5f %3.5f\n",dispersion->group_period[i], dispersion->group_velocity[i], dispersion->group_error[i]);
        }

return;
}
//// END ///

//append_input_models //
ALL_INMODELS *append_input_models(ALL_INMODELS *all_input_models, MODEL initial_model, int x, int y, int n)
{
	all_input_models->input_model[x][y][n] = initial_model;
	all_input_models->n_models = all_input_models->n_models + 1;
	if (x == 0 && y == 0 && n == 0) {
		all_input_models->n_layers = initial_model.n_layers;
	}
	
	return all_input_models;
}
//END///

//smooth_all_input_models///
ALL_INMODELS *smooth_all_input_models(ALL_INMODELS *all_input_models, int smooth_flag)
{
	int x, y, xx, yy, xx_min, xx_max, yy_min, yy_max, nn;
	int x_node, y_node, i;
	float weight, count, sum_depth[all_input_models->n_layers], sum_compressional_velocity[all_input_models->n_layers],sum_shear_velocity[all_input_models->n_layers],sum_density[all_input_models->n_layers],sum_error[all_input_models->n_layers];
	MODEL temp_model;
	
	x_node = all_input_models->x_node;
	y_node = all_input_models->y_node;
	
	for (x = 0; x<x_node; x++) {
		for (y=0; y<y_node; y++) {
			//first checking the x max and mins to avoid negative indices  
if (smooth_flag == 25) {
    if (x >= 2 ) {
        xx_min = x-2;
    } else if (x == 1) {
        xx_min = x-1;
    } else if (x == 0) {
        xx_min = x;
    } 
    if (x <= x_node - 3 ) {
        xx_max = x + 2;
    } else if (x == x_node - 2) {
        xx_max = x + 1; 
    } else if (x == x_node - 1) {
        xx_max = x;
    }
//now the y's same thing
    if (y >= 2 ) {
        yy_min = y-2;
    } else if (y == 1) {
        yy_min = y-1;
    } else if (y == 0) {
        yy_min = y;
    }
    if (y <= y_node - 3 ) {
        yy_max = y + 2;
    } else if (y == y_node - 2) {
        yy_max = y + 1;
    } else if (y == y_node - 1) {
        yy_max = y;
    }
} else if (smooth_flag == 9) {
    if (x >= 1 ) {
        xx_min = x-1;
    } else if (x == 0) {
        xx_min = x;
    } 
    if (x <= x_node - 2 ) {
        xx_max = x + 1; 
    } else if (x == x_node - 1) {
        xx_max = x;
    }
//now the y's same thing
    if (y >= 1 ) {
        yy_min = y-1;
    } else if (y == 0) {
        yy_min = y;
    }
    if (y <= y_node - 2 ) {
        yy_max = y + 1;
    } else if (y == y_node - 1) {
        yy_max = y;
    }
} else if (smooth_flag == 49) {
    if (x >= 3) {
       xx_min = x-3;
    } else if (x == 2 ) {
        xx_min = x-2;
    } else if (x == 1) {
        xx_min = x-1;
    } else if (x == 0) {
        xx_min = x;
    }
    if (x <= x_node - 4 ) {
        xx_max = x + 3;
    } else if (x == x_node - 3 ) {
        xx_max = x + 2;
    } else if (x == x_node - 2) {
        xx_max = x + 1;
    } else if (x == x_node - 1) {
        xx_max = x;
    }
//now the y's same thing
    if (y >=3 ) {
       yy_min = y-3;
    } else if (y == 2 ) {
        yy_min = y-2;
    } else if (y == 1) {
        yy_min = y-1;
    } else if (y == 0) {
        yy_min = y;
    }
    if (y <= y_node - 4) {
       yy_max = y + 3;
    } else if (y == y_node - 3 ) {
        yy_max = y + 2;
    } else if (y == y_node - 2) {
        yy_max = y + 1;
    } else if (y == y_node - 1) {
        yy_max = y;
    }
} else if (smooth_flag == 81) {
    if (x >= 4) {
       xx_min = x-4;
    } else if (x == 3 ) {
        xx_min = x-3;
    } else if (x == 2) {
        xx_min = x-2;
    } else if (x == 1) {
        xx_min = x-1;
    } else if (x == 0) {
        xx_min = x;
    }
    if (x <= x_node - 5 ) {
        xx_max = x + 4;
    } else if (x == x_node - 4 ) {
        xx_max = x + 3;
    } else if (x == x_node - 3) {
        xx_max = x + 2;
    } else if (x == x_node - 2) {
        xx_max = x+1;
    } else if (x == x_node - 1) {
        xx_max = x;
   }
//now the y's same thing
    if (y >=4 ) {
       yy_min = y-4;
    } else if (y == 3 ) {
        yy_min = y-3;
    } else if (y == 2) {
        yy_min = y-2;
    } else if (y == 1) {
        yy_min = y-1;
    } else if (y == 0) {
        yy_min = y;
    }
    if (y <= y_node - 5) {
       yy_max = y + 4;
    } else if (y == y_node - 4 ) {
        yy_max = y + 3;
    } else if (y == y_node - 3) {
        yy_max = y + 2;
    } else if (y == y_node - 2) {
        yy_max = y + 1;
    } else if (y == y_node - 1) {
        yy_max = y;
    }
}

// now we apply the smoothing window
	  for (nn = 0; nn<2; nn++) {
      for (xx = xx_min; xx<=xx_max; xx++) {
	  for (yy = yy_min; yy<=yy_max; yy++) {
 //             printf("dat[%d][%d][%d]: %f\n", z,xx,yy,dat[z][xx][yy]);
             weight = 1.0 - sqrt((float) pow( ((xx - xx_min) - (xx_max - xx_min) / 2),2) + (float) pow( ((yy - yy_min) - (yy_max - yy_min) / 2),2)  ) / 10.0; 
             temp_model = all_input_models->input_model[xx][yy][nn];
             for (i=0; i<all_input_models->n_layers; i++) {
           	 	sum_depth[i] = sum_depth[i] + temp_model.depth[i];
           	 	sum_compressional_velocity[i] = sum_compressional_velocity[i] + temp_model.compressional_velocity[i];
           	 	sum_shear_velocity[i] = sum_shear_velocity[i] + temp_model.shear_velocity[i];
           	 	sum_density[i] = sum_density[i] + temp_model.density[i];
           	 	sum_error[i] = sum_error[i] + temp_model.error[i];
           	 }
             count = count + weight;
	  	  }
        }
      }

//printf("count: %d, sum: %f\n",count,sum);
   	
    for (i=0; i<all_input_models->n_layers; i++) {
    
   	all_input_models->input_model[xx][yy][2].depth[i] = sum_depth[i] / count;
   	all_input_models->input_model[xx][yy][2].compressional_velocity[i] = sum_compressional_velocity[i] / count;
   	all_input_models->input_model[xx][yy][2].shear_velocity[i] = sum_shear_velocity[i] / count;
   	all_input_models->input_model[xx][yy][2].density[i] = sum_density[i] / count;
   	all_input_models->input_model[xx][yy][2].error[i] = sum_error[i] / count;
           	 	sum_depth[i] = 0.0;
           	 	sum_compressional_velocity[i] = 0.0;
           	 	sum_shear_velocity[i] = 0.0;
           	 	sum_density[i] = 0.0;
           	 	sum_error[i] = 0.0;
           	 }
    count = 0.0;
    }
  }


	return all_input_models;	
}
//END///

//initialize_all_input_models///
ALL_INMODELS *initialize_all_input_models(ALL_INMODELS *all_input_models, int x_node, int y_node, int n_passes)
{
	//all_input_models->n_models = x_node * y_node;
	all_input_models->x_node = x_node;
	all_input_models->y_node = y_node;
	all_input_models->n_passes = n_passes;
	return all_input_models;
}
/// END///

//get_smooth_model///
MODEL get_smooth_model(ALL_INMODELS *all_input_models, int x, int y)
{
	MODEL output_model;
	output_model = all_input_models->input_model[x][y][2];
		
	return output_model;
}
///END



