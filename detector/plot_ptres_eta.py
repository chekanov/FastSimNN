# Plot validation and NN data after some cut on 2,3,4 variable


Cut=100 # cut on pT


# apply cut on 2nd input (eta) 
# this is a cut on 2nd input variable which changes betwenn -pi and pi
# The spread of 1st variable significantly depends on 2nd variable
Cut1=2

# apply cut on 3rd input
# this is a cut on 2nd input variable which changes betwenn -pi and pi
# The spread of 1st variable significantly depends on 2nd variable
Cut2=-999



from ROOT import gROOT,gPad,gStyle,TCanvas,TSpline3,TFile,TLine,TLatex,TAxis,TLegend,TPostScript
from ROOT import TArrow,TCut,TPad,TH1D,TF1,TObject,TPaveText,TGraph,TGraphErrors,TGraphAsymmErrors
from ROOT import gSystem,gDirectory
import ROOT,sys,math
sys.path.append("modules/")
from AtlasUtils import *

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

h1=TH1D("valid1","valid1", bins, xmin,xmax)
h1.Sumw2()
h1.SetLineWidth(2)
h1.SetLineStyle(1)
h1.SetLineColor(1);
h1.SetStats(0)
h1.SetTitle("")

h2=TH1D("neuralnet1","neuralnet1", bins, xmin,xmax)
h2.Sumw2()
h2.SetMarkerColor(2)
h2.SetMarkerSize(0.7)
h2.SetMarkerStyle(21)
h2.SetLineWidth(2)
h2.SetLineStyle(1)
h2.SetLineColor(1);
h2.SetStats(0)
h2.SetTitle("")


h1b=TH1D("valid2","valid2", bins, xmin,xmax)
h1b.Sumw2()
h1b.SetLineWidth(2)
h1b.SetLineStyle(1)
h1b.SetLineColor(4);
h1b.SetStats(0)
h1b.SetTitle("")

h2b=TH1D("neuralnet2","neuralnet2", bins, xmin,xmax)
h2b.Sumw2()
h2b.SetMarkerColor(4)
h2b.SetMarkerSize(0.7)
h2b.SetMarkerStyle(22)
h2b.SetLineWidth(2)
h2b.SetLineStyle(1)
h2b.SetLineColor(1);
h2b.SetStats(0)
h2b.SetTitle("")


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
          if ( output1[1]>0 and inputs1[0]>Cut and abs(inputs1[1])<Cut1): h1.Fill(  output1[0]/inputs1[0]  )
          if ( output2[1]>0 and inputs2[0]>Cut and abs(inputs2[1])<Cut1): h2.Fill(  output2[0]/inputs2[0]  )
          if ( output1[1]>0 and inputs1[0]>Cut and abs(inputs1[1])>Cut1): h1b.Fill(  output1[0]/inputs1[0]  )
          if ( output2[1]>0 and inputs2[0]>Cut and abs(inputs2[1])>Cut1): h2b.Fill(  output2[0]/inputs2[0]  )


#h1.Scale(1.0/h1.Integral())
#h2.Scale(1.0/h2.Integral())

ax=h1.GetXaxis(); ax.SetTitleOffset(0.8)
ax.SetTitle( "(p_{T}^{rec} - p_{T}^{true})  / p_{T}^{true}" );
ay=h1.GetYaxis(); ay.SetTitleOffset(0.8)
ay.SetTitle( "events" );
ax.SetTitleOffset(1.1); ay.SetTitleOffset(1.4)
ax.Draw("same")
ay.Draw("same")

h1.Draw("histo")
h2.Draw("pe same") 

h1b.Draw("same histo")
h2b.Draw("pe same")

leg2=TLegend(0.12, 0.7, 0.5, 0.92);
leg2.SetBorderSize(0);
leg2.SetFillColor(10);
leg2.SetTextSize(0.034);
leg2.SetHeader("|#eta(jet)|<"+str(Cut1)+":")
leg2.AddEntry(h1,"Delphes","bl")
leg2.AddEntry(h1,"p="+"{0:.3f}".format(h1.GetMean())+ " rms="+"{0:.2f}".format(rms90(h1)),"")
leg2.AddEntry(h2,"NN","pl")
leg2.AddEntry(h2,"p="+"{0:.3f}".format(h2.GetMean())+ " rms="+"{0:.2f}".format(rms90(h2)),"")

leg3=TLegend(0.12, 0.5, 0.5, 0.65);
leg3.SetBorderSize(0);
leg3.SetFillColor(10);
leg3.SetTextSize(0.034);
leg3.SetHeader("|#eta|>"+str(Cut1)+":")
leg3.AddEntry(h1b,"Delphes","bl")
leg3.AddEntry(h1b,"p="+"{0:.3f}".format(h1b.GetMean())+ " rms="+"{0:.2f}".format(rms90(h1b)),"")
leg3.AddEntry(h2b,"NN","pl")
leg3.AddEntry(h2b,"p="+"{0:.3f}".format(h2b.GetMean())+ " rms="+"{0:.2f}".format(rms90(h2b)),"")


leg2.Draw("same");
leg3.Draw("same");

print "Cut on 2nd input=", Cut1
print "True:  mean=",h1.GetMean()," RMS=",h1.GetRMS()," entries=",h1.GetEntries() 
print "NN  :  mean=",h2.GetMean()," RMS=",h2.GetRMS()," entries=",h2.GetEntries()

print "Efficiency True=",eff1/nn1
print "Efficiency NN=",eff2/nn2

print epsfig
gPad.RedrawAxis()
c1.Update()
ps1.Close()
if (myinput != "-b"):
              if (raw_input("Press any key to exit") != "-9999"):
                         c1.Close(); sys.exit(1);




