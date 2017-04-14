#!/usr/bin/env python
#test for plotting SNR vs period graph

import pandas as pd
import matplotlib.pyplot as plt
import matplotlib
import numpy as np
import glob
import os
from scipy import stats
import argparse

#Open all the .DISP.1 files and extract the period and SNR information, then append it a series of lists
#the aim will be to plot a scatter plot of all the data, then draw a regression line to indicate the 'average' 
#since the measurements were taken at different periods, this is all we can really do

def main():

	parser = argparse.ArgumentParser()
	parser.add_argument('-indir',action='store',dest='indir',help='Full path to a directory containing the XC directory structure')
	results = parser.parse_args()

	if os.path.exists(results.indir):
		indir = results.indir
	else:
		print 'Provided dir does not exist'
		sys.exit(1)

	cwd = os.getcwd()
	os.chdir(indir)

	periods = []
	SNR = []
	thres = 10

	disp_files = glob.glob('*_2_DISP.1')

	i = 0 
	for infile in disp_files:
	    
	    #print infile
	    
	    resdf = pd.read_csv(infile,sep="\s+|\t+|\s+\t+|\t+\s+",\
	                  names=['Period_number','CentralPeriod','ObservedPeriod','GroupVel','PhaseVel','Amp','SNR'])
	    
	    select = resdf[(resdf.SNR <= 80) & (resdf.SNR >= thres)]
	    
	    pers = list(select.ObservedPeriod.values)
	    snrs = list(select.SNR.values)
	    periods = periods + pers
	    SNR = SNR + snrs
	        
	    i += 1

	#plot the SNR values
	plt.scatter(periods,SNR,alpha=0.3)

	#Determine a linear regression line and plot it
	slope, intercept, r_value, p_value, std_err = stats.linregress(periods,SNR)
	X = np.linspace(10,40,1000)
	Y = slope*X + intercept
	plt.plot(X,Y,'r--')
	plt.grid()
	plt.xlabel('Period (S)')
	plt.ylabel('SNR value')
	plt.show()

if __name__ == '__main__':

	main()
