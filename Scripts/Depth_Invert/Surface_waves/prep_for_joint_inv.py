#!/usr/bin/env python
#RMS 2017
#Convert the output from gin teleseismic surface wave tomography into a version that can 
#be used alongside phase output from the ambient noise tomography

import glob 
import os
import pandas as pd
import numpy as np
import sys

def main():

    #You should run this in the folder that contains phase velocity output from the ambient noise processing workflow and ASWMS

	ambph = glob.glob('data*')
	surph = glob.glob('helmholtz*')

	if len(surph) > 1:
		print 'Renaming teleseismic outputs'

		for datafile in surph:
			df = pd.read_csv(datafile,sep=' ',names=['lat','lon','vel'])
			df = df[['lon','lat','vel']]
			df.lon = df.lon + 360
			#print df.head(10)
			#sys.exit(1)

			#Remove tails
			#df = remove_tails(df)

			df.to_csv(datafile,sep=' ',index=0,header=0)
			period = datafile.split('_')[-1].split(".")[0]
			outfilename = 'Tele_data'+period+'s_ph.txt_'+period+'.1'
			print outfilename
			os.system('mv %s %s' %(datafile,outfilename))
	else:
		print 'Teleseismic outputs already renamed (none found)'

	if len(ambph) > 1:

		print 'Renaming ambient noise outputs'

		for datafile in ambph:
			outfile = 'Amb_'+datafile
			os.system('mv %s %s' %(datafile,outfile))

	else:
		print 'ANT outputs already renamed'

	infiles = glob.glob('*data*.1')
	make_grds(infiles)

def remove_tails(df):
    
    meanv = np.mean(df.vel)
    stdv = np.std(df.vel)
    #remove values more than two standard deviations from the mean
    #and set them to those values
    m1 = meanv + 2*stdv
    m2 = meanv - 2*stdv
    df.ix[df.vel > m1, 'vel' ] = m1
    df.ix[df.vel < m2, 'vel' ] = m2

    return df


def crop_file(infile):

	"Set values in a file that lie outside some spatial region to the mean"

	print infile

	df = pd.read_csv(infile,sep='\s+',names=['lon','lat','vel'])
	dftmp = df.dropna(subset=['vel'])
       
        #get the mean velocity value and fill parts of the model that do not have data with this 
	#value
	meanv = np.mean(dftmp.vel)
	print meanv
	df = df.fillna(value=meanv)
	df.to_csv(infile,sep=' ',header=0,index=0)

	return 0



def make_grds(infiles):

	increment=0.2
	minlat=57
	maxlat=70
	minlon=198
	maxlon=226

	for datafile in infiles:

		#get the period value
		period = str(datafile.split('_')[-1].split('.')[0])

		outfilename = datafile+'.grd'
		#outfile2 = datafile+'2.grd'

		print 'Working on %s' %datafile
		crop_file(datafile)

		#Convert to GRD
		os.system('gmt xyz2grd %s -R%g/%g/%g/%g -I0.5 -Gtmp.grd' %(datafile,minlon,maxlon,minlat,maxlat))
		os.system('gmt grdsample tmp.grd -I%g/%g -G%s' %(increment,increment,outfilename))
		#os.system('gmt surface %s -R%g/%g/%g/%g -I%g/%g -G%s' %(datafile,minlon,maxlon,minlat,maxlat,increment,increment,outfilename))

		#Extract to text file in the appropriate format
		if 'Amb' in datafile: 
			ofname = 'Amb_tmp_%04d.ph' %float(period)
		elif 'Tele' in datafile:
			ofname = 'Tele_tmp_%04d.ph' %float(period)

		os.system('gmt grd2xyz %s > %s' %(outfilename,ofname))
		crop_file(ofname)


def build_database():

        #cap the period range 
    	pcap = 130

    	#Delete the .db file if it already exists
	if os.path.exists('Alaska_ALL.db'):
	    os.system('rm Alaska_ALL.db')
	
	#delete all TMP2 files if they already exist
	os.system('rm *TMP2')

	#define the periods at which there will be overlaps between the amb and tele phase velocity predictions
    	overlaps=[25,26,27,28,29,30,31,32,33,34,35]

	datafiles = glob.glob('*ph')

	#generate list of periods
	pers = []

	for f in datafiles:
		per = float(f.split('_')[2].split('.')[0])
		if (per not in pers) and (per <= pcap):
			pers.append(per)

	pers = list(sorted(pers))

	#find files associated with each period and append:
	for p in pers:

			pascfiles = glob.glob('*tmp_*0%s.ph' %(str(int(p))))
			print 'Dealing with period %s' %p 

			if (p < overlaps[0] or p > overlaps[-1]):
				infile = pascfiles[0]
				print 'Using file %s' %infile
				df = pd.read_csv(infile,sep='\s+',names=['lon','lat','vel'])

				out = df.isnull().any()
				if out.vel == True:
				    print 'Error" found NAN in file %s' %infile
				    sys.exit(1)

				print len(df)

				ofilename = infile+'TMP2'
				ofile = open(ofilename,'w')
				for index, row in df.iterrows():
					ofile.write('%3.1f   %3.1f   %3.0f   %3.5f\n' %(row.lon,row.lat,float(p),row.vel))
				ofile.close()

				os.system('paste %s >> Alaska_ALL.db' %ofilename)

			elif (p in overlaps):
		    		per1 = float(pascfiles[0].split('_')[-1].split('.')[0])
		    		per2 = float(pascfiles[1].split('_')[-1].split('.')[0])

		    		print 'Combining amb and tele files for period %i' %per1

		    		if per1 == per2:
					ind = overlaps.index(int(p))
					f1 = pascfiles[0]
					f2 = pascfiles[1]

					if 'Tele' in f1:
			    			f1 = pascfiles[1]
			    			f2 = pascfiles[0]

					df3 = scalefiles(f1,f2,ind)
					print len(df3)
					ofilename = 'TeleAmb_'+f1[3:]+'TMP2'

					ofile = open(ofilename,'w')
					for index, row in df3.iterrows():
			   			ofile.write('%3.1f   %3.1f   %3.0f   %3.5f\n' %(row.lon,row.lat,float(p),row.vel))
					ofile.close()

					os.system('paste %s >> Alaska_ALL.db' %ofilename)

			else:

		    		print 'File %s not used!' %pascfiles


def scalefiles(f1,f2,index):

    '''f1 comes from ambient noise, f2 from teleseismic observations'''

    #scale refers to teleseismic contribution
    scales = [0,0.1, 0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0]
    scale1 = scales[index]
    scale2 = 1-scale1

    df1 = pd.read_csv(f1,sep='\s+',names=['lon','lat','vel'])
    df2 = pd.read_csv(f2,sep='\s+',names=['lon','lat','vel'])

    d = {'lon': df1.lon, 'lat': df1.lat, 'vel': df1.vel*scale2 + df2.vel*scale1}
    df3 = pd.DataFrame(d)
    return df3


if __name__ == "__main__":
    main()
    build_database()

