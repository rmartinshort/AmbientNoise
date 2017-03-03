/* file only contains subroutines
header information in ../../include/lib_prep.h
*/

#include "lib_prep.h"


void print_sac_pole_zero_file(POLE_ZERO *pz) {
        int i;
        fprintf(stdout,"Scalar Constant: %lg\n",pz->scale);
        fprintf(stdout,"Poles:\n");
        for (i=0;i<pz->n_poles; i++) {
                fprintf(stdout,"%lg %lg\n",pz->poles[i].real, pz->poles[i].imag);
        }
        fprintf(stdout,"Zeroes:\n");
        for (i=0;i<pz->n_zeroes; i++) {
                fprintf(stdout,"%lg %lg\n",pz->zeroes[i].real, pz->zeroes[i].imag);
        }
        return;
}

//to initailize a pole zero structure
void zero_pole_zero(POLE_ZERO *pz) {
        int i;
        pz->scale = 0.0;
        pz->n_zeroes = 0;
        pz->n_poles = 0;
        for (i=0; i<N_MAX_ZEROES;i++) {
                pz->zeroes[i].real = 0.0;
                pz->zeroes[i].imag = 0.0;
        }
        for (i=0; i<N_MAX_POLES;i++) {
                pz->poles[i].real = 0.0;
                pz->poles[i].imag = 0.0;
        }

        return;
}
//END

//read a pole zero file
int read_sac_pole_zero_file(char *filename, POLE_ZERO *pz_out) {
	char buff[100], temp[100], tmp2[2];
	int i=-1;
	int ival;
	int n_zeros, n_poles;
	int zero_scan_flag = 0, pole_scan_flag = 0;
	double fval1, fval2;
	double constant;
	FILE *pz_file;	

	//initialize the pole zero file
	zero_pole_zero(pz_out);

	//try to open the file. return -1 as failure value
	if (! (pz_file = fopen(filename, "r"))) {
		return -1;
	}

	while (fgets(buff,100,pz_file)) {
		sscanf(buff,"%s %d\n", temp, &ival);
		//apparently comments are now put in with a single * in the first column.
		//I will also check against using # as a comment here.
		sscanf(temp,"%s",tmp2);
		if (!strcmp(tmp2,"*")) {
		} else if (!strcmp(tmp2,"#")) {
		} else if (!strcmp(temp,"ZEROS")) {
			zero_scan_flag = 1;
			pole_scan_flag = 0;
			i=-1;
			n_zeros = ival;
			if (n_zeros > N_MAX_ZEROES) {
				printf("Warning: max zeroes greater than hardwired value %d\n",n_zeros);
				return -2;
			}
			pz_out->n_zeroes = n_zeros;
		} else if (!strcmp(temp,"ZEROES")) {
			zero_scan_flag = 1;
			pole_scan_flag = 0;
			i=-1;
			n_zeros = ival;
			if (n_zeros > N_MAX_ZEROES) {
				printf("Warning: max zeroes greater than hardwired value %d\n",n_zeros);
				return -2;
			}
			pz_out->n_zeroes = n_zeros;
		} else if (! strcmp(temp,"POLES")) {
			zero_scan_flag = 0;
			pole_scan_flag = 1;
			i=-1;
			n_poles = ival;
			if (n_poles > N_MAX_POLES) {
				printf("Warning: max poles greater than hardwired value %d\n",n_poles);
				return -3;
			}
			pz_out->n_poles = n_poles;
		} else if (! strcmp(temp,"CONSTANT")) {
			zero_scan_flag = 0;
			pole_scan_flag = 0;
			i=-1;
			sscanf(buff,"%s %lf",temp,&constant);
			if (constant <= 0.0) {
				printf("Error, constant must be positive %lg\n",constant);
				return -4;
			}
			pz_out->scale = constant;
		} else if (zero_scan_flag == 1) {
			i++;
			sscanf(buff,"%lf %lf", &fval1, &fval2);
			pz_out->zeroes[i].real = fval1;
			pz_out->zeroes[i].imag = fval2;
		} else if (pole_scan_flag == 1) {
			i++;
			sscanf(buff,"%lf %lf", &fval1, &fval2);
			pz_out->poles[i].real = fval1;
			pz_out->poles[i].imag = fval2;
		}
	}
	
	fclose(pz_file);

	if (pz_out->n_poles <= 0) {
		return -5;
	}
	if (pz_out->n_zeroes <= 0) {
		return -6;
	}
	return 1;
}
//END

//generate response
//generates the response for a specific frequency given in hertz
COMPLEX_RP generate_response(POLE_ZERO *pz, double freq) {	
	/* local variables */
	int i;
	double mod_squared;
	COMPLEX_RP omega, denom, num, temp;
	COMPLEX_RP out;

	/* initializations */
	freq = freq * 2 * PI;
	omega.real = 0.0;
	omega.imag = freq;
	denom.real = 1.0;
	denom.imag = 1.0;
	num.real = 1.0;
	num.imag = 1.0;
	
	/* compute the complex laplacian */
	for (i=0; i<pz->n_zeroes; i++) {
		temp = complex_subtract(&omega,&pz->zeroes[i]);
		num = complex_multiply(&num,&temp);
	}
	for (i=0; i<pz->n_poles; i++) {
		temp = complex_subtract(&omega,&pz->poles[i]);
		denom = complex_multiply(&denom,&temp);
	}
	
	/* compute the final zeros / poles */
	temp = conjugate(&denom);
	temp = complex_multiply(&temp,&num);
	mod_squared = denom.real * denom.real + denom.imag * denom.imag;
	temp.real = temp.real / mod_squared;
	temp.imag = temp.imag / mod_squared;
	out.real = pz->scale * temp.real;
	out.imag = pz->scale * temp.imag;
	
	return out;	
}
//END

void generate_pz_response_spectrum(SPECTRUM *spect_out, char *pzFileName, double dw) {
	//local variables
	int i, ierr;
	double tmpFreq;
	COMPLEX_RP tmpCmplx;
	POLE_ZERO pz;
	
	//read pole zero file
	ierr = read_sac_pole_zero_file(pzFileName, &pz);
	//convert from displacement to velocity in response
	pz.n_zeroes--;
	//loop over frequencies computing the complex spectra
	for (i=0; i<spect_out->N; i++) {
		tmpFreq = dw * (i+1);
		tmpCmplx = generate_response(&pz, tmpFreq);
//printf("%lf %lf+%lf i\n",tmpFreq,tmpCmplx.real, tmpCmplx.imag);
		spect_out->re[i] = tmpCmplx.real;
		spect_out->im[i] = tmpCmplx.imag;
	}

	return;
}


//phase_shift  
void phase_shift(double t_shift, double dt,int np, SPECTRUM *spect_in) {
        double tpi, t, w, phi;
        int i, j;
        COMPLEX_RP ca, cx, temp_complex, cb;

	tpi = 2.0 * PI;

        t = (double) np * dt;

//      for (i=1; i < np / 2+1; i++) {
        for (i=0; i < spect_in->N; i++) {
                temp_complex.real = spect_in->re[i];
                temp_complex.imag = spect_in->im[i];
//              printf("i: %d\n",i);
//              j = i - 1;
                j=i;
                w = tpi * (double) j / t;
                phi = w * t_shift;
                ca.real = 0.0;
                ca.imag = -phi;
                cb = complex_exp(&ca);
                cx = complex_multiply(&temp_complex,&cb);
                spect_in->re[i] = cx.real;
                spect_in->im[i] = cx.imag;
        }
        return;
}
//END

COMPLEX_RP complex_exp(COMPLEX_RP *comp) {
         COMPLEX_RP comp_out;
         comp_out.real = exp(comp->real) * cos(comp->imag);
         comp_out.imag = exp(comp->real) * sin(comp->imag);
         return comp_out;
}

//normalize the record length
//time array starts exactly at 00:00:00.0000 and ends at 23:59:59.00000
//cut extra samples on either end and zero out samples that are before the start of the record
void normalize_record_length(double *arr_in, double *arr_out, SAC_HD *header, int npts_out, int jday) {
	int i, j=0, first_sample;
	double current_time, record_start_time;
	
	//first we check if the record starts before the day we're working on
	if (header->nzjday < jday) {
		//first_sample is the number of samples before the start of the current day
		first_sample = (1.0/header->delta) * ( (23 - header->nzhour) * 3600 + (59 - header->nzmin) * 60 + (60 - header->nzsec));
		for (i=0; i<npts_out; i++) {
			arr_out[i] = arr_in[first_sample + i];
		}
	} else {
		record_start_time = header->nzhour * 3600 + header->nzmin * 60 + header->nzsec;
		for (i=0; i<npts_out; i++) {
                	current_time = i * header->delta;
			if (current_time < record_start_time) {
				arr_out[i] = 0.0;
			} else {
				arr_out[i] = arr_in[j];
				j++;
			}
		}
	}

	//update the header
	header->nzhour = 0;
	header->nzjday = jday;
	header->nzmin = 0;
	header->nzsec = 0;
	header->npts = npts_out;

	return;
}



//cuts the record to start at 00:30:00.0000 and end at 23:30:00.0000
//still too much that could go wrong. Better to use normalize_record_length above
void cut_record(double *arr_in,double *arr_out, SAC_HD *header, int npts_out) {
	int i, hour_diff, min_diff, sec_diff, total_diff;
	int first_sample;

	//first we check if the hour is greater than 22, this implies the record starts a day before the target date
	if (header->nzhour >= 22) {  //starts a day early
		//find the appropriate first sample
		hour_diff = 23 - header->nzhour;
		min_diff = 59 - header->nzmin;
		sec_diff = 60 - header->nzsec; 
		total_diff = sec_diff + 60 * min_diff + 3600 * hour_diff;
		first_sample = (total_diff + 30 * 60) / header->delta;
		//copy the data from array in to array out
		for (i=0; i<npts_out; i++) {
			arr_out[i] = arr_in[i+first_sample];
		}
		//update header
		header->nzjday = header->nzjday + 1;
		header->nzhour = 0;
		header->nzmin = 30;
		header->nzsec = 0;
	} else {  //starts on the proper day
		if ((header->nzmin >= 30 && header->nzsec >= 0) || header->nzhour > 0 ) {
			hour_diff = header->nzhour - 0;
			min_diff = header->nzmin - 30;
			sec_diff = header->nzsec - 0;
			total_diff = sec_diff + 60 * min_diff + 3600 * hour_diff;
			first_sample = total_diff / header->delta;
			for (i=0; i<npts_out; i++) {
				if (i<first_sample) {
					arr_out[i] = 0.0;
				} else {
					arr_out[i] = arr_in[i];
				}
			}
		} else {
			min_diff = 29 - header->nzmin;
			sec_diff = 60 - header->nzsec;
			total_diff = sec_diff + 60 * min_diff; 
			first_sample = total_diff / header->delta;
			for (i=0; i<npts_out; i++) {
	                        arr_out[i] = arr_in[i+first_sample];
	                }
		}
		//update header
		header->nzhour = 0;
		header->nzmin = 30;
		header->nzsec = 0;
	}

	header->npts = npts_out;

	return;
}
//END


//read_sac_d
//provides a cast to allow reading a sac file and returning a double precision array
/*--------------------------------------------------------------------------*/
        SAC_HD *read_sac_d (char *fname, double *sig_d, SAC_HD *SHD, int nmax)
/*----------------------------------------------------------------------------
----------------------------------------------------------------------------*/
{
 FILE *fsac;
 float *sig;
 int i;
/*..........................................................................*/
        fsac = fopen(fname, "rb");
        if ( !fsac )
        {
         //fclose (fsac);
//      fprintf(stderr,"read_sac_d: ERROR file %s not found!\n",fname);
         return NULL;
        }

        if ( !SHD ) SHD = &SAC_HEADER;

         fread(SHD,sizeof(SAC_HD),1,fsac);
         if ( SHD->npts > nmax )
         {
          fprintf(stderr,
           "ATTENTION !!! in the file %s the number of points is limited to %d\n",fname,nmax);

          SHD->npts = nmax;
         }

        sig = malloc(sizeof(*sig) * SHD->npts);

         fread(sig,sizeof(float),(int)(SHD->npts),fsac);

        for (i=0; i<SHD->npts; i++) {
          sig_d[i] = (double) sig[i];
        }

        free(sig);

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
        return SHD;
}
//////END



//write_sac_d
void write_sac_d(char *fname, double *sig_d, SAC_HD *SHD)
/*----------------------------------------------------------------------------
----------------------------------------------------------------------------*/
{
 FILE *fsac;
 int i;
 float *sig;
/*..........................................................................*/
        fsac = fopen(fname, "wb");

        if ( !SHD ) SHD = &SAC_HEADER;
        sig = malloc(SHD->npts * sizeof(*sig));
        for (i=0; i<SHD->npts; i++) {
                sig[i] = (float) sig_d[i];
        }

        SHD->iftype = (long)ITIME;
        SHD->leven = (long)TRUE;

        SHD->lovrok = (long)TRUE;
        SHD->internal4 = 6L;

  /*+++++++++++++++++++++++++++++++++++++++++*/
     SHD->depmin = sig[0];
     SHD->depmax = sig[0];

   for ( i = 0; i < SHD->npts ; i++ )
   {
    if ( SHD->depmin > sig[i] ) SHD->depmin = sig[i];
    if ( SHD->depmax < sig[i] ) SHD->depmax = sig[i];
   }

         fwrite(SHD,sizeof(SAC_HD),1,fsac);
         fwrite(sig,sizeof(double),(int)(SHD->npts),fsac);

        fclose(fsac);
        free(sig);
}
////END

//calculate a 10% cosine taper
void cos_taper(double *arr_in, double *arr_out, int npts)
{
        int i,M;
        M = floor( ((npts - 1) / 10)/2 + .5);
//      printf("M: %d\n",M);
//      cos_taper = malloc(npts*sizeof(cos_taper));

        for (i=0; i<npts; i++) {
            if (i<M) {
                arr_out[i] = arr_in[i] * (0.5 * (1-cos(i * PI / (M + 1)))) ;
            } else if (i<npts - M - 2) {
                arr_out[i] = arr_in[i];
            } else if (i<npts) {
                arr_out[i] = arr_in[i] * ( 0.5 * (1-cos((npts - i - 1) * PI / (M + 1))));
            }
        }
        return;
        //http://www.cbi.dongnocchi.it/glossary/DataWindow.htm
}
/////END

//remove_mean
void remove_mean(double *arr_in, double *arr_out, int npts)
{
        int i;
        double ave=0.0, sum=0.0;

        for (i=0; i<npts; i++) {
                sum = sum + arr_in[i];
        }
        ave = sum / (double) npts;
        for (i=0; i<npts;i++) {
                arr_out[i] = arr_in[i] - ave;
        }
        return;
}
///END

void remove_trend(double *arr_in, double *arr_out, int npts)
{
        int i;
        double ata11, ata12, ata21, ata22, atb1, atb2;
        double fn, di, d, a, b;
        double ata11i, ata12i, ata21i, ata22i;
//      double mean, sum=0.0;

/* removes trend with a linear least squares approach */
ata11 = 0.0;
ata12 = 0.0;
ata21 = 0.0;
ata22 = 0.0;
atb1 = 0.0;
atb2 = 0.0;
fn = (double) npts * 1.0;

for (i=0; i<npts; i++) {
        di = (double) i;
        ata11 = ata11 + 1.0;
        ata12 = ata12 + di;
        ata21 = ata21 + di;
        ata22 = ata22 + di * di;
        atb1 = atb1 + arr_in[i];
        atb2 = atb2 + arr_in[i] * di;
//      sum = sum + arr_in[i];
}
//mean = sum / ((double) npts);

d = ata11 * ata22 - ata12 * ata21;
ata11i = ata22 / d;
ata12i = -1 * ata12 / d;
ata21i = -1 * ata21 / d;
ata22i = ata11 / d;
a = ata11i * atb1 + ata12i * atb2;
b = ata21i * atb1 + ata22i * atb2;

//printf("a: %lf, b: %lf\nmean: %lf\n",a,b, mean);

for (i=0; i<npts; i++) {
        arr_out[i] = arr_in[i] - (a + b * (double)i);
}
return;
} /* end linear detrend */
////END

//initialize_spectrum
void initialize_spectrum(SPECTRUM *spect, int npts) {
        spect->N = npts / 2 + 1;
        spect->log_N = floor(log_n(npts,2)+0.5);
        spect->re = malloc(sizeof(*(spect->re)) * spect->N);
        spect->im = malloc(sizeof(*(spect->im)) * spect->N);
        return;
}
///END

///fftw_forward///
void fftw_forward(double *input, SPECTRUM *spectrum, int npts)
{
  int i;
  double *in;
  int nc;
  fftw_complex *out;
  fftw_plan plan_forward;

  in = fftw_malloc ( sizeof (double) * npts);
  for (i=0; i<npts; i++) {
        in[i] = (double) input[i];
  }

//  free(input);

  nc = spectrum->N;

  out = fftw_malloc ( sizeof ( *out ) * nc );

  plan_forward = fftw_plan_dft_r2c_1d ( npts, in, out, FFTW_ESTIMATE );

  fftw_execute ( plan_forward );
  fftw_destroy_plan ( plan_forward );

  fftw_free ( in );

  for (i=0; i<nc; i++) {
        spectrum->re[i] = (double) out[i][0];
        spectrum->im[i] = (double) out[i][1];
  }

  fftw_free(out);
  return;
}
////END


///fftw_backward////
void fftw_backward(SPECTRUM *spectrum, double *array_out)
{
  int i;
  double *temp;
  fftw_complex *spect;
  fftw_plan plan_backward;
  int n;

  spect = fftw_malloc( sizeof ( *spect ) * spectrum->N);

  //initialize spect from spectrum
  n = (spectrum->N - 1) * 2;

  for (i=0; i<spectrum->N; i++) {
        spect[i][0] = spectrum->re[i];
        spect[i][1] = spectrum->im[i];
  }

  temp = fftw_malloc ( sizeof (double) * n );
  plan_backward = fftw_plan_dft_c2r_1d(n,spect,temp,FFTW_ESTIMATE);

  fftw_execute(plan_backward);
  fftw_destroy_plan(plan_backward);

  fftw_free(spect);
  for ( i = 0; i < n; i++ )
  {
    array_out[i] = temp[i] / (double) n;
  }

//  free_spectrum(spectrum); 
  fftw_free ( temp );

  return;
}

///prep_signal
void prep_signal(double *arr_in, double *arr_out, int npts)
{
//      int i;

        remove_mean(arr_in,arr_out,npts);
        remove_trend(arr_out,arr_out,npts);
        cos_taper(arr_out,arr_out,npts);

//      for (i=0; i<npts; i++) {
//              arr_out[i] = arr_out[i] * 1.3;
//      }

        return;
}
/// END

//decimate_signal
void decimate_signal(double *input_signal, double *arr_out, int npts, double dt_in, double dt_out) {
        int npts_out;
        int i, j;

        npts_out = npts * dt_in / dt_out;
        j = 0;

        for (i=0; i<npts_out; i++) {
                j = i * dt_out / dt_in;
                if (j<npts) {
                        arr_out[i] = input_signal[j];
                } else {
                        arr_out[i] = input_signal[npts-1];
                }
        }

        return;
}
///END  

//running mean (general - previous function uses the "k" argument with a function of log10 to be specific to power spectral densities).
//also new is the psd version is directly from david dolenc's code while this is more from rob's smoothing code, just simplified from the 2-d case to the 1-d case.
void smooth_1d_array(double *arr_in, double *arr_out, int npts, int n_smooth)
{
int i, j, start, end, count;
double sum;
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
    arr_out[i] = sum / (double) count;
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
    arr_out[i] = sum / (double) count;
  }
}
return;
}
//END

//"Tapering subroutine itself"///
void flt4(SPECTRUM *spect, SPECTRUM *spect_out, double delta_omega, int npow, double f1, double f2, double f3, double f4) {
        int i, j;
        double d1, d2, f, ss, s=0.0;

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
//decon_response_function
void decon_response_function(SPECTRUM *data_spect, SPECTRUM *response_spect) {
        int i;
        COMPLEX_RP temp_data, temp_response, temp_out;

        for (i=0; i<data_spect->N; i++) {
                temp_data.real = data_spect->re[i];
                temp_data.imag = data_spect->im[i];
                temp_response.real = response_spect->re[i];
                temp_response.imag = response_spect->im[i];
                temp_out = complex_divide(&temp_data,&temp_response);
                data_spect->re[i] = temp_out.real;
                data_spect->im[i] = temp_out.imag;
                //printf("spect_out[%d]: %lf + %lfi\n",i,spect_out.re[i], spect_out.im[i]);
        }

        return;

}
///END

COMPLEX_RP conjugate(COMPLEX_RP *comp) {
        COMPLEX_RP comp_out;
        comp_out.real = comp->real;
        comp_out.imag = -1.0 * comp->imag;
        return comp_out;
}

COMPLEX_RP complex_multiply(COMPLEX_RP *comp1, COMPLEX_RP *comp2) {
        COMPLEX_RP comp_out;
        comp_out.real = comp1->real * comp2->real - comp1->imag * comp2->imag;
        comp_out.imag = comp1->real * comp2->imag + comp2->real * comp1->imag;
        return comp_out;
}

COMPLEX_RP complex_divide(COMPLEX_RP *comp1, COMPLEX_RP *comp2) {
	double denom;
	COMPLEX_RP out;

	denom = comp2->real * comp2->real + comp2->imag * comp2->imag;
//	if (abs(denom) < pow(10,-100)) {
	//	fprintf(stderr,"Warning. Dividing by near 0 (1/%lg) in complex division.\n",denom);
//		denom = denom + 1.1 * pow(10,-100);
//	}
	out.real = (comp1->real * comp2->real + comp1->imag * comp2->imag) / denom;
	out.imag = (comp1->imag * comp2->real - comp1->real * comp2->imag) / denom;
	return out;
/*
        COMPLEX_RP comp_out, temp_conjugate, numerator;
        double denom;

        temp_conjugate = conjugate(comp2);
        numerator = complex_multiply(comp1, comp2);
        denom = comp2->real * comp2->real + comp2->imag * comp2->imag + 1.2e-32;

        comp_out.real = numerator.real / denom;
        comp_out.imag = numerator.imag / denom;

        return comp_out;
*/
}

///interpolate_spectrum
void interpolate_spectrum(SPECTRUM *spect, SPECTRUM *spect_out, double *freqs, int nfreqs_out) {
        int i, klo, khi;
        double F, incr, real_slope, imag_slope, df;

        incr = (freqs[spect->N-1] - freqs[0]) / (nfreqs_out - 1);

        F = freqs[0];
        klo = 0;
        khi = 1;
        real_slope = (spect->re[khi] - spect->re[klo]) / (freqs[khi] - freqs[klo]);
        imag_slope = (spect->im[khi] - spect->im[klo]) / (freqs[khi] - freqs[klo]);

        for (i=0; i<nfreqs_out;i++) {
                df = F-freqs[klo];
                spect_out->re[i] = spect->re[klo] + df * real_slope;
                spect_out->im[i] = spect->im[klo] + df * imag_slope;

                F = F + incr;
                if (F > freqs[spect->N-1]) F = freqs[spect->N-1];
                if (F > freqs[khi]) {
                        klo++;
                        khi++;
                        real_slope = (spect->re[khi] - spect->re[klo]) / (freqs[khi] - freqs[klo]);
                        imag_slope = (spect->im[khi] - spect->im[klo]) / (freqs[khi] - freqs[klo]);
                }
        }

        return;
}
///END


void generate_instrument_response(SPECTRUM *spect_out, char *response_dir, char *station, char *component, char *locid, char *net_code, char *year, double dt, int npts) {


        int nfreqs, nfreqs_out, i, temp_npts;
        int start_stage = -1, stop_stage = 0, stdio_flag = 0;
        int n_discrete = 20;
        char file[100];
//seems like locations codes are coming back into vogue and thus this is becoming a passed in parameter
//        char *locid = "*"; // hardwired here because location codes are largely depreciated
        char datime[5];
        char *units = "VEL";  //standard is velocity. 
        char *rtype = "CS";   //returning real and imaginary components rather than amplitude and phase because its easier to work with
        char *verbose = 0;    //useless information for automated processing - would turn off warnings too, but can't
        double *freqs;
        double dw;
        struct response *first;
        SPECTRUM temp_spect;

        strcpy(file,response_dir);
        strcpy(datime, year);
        //for the sake of speed, we'll only compute n_discrete points with evresp and then interpolate manually. Tests have shown this increases the speed n_discrete-times with no clear change in the actual response. 
        nfreqs = 1 + (npts/2+1)/n_discrete;
        nfreqs_out = nfreqs * n_discrete - (n_discrete-1);
        dw = n_discrete / (dt * npts);
        temp_npts = (nfreqs - 1) * 2;

        //generate the freqs array used in evresp
        freqs = malloc(sizeof(*freqs) * nfreqs);
        for (i=0; i<nfreqs; i++) {
                freqs[i] = dw * i;
//              first->rvec[i].real = 1.0;
//              first->rvec[i].imag = 1.0;
        }

        initialize_spectrum(&temp_spect,temp_npts);
        //call to evresp - puts a complex spectrum into "first->rvec"
        first = evresp(station, component, net_code, locid, datime, units, file, freqs, nfreqs, rtype, verbose, start_stage, stop_stage, stdio_flag);
//      printf("evresp returned first for %s %s %s %s\n", station, component,net_code, datime);
        for (i=0; i<nfreqs; i++) {
                if (first->rvec[i].real != 0.0) {
                        temp_spect.re[i] = first->rvec[i].real;
                } else {
                        temp_spect.re[i] = 1.0;
                }
                if (first->rvec[i].imag != 0.0) {
                        temp_spect.im[i] = first->rvec[i].imag;
                } else {
                        temp_spect.im[i] = 1.0;
                }
        }
        interpolate_spectrum(&temp_spect, spect_out, freqs, nfreqs_out);

        free(freqs);
        free_spectrum(&temp_spect);

        return;
}
///END

///log_n
//log of a generic base - with error reporting
double log_n(double x, double b) {
        if (x > 0 && b > 0) {
                return log(x) / log(b);
        } else if (x <= 0) {
                fprintf(stderr,"log_n Error: x must be positive (value given: %lf)\n", x);
                return -1.0;
        } else if (b <=0) {
                fprintf(stderr,"log_n Error: b must be positive (value given: %lf)\n", b);
                return -1.0;
        }
        return 0;
}
///END



//append_array
void append_array(double *arr_1, int npts_1, double *arr_2, int npts_2) {
        int i;
        for (i=npts_1; i<(npts_1+npts_2); i++) {
               arr_1[i] = arr_2[i-npts_1];
        }
}
///END

//append_array_debug
void append_array_debug(double *arr_1, int npts_1, double *arr_2, int npts_2) {
        int i;
        for (i=npts_1; i<(npts_1+npts_2); i++) {
//		printf("npts_1+npts_2: %d, npts_1: %d, npts_2: %d,i: %d, i-npts_1: %d\n",npts_1+npts_2,npts_1, npts_2, i, i-npts_1);
		printf("%d\n",NPTS_SAC_FILE_MAX);
		printf("arr_1[%d]: %lf\n",i,arr_1[i]);
		printf("arr_2[%d]: %lf\n",i-npts_1, arr_2[i-npts_1]);
               arr_1[i] = arr_2[i-npts_1];
        }
}
///END



///compute the mean from an array
double compute_mean_from_array(double *arr_in, int npts) {
        int i;
        double sum=0.0;

        for (i=0; i<npts; i++) {
                sum = sum + arr_in[i];
        }
        return sum / npts;
}
///END

//makes an array to fill a gap
void fill_gap(double *arr_out, double start, double end, double mean, int n) {
        int i;
        double slope;
        slope = (end - start) / n;

        for (i=0; i<n; i++) {
                arr_out[i] = mean + i * slope;
        }
        return;
}
///END

//find_number_of_blanks
int find_number_of_blanks(SAC_HD *sac_header_1, SAC_HD *sac_header_2) {
        int out;
        double start, end;

        //first check the headers for the same time step

        if (fabs(sac_header_1->delta - sac_header_2->delta) > .001) {
                fprintf(stderr,"oops! the sac files have different time steps!\n");
                char station_1[7], station_2[7];
                sscanf(sac_header_1->kstnm,"%[A-Z,a-z,0-9]",station_1);
                sscanf(sac_header_2->kstnm,"%[A-Z,a-z,0-9]",station_2);
                fprintf(stderr,"station1: %s, station2: %s\n",station_1, station_2);
                fprintf(stderr,"delta1: %lf, delta2: %lf\n",sac_header_1->delta, sac_header_2->delta);
                fprintf(stderr,"Changing the time step in file 2 from %lf to %lf\n",sac_header_1->delta,sac_header_2->delta);
                sac_header_2->delta = sac_header_1->delta;
        }

        //Now check the headers for the same year
        if (sac_header_1->nzyear != sac_header_2->nzyear) {
               //In this instance we know the numerical jday's are not congruent. They are either 365 or 366 and 1. Therefore we ignore the time due to the first day and simply assume the second day starts 24 hours after the first and negate the contribution due to jday1 and jday2
                end = 3600.0 * sac_header_1->nzhour + 60.0 * sac_header_1->nzmin + sac_header_1->nzsec + .001 * sac_header_1->nzmsec + sac_header_1->npts * sac_header_1->delta;
                start = 86400.0 + 3600.0 * sac_header_2->nzhour + 60.0 * sac_header_2->nzmin + sac_header_2->nzsec + .001 * sac_header_2->nzmsec;       } else {

        //find the time where the first array ends
                end = 86400.0 * sac_header_1->nzjday + 3600.0 * sac_header_1->nzhour + 60.0 * sac_header_1->nzmin + sac_header_1->nzsec + .001 * sac_header_1->nzmsec + sac_header_1->npts * sac_header_1->delta;
                start = 86400.0 * sac_header_2->nzjday + 3600.0 * sac_header_2->nzhour + 60.0 * sac_header_2->nzmin + sac_header_2->nzsec + .001 * sac_header_2->nzmsec;
        }

        out = (int) floor((start - end) / sac_header_1->delta+0.5);

        return out;
}
///END

///find_and_remove_excess_samples
void find_and_remove_excess_samples(double *signal, double *arr_out, SAC_HD *sac_header) {
        int i, first_sample, start_jday, npts_out;

        if (sac_header->nzhour > 22) {
                start_jday = sac_header->nzjday + 1;
                first_sample = find_first_sample(sac_header, start_jday);
        //      printf("1\n");
        } else {
                start_jday = sac_header->nzjday;
                first_sample = 0;
        //      printf("2\n");
        }


        npts_out = SECS_PER_DAY / sac_header->delta;
        //printf("first_sample: %d, npts_out: %d\n, npts_in: %d\n",first_sample,npts_out, sac_header->npts);

        for (i=0; i<(SECS_PER_DAY / sac_header->delta); i++) {
                if (i < sac_header->npts) {
                        arr_out[i] = signal[i+first_sample];
                } else {
                        arr_out[i] = (double) sac_header->depmen;
                }
        }

        sac_header->nzjday = start_jday;
        sac_header->nzhour = 0;
        sac_header->nzmin = 0;
        sac_header->nzsec = 0;
        sac_header->npts = SECS_PER_DAY / sac_header->delta;

        return;
}
///END
///free spectrum
void free_spectrum(SPECTRUM *spect) {
        free(spect->re);
        free(spect->im);
        return;
}
//END

///prepare for whitening
//filter_and_smooth()
void filter_and_smooth(double *arr_in, double *arr_out, int npts, int order, char *filter_type, double fmin, double fmax, double delta, int npasses, int width) {
        int i;
        int npow;
        double *temp_arr;
        double f1, f2, f3, f4, delta_omega;
        SPECTRUM temp_spect;

        temp_arr = malloc(sizeof(*temp_arr) * npts);
//      filter_rp(arr_in, temp_arr, npts, order, filter_type, fmin, fmax, delta, npasses);
        temp_spect.re = malloc(sizeof(*(temp_spect.re)) * (npts/2 + 1));
        temp_spect.im = malloc(sizeof(*(temp_spect.im)) * (npts/2 + 1));
        temp_spect.N = npts/2 + 1;

        npow = 1;
        delta_omega = 1.0 / (delta * npts);
        f1 = fmin - .001;
        f2 = fmin;
        f3 = fmax;
        f4 = fmax + .001;


        fftw_forward(arr_in,&temp_spect,npts);
        flt4(&temp_spect, &temp_spect, delta_omega, npow, f1, f2, f3, f4);
        fftw_backward(&temp_spect, temp_arr);

        for (i=0; i<npts; i++) {
                temp_arr[i] = fabs(temp_arr[i]);
        }

        smooth_1d_array(temp_arr, temp_arr, npts, width);

        for (i=0; i<npts; i++) {
                arr_out[i] = arr_in[i] / temp_arr[i];
        }
        free(temp_arr);

        return;

}
//END

//reject_band
void reject_band(SPECTRUM *spect, SPECTRUM *spect_out, double delta_omega, int npow, double f1, double f2, double f3, double f4) {
        int i, j;
        double freqmin = 0.5;
        double f, d1, d2, ss, scale = 1.0;

        //initialize output spectrum

        for (i=0; i<spect->N; i++) {
                f = i * delta_omega;
                if (f <= f1) {
                        scale = 1.0;
                } else if ( f <= f2) {
                        d1 = PI / (f2 - f1);
                        ss = 1.0;
                        for (j=1; j<=npow;j++) {
                                ss = ss * (1 + cos(d1 * (f1-f))) / 2.0 * (1.0 - freqmin) + freqmin;
                        }
                        scale = ss;
                } else if (f <= f3) {
                        scale = freqmin;
                } else if (f<=f4) {
                        d2 = PI /(f4-f3);
                        ss = 1.0;
                        for (j=1; j<=npow; j++) {
                                ss = ss * (1-cos(d2 *(f3-f))) / 2.0 * (1.0 - freqmin) + freqmin;
                        }
                        scale = ss;
                } else if (f >= f4) {
                        scale = 1.0;
                }
                spect_out->re[i] = spect->re[i] * scale;
                spect_out->im[i] = spect->im[i] * scale;
        }

        return;
}
//END

//////smooth_cub
void smooth_cub(SPECTRUM *spect, SPECTRUM *spect_out, double delta_omega, int smooth_flag, double f1, double f2, double f3, double f4) {
        int i, j, ij;
        double f, sum;
        double *sorig, sout=0;

        sorig = malloc(sizeof(*sorig) * spect->N);

        for (i=0; i<spect->N; i++) {
                sorig[i] = sqrt(pow(spect->re[i],2) + pow(spect->im[i],2));
        }

        for (i=0; i<spect->N; i++) {
                f = i * delta_omega;
                if (f >= f1 && f <= f4) {
                        sum = 0.0;
                        for (j=-smooth_flag; j<=smooth_flag; j++) {
                                ij = i + j;
                                sum = sum + sorig[ij];
                        }
                        sout = 1.0 / (sum / (2.0 * smooth_flag + 1.0));
                } else {
                        sout = 0.0;
                }

                spect_out->re[i] = spect->re[i] * sout;
                spect_out->im[i] = spect->im[i] * sout;
        }

        free(sorig);

        return;
}
//END

void whiten_rp(SPECTRUM *spect, SPECTRUM *temp_spect, int npts, int npow, double dt, double f1, double f2, double f3, double f4) {
        int i, smooth_flag = 20;
        double rej1 = 0.035, rej2 = 0.036, rej3 = 0.039, rej4 = 0.040;
        double delta_omega;
        delta_omega = 1.0 / (dt * npts);

        for (i=0; i<spect->N; i++) {
                temp_spect->re[i] = spect->re[i];
                temp_spect->im[i] = spect->im[i];
        }

	rej1 = 0.035;
	rej2 = 0.036;
	rej3 = 0.039;
	rej4 = 0.040;

        smooth_cub(temp_spect, temp_spect, delta_omega, smooth_flag, f1, f2, f3, f4);
        reject_band(temp_spect, temp_spect, delta_omega, npow, rej1, rej2, rej3, rej4);
        flt4(temp_spect, temp_spect, delta_omega, npow, f1, f2, f3, f4);
        temp_spect->re[0] = temp_spect->re[0] / 2.0;
        temp_spect->im[0] = temp_spect->im[0] / 2.0;
        temp_spect->re[temp_spect->N-1] = temp_spect->re[temp_spect->N-1];
        temp_spect->im[temp_spect->N-1] = 0.0;

        return;
}
//END

//find first sample
int find_first_sample(SAC_HD *sac_header, int start_jday)
{
        int out;
        int out_nzmsec, out_nzsec, out_nzmin, out_nzhour;
        float dt;
        double sec_difference;
        dt = sac_header->delta;
        //time: 1000 - nzmsec; 59 - nzsec; 59 - nzmin; 23 - nzhour
        out_nzmsec = 1000 - sac_header->nzmsec;
        out_nzsec = 59 - sac_header->nzsec;
        out_nzmin = 59 - sac_header->nzmin;
        out_nzhour = 23 - sac_header->nzhour;

        sec_difference = (double) out_nzhour * 3600.0 + (double) out_nzmin * 60.0 + (double) out_nzsec * 1.0 + out_nzmsec / 1000.000;

        if (sac_header->nzjday < start_jday) {
                out = (int) floor(sec_difference * dt+0.5);
        } else {
                fprintf(stderr,"sac file does not start a day before it should! Returning 0.\n");
                out = 0;
        }
        return out;
}
//END


COMPLEX_RP complex_add(COMPLEX_RP *comp1, COMPLEX_RP *comp2) {
        COMPLEX_RP comp_out;
        comp_out.real = comp1->real + comp2->real;
        comp_out.imag = comp1->imag + comp2->imag;
        return comp_out;
}

COMPLEX_RP complex_subtract(COMPLEX_RP *comp1, COMPLEX_RP *comp2) {
        COMPLEX_RP comp_out;
        comp_out.real = comp1->real - comp2->real;
        comp_out.imag = comp1->imag - comp2->imag;
        return comp_out;
}



