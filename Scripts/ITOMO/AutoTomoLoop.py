#!/usr/bin/env python 
#RMS 2017
import os
import glob 


def main():

    #alphavals = [25,50,100,200,300,400,500,600]
    alphavals = [500,600,700]
    betavals = [50,100,200,400]
    sigmavals = [100]

    for alpha in alphavals:
	for beta in betavals: 
	    for sigma in sigmavals:

		outdirname = '%s_%s_%s' %(str(alpha),str(beta),str(sigma))
		os.system('mkdir -p %s' %outdirname)
		os.system('./Run_TomoLoop_auto.sh %s %s %s' %(str(alpha),str(beta),str(sigma)))
		os.system('mv *.grd %s' %outdirname)
		os.system('mv *.rea %s' %outdirname)
		os.system('mv *.res %s' %outdirname)

		print 'done with alpha = %g, beta = %g, sigma = %g' %(alpha, beta, sigma)

def prep_for_copy():

    path = os.getcwd()
    i = 0

    for directory in os.walk(path):
	if i > 0:
	    os.chdir(directory[0])
	    os.system('rm *.rea *.res')
	i += 1 



if __name__ == "__main__":

	main()
	#prep_for_copy()
