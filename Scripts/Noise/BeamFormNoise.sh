#!/bin/bash 

minvel=10
maxtime=$1

/data/dna/rmartin/Ambient_noise/CODES/src/Noise_Location/src/bin/locate_energy station.lst sacfiles << EOF
$minvel
$maxtime
EOF
