#!/usr/bin/env python

#RMS 2017
#QC code for the RF inversion. Looks at a log file produced by the inversion and extracts the best fitting RFS
#If the RFs are not fit to below some minimum criteria, they are rejected and placed in a separate folder. The inversion can then be run
#again using just the high quality RFs


import os
import glob
import sys
import re

def main():

    #datadir = "/home/rmartinshort/Documents/Berkeley/Ambient_Noise/Depth_invert/Station_grid/Alaska_stations_plus_ghost_500_RFJ_both"
    #datadir = "/home/rmartinshort/Documents/Berkeley/Ambient_Noise/Depth_invert/Station_grid/Alaska_stations_plus_ghost_400_RFJ_2014_2.5"
    datadir = "/home/rmartinshort/Documents/Berkeley/Ambient_Noise/Depth_invert/Station_grid/Alaska_stations_plus_ghost_RF_1999_2017_ALL"

    #return_unfit_files(datadir)

    #specify full path to the logdir
    logfile = "/home/rmartinshort/Documents/Berkeley/Ambient_Noise/AmbientNoise/Scripts/Depth_Invert/ALL_logQC1.dat"
    lf = open(logfile,'r')

    unfit_files = parse_logfile(lf)
    print unfit_files

    #Enter the datadir and move the unfit away from where they will be processed:
    move_unfit_files(datadir,unfit_files)


def parse_logfile(openfile):

    #returns object comaining station and list of bad RFs

    #The percentage fit that must be exceeded
    fit_criterion = 70.0

    lines = openfile.readlines()
    openfile.close()

    stations_bad_sacfiles = {}

    lc = 0
    for line in lines:
        if "Model name: start.mod" in line:
            #count backwards until we get to the stopper
            bc = lc
            stopper = False
            while stopper == False:
                tmpline = lines[bc-1]
                if '---------------------------' in tmpline:
                    stopper = True
                else:
                    if ' T' in tmpline:
                        parts = tmpline.split()
                        finalfit = float(parts[7])
                        sacfilename = parts[-1]
                        stationpart = sacfilename.split('_')[1].split('-')

                        station = '_'.join(stationpart[:2])
                        print station
                        #station = stationpart.replace('-','_')
                        #station = station[:-1]

                        if station not in stations_bad_sacfiles:
                            stations_bad_sacfiles[station] = []
                        if finalfit < fit_criterion:
                            stations_bad_sacfiles[station].append(sacfilename)

                    bc = bc - 1

        lc += 1

    return stations_bad_sacfiles


def move_unfit_files(datadir,flist):

    '''Move unfit files away from the inversion dir'''

    cwd = os.getcwd()

    os.chdir(datadir)
    print flist

    for stationname in flist:

        print 'In dir %s' %stationname

        if '_0' not in stationname:
        
           os.chdir(stationname)
           os.system('rm rtfn.lst')
           os.system('touch rtfn.lst')

           if not os.path.exists('reject_RFs'):
              os.system('mkdir reject_RFs')

           if (len(flist[stationname]) > 0):
               for sacfile in flist[stationname]:
                  os.system('mv %s reject_RFs' %sacfile)


        os.chdir(datadir)

    os.chdir(cwd)


def return_unfit_files(datadir):

    '''Return any unfit files to the inversion dir'''

    cwd = os.getcwd()

    os.chdir(datadir)

    flist = glob.glob('*_*')

    for stationname in flist:

        if os.path.isdir(stationname):

          print "In dir %s" %stationname

          os.chdir(stationname)

          if os.path.exists('reject_RFs'):
             os.system('mv reject_RFs/* .')

          os.chdir(datadir)

    os.chdir(cwd)



if __name__ == "__main__":

    main()
