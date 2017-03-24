/*
 * The sample of test driver for FTAN with phase match filter for
 * subroutines aftanpg and aftanipg
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../include/aftan.h"
#define NPERIODS 32

/*--------------------------------------------------------------------------*/
int main (int argc, char *argv[])
/*--------------------------------------------------------------------------*/
{
  static int n, npoints, nfin, nfout1, nfout2, ierr, nprpv;
  static double t0, dt, delta, vmin=1.5, vmax=5.0, tmin=5.0, tmax=100.0;
  static double snr=.2, tresh=20.0, ffact=1.0, perc, taperl=1.0,fmatch=2.0,piover4=-1.0;
  static float sei[32768];
  static double arr1[100][8],arr2[100][7];
  static double tamp, ampo[NPERIODS][32768], pred[2][300];
  static int nrow, ncol, npred;
//  static double prpvper[300],prpvvel[300]; // phase vel prediction files
  static double prpvper[500],prpvvel[500]; // phase vel prediction files

  char  *p,name[160],buf[200],phvelname[160],root[160];
  //char sta1[6], sta2[6], net1[3],net2[3],cha1[4],cha2[4];
  FILE  *in, *inv;
  int   i, j, skip_flag=0, nanFlag=0, infFlag=0;
  int   sac = 1; // =1 - SAC, =0 - ftat files
  int amp_flag = 0;
  float dataMax, dataMin;
  float user0;

// input command line arguments treatment
  if(argc != 2) {
      //fprintf(stderr,"Hello!\n");
      fprintf(stderr,"USAGE: %s [parameter_file]\n", argv[0]);
      fprintf(stderr,"Parameter file format:\n");
      fprintf(stderr,"piover4 vmin vmax tmin tmax thresh ffact taperl snr fmatch amp_flag data_file\n");
      fprintf(stderr,"piover4 = -1 for noise correlations\nvmin and vmax are the minimum and maximum velocities possible\n");
      fprintf(stderr,"tmin and tmax are the min and max periods which could be measured. The fortran codes choose their own set of 32 periods somewhere between tmin and tmax\n");
      fprintf(stderr,"thressh, ffact, taperl, and snr are parameters I'm not fully sure of...\n");
      fprintf(stderr,"fmatch - 2 = use the phase matched filters and 1 = do not use phase matched filters (suggest 2)\n");
      fprintf(stderr,"amp_flag - 1 = write a file with the amplitude search space (file.AMP) 0 = do not write that file\n");
      fprintf(stderr,"Typical values: -1 1.5 5 5 100 20 1 1 .2 2 1\n");
      fprintf(stderr,"Looks for file 'phase_prediction.dat' in current directory\n");
      exit(-1);
  }
// open and read contents of parameter file
  if((in = fopen(argv[1],"r")) == NULL) {
      printf("Can not find file %s.\n",argv[1]);
      exit(1);
  }

  while((n = fscanf(in,"%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %d %s",
             &piover4,&vmin,&vmax,&tmin,&tmax,&tresh,&ffact,&taperl,&snr,&fmatch,&amp_flag,
             name)) != EOF) { // start main loop
  strcpy(root,name);
  p = strrchr(root,'.');


  //Some temp params
 // snr=.2;
 // tresh=20.0;
 // ffact=1.0;
 // taperl=1.0;
 // fmatch=2.0;
 // piover4=-1.0;

  *(p+1) = '\0';
  //strcpy(phvelname,root);
  //strcat(phvelname,"SAC_PHP");
      if(n == 0 || n != 12) break;

//      printf("vmin= %lf, vmax= %lf, tmin= %lf, tmax= %lf\n",
//              vmin,vmax,tmin,tmax);
// remove quotes from file names
      j = 0;
      for(i = 0; i < strlen(name); i++) {
          if(name[i] == '\'' || name[i] == '\"') continue;
          name[j] = name[i]; j++;
      }
      name[j] = '\0';
//      printf("Tresh= %lf, Filter factor= %lf, SNR= %lf, Match = %lf\nData file name=%s\n",
//             tresh,ffact,snr,fmatch,name);
// if presents, read phase velocity prediction file
// ---
nprpv = -1;
                                                                                
/*
 *  read phase velocity information
 */
//    sscanf(root, "COR_%[A-Z,a-z,0-9]_%[A-Z,a-z,0-9].", sta1, sta2);
//    sscanf(name,"COR_%[A-Z]_%[A-Z,a-z,0-9]_%[A-Z,0-9].%[A-Z]_%[A-Z,a-z,0-9]_%[A-Z,0-9].SAC_s",net1, sta1, cha1, net2, sta2, cha2);

//    sprintf(phvelname, "/ref/isis/rwporrit/software/ANT/data/scalifornia_avg_phvel.dat");
    sprintf(phvelname,"Mean_phase_vel_prediction.dat");
//    fprintf(stderr, "predicted phase velocity %s \n",phvelname);

  if((inv = fopen(phvelname,"r")) == NULL) {
      printf("Can not find file %s.\n",phvelname);
      nprpv = 0;
  } else {


  fprintf(stderr,"Starting FTAN LOOP");

  while(fgets(buf,200,inv) != NULL) {
         if(nprpv == -1) { nprpv++; continue; }
         if((n = sscanf(buf,"%lf %lf",&prpvper[nprpv],&prpvvel[nprpv])) < 2) break;
         nprpv++;
     }
         fclose(inv);
//         printf("Phase velocity prediction file name= %s\n",phvelname);
  }

/*
 *   read SAC or ascii data 
 */
      readdata(sac,name,&n,&dt,&delta,&t0,sei,&user0);
	/* check the data is not zero */
	dataMin = 1e6;
	dataMax = -1e6;
	nanFlag = 0;
	infFlag = 0;
	for (i=0; i<n; i++) {
		if (sei[i] > dataMax) {
			dataMax = sei[i];
		//	printf("max: %d\n",i);
		}
		if (sei[i] < dataMin) {
			dataMin = sei[i];
		//	printf("min: %d\n",i);
		}
		if (isnan(sei[i])) {
			nanFlag = 1;
		}
		if (isinf(sei[i])) {
			infFlag = 1;
		}
	}
//	printf("%f %f %f\n",dataMin, dataMax, user0);
//exit(1);
	if (dataMin >= dataMax || user0 < 2.0 || nanFlag == 1 || infFlag == 1 || isnan(dataMin) || isinf(dataMin) || isnan(dataMax) || isnan(dataMin)) {
		skip_flag = 1;
		printf("No data found for %s. skipping...\n",name);
	} else {
		skip_flag = 0;
	}
/*
 * Read group velocity prediction file
 */
//      strcpy(name1,root);
//      strcat(name1,"SAC_GRP");
//printf("root: %s\n", root);
//      sscanf(root, "%[^/]/COR_%[0-9,A-Z,a-z]_%[0-9,A-Z,a-z].", cor1, sta1, sta2);
//printf("cor1=%s, sta1-%s, sta2-%s\n", cor1, sta1, sta2); 
//	char name1[160];
//      sprintf(name1, "/ref/isis/rwporrit/software/ANT/data/PRED_files/COR_%s_%s.SAC_PRED", sta1, sta2);
//      sprintf(name1,"group_prediction.dat");
//      fprintf(stderr,"Group velocity prediction curve: %s\n",name1);
// This version of the code does not use the COR*PRED files. It relies only 
// on the initial dispersion measurement to do phase match filtering.
//      if((fd = fopen(name1,"r")) == NULL) {
//          printf("Can not find file %s.\n",name1);
//          sprintf(name1, "/data/isis/rwporrit/software/ANT/data/PRED_files/COR_%s_%s.SAC_PRED", sta2, sta1);
//          if((fd = fopen(name1,"r")) == NULL) {
//            printf("Can not find file %s.\n",name1);
//            exit(1);
//          }
//      }
//      i = 0;
//      fgets(str,100,fd);
//      while((nn = fscanf(fd,"%lf %lf",&pred[0][i],&pred[1][i])) == 2) i++;
//      npred = i;
//      fclose(fd);
/* ====================================================================
 * Parameters for aftanipg function:
 * Input parameters:
 * piover4 - phase shift = pi/4*piover4, for cross-correlation
 *           piover4 should be -1.0 !!!!     (double)
 * n       - number of input samples, (int)
 * sei     - input array length of n, (float)
 * t0      - time shift of SAC file in seconds, (double)
 * dt      - sampling rate in seconds, (double)
 * delta   - distance, km (double)
 * vmin    - minimal group velocity, km/s (double)
 * vmax    - maximal value of the group velocity, km/s (double)
 * tmin    - minimal period, s (double)
 * tmax    - maximal period, s (double)
 * tresh   - treshold, usually = 10, (double)
 * ffact   - factor to automatic filter parameter, (double)
 * perc    - minimal length of of output segment vs freq. range, % (double)
 * npoints - max number points in jump, (int)
 * taperl  - factor for the left end seismogram tapering,
 *           taper = taperl*tmax,    (double)
 * nfin    - starting number of frequencies, nfin <= 32, (int)
 * snr     - phase match filter parameter, spectra ratio to
 *           determine cutting point    (double)
 * fmatch  - factor to length of phase matching window
 * npred   - length of prediction table
 * pred    - prediction table: pred[0][] - periods in sec,
 *                             pred[1][] - pedicted velocity, km/s
 * ==========================================================
 * Output parameters are placed in 2-D arrays arr1 and arr2,
 * arr1 contains preliminary results and arr2 - final.
 * ==========================================================
 * nfout1 - output number of frequencies for arr1, (int)
 * arr1   - the first nfout1 raws contain preliminary data,
 *          (double arr1[n][5], n >= nfout1)
 *          arr1[:,0] -  central periods, s (double)
 *          arr1[:,1] -  apparent periods, s (double)
 *          arr1[:,2] -  group velocities, km/s (double)
 *          arr1[:,3] -  phase velocities, km/s (double)
 *          arr1[:,4] -  amplitudes, Db (double)
 *          arr1[:,5] -  discrimination function, (double)
 *          arr1[:,6] -  signal/noise ratio, Db (double)
 *          arr1[:,7] -  maximum half width, s (double)
 * nfout2 - output number of frequencies for arr2, (int)
 *          If nfout2 == 0, no final result.
 * arr2   - the first nfout2 raws contains final data,
 *          (double arr2[n][5], n >= nfout2)
 *          arr2[:,0] -  central periods, s (double)
 *          arr2[:,1] -  apparent periods, s (double)
 *          arr2[:,2] -  group velocities, km/s (double)
 *          arr2[:,3] -  amplitudes, Db (double)
 *          arr2[:,4] -  signal/noise ratio, Db (double)
 *          arr2[:,5] -  maximum half width, s (double)
 *          tamp      -  time to the beginning of ampo table, s (double)
 *          nrow      -  number of rows in array ampo, (int)
 *          ncol      -  number of columns in array ampo, (int)
 *          ampo      -  Ftan amplitude array, Db, (double [32][32768])
 * ierr   - completion status, =0 - O.K.,           (int)
 *                             =1 - some problems occures
 *                             =2 - no final results
 */

//  t0      = 0.0;
  nfin    = NPERIODS;
  npoints = 10;        // only 3 points in jump
  perc    = 50.0;     // 50 % for output segment
//  taperl  = 2.0;      // factor to the left end tapering
//  printf("pi/4 = %5.1lf, t0 = %9.3lf\n",piover4,t0);
//  printf("#filters= %d, Perc= %6.2f %s, npoints= %d, Taper factor= %6.2f\n",
//          nfin,perc,"%",npoints,taperl);
/* Call aftanipg function, FTAN + prediction         */

// No call to FTAN with the global prediction file for phase match filtering
//  printf("FTAN + prediction curve\n");
//  ffact =2.0;
//  aftanipg_(&piover4,&n,sei,&t0,&dt,&delta,&vmin,&vmax,&tmin,&tmax,&tresh,
//       &ffact,&perc,&npoints,&taperl,&nfin,&snr,&fmatch,&npred,pred,
//       &nprpv,prpvper,prpvvel,
//        &nfout1,arr1,&nfout2,arr2,&tamp,&nrow,&ncol,ampo,&ierr);
//  printres(dt,nfout1,arr1,nfout2,arr2,tamp,nrow,ncol,ampo,ierr,name,"_P",delta);   
//  if(nfout2 == 0) continue;   // break aftan sequence
//  printf("Tamp = %9.3lf, nrow = %d, ncol = %d\n",tamp,nrow,ncol);

/* FTAN with phase with phase match filter. First Iteration - _1_DISP files */
//  fprintf(stderr,"FTAN - the first iteration\n");
  ffact =1.0;
  if (skip_flag == 0) {
      //fprintf(stderr,"\nDoing first round of FTAN\n");
    aftanpg_(&piover4,&n,sei,&t0,&dt,&delta,&vmin,&vmax,&tmin,&tmax,&tresh,
        &ffact,&perc,&npoints,&taperl,&nfin,&snr,&nprpv,prpvper,prpvvel,
        &nfout1,arr1,&nfout2,arr2,&tamp,&nrow,&ncol,ampo,&ierr);
   
    //Write the first round to file 
    //fprintf(stderr,"Writing first round to file");
    //printres(dt,nfout1,arr1,nfout2,arr2,tamp,nrow,ncol,ampo,ierr,name,"_1", delta, amp_flag);
    if(nfout2 == 0) continue;   // break aftan sequence
//  fprintf(stderr,"Tamp = %9.3lf, nrow = %d, ncol = %d\n",tamp,nrow,ncol);

/* Make prediction based on the first iteration */
    npred = nfout2;
    tmin = arr2[0][1];
    tmax = arr2[nfout2-1][1];
    for(i = 0; i < nfout2; i++) {
        pred[0][i] = arr2[i][1];   // apparent period // central periods
        pred[1][i] = arr2[i][2];   // group velocities
    }

/* FTAN with phase with phase match filter. Second Iteration - _2_DISP files */
//  fprintf(stderr,"FTAN - the second iteration (phase match filter)\n");
    ffact = 2.0;
    //fprintf(stderr,"\nDoing second round of FTAN -phase matching-\n");
    //fprintf(stderr,"\nSNR: %g, fmatch: %g\n",snr,fmatch);
    aftanipg_(&piover4,&n,sei,&t0,&dt,&delta,&vmin,&vmax,&tmin,&tmax,&tresh,
        &ffact,&perc,&npoints,&taperl,&nfin,&snr,&ffact,&npred,pred,
        &nprpv,prpvper,prpvvel,
        &nfout1,arr1,&nfout2,arr2,&tamp,&nrow,&ncol,ampo,&ierr);
    printf("Results for %s: ",name);
    //Write the second round to file
    printres(dt,nfout1,arr1,nfout2,arr2,tamp,nrow,ncol,ampo,ierr,name,"_2", delta,amp_flag);
//  printf("Tamp = %9.3lf, nrow = %d, ncol = %d\n",tamp,nrow,ncol);
    }
  }
  fclose(in);
  return 0;
}
