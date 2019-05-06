# Plot validation and NN data after some cut on 2,3,4 variable


Cut=500 # cut on pT


# apply cut on 2nd input
# this is a cut on 2nd input variable which changes betwenn -pi and pi
# The spread of 1st variable significantly depends on 2nd variable
Cut1=2 # cut on eta 

# apply cut on 3rd input
# this is a cut on 2nd input variable which changes betwenn -pi and pi
# The spread of 1st variable significantly depends on 2nd variable
Cut2=-999


Ymin=0.0
Ymax=0.5
Xmin=20
Xmax=3500


from ROOT import gROOT,gPad,gStyle,TCanvas,TSpline3,TFile,TLine,TLatex,TAxis,TLegend,TPostScript
from ROOT import TArrow,TCut,TPad,TH1D,TF1,TObject,TPaveText,TGraph,TGraphErrors,TGraphAsymmErrors
from ROOT import gSystem,gDirectory
import ROOT,sys,math
sys.path.append("modules/")
from AtlasUtils import *
from global_module import *


print ('Number of arguments:', len(sys.argv), 'arguments.')
print ('Argument List:', str(sys.argv))
print ('Use as: script.py -b 0 (or 1,2)')
myinput="interactive"
if (len(sys.argv) ==2):
   myinput = sys.argv[1]
print ("Mode=",myinput)



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
       if (kk%100000==0): print "Read data=",kk

   print "events=",events
   print "inputs=",inputs
   print "outputs=",outputs
   return data_in,data_out


nameX=""
nameY=""

Ymin=0.0
Ymax=0.5
Xmin=20
Xmax=3500


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
gPad.SetBottomMargin(0.14)
gPad.SetLeftMargin(0.15)
gPad.SetRightMargin(0.05)

data1_in,data1_out=getData("data/test.data")
data2_in,data2_out=getData("data/neuralnet.data")

xmin=-4000-1 
xmax=4000-1 
bins=100
print "Calculate histogram ranges"
data=[]
for i in xrange(len(data1_in)):
          inputs1=data1_in[i]
          output1=data1_out[i]
          #if (abs(inputs1[0])>1000): print i, inputs1[0] 
          if (inputs1[0] != 0): 
                 data.append(output1[0]/inputs1[0])
xmin=min(data)
xmax=max(data)
print "Min value =",xmin," Max value=",xmax
xmin=-0.5
xmax=0.5

PtMin=25
ptBins = (PtMin,40,60,80,100,140,180, 210, 240, 290, 340, 400, 500, 800, 1000, 1500,  2000, 2500)

histosT=[]
histosN=[]
xrangesT=[]
xrangesN=[]
for i in range(len(ptBins)):
        h1=TH1D("test"+str(i),"test"+str(i), bins, xmin,xmax)
        h1.Sumw2()
        h1.SetTitle("")
        h2=TH1D("nn"+str(i),"nn"+str(i), bins, xmin,xmax)
        h2.Sumw2()
        h2.SetTitle("")
        histosT.append(h1)
        histosN.append(h2)
        # keep mean value
        h3=TH1D("xrangeT"+str(i),"xrangeT"+str(i), 1000, 0,4000)
        xrangesT.append(h3)
        h3=TH1D("xrangeN"+str(i),"xrangeN"+str(i), 1000, 0,4000)
        xrangesN.append(h3)


eff1=0
eff2=0
nn1=0
nn2=0


# loop over all events
for i in xrange(len(data1_in)):

          # original validation file
          inputs1=data1_in[i]
          output1=data1_out[i]

          # predicted by NN 
          inputs2=data2_in[i]
          output2=data2_out[i]

          eff1=eff1+output1[1]
          eff2=eff2+output1[1]
          nn1=nn1+1.0 
          nn2=nn2+1.0 

          if (i%100000==0): print "calculate=",i  
          pT=inputs1[0]
          eta=abs(inputs1[1]) 
          if (eta<Cut1): 
           for m in range(len(ptBins)-1):
                if (pT>ptBins[m] and pT<ptBins[m+1]):
                   if ( output1[1]>0): # non-zero efficiency 
                                      h1=histosT[m]
                                      h1.Fill(  output1[0]/inputs1[0]  )
                                      xrangesT[m].Fill(pT)

          pT=inputs2[0]
          eta=abs(inputs2[1])
          if (eta<Cut1):
            for m in range(len(ptBins)-1):
                if (pT>ptBins[m] and pT<ptBins[m+1]):
                   if ( output2[1]>0): # non-zero efficiency 
                                      h1=histosN[m]
                                      h1.Fill(  output2[0]/inputs2[0]  )
                                      xrangesN[m].Fill(pT)


h=gPad.DrawFrame(Xmin,Ymin,Xmax,Ymax)
gPad.SetLogy(0)
gPad.SetLogx(1)
ax=h.GetXaxis(); ax.SetTitleOffset(1.0)
ax.SetTitle( "p_{T}^{jet} [GeV]"  );
ay=h.GetYaxis();
ay.SetTitle( "#sigma (p_{T}^{jet}) / p_{T}^{jet}" );
ay.SetTitleSize(0.05);
ax.SetTitleSize(0.05);
ay.SetLabelSize(0.04)
ax.SetTitleOffset(1.1);
ay.SetTitleOffset(1.25)
ay.SetLabelFont(42)
ax.SetLabelFont(42)
ax.SetLabelSize(0.04)

g1=getResponseGraph(histosT, xrangesT)
g2=getResponseGraph(histosN, xrangesN)
g2.SetMarkerColor( 2 )
g2.SetMarkerStyle( 24 )

g1.Draw("pe same")
g2.Draw("pe same")

leg2=TLegend(0.5, 0.7, 0.85, 0.91);
leg2.SetBorderSize(0);
leg2.SetTextFont(62);
leg2.SetFillColor(10);
leg2.SetTextSize(0.04);
leg2.AddEntry(g1,dlab, "pl")
leg2.AddEntry(g2,nnlab, "pl")
leg2.Draw("same");

myText( 0.2,0.84,4,0.04,"antiKT R=0.4 jets")
myText( 0.2,0.9,2,0.05,"|#eta(jet)|<"+str(Cut1))


print epsfig
gPad.RedrawAxis()
c1.Update()
ps1.Close()
if (myinput != "-b"):
              if (raw_input("Press any key to exit") != "-9999"):
                         c1.Close(); sys.exit(1);




