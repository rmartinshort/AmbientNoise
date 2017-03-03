#!/bin/bash

#----------------------------------------------
# RMS 2017
#----------------------------------------------
#Runs the Cut to Sym comps C code


#Directory where the month stacks can be found
stackdir=/data/dna/rmartin/Ambient_noise/Alaska/DATA/COR/STACKS/

cwd=(`pwd`)
echo $cwd

cd $stackdir
ls *SAC > filelist.cut

/data/dna/software/ANT/MAIN_ANT_CODES/src/CUT_TO_SYM_COMPONENT/rms_version/bin/Cut_To_Sym_Comps.exe filelist.cut

mkdir TWO_SIDED
mkdir SYM

mv *_s SYM
mv *.SAC TWO_SIDED

cd $cwd
