#!/bin/bash
low=$1
high=$2

for infile in *.SAC; do 
    echo $infile

sac<<EOF
r $infile
bp co $low $high
w $infile.tmp
quit
EOF

done
