#!/bin/bash
#Filtering of SAC files for use with AIMBAT
file=$1
bp1=$2
bp2=$3
outfile=$4
sac <<EOF
read $file
bp co $bp1 $bp2
write $outfile
quit
EOF
