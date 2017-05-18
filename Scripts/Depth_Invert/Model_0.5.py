#!/usr/bin/env python
#script to make use of the 3Dmodel class to build a 0.5x0.5 
#Vs model

import TDmodel_manip as tdm

def main():
    
    datadir = '/home/rmartinshort/Documents/Berkeley/Ambient_Noise/Depth_invert/ForHermannInvert_0.5_v3_nosmooth'
    inv = tdm.PrepMod(datadir)
    inv.add_tdmodscript('/home/rmartinshort/Documents/Berkeley/Ambient_Noise/AmbientNoise/Scripts/Depth_Invert/create_3d_mod_0.5.sh')
    inv.add_invertscript('CPS_invert_vel100_v3_nosmooth.sh')
    inv.run_inversion()
    inv.generate_3d_model()
    inv.add_matlab_script('interpfive.m')
    inv.generate_final_model()
    inv.setup_for_plotting()
    
if __name__ == "__main__":
    main()
