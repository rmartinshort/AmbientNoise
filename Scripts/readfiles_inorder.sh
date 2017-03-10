#!/bin/bash


if [ -f 'tlist.tmp' ]; then 
	rm tlist.tmp
fi

sacfiles=(*_s)


for sacfile in "${sacfiles[@]}"; do 
   dist=`sac getsacdist.m $sacfile | awk '{if ($1=="dist") print $3}'`
   echo $sacfile $dist 
   echo $sacfile $dist >> tlist.tmp
done

sort -n -k2 tlist.tmp | awk '{if (NR==1) printf "r %s ", $1; else if (NR%10==1) printf "r more %s ", $1; else if (NR%10==0) printf "%s \n", $1; else printf "%s ", $1}' > olist.m

