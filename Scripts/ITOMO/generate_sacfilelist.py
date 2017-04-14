#!/usr/bin/env python
import glob

def main():

    dispfiles = glob.glob('*DISP.1')

    outfile = open('sacFileList','w')

    for infile in dispfiles:

	outfile.write('%s\n' %infile[:-9])

    outfile.close()


if __name__ == "__main__":

    main()
