#!/usr/bin/env python
#RMS2017
#Class for preparing and running Hermann inversions

import numpy as np
import pandas as pd
from scipy.interpolate import interp1d
import matplotlib.pyplot as plt
import os
import glob
import sys 


class PrepMod():
    
    '''Class to aid in the Hermann inversion workflow'''
    
    def __init__(self,datadir):
        
        '''Datadir should be the path to the Hermann inversion directory, which contins subdirectories corresponding to each
        surface point on the ambient noise grid'''
        
        self.hermdir = datadir 
        #location of the generic files used by the inversion 
        self.surfparms = '/home/rmartinshort/Documents/Berkeley/Ambient_Noise/AmbientNoise/Scripts/Depth_Invert/surf_params/'
        #location of previous 3D modelfile
        self.pastmodfile = None
        #script to run the inversion
        self.invertscript = None
        #script to produce the 3D model
        self.tdmodscript = '/home/rmartinshort/Documents/Berkeley/Ambient_Noise/AmbientNoise/Scripts/Depth_Invert/create_3d_mod.sh'
        #directory containing matlab script to load and smooth the matrix (should be re-written in python for ease)
        self.mdir = '/home/rmartinshort/Documents/Berkeley/Ambient_Noise/AmbientNoise/Scripts/Depth_Invert/matlab'
        #dirctory containing the plotting scripts: these get copied into the data directory for ease of use
        self.plottingdir = '/home/rmartinshort/Documents/Berkeley/Ambient_Noise/Depth_invert/plotting'
        #matlab script used to make the final model
        self.mscript = 'interp.m'
        
        if not os.path.isdir(self.hermdir):
            print 'Provided directory path %s does not exist' %self.hermdir
            sys.exit(1)
            
    def add_tdmodscript(self,newpath):
        
        '''Update the path to the 3D model script that is being used by this code'''
        
        if not os.path.exists(newpath):
            print 'Provided file path does not exist!'
        
        else:
            
            print 'Adding new 3D model script %s' %newpath
            
            self.tdmodscript = newpath
        
        
    def restart_inversion(self):
        
        '''Preps file struture to rertart an inversion by renaming all the 'end.mod' files as 'start.mod'
        the provided inversion script can then be run'''
        
        cwd = os.getcwd()
        os.chdir(self.hermdir)
        
        for directory in glob.glob('*_1D'):
            
            os.chdir(directory)
            
            if not os.path.exists('end.mod'):
                
                print 'Required file end.mod does not exist in dir %s: inversion may not have been run' %directory
                sys.exit(1)        
            
            if not os.path.exists('restarts.dat'):
                
                rs = open('restarts.dat','w')
                rs.write('R\n')
                rs.close()
                repeats = 1
            
            else:
                
                rs = open('restarts.dat','a')
                lines = rs.readlines()
                repeats = len(lines) + 1
                rs.write('R\n')
                rs.close()
                
            olddataname = 'inversion_%s' %str(repeats-1)
                
            os.system('mkdir -p %s' %olddataname)
            os.system('mv !(%s|restarts.dat) %s' %(olddataname,olddataname))
            os.system('cp %s/sobs.d .' %olddataname)
            os.system('cp %s/end.mod start.mod' %olddataname)
            os.system('cp %s/nnall.dsp .' %olddataname)
            
            print 'Done preparing folder %s' %directory
            
            os.chdir(self.hermdir)
        
        os.chdir(cwd)
        
    
    def add_invertscript(self,scriptpath):
        
        '''Add an inversion script to this instance of the class. The script defines how surf96 is set up for this particular inversion'''
        
        if not os.path.exists(scriptpath):
            
            print 'Input inversion script %s does not exist!' %scriptpath
        
        else:
            
            print 'Adding invert script %s' %scriptpath
            
            self.invertscript = scriptpath
    
    def run_inversion(self):
        
        '''Run the bash script that controls the inversion'''
        
        if self.invertscript:
        
            print 'Running inversion'
            print './%s' %self.invertscript
        
            os.system('./%s %s' %(self.invertscript,self.hermdir))
        
        else:
            
            print 'No valid scriptpath entered. Use add_invertscript() to do this'
            sys.exit(1)
            
    
    def generate_3d_model(self):
        
        '''Run the bash script to create a 3D model from the hermann inversion outputs'''
        
        if self.tdmodscript:
            
            print 'Generating 3D model'
            print '%s' %self.tdmodscript
            
            os.system('%s %s' %(self.tdmodscript,self.hermdir))
        
        else:
            
            print 'No valid 3D model script entered'
            
    def add_matlab_script(self,script):
        
        '''Add name of matlab script that will be used to generae the final model. Replaces interp.m'''
        
        self.mscript = script
        
        print 'script to produce final model set to %s' %self.mscript
            
    
    def generate_final_model(self):
        
        '''Run matlab script to generate the final model, ready for plotting'''
        
        print 'Calling MATLAB to generate final model'
        
        #requires mlabwrap - see http://mlabwrap.sourceforge.net/ and ensure that csh is installed 
        from mlabwrap import mlab
        
        if self.mdir:
            
            if not os.path.isdir(self.mdir):
                
                print 'provided matlab script directory %s does not exist' %self.mdir
                sys.exit(1)
            
            cwd = os.getcwd()
            os.chdir(self.mdir)
            
            print self.mscript
            print os.getcwd()
            
            #Ensure that the path to the hermdir is updated in the file interp.m!
            mlab.run(self.mscript)
            
            os.chdir(cwd)
        
        else:
            
            print 'No matlab script dir entered!'
    
    
    def setup_for_plotting(self):
        
        '''Directory maniplulation to prepare for visualization of the model'''
        
        print 'Setting up for plotting'
        
        cwd = os.getcwd()
        
        if self.plottingdir:
            
            if not os.path.isdir(self.plottingdir):
                print 'provided plotting directory %s does not exist' %self.plottingdir
                sys.exit(1)
            
            
            os.chdir(self.hermdir)
            os.system('mkdir -p plotting')
            os.system('mv *smooth* plotting')
            os.system('cp %s/* plotting' %self.plottingdir)
            
        else:
            
            print 'No plotting dir entered'
            
            



if __name__ == '__main__':
    
    #datadir1 = '/home/rmartinshort/Documents/Berkeley/Ambient_Noise/Depth_invert/ForHermannInvert_0.5'
    #datadir2 = '/home/rmartinshort/Documents/Berkeley/Ambient_Noise/Depth_invert/ForHermannInvert_0.5_const'
    datadir = '/home/rmartinshort/Documents/Berkeley/Ambient_Noise/Depth_invert/ForHermannInvert_0.5_ebh'
    inversionscript = 'CPS_invert_ebp2.sh'
    
    inv = PrepMod(datadir)
    #inv2 = PrepMod(datadir2)
    #inv.restart_inversion()
    inv.add_invertscript(inversionscript)
    inv.run_inversion()
    inv.generate_3d_model()
    inv.generate_3d_model()
    inv.generate_final_model()
    inv.setup_for_plotting()
            
        
        
