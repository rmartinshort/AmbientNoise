/*
 reads a database of
 longitude latitude period groupvel phasevel
 
 and creates 1D dispersion curves at each latitude/longitude point.
 longitude increases first, then latitude decreases, then period increases
 
 */
 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif
#include <time.h>
#include <sys/stat.h>



void mkdir_rp(char *dirname);
int compute_nodes(float max, float min, float inc);


//makes a directory and parents if necessary
void mkdir_rp(char *dirname) {
    char str[500];
    struct stat st;
    
    if (stat(dirname,&st) == -1) {
        fprintf(stderr,"dir: %s not found. Creating...\n",dirname);
        sprintf(str,"mkdir -p %s\n",dirname);
        system(str);
    }
    
    return;
}

int compute_nodes(float max, float min, float inc) {
    return floor((max - min) / inc + 1.5);
}




/* --- main --- */
int main(int argc, char *argv[]) {
    
    int nbins, l, periodIncFoundFlag=0, latIncFoundFlag=0, lonIncFoundFlag=0;
    int nlat, nlong, nper, ilat, ilon, iper;
    double tmpLon, tmpLat, tmpPer, tmpGp, tmpPh;
    double prevPer, prevLong, prevLat;
    double MinLongitude, MinLatitude, MaxLongitude, MaxLatitude, MinPeriod, MaxPeriod;
    double IncPeriod, IncLongitude, IncLatitude;
    double *phasevel;
    char buff[300], paramFileName[300], inputFileName[300], OutputBaseDirectory[300];
    char outputTmpDir[600], outputFileName[900];
    FILE *paramFile, *inputFile, *outputFile;
    
    
    if (argc != 2) {
        fprintf(stdout,"Usage: %s paramfile\n",argv[0]);
        fprintf(stdout,"Parameter file format:\n");
        fprintf(stdout,"InputDispersionDatabaseName\n");
        fprintf(stdout,"OutputBaseDirectory\n");
        exit(1);
    }
    
    //read parameter file
    sscanf(argv[1],"%s",paramFileName);
    if ((paramFile = fopen(paramFileName,"r")) == NULL) {
        fprintf(stderr,"Error, file %s not found!\n",paramFileName);
        exit(1);
    }
    
    l=0;
    while (fgets(buff,300,paramFile)) {
        if (l == 0) {
            sscanf(buff,"%s",inputFileName);
            l++;
        } else if (l == 1) {
            sscanf(buff,"%s",OutputBaseDirectory);
            l++;
        }
    }
    fclose(paramFile);
    
    //find the size of the model by reading it once and learning the mins/maxs/step
    if ((inputFile = fopen(inputFileName,"r")) == NULL) {
        fprintf(stderr,"Error, file %s not found\n",inputFileName);
        exit(1);
    }
    nbins=0; 
    while (fgets(buff,300,inputFile)) {
        // avoids the header line
        if (nbins != -1) {
            //gets info from the data file
            sscanf(buff,"%lf %lf %lf %lf",&tmpLon, &tmpLat, &tmpPer, &tmpPh);
            
            if (nbins == 0) {
                //set the initial values based on the first data line
                prevPer = tmpPer;
                prevLong = tmpLon;
                prevLat = tmpLat;
                
                MinLongitude = tmpLon;
                MaxLongitude = tmpLon;
                MinLatitude = tmpLat;
                MaxLatitude = tmpLat;
                MinPeriod = tmpPer;
                MaxPeriod = tmpPer;
                
            } else {
                // check extremes
                if (tmpLon < MinLongitude) {
                    MinLongitude = tmpLon;
                }
                if (tmpLon > MaxLongitude) {
                    MaxLongitude = tmpLon;
                }
                
                if (tmpLat < MinLatitude) {
                    MinLatitude = tmpLat;
                }
                if (tmpLat > MaxLatitude) {
                    MaxLatitude = tmpLat;
                }
                
                if (tmpPer < MinPeriod) {
                    MinPeriod = tmpPer;
                }
                if (tmpPer > MaxPeriod) {
                    MaxPeriod = tmpPer;
                }
                
                //check increments
                if (tmpLon != prevLong && lonIncFoundFlag == 0) {
                    lonIncFoundFlag = 1;
                    IncLongitude = fabs(tmpLon - prevLong);
                }
                if (tmpLat != prevLat && latIncFoundFlag == 0) {
                    latIncFoundFlag = 1;
                    IncLatitude = fabs(tmpLat - prevLat);
                }
                if (tmpPer != prevPer && periodIncFoundFlag == 0) {
                    periodIncFoundFlag = 1;
                    IncPeriod = fabs(tmpPer - prevPer);
                }
            }
        }
        nbins++;
    }
    rewind(inputFile);

    printf("IncLon: %g ", IncLongitude);
    printf("IncLat: %g ", IncLatitude);
    printf("Incper: %g ", IncPeriod);

    //printf("Lims: %g/%g/%g/%g ",MinLongitude,MaxLongitude,MinLatitude,MaxLatitude);

    // compute spatial nodes
    nlong = compute_nodes(MaxLongitude, MinLongitude, IncLongitude);
    nlat = compute_nodes(MaxLatitude, MinLatitude, IncLatitude);
    nper = compute_nodes(MaxPeriod, MinPeriod, IncPeriod);
    
    // prepare memory
    phasevel = malloc(sizeof(phasevel) * nlong * nlat * nper);
    
    for (iper=0; iper<nper; iper++) {
        for (ilat=nlat-1; ilat>=0; ilat--) {
            for (ilon=0; ilon<nlong; ilon++) {
                l = ilon + nlong * (ilat + nlat * iper);
                fgets(buff,300,inputFile);
                sscanf(buff,"%lf %lf %lf %lf\n",&tmpLon, &tmpLat, &tmpPer, &phasevel[l]);
            }
        }
    }
    fclose(inputFile);
    
    // Prepare output. first make the base directory
    mkdir_rp(OutputBaseDirectory);
    
    for (ilon=0; ilon<nlong; ilon++) {
        for (ilat=0; ilat<nlat; ilat++) {
            sprintf(outputTmpDir,"%s/disp_%03.1f_%03.1f_1D", OutputBaseDirectory, MinLongitude+ilon * IncLongitude, MinLatitude + ilat * IncLatitude);
            mkdir_rp(outputTmpDir);
            
            sprintf(outputFileName,"%s/nnall.dsp",outputTmpDir);
            outputFile = fopen(outputFileName,"w");
            
            //writes the phase velocities
            for (iper=0; iper<nper; iper++) {
                l = ilon + nlong * (ilat + nlat * iper);
                fprintf(outputFile,"SURF96 R C X 0 %f %f 0.2 20\n",MinPeriod + iper * IncPeriod, phasevel[l]);
            }
            fclose(outputFile);
        }
    }
    
    free(phasevel);
    
    return 0;
}




