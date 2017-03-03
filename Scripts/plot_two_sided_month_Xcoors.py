#!/usr/bin/env python

#plot the two sided, month long cross correlations for a given directory and filter bands

import os
import glob
import matplotlib.pylab as plt
import obspy
import argparse
import sys

def gen_stream(infiles):

    '''Make an obspy stream and order it by distance'''

    instream = obspy.Stream()

    print 'Loading traces ...'

    for sacfile in infiles:
        tr = obspy.read(sacfile,format='SAC')
        tr[0].stats.distance = tr[0].stats.sac.dist*1000
        instream += tr[0]

    sortedstream = obspy.Stream()

    traces = []
    distances = []

    for trace in instream:

	   traces.append(trace)
	   distances.append(trace.stats.sac.dist)

    print 'Sorted distances'
    sorted_dist = list(sorted(distances))

    for distance in sorted_dist:

        idx = distances.index(distance)
        tr = traces[idx]
        sortedstream += tr

    return sortedstream, sorted_dist


def plotstream(sortedstream,distances,maxtraces):

    '''There may be too many traces to make a nice/quick plot - only plot a selection of them'''

    ld = len(distances)


    if ld > maxtraces:

        plotstream = obspy.Stream()

        step = int(ld/maxtraces)

        for i in range(0,ld,step):
            plotstream += sortedstream[i]


    else:

        plotstream = sortedstream


    plotstream.plot(type='section',recordstart=4000,recordlength=2000,orientation='horizontal')
    plt.show()




def main():

    parser = argparse.ArgumentParser()
    parser.add_argument('-indir',action='store',dest='indir',help='Full path to directory containing X correlation sac files')
    results = parser.parse_args()

    if os.path.exists(results.indir):

        indir = results.indir

    else:

        raise NameError('Provided dir does not exist')
        sys.exit(1)


    cwd = os.getcwd()
    os.chdir(indir)

    sacfiles = glob.glob('*SAC')
    sstream, sdists = gen_stream(sacfiles)

    #plot the two sided X correlations 
    plotstream(sstream,sdists,100)





if __name__ == '__main__':

    main()




