#! /usr/bin/env python

import ROOT as R
import os
import optparse

usage = "usage: run from Timing-TOFPET: process_runs.py -r 1,2,3,4,5 [or 1-5] -d myfolder"

parser = optparse.OptionParser(usage)

parser.add_option("-r", "--runs", dest="runs",
                  help="list of run numbers to be analyzed: 1,2,3,4,5,6 or 1-6")

parser.add_option("-d", "--directory", dest="directory",
                  help="input/output directory")

(opt, args) = parser.parse_args()

if not opt.runs:   
    parser.error('Run list not provided')

if not opt.directory:   
    parser.error('Input/Output directory not provided')

################################################

R.gROOT.SetBatch(True)

################################################

run_list = []
if ("," in opt.runs) and ("-" not in opt.runs):
    run_list = opt.runs.split(",")
    print("Runs to be processed ", run_list, " in directory ", opt.directory)
if ("-" in opt.runs) and ("," not in opt.runs):
    run_list_tmp = opt.runs.split("-")
    for ii in range( int(run_list_tmp[0]) , int(run_list_tmp[1])+1 ) :
        run_list.append(str(ii))
    print("Runs to be processed ", run_list, " in directory ", opt.directory)
if ( ("," not in opt.runs) and ("-" not in opt.runs)):
    run_list.append(opt.runs)

if len(run_list) == 0:
    parser.error('No good run list provided')

list_allfiles = os.listdir(opt.directory)

for irun in run_list:

    run = "Run"+irun.zfill(6)
    print("Processing ", run, " .....")

    input_filename_rawf = ""

    list_allfiles = os.listdir(opt.directory)
    #print list_allfiles                                                                                                                                                                  
    for thisfile in list_allfiles:

        if ("~" in thisfile):
            continue
        
        if ( (run in thisfile) and (thisfile.split('.')[-1] == "WData") ):
            input_filename_rawf = opt.directory + "/" + thisfile
            print(input_filename_rawf)

    if (input_filename_rawf==""):
        parser.error('missing rawf file for '+run)

    fileNameNoExtension = input_filename_rawf.split(".WData")[0]
    #print fileNameNoExtension

    ###############################
    ### 2) Create file with singles
    ###############################

    ## Add branches to root tree (singles)
    print("Raw2Root...")
    #inputfilename = fileNameNoExtension+"_singles.root"
    R.gROOT.ProcessLine('o = TString(gSystem->GetMakeSharedLib()); o = o.ReplaceAll(" -c ", " -std=c++11 -c "); gSystem->SetMakeSharedLib(o.Data());')
#    R.gBenchmark.Start( 'spettro_run%d'%int(irun))
    R.gROOT.ProcessLine('.X /home/cmsdaq/TransmissionBench_Analysis/spettro_photo_newRange.C+("%s");'%input_filename_rawf)
#    R.gBenchmark.Show( 'spettro_run%d'%int(irun))
    print("Raw2Root completed")
    print("\n")
