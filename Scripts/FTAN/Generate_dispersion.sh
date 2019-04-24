#!/bin/bash

#RMS 2017
#Generate example dispersion curve for use with tomo codes
#Change the earth model to fit your region of interest


lat=57.0
lon=-147.0

echo "Input earth model: inmodel.prem:"
#cat inmodel.prem

echo "Example dispersion curve:"
echo " 0.0_0.0_calculated_dispersion"

generate_dispersion inmodel.prem 0.0_0.0_calculated_dispersion 57.0 -147.0 

rm tmpsrf*

infilename=${lat}_${lon}_calculated_dispersion
echo $infilename

./Extract_example_dispersion_curve.py -infile $infilename -LR R -PG Ph
