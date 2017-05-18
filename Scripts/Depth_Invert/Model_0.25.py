#!/usr/bin/env python
#script to make use of the 3Dmodel class to build a 0.1x0.1 
#Vs model

import TDmodel_manip as tdm

def main():
    
    datadir = '/home/rmartinshort/Documents/Berkeley/Ambient_Noise/Depth_invert/ForHermannInvert_0.25_v1'
    inv = tdm.PrepMod(datadir)
    inv.addinvertscript('/home/rmartinshort/Documents/Berkeley/Ambient_Noise/AmbientNoise/Scripts/Depth_Invert/CPS_invert_vel100_v1.sh')
    inv.add_tdmodscript('/home/rmartinshort/Documents/Berkeley/Ambient_Noise/AmbientNoise/Scripts/Depth_Invert/create_3d_mod_0.25.sh')
    inv.run_inversion()
    inv.generate_3d_model()
    inv.add_matlab_script('interptwofive.m')
    inv.generate_final_model()
    inv.setup_for_plotting()
    
if __name__ == "__main__":
    main()