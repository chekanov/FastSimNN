
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TH1D.h>
#include <TH1.h>
#include "TMath.h"
#include"time.h"
#include <dirent.h>
#include <string>
#include <vector>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <TProfile2D.h>

double RMS90(TH1* histogram)
  {
 

  double _width=0;
  double _mean=0;

 if ( histogram== NULL) return -1000;
         
   float sum   = 0;
   double sx   = 0.;
   double sxx  = 0.;
     
    int nbins = histogram->GetNbinsX();
    
   float total = 0;
    
   for(int i = 1; i <= nbins; i++)
   {
     float binx = histogram->GetBinLowEdge(i)+histogram->GetBinWidth(i)/2.0;
     float yi   = histogram->GetBinContent(i);
      sx        += yi*binx;
      sxx       += yi*binx*binx;
      total     +=yi;
     }

    int is0 = 0;
    
   for(int i = 1; i <= nbins && sum < total/10.; i++)
       {
       float yi = histogram->GetBinContent(i);
       sum      += yi;
         is0      = i;
       }
   
     float xmean = 0;
     for(int istart = 1; istart <= is0; istart++)
      {
         double sumn   = 0.;
         double csum   = 0.;
         double sumx   = 0.;
         double sumxx  = 0.;
         int iend      = 0;

         for(int i = istart; i <= nbins && csum <0.9*total; i++)
           {
             float binx = histogram->GetBinLowEdge(i)+histogram->GetBinWidth(i)/2.0;
             float yi   = histogram->GetBinContent(i); 
             csum += yi;
 
             if(sumn < 0.9*total)
               {
                 sumn   += yi;
                 sumx   += yi*binx;
                 sumxx  += yi*binx*binx;
                 iend   = i;
               }
           }
       
         float mean   = sumx/sumn;
         float meansq = sumxx/sumn;
         float rms    = sqrt(meansq - mean*mean);

         if(istart == 1 || rms < _width)
           {
             xmean = mean;
             //_range.first  = histogram->GetBinLowEdge(istart);
            //_range.second = histogram->GetBinLowEdge(iend);
             _width     = rms;
             _mean    = xmean;
           }
       }
     //converting from rms90 to something resembling a sigma
     //_width=_width/0.8;

     return _width;
 
   }
