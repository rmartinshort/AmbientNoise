#!/usr/bin/env python
#Compare group and phase velocity curves estimated after initial FTAN vs after phase-matching

import pandas as pd
import matplotlib.pyplot as plt
import glob
import argparse
import os
import numpy as np
from scipy import interpolate


def meancurve(intype='group'):

	'''Determine the mean phase velocity curve from a group of measurements in disp files'''

	fig = plt.figure()

	ftanfiles = glob.glob('*2_DISP.1')


	if intype == 'phase':
		df_dispersion = pd.read_csv('Alaska_example_dispersion_R_Ph.dat',sep=' ',names=['Period','Velocity'])
		col = 4
	elif intype == 'group':
		df_dispersion = pd.read_csv('Alaska_example_dispersion_R_Gr.dat',sep=' ',names=['Period','Velocity'])
		col = 3

	i = 0

	for infile in ftanfiles:

		df = pd.read_csv(infile,sep="\s+|\t+|\s+\t+|\t+\s+",\
                  names=['Period_number','CentralPeriod','ObservedPeriod','GroupVel','PhaseVel','Amp','SNR'])
		
		noper = max(df.Period_number) #number of periods being measured 

		plt.plot(df.CentralPeriod,df.ix[:,col],'k-',alpha=0.1)
		plt.hold('on')

	plt.plot(df_dispersion.Period,df_dispersion.Velocity,'r-',alpha=0.8)
	plt.hold('on')

	if intype == 'phase':
		plt.ylabel('Phase velocity (km/s)')
		plt.xlim([0,60])
		plt.ylim([2.9,4.1])

		x,y = construct_example()
		plt.plot(x,y,'r--',alpha=1)
	else:
		plt.ylabel('Group velocity (km/s)')
		plt.xlim([0,60])
		plt.ylim([2.5,3.8])

	plt.xlabel('Period (s)')

	plt.grid('on',linestyle='-',color='b',zorder=10)
	plt.show()



def construct_example():

	'''Generate example phase velocity curve'''

	x = np.linspace(6,100,95)

	df_dispersion = pd.read_csv('Alaska_example_dispersion_R_Ph.dat',sep=' ',names=['Period','Velocity'])
	pvals = list(df_dispersion.Period.values[34:])
	#print pvals
	vvals = list(df_dispersion.Velocity.values[34:])
	#print vvals

	m_pvals = [6,10,15,20,25,30,35]
	m_vvals = [3.1,3.17,3.33,3.48,3.61,3.72,3.81]

	tmpvals = m_pvals + pvals
	tmvvals = m_vvals + vvals

	#print tmpvals
	#print tmvvals

	f = interpolate.interp1d(tmpvals,tmvvals,kind='cubic')
	y = f(x)

	#write output dispersion curve
	outfile = open('Mean_phase_vel_prediction.dat','w')

	for element in zip(x,y):
	    outfile.write('%.4f %.4f\n' %(element[0],element[1]))

 	outfile.close()

	return x,y



def compare2():

	'''Compare predictions from the FTAN and phase matching + FTAN stages'''

	res1 = 'COR_TA_EPYK_LHZ.TA_P17K_LHZ.SAC_s_1_DISP.1'
	res2 = 'COR_TA_EPYK_LHZ.TA_P17K_LHZ.SAC_s_2_DISP.1'

	ftan1 = pd.read_csv(res1,sep="\s+|\t+|\s+\t+|\t+\s+",\
                  names=['Period_number','CentralPeriod','ObservedPeriod','GroupVel','PhaseVel','Amp','SNR'])
	ftan2 = pd.read_csv(res2,sep="\s+|\t+|\s+\t+|\t+\s+",\
                  names=['Period_number','CentralPeriod','ObservedPeriod','GroupVel','PhaseVel','Amp','SNR'])

	plt.plot(ftan1.GroupVel,label='FTAN1_GV')
	plt.hold('on')
	plt.plot(ftan2.GroupVel,label='FTAN2_GV')
	plt.hold('on')
	plt.plot(ftan1.PhaseVel,label='FTAN1_PV')
	plt.hold('on')
	plt.plot(ftan2.PhaseVel,label='FTAN2_PV')
	plt.grid('on')
	plt.legend(loc='best')
	plt.show()


def main():

	parser = argparse.ArgumentParser()
	parser.add_argument('-indir',action='store',dest='indir',help='Full path to a directory containing the XC directory structure')
	parser.add_argument('-type',action='store',dest='type',help='Velocity type to plot: enter "group" or "phase"')
	results = parser.parse_args()

	if os.path.exists(results.indir):
		indir = results.indir
	else:
		print 'Provided dir does not exist'
		sys.exit(1)

	cwd = os.getcwd()
	os.chdir(indir)

	#construct_example()

	meancurve(results.type)



if __name__ == '__main__':

	main()
