#!/usr/bin/env python
#Apply resolution masks to the perturbation grd files

import glob
import os

def main():

	velpertfiles = glob.glob('*data*.grd')
	maskfiles = glob.glob('reslengths/*ph_mask.txt')

	print maskfiles
	print velpertfiles

    #Generate and apply the mask grid file
	for maskfile in maskfiles:
		per = maskfile.split('/')[-1].split('_')[0]
		
		for pertfile in velpertfiles:

			if per in pertfile:

				print maskfile, pertfile

				maskgrd = 'masked_'+pertfile
				mask = 'M_'+pertfile

				#Generate mask
				os.system('gmt grdmask %s -G%s -R188.5/237.5/51.5/73.5 -I0.2 -NNaN/1/1' %(maskfile,mask))
				#Downsample
				os.system('gmt grdsample %s -G%s -I0.2' %(pertfile,pertfile))
				#Apply mask
				os.system('gmt grdmath %s %s MUL = %s' %(mask,pertfile,maskgrd))
				#Upsample
				os.system('gmt grdsample %s -G%s -I0.1' %(maskgrd,maskgrd))



if __name__ == '__main__':

	main()
