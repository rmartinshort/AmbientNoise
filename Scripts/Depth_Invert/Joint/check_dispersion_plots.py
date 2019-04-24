#!/usr/bin/env python
#Gather all the dispersion.ps files from each station and put them in a folder, ready for QC

import glob
import os

def main():

    dirs = glob.glob('*_*')

    cwd = os.getcwd()

    if not os.path.exists('Dispersion_curves'):
        os.system('mkdir Dispersion_curves')

    for directory in dirs:

        if os.path.isdir(directory):
            os.chdir(directory)
            if os.path.exists('dispersion.ps'):
                cname = directory+'_dispersion.ps'
                os.system('cp dispersion.ps ../Dispersion_curves/%s' %cname)
                print "Copied dispersion.ps from %s" %directory
            os.chdir(cwd)

    os.system('zip -r Dispersion_plots.zip Dispersion_curves')



if __name__ == "__main__":

    main()
