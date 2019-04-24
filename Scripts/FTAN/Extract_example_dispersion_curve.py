#!/usr/bin/env python

#RMS Feb 2017
#Extract example dispersion curve values from a file
import pandas as pd
import sys
import argparse


def main():

	parser = argparse.ArgumentParser()

	parser.add_argument('-infile',action='store',dest='inputfile',help='Input the name of the dispersion curve file to extract from')
	parser.add_argument('-LR',action='store',dest='LR',help='Input L to extract Love velocities, R to extract Rayleigh velocities')
	parser.add_argument('-PG',action='store',dest='GP',help='Input Ph to extract phase velocities, Gr to extract group velocities')

	results = parser.parse_args()


	infilename = results.inputfile   #'57.0_-147.0_calculated_dispersion'

	#infile = open('57.0_-147.0_calculated_dispersion','r')
	#lines = infile.readlines()
	#infile.close()

	extractphase = results.LR #should be R or L
	extracttype= results.GP  #should be Ph or Gr

	outfilename = 'Alaska_example_dispersion_%s_%s.dat' %(extractphase,extracttype)


	data = pd.read_csv(infilename,sep=' ',header=None,names=['L_R','Ph_Gr','Fundamental','Period','Velocity','Uncertainty'])
	
	if extractphase == 'R':

		tmp = data[data.L_R == 2]

	elif extractphase == 'L':

		tmp = data[data.L_R == 1]

	else:

		sys.exit(1)

	if extracttype == 'Ph':

		tmp = tmp[tmp.Ph_Gr == 1]

	elif extracttype == 'Gr':

		tmp = tmp[tmp.Ph_Gr == 2]

	else:

		sys.exit(1)

	
	pvs = tmp.drop(['L_R','Ph_Gr','Fundamental','Uncertainty'],axis=1)
	pvs.to_csv(path_or_buf='%s' %outfilename,header=None,sep=' ',index=False)



if __name__ == '__main__':

	main()

