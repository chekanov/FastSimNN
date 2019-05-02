# Plot validation and NN data after some cut on 2,3,4 variable

# apply cut on 2nd input
Cut1=1.5 


from ROOT import gROOT,gPad,gStyle,TCanvas,TSpline3,TFile,TLine,TLatex,TAxis,TLegend,TPostScript
from ROOT import TH2D,TF2,TArrow,TCut,TPad,TH1D,TF1,TObject,TPaveText,TGraph,TGraphErrors,TGraphAsymmErrors
from ROOT import TGraph2D,TTree,TMultiGraph,TBranch,gSystem,gDirectory
from ROOT import TPaveStats,TProfile2D 
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



def getData(inputf):
  kk=0
  inputs=0
  outputs=0
  data_in=[]
  data_out=[]
  with open(inputf) as f:
   for line in f:
       pars=line.split();
       if (kk==0):
             header=line.split();
             events=int(header[0])
             inputs=int(header[1])
             outputs=int(header[2])
             print events,inputs,outputs
       else:
            pars = [float(i) for i in pars]
            if (inputs==len(pars)): data_in.append(pars);
            if (outputs==len(pars)): data_out.append(pars);
       kk=kk+1
       if (kk%100000==0): print "Process=",kk

   print "events=",events
   print "inputs=",inputs
   print "outputs=",outputs
   return data_in,data_out


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

data1_in,data1_out=getData("data/valid1.data")
data2_in,data2_out=getData("data/neuralnet.data")

xmin=-4000-1 
xmax=4000-1 
bins=100

h1=TH1D("valid","valid", bins, xmin,xmax)
h1.SetLineWidth(2)
h1.SetLineStyle(1)
h1.SetLineColor(1);
h1.SetStats(0)
h1.SetTitle("")

h2=TH1D("neuralnet","neuralnet", bins, xmin,xmax)
h2.SetMarkerColor(2)
h2.SetMarkerSize(0.7)
h2.SetMarkerStyle(21)
h2.SetLineWidth(2)
h2.SetLineStyle(1)
h2.SetLineColor(1);
h2.SetStats(0)
h2.SetTitle("")

# loop over all events
for i in xrange(len(data1_in)):

          # original validation file
          inputs1=data1_in[i]
          output1=data1_out[i]

          # predicted by NN 
          inputs2=data2_in[i]
          output2=data2_out[i]

          # set a cut on one variable
          if (inputs1[1]>Cut1): 
             if ( output1[1]>0): h1.Fill(  output1[0]  )

          # set a cut on one variable
          if (inputs2[1]>Cut1):
             if ( output2[1]>0): h2.Fill(  output2[0]  )

h1.Draw("histo")
h2.Draw("pe same") 

leg2=TLegend(0.6, 0.8, 0.85, 0.94);
leg2.SetBorderSize(0);
leg2.SetFillColor(10);
leg2.SetTextSize(0.035);
leg2.AddEntry(h1,"Original","pl")
leg2.AddEntry(h1,"M="+"{0:.2f}".format(h1.GetMean())+ " rms="+"{0:.2f}".format(h1.GetRMS()),"")
leg2.AddEntry(h2,"Neural Net","pl")
leg2.AddEntry(h2,"M="+"{0:.2f}".format(h2.GetMean())+ " rms="+"{0:.2f}".format(h2.GetRMS()),"")
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




