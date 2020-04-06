#!/usr/bin/python

import sys, getopt
import numpy


RESULT_FORMAT="{:8.2f} {:8.2f} {:8.2f} {:8.2f} {:8.2f} {:8.2f}"

def main(argv):
   inputfile = ''
   outputfile = ''
   try:
      opts, args = getopt.getopt(argv,"hi:o:",["ifile=","ofile="])
   except getopt.GetoptError:
      print 'test.py -i <inputfile> -o <outputfile>'
      sys.exit(2)
   for opt, arg in opts:
      if opt == '-h':
         print 'test.py -i <inputfile> -o <outputfile>'
         sys.exit()
      elif opt in ("-i", "--ifile"):
         inputfile = arg
      elif opt in ("-o", "--ofile"):
         outputfile = arg

   with open(inputfile) as fp:
    Lines = fp.readlines()
    del Lines[0]
    res = [line.rstrip() for line in Lines]
    microres = [float(line)*1000000 for line in res]
    a=numpy.array(microres)
    print RESULT_FORMAT.format(numpy.mean(a), numpy.std(a), numpy.percentile(a,
         90), numpy.percentile(a,95), numpy.percentile(a,99),
         numpy.percentile(a,99.9))


if __name__ == "__main__":
   main(sys.argv[1:])



