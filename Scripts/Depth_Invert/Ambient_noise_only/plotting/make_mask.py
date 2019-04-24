#!/usr/bin/env python
#Create a mask file that excludes regions known to have poor resolution - this is a somewhat arbitary choice but its just for graphics 
#purposes anyhow 

import pandas as pd
import os
import glob


def main():
    
    df = pd.read_csv('dslice_0.dat',sep=' ',header=0,names=['lon','lat','vel'])
    outfilename = 'Resmask.dat'
    outfile = open(outfilename,'w')
    
    lons = df.lon.values
    lats = df.lat.values
    
    for (lo,la) in zip(lons,lats):
        crt = app_crit(lo,la)
        if crt == True:
            outfile.write('%g %g\n' %(lo,la))
    
    outfile.close()
    
    #make the resolution mask
    os.system('gmt grdmask %s -Gresmask.grd -R189/237/56/73 -I0.2 -NNan/1/1' %outfilename)
              
    #apply the mask to all grdfiles
    grdfiles = glob.glob('dslice*.grd')
    
    for grdfile in grdfiles:
        
        if 'masked' not in grdfile:
            print 'Dealing with file %s' %grdfile 
            maskfilename = grdfile.split('.')[0]+'_masked.grd'
            os.system('gmt grdsample %s -I0.2 -Gtmp.grd' %grdfile)
            os.system('gmt grdmath tmp.grd resmask.grd MUL = %s' %maskfilename)
            os.system('gmt grdsample -I0.1 %s -G%s' %(maskfilename,maskfilename))
    
    os.system('rm tmp.grd')
        

def app_crit(lo,la):
    
    if (204 < lo < 230) and (58 < la < 69):
        return True
    else:
        return False
    
    


if __name__ == "__main__":
    main()