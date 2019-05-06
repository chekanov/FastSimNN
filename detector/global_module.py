
import sys
sys.path.append("modules/")

from array import *
from math import *
from AtlasUtils import *
from ROOT import TH1D,TF1,TFile,TLatex,TLegend,TPaveText,TGraphErrors,kRed,kGreen,kYellow,kTRUE
from ROOT import gROOT,TMath
import math
import ROOT

PTlab="|#eta|<2.5"
dlab="Delphes ATLAS-like"
tlab="MG5 t#bar{t}+jets"
nnlab="ANN ATLAS-like"


# par[1] - mean
# par[2] - sigma
class Gauss:
   def __call__( self, x, par ):
       out=par[0] * TMath.Gaus(x[0],par[1],par[2])
       return out;


# get resolution 
def getResponseGraph( histos, xranges, isFit=False ):
 i=0
 g1=TGraphErrors()
 for m in range(len(histos)-1): 
       h1=histos[m]
       h2=xranges[m]
       pt=h2.GetMean()
       rms=h1.GetRMS()
       if (pt == 0 or rms==0): continue
       pt_err=h2.GetMeanError()
       sigma=rms90(h1);
       print "pt=", pt, " sigma=", sigma
       scale=sigma/h1.GetRMS() 
       err=scale*h1.GetRMSError()

       if (isFit==True):
                xmean=h1.GetMean()
                xrms=h1.GetRMS()
                MyMin=xmean-4*xrms;
                MyMax=xmean+4*xrms;
                binmax = h1.GetMaximumBin();
                yyy = 0.9*h1.GetBinContent(binmax);
                signal=TF1("signal",Gauss(),MyMin,MyMax,3);
                signal.SetNpx(200); signal.SetLineColor(2); signal.SetLineStyle(1)
                signal.SetLineWidth(2)
                binmax = h1.GetMaximumBin();
                signal.SetParameter(0,yyy) # amplitude
                signal.SetParameter(0,yyy) # amplitude  
                signal.SetParameter(1,xmean)
                signal.SetParameter(2,xrms)
                signal.SetParLimits(2,0.00001,1.0)
                h1.Fit(signal,"MR0E+","",MyMin,MyMax)
                chi2= signal.GetChisquare()
                ndf=signal.GetNDF()
                chi2ndf=0
                chi2ndf=chi2/ndf
                print "Chi2=", chi2," ndf=",ndf, " chi2/ndf=",chi2ndf
                par = signal.GetParameters()
                err=signal.GetParErrors()
                c='Peak=%.3f'%( par[1] ) + " #pm " + '%.3f GeV'%(err[1])
                b='#sigma=%.3f'%( par[2] ) + " #pm " + '%.3f GeV'%(err[2])
                d='#chi^{2}/ndf=%.2f'%( chi2ndf )
                sigma=par[2]
                err=err[2]
                g1.SetPoint(i,pt,sigma)
                ex=pt_err;
                ey=err;
                g1.SetPointError(i,ex,ey)
                i=i+1;

       if (isFit == False):
            g1.SetPoint(i,pt,sigma)
            ex=pt_err;
            ey=err;
            g1.SetPointError(i,ex,ey)
            i=i+1;
 g1.SetMarkerColor( 1 )
 g1.SetMarkerStyle( 20 )
 g1.SetMarkerSize( 0.8 )
 # g1->Print();
 return g1


def myText(x,y,color=1,size=0.08,text=""):
  l=TLatex()
  l.SetTextSize(size);
  l.SetNDC();
  l.SetTextColor(color);
  l.DrawLatex(x,y,text);

