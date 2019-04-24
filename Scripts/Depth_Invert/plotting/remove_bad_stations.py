#!/usr/bin/env python

import glob
import sys
import os

def main():

   infile = open('bad_stations','r')
   lines = infile.readlines()

   if not os.path.isdir('reject_stations'):
      os.system('mkdir reject_stations')

   for line in lines:

      if line[0] != '#':
           station_dir = line.strip()
           os.system('mv %s reject_stations' %station_dir)


if __name__ == "__main__":

    main()
