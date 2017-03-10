/* Matthieu LANDES
   Institut du Globe de Physique de Paris
   fevrier 2008

Programme de beamforming
input: fichier Stations, contenant les informations sur les stations
       contains the stations
       fichier correlations, contenant la liste des fichiers sac
       contains the sac files

       Vmin, minimm velocity, perform the network analysis between [-1/Vmin,1/Vmin]x[-1/Vmin,1/Vmin]
		at about 4*NPT*NPT points. For more precision, increase NPT.


output: fk.txt, ascii file with "SlownessEast SlownessNorth Energy" for all
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../lib/sac_db.h"
#include "../lib/mysac.h"
#include "../lib/lib_prgm.h"

#define ST_FILE "station.lst"


#define NPT 20
/*half a side of the space. increase for more precision*/

#define RHO 1.

#define MAXDT 100.
#define MAX_SIG 15000

#define PI180 .017453292519  //pi / 180
#define PI 3.1415926535897
#define TWOPI 6.2831853071794

#define EARTH_RADIUS 6378.137

#define L_BUFFER 300


typedef struct {
	int log_N;
	int N;
	float *re;
	float *im;
} SPECTRE;

typedef struct {
	SPECTRE sp;
	float delta;
	float b;
	float stEst;
	float stNord;
} CORR;


#define NSTATIONS 200
/* maximum number of stations*/
#define NCORR NSTATIONS*NSTATIONS/2
/* maximum number of correlations*/

STATION st[NSTATIONS];
CORR lcorr[NCORR];
CORR corr_stack;
int nst=0;
int ncorr=0;


SAC_HD shd;
float sig[MAX_SIG];

char buffer[L_BUFFER];
int npts0;

int corr2vect(char *, float *, float * );

/* compute the distance of two points 1 and 2 on the earth */
float lldist(float lat1, float lat2, float lon1, float lon2) {
	double t1, t2, a, angles;


	t1 = sin((double)(lat2-lat1)/2.);
	t2 = sin((double)(lon2-lon1)/2.);
	a = t1*t1 + cos((double)lat1) * cos((double)lat2) * t2*t2;
	//a = (sin(dlat/2)).^2 + cos(lat1) .* cos(lat2) .* (sin(dlon/2)).^2;
 
	angles = 2.* atan2( sqrt(a), sqrt(1-a) );
	return (float)EARTH_RADIUS * angles;
}


void initCORR(CORR *corr, int npts, float stE, float stN, float delta, float b) {
	corr->stEst=stE;
	corr->stNord=stN;

	corr->delta=delta;
	corr->b=b;

	corr->sp.log_N = (int)ceil(log((double)npts)/log(2.0));
	corr->sp.N = 1 << corr->sp.log_N; //2**r

	corr->sp.re = (float *)calloc(corr->sp.N, sizeof(float));
	corr->sp.im = (float *)calloc(corr->sp.N, sizeof(float));
}

void freeCORR(CORR *corr) {
	free(corr->sp.re);
	free(corr->sp.im);
}


//charge le fichier des stations
//load the station file
void load_stations(char *file_st) {
	FILE *flux;
	int i;
	nst=0;
	printf("loading stations ");
	if( (flux = fopen(file_st,"r")) == NULL ) {
		fprintf(stderr,"ERROR %s is unreadable.\n", file_st);
		exit(0);
	}
	
	for(i=0; ;i++) {
		if( !fgets(buffer, L_BUFFER, flux) ) break;
		sscanf(buffer, "%s %g %g\n", st[i].name, &(st[i].lat), &(st[i].lon));
		printf("*******%s***%g***%g******\n",st[i].name, st[i].lat, st[i].lon);
//network?, station_name, latitude (deg), longitude (deg)
		st[i].lat*=PI180;
		st[i].lon*=PI180;
		nst++;
		if(nst >= NSTATIONS) {
			printf("ERROR too many stations -> increase NSTATIONS and recompile\n");
			exit(0);
		}
		putchar('.');
	}
	putchar('\n');
	fclose(flux);

}

// load the correlation file
void load_corr(char *list_file) {
	FILE *flux;
        char buff[L_BUFFER];
	float stE, stN;
	int err, i0, i;

	printf("loading correlations "); 
	ncorr=0;

	if( (flux=fopen(list_file ,"r")) == NULL) {
		printf("ERROR can't open file '%s'\n",list_file);
		exit(0);
	}

	//initialize the stack of energy or correlations
	if(fscanf(flux, "%300s[^\n]", buff) == EOF) {
		printf("no correlation\n");
		exit(0);
	}
	if ( !read_sac(buff, sig, &shd, MAX_SIG ) ) {
        	printf("ERROR : cannot open file '%s'.\n", buff);
		exit(1);
	}
	initCORR(&corr_stack, MAX_SIG+(2.*MAXDT/shd.delta)+2, 0.0, 0.0, shd.delta, shd.b-MAXDT);


	//initialize the correlations
	rewind(flux);
	while( fscanf(flux, "%300s[^\n]", buff) !=EOF ) {
                if( fgetc(flux) != '\n' ) {
                        printf("ERROR path too long for '%s\n", buff);
                        continue;
                }
		//try to open the file	
		if ( !read_sac(buff, sig, &shd, MAX_SIG ) ) {
        	        printf("ERROR : cannot open sac file '%s'.\n", buff);
                	continue;
	        }
		//test the time step is the same
		if( fabs(corr_stack.delta-shd.delta) > 1.e-6 ) {
	                printf("ERROR delta incompatible '%s' skipped.\n", buff);
        	        continue;
		}
		//test the begin time
		i0=(int)((shd.b-corr_stack.b)/corr_stack.delta);

		if( i0 <0 ) {
			printf("ERROR begin time, file '%s' skipped -> take a bigger MAXDT.\n", buff);
			continue;
		}
                //find the coordinates of the two stations from the correlation
                err=corr2vect(buff, &stE, &stN);
                if( err ) continue;

		putchar('.'); fflush(stdout);
		initCORR(&lcorr[ncorr], corr_stack.sp.N, stE, stN, corr_stack.delta, corr_stack.b);

		for(i=0; i<shd.npts; i++) {
                	lcorr[ncorr].sp.re[i0+i]=sig[i]; 
                }

          	//transform to the frequency domain
                f_fu_trans(lcorr[ncorr].sp.log_N, lcorr[ncorr].sp.re, lcorr[ncorr].sp.im, -1);
        
                ncorr++;


                if(ncorr >= NCORR) {
                     printf("ERROR too many correlations -> see NCORR\n");
                     exit(0);
               	}

       }
	putchar('\n');
	fclose(flux);
}

//function to calculate the coordinates of the vector between the stations
//output 0 all is good
//       1 one station not found
//       2 neither station is found
char cpyCorrFile[L_BUFFER];
int corr2vect(char *corrFile, float *stEst, float *stNord) {
	int i, err=2;
	float lat1, lat2, lon1, lon2;
/*	double t1, t2, a;    */
	char *tmp;
	strcpy(cpyCorrFile, corrFile);
	printf("********%d*******\n",nst);
	//check station 1
	for(i=0; i<nst; i++) {
		//for the first station, check the text file name :
		// _NameStation_ in the sac file name
		//correlation
		buffer[0]='_';
		buffer[1]='\0';
		strcat(buffer, st[i].name);
		strcat(buffer, "_");
		printf("%s\n",buffer);
		printf("%s\n",cpyCorrFile);
		if( (tmp=strstr( cpyCorrFile, buffer)) != NULL ) {
			*tmp=' ';
			lat1=st[i].lat;
			lon1=st[i].lon;
			err--;
			break;
		}
	}

	//check station 2
	for(i=0; i<nst; i++) {
		//for the second station, check the text :
		// _NomStation.SAC is the file name
		//correlation
		buffer[0]='_';
		buffer[1]='\0';
		strcat(buffer, st[i].name);
		strcat(buffer, "_");
//              strcat(buffer,".SAC");

		printf("%s\n",buffer);
		printf("%s\n",cpyCorrFile);

		if( strstr( cpyCorrFile, buffer) != NULL ) {
			lat2=st[i].lat;
			lon2=st[i].lon;
			err--;
			break;
		}
	}
//	printf("distance = %f\n", lldist(lat1, lat2, lon1, lon2));
	//In case of an error message
	if( err ) printf("ERROR in the correlation '%s':\n\t %d station(s) unreadable*******\n", corrFile, err);
	*stEst = ((lon2>lon1)? 1. : -1.)*lldist(lat1, lat1, lon1, lon2);
	*stNord = ((lat2>lat1)? 1.: -1.)*lldist(lat1, lat2, lon1, lon1);
	
	return err;
}

float stack(float sEst, float sNord, float T) {
	int i, j, ib, ie;
	float arg, cs, sn, rr, ii;
	float E=0.0, dt;
	if(!ncorr) return E;
	
	/* initialization */
        for(j=0; j<corr_stack.sp.N; j++) {
                corr_stack.sp.re[j]=0.0;
                corr_stack.sp.im[j]=0.0;
	}

	for(i=0; i<ncorr; i++) {
		//time step = slownessEast * eastDistance + slownessNorth * northDistance (seconds/km * km)
		dt=-(sEst*lcorr[i].stEst + sNord*lcorr[i].stNord);
		//compute the hilbert transform on the first half of the file. The second half is zero.
		//calculate the envelope using an analytical signal technique
		for(j=0; j<corr_stack.sp.N/2.; j++) {
			/* Time shift */
			/*to shift the spectrum, multiply by exp(i*2*pi*f*dt) 
			(shift the files by dt)*/
			arg=-TWOPI*(float)j/corr_stack.sp.N/corr_stack.delta*dt;
			cs=cos(arg);
	               	sn=sin(arg);
			rr=lcorr[i].sp.re[j]*cs-lcorr[i].sp.im[j]*sn;
			ii=lcorr[i].sp.re[j]*sn+lcorr[i].sp.im[j]*cs;
			/* Stack */
			corr_stack.sp.re[j] += rr;
			corr_stack.sp.im[j] += ii;
		}
	}

	/* IFFT */
	f_fu_trans(corr_stack.sp.log_N, corr_stack.sp.re, corr_stack.sp.im, 1);

	/* envelope */
	for(j=0; j<corr_stack.sp.N; j++) 
		corr_stack.sp.re[j]=sqrt(corr_stack.sp.re[j]*corr_stack.sp.re[j]
			+corr_stack.sp.im[j]*corr_stack.sp.im[j]);

	//Integrate for energy in the time window from [-T,+T]
	/*integration of the envelope between -T and T */
	ib=(int)((-corr_stack.b-T)/corr_stack.delta);
	ie=(int)((-corr_stack.b+T)/corr_stack.delta);
	for(i=ib; i<=ie; i++) 
		E += corr_stack.sp.re[i]*corr_stack.delta;

	return E;
}




int main(int narg, char *argv[]) {

	FILE *flux;
/*	float dt, sEst=1.0, sNord=1.0, E, T, rho, Vmin;   */
        float sEst=1.0, sNord=1.0, T, rho, Vmin; 
	int i,j;
	int npt;
	char output_file[300];
	npt=NPT;

	if( narg < 3 ) {
		printf("ERROR usage %s stations_file list_file [out_file_id]\n", argv[0]);
		exit(0);
	}

	printf("Minimum velocity Vs(in km/s)= ");
	scanf("%g", &Vmin);
	if(Vmin < 0.0) {
		printf("ERROR Vmin must be greater than 0.\n");
		exit(0);
	}


	printf("Input the integration time window T(in s)= ");
	scanf("%g", &T);
	if(T < 0.0) {
		printf("ERROR T must be greater than 0.\n");
		exit(0);
	}


	load_stations(argv[1]);
	load_corr(argv[2]);



	if( narg == 4) sprintf(output_file, "noise_%s.txt", argv[3]);
        else strcpy(output_file, "noise.txt");
        flux=fopen(output_file, "w");
	/*sprintf(buffer, "fk_V%2.1f_T%2.1f.txt", Vmin, T);
	flux=fopen("fk.txt","w");*/
	rho=1./Vmin/npt;
	printf("noise source diagram being made ");
	for(i=-npt; i<=npt; i++) for(j=-npt; j<=npt; j++) {
		putchar('.'); fflush(stdout);
		sEst=i*rho;
		sNord=j*rho;
		fprintf(flux, "%f %f %f\n", sEst, sNord, stack(sEst, sNord, T));
		fflush(flux);
		//fprintf(flux, "%f %f %f\n", sEst, -sNord, stack(sEst, -sNord, T));
		//fprintf(flux, "%f %f %f\n", -sEst, sNord, stack(-sEst, sNord, T));
		//fprintf(flux, "%f %f %f\n", -sEst, -sNord, stack(-sEst, -sNord, T));
	}
	putchar('\n');
	fclose(flux);
	printf("Result in '%s'.\n", output_file);

	for(i=0; i<ncorr; i++)
		freeCORR(&lcorr[i]);
	freeCORR(&corr_stack);

 return 0;

}	
