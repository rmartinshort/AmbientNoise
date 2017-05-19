#!/bin/bash 

#Delete large numbers of files (when rm won't work)
cwd=`pwd`
datadir=ForHermannInvert

cd $datadir

for dir in *; do
    echo $dir
    rm -rf $dir
done


cd cwd

