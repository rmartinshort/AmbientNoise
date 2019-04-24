#!/bin/bash

for grdfile in *.grd; do
    echo $grdfile
    ./map_vel_dep.sh $grdfile
done

