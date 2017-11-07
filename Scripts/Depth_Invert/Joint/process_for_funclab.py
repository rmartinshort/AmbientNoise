#!/usr/bin/env python
#RMS Oct 2017
#processing for data to be read into Funclab

import obspy as op
from obspy.taup import TauPyModel
from obspy.geodetics import locations2degrees
import numpy as np
import os
import glob
import sys


def main():

    cwd = os.getcwd()
    indir = "/home/rmartinshort/Documents/Berkeley/funclab/data_2014_2017_AK/2014-01-01_2017-10-01"
    rearrange(indir)

    #dir1="/home/rmartinshort/Documents/Berkeley/funclab/data_2017_TA/Merged/2017-01-01_2017-10-01"
    #dir2="/home/rmartinshort/Documents/Berkeley/funclab/data_2017_AK/2017-01-01_2017-10-01"
    #mergedirs(dir1,dir2)

    #process(indir)

    #Check that the data is of sufficient quality
    quality_check(indir)
    os.chdir(cwd)


def rearrange(indir):

    '''Convert directory structure from obspyDMT into that expected by funclab'''

    if not os.path.isdir(indir):
        print "Path %s does not exist" %indir
        return

    os.chdir(indir)
    events = glob.glob('20*')

    if len(events) < 1:
        print "Appears processing has already occured in this directroy. Aborts. "
        return

    #Enter each event and grab the correct time                trace.write(newname,format='SAC')
    eventtimes = []

    for event in events:
        os.chdir(event)
        print event
        os.system('rm -rf BH_RAW info Resp')
        if os.path.isdir('BH_VEL'):
            os.chdir('BH_VEL')
            trfiles = glob.glob('*.BHZ')
            st = op.read(trfiles[0],format="SAC")
            eventtime = st[0].stats.starttime
            newname = 'Event_%i_%03d_%02d_%02d_%02d' %(eventtime.year,eventtime.julday,\
            eventtime.hour,eventtime.minute,eventtime.second)
            os.system('mv * ../')
            os.chdir('../')
            os.system('rm -rf BH_VEL')
            os.chdir('../')
            #os.system('pwd')
            os.system('mv %s %s' %(event,newname))
        else:
            print "Processing has already occured in event %s" %event
        os.chdir(indir)


def mergedirs(dir1,dir2):

    '''Merges the events files present in dir1 and dir2, to a new direcory called RAWTRACES, in the cwd'''

    cwd = os.getcwd()
    os.system('mv %s RAWTRACES' %dir1)

    os.chdir(dir2)
    upperleveldir = dir2.split('/')[-1]
    events = glob.glob('Event*')
    for event in events:
        os.system('mv %s/* %s/RAWTRACES/%s/%s' %(event,cwd,upperleveldir,event))
        print 'Moved files from %s/%s to RAWTRACES/%s' %(dir2,event,event)

    os.chdir(cwd)


def process(eventsdir):

    '''Basic processing and renaming of files for funclab convention'''

    os.chdir(eventsdir)
    events = glob.glob('Event*')

    #number of seconds before and after the P arrival to trim
    #offset = 180
    #Set the 1D velocity model that we want to use
    velmodel = TauPyModel(model="iasp91")

    for event in events:

        os.chdir(event)
        st = op.Stream()
        os.system('rm *.trim')
        os.system('rm *.01.*')
        sacfiles = glob.glob('*.BH*')
        fnames = []

        print '======================================'
        print "Reading files in event %s" %event
        print '--------------------------------------'

        for infile in sacfiles:
            try:
                fnames.append(infile)
                tr = op.read(infile,format='SAC')
                st.append(tr[0])
            except:
                print "Error on reading file %s and appending to stream object" %infile

        #Basic processing - demean, detrend and resample the stream
        print "Processing stream"
        print '--------------------------------------'


        #Don't need a high sampling rate since we're concerned with low frequency data
        st.interpolate(sampling_rate=20)
        st.detrend(type='demean')
        st.detrend(type='linear')

        #Determine file name and write out
        i = 0

        for trace in st:
            name = fnames[i]
            netname = trace.stats.sac.knetwk
            stname = trace.stats.sac.kstnm
            try:
                compname = trace.stats.sac.kcmpnm

                if compname == "BHE":
                    trace.stats.sac.cmpinc = 90.0
                    trace.stats.sac.cmpaz = 90.0
                elif compname == "BHN":
                   trace.stats.sac.cmpinc = 90.0
                   trace.stats.sac.cmpaz = 0.0

            except:
                if 'BHT' in name:
                    compname = 'BHT'
                elif 'BHR' in name:
                    compname = 'BHR'
                else:
                    print 'Error in compname location'

            eventtime = trace.stats.starttime
            newname = '%s.%s..%s.M.%i.%03d.%02d.%02d.%02d.SAC' %(netname,stname,compname,\
            eventtime.year,eventtime.julday,eventtime.hour,eventtime.minute,\
            eventtime.second)
            #trace.write(newname,format='SAC')
            #print "Written file %s" %newname

            evlat = trace.stats.sac.evla
            evlon = trace.stats.sac.evlo
            evdep = trace.stats.sac.evdp/1000.0
            stlat = trace.stats.sac.stla
            stlon = trace.stats.sac.stlo
            dist_degree = locations2degrees(stlat,stlon,evlat,evlon)
            arrivals = velmodel.get_travel_times(source_depth_in_km=evdep,\
            distance_in_degree=dist_degree,phase_list=["P"])

            try:
                arr = arrivals[0]
                arrtime = arr.time
                starttime = trace.stats.starttime
                trace.stats.sac.t0 = arrtime
                trace.stats.sac.o = 0.0
                trace.write(newname,format='SAC')
                print "Written file %s" %newname
            except:
                    print "Could not find P arrival for event"

            i += 1


        #for fname in fnames:
        #    os.system('rm -rf %s' %fname)


        os.chdir(eventsdir)


def quality_check(eventsdir):

    '''Check data quality of the sac files and discard bad data'''

    os.chdir(eventsdir)
    events = glob.glob('Event*')
    for event in events:

        os.chdir(event)
        print "Checking event %s" %event
        #Check that we have 3 comp data
        zfiles = glob.glob('*BHZ*.SAC')
        for fname in zfiles:
            spart = fname[:8]
            compfiles = glob.glob('%s*' %spart)
            if (len(compfiles) != 3):
                print "The following is not 3 comp data:"
                print compfiles
                #remove anyu files that do not have three comp data
                os.system('rm %s*' %spart)
            else:
                tr = op.read(fname,format='SAC')
                if max(tr[0].data) < 1e-7:
                    print "Something odd about files %s" %spart
                    os.system('rm %s*' %spart)
                elif tr[0].stats.sac.npts != 20000:
                    print "Files %s have incorrect sample number" %spart
                    os.system("rm %s*" %spart)


        os.chdir(eventsdir)


if __name__ == "__main__":

    main()
