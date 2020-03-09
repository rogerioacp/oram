#!/usr/bin/python

import sys, getopt
import numpy


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
   #print 'Input file is "', inputfile
   #print 'Output file is "', outputfile

   with open(inputfile) as fp:
    #print "going to read lines"
     Lines = fp.readlines()
     del Lines[0]
     res = [line.rstrip() for line in Lines]
     microres = [float(line)*1000000 for line in res]
     a=numpy.array(microres)

     #print a
     #res = a.map(lambda x: x*1000000)
     #print "mean is ", numpy.mean(a)
     #print "stddev is ", numpy.std(a)
     #print "90%", numpy.percentile(a, 90)
     #print "95%", numpy.percentile(a, 95)
     #print "99%", numpy.percentile(a, 99)
     #print "99.9%", numpy.percentile(a, 99.9)
     print numpy.mean(a), ",", numpy.std(a), ",", numpy.percentile(a, 90), ",", numpy.percentile(a, 95), ",", numpy.percentile(a, 99), ",", numpy.percentile(a, 99.9)





if __name__ == "__main__":
   main(sys.argv[1:])



