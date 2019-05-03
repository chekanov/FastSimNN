# Plot validation and NN data after some cut on 2,3,4 variable

# apply cut on 2nd input
# this is a cut on 2nd input variable which changes betwenn -pi and pi
# The spread of 1st variable significantly depends on 2nd variable
Cut1=1 

# apply cut on 3rd input
# this is a cut on 2nd input variable which changes betwenn -pi and pi
# The spread of 1st variable significantly depends on 2nd variable
Cut2=-9999


from ROOT import gROOT,gPad,gStyle,TCanvas,TFile,TLine,TLatex,TAxis,TLegend,TPostScript
from ROOT import TH2D,TF2,TArrow,TCut,TPad,TH1D,TF1,TObject,TPaveText,TGraph,TGraphErrors,TGraphAsymmErrors
import math
import ROOT
import sys
#sys.path.append("nnet/")
#import bpnn

print ('Number of arguments:', len(sys.argv), 'arguments.') 
print ('Argument List:', str(sys.argv))
print ('Use as: script.py -b 0 (or 1,2)') 
myinput="interactive"
if (len(sys.argv) ==2):
   myinput = sys.argv[1]
print ("Mode=",myinput) 


out=[0]
gROOT.Reset()
figdir="figs/"
epsfig=figdir+__file__.replace(".py",".eps")


nameX=""
nameY=""
Ymin=0.0
Ymax=500000
Xmin=0
Xmax=6.0 


NN=0

######################################################
gROOT.SetStyle("Plain");
gStyle.SetLabelSize(0.035,"xyz");

######################################################
gROOT.SetStyle("Plain");
gStyle.SetLabelSize(0.035,"xyz");
c1=TCanvas("c_massjj","BPRE",10,10,600,500);
c1.Divide(1,1,0.008,0.007);
ps1 = TPostScript( epsfig,113)
c1.SetGrid();

c1.cd(1);
gPad.SetLogy(0)
gPad.SetTopMargin(0.05)
gPad.SetBottomMargin(0.1)
gPad.SetLeftMargin(0.1)
gPad.SetRightMargin(0.05)

ff1=TFile("data/output.root")
ff2=TFile("data/input.root")

xmin=-4000-1 
xmax=4000-1 
bins=100

h1= ff2.Get("input_res")
h1.SetLineWidth(2)
h1.SetLineStyle(1)
h1.SetLineColor(1);
h1.SetStats(0)
h1.SetTitle("")

h2=ff1.Get("nn_res")
h2.SetMarkerColor(2)
h2.SetMarkerSize(0.7)
h2.SetMarkerStyle(21)
h2.SetLineWidth(2)
h2.SetLineStyle(1)
h2.SetLineColor(1);
h2.SetStats(0)
h2.SetTitle("")

h3=ff1.Get("predicted_res")
h3.SetMarkerColor(3)
h3.SetMarkerSize(0.7)
h3.SetMarkerStyle(22)
h3.SetLineWidth(2)
h3.SetLineStyle(1)
h3.SetLineColor(1);
h3.SetStats(0)
h3.SetTitle("")

h1.Scale(1.0/h1.Integral())
h2.Scale(1.0/h2.Integral())
h3.Scale(1.0/h3.Integral())


h1.Draw("histo")
ax=h1.GetXaxis(); ax.SetTitleOffset(0.8)
ax.SetTitle( "rec-true" );
ay=h1.GetYaxis(); ay.SetTitleOffset(0.8)
ay.SetTitle( "events" );
ax.SetTitleOffset(1.1); ay.SetTitleOffset(1.4)
ax.Draw("same")
ay.Draw("same")



h2.Draw("pe same") 
h3.Draw("p same")

leg2=TLegend(0.6, 0.7, 0.85, 0.94);
leg2.SetBorderSize(0);
leg2.SetFillColor(10);
leg2.SetTextSize(0.035);
leg2.AddEntry(h1,"Original","pl")
leg2.AddEntry(h1,"M="+"{0:.2f}".format(h1.GetMean())+ " rms="+"{0:.2f}".format(h1.GetRMS()),"")
leg2.AddEntry(h2,"NN neurons","pl")
leg2.AddEntry(h2,"M="+"{0:.2f}".format(h2.GetMean())+ " rms="+"{0:.2f}".format(h2.GetRMS()),"")
leg2.AddEntry(h3,"NN predicted","pl")
leg2.AddEntry(h3,"M="+"{0:.2f}".format(h3.GetMean())+ " rms="+"{0:.2f}".format(h3.GetRMS()),"")
leg2.Draw("same");

print "Cut on 2nd input=", Cut1
print "True:  mean=",h1.GetMean()," RMS=",h1.GetRMS()," entries=",h1.GetEntries() 
print "NN  :  mean=",h2.GetMean()," RMS=",h2.GetRMS()," entries=",h2.GetEntries()


print epsfig
gPad.RedrawAxis()
c1.Update()
ps1.Close()
if (myinput != "-b"):
              if (raw_input("Press any key to exit") != "-9999"):
                         c1.Close(); sys.exit(1);




