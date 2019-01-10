import sys
sys.path.append("modules/")

	
from global_module import *

# is it batch?
# if batch, set input to batch
myinput="no any input"
if (len(sys.argv) > 1):
   myinput = sys.argv[1]
print "Mode=",myinput


# import atlas styles
from AtlasStyle import *
from AtlasUtils import *
from initialize  import *
from module_functions import *


tag="rfast008"
if (len(sys.argv) ==2):
   myinput = sys.argv[1]
print "mode =",myinput

if (len(sys.argv) ==3):
   tag = sys.argv[1]
   myinput = sys.argv[2]
print "TAG =",tag
print "mode =",myinput


gROOT.Reset()
figdir="figs/"
epsfig=figdir+__file__.replace("py","eps")
epsfig=epsfig.replace(".eps","_"+tag+".eps")
Ymin=0.0 
Ymax=0.3 
Xmin=-3   
Xmax=3

#if (tag == "rfast007"):
#        Ymax=0.8;
#if (tag == "rfast006"):
#        Ymax=1.2;


######################################################
gROOT.SetStyle("Plain");


c1=TCanvas("c","BPRE",10,10,500,400);
ps1 = TPostScript( epsfig,113)
c1.Divide(1,3,0.005,0.005);
c1.SetTickx()
c1.SetTicky()
c1.SetTitle("")
c1.SetLineWidth(3)
c1.SetBottomMargin(0.13)
c1.SetTopMargin(0.05)
c1.SetLeftMargin(0.14)
c1.SetRightMargin(0.05)
c1.SetFillColor(0)

h=gPad.DrawFrame(Xmin,Ymin,Xmax,Ymax)
gPad.SetLogy(0)
gPad.SetLogx(0)
ax=h.GetXaxis(); ax.SetTitleOffset(1.0)
ax.SetTitle( "#eta^{jet} [GeV]"  );
ay=h.GetYaxis();
ay.SetTitle( "#sigma (#eta^{jet}) / #eta^{jet}" );
ay.SetTitleSize(0.05);
ax.SetTitleSize(0.05);
ay.SetLabelSize(0.04)
ax.SetTitleOffset(1.1);
ay.SetTitleOffset(1.25)
ay.SetLabelFont(42)
ax.SetLabelFont(42)
ax.SetLabelSize(0.04)

ax.SetTitleFont(62)
ay.SetTitleFont(62)


ptmin=-3
ptmax=3
s1="sqrt(([0]/sqrt(x))*([0]/sqrt(x))+([1]/x)*([1]/x)+[2]*[2])"

"""
https://twiki.cern.ch/twiki/bin/view/Main/JetResolutionPlots
a) sampling and statistical fluctuations (a^2/E), which is dominating over most of the useful range of calorimeters
b) noise, which dominates the low-energy performance of calorimeters (b^2/E^2)
c) effects coming from calibration errors, non-uniformities and non-linearities 
"""

f1=TF1("f1",s1,ptmin,ptmax);
f1.SetLineStyle(1)
f1.SetLineWidth(2)
f1.SetLineColor(2)
f1.SetParameter(0,0.9)
f1.SetParLimits(0,0.2,2.0)
f1.SetParameter(1,4.58662e-01)
f1.SetParameter(1,4.58662e-01)
f1.SetParLimits(1,0.01,0.8)
f1.FixParameter(1,0)
# f1.FixParameter(2,0)
f1.SetParameter(2,2.02282e-02)
f1.SetParLimits(2,0.005,0.8)

filename="root/resolution_jeteta1_"+tag+".txt"
h1=getResolutionGraph(filename)

filename="root/resolution_jeteta2_"+tag+".txt"
h2=getResolutionGraph(filename)


#h1.SetMarkerColor(ROOT.kBlack)
h1.SetMarkerSize(1.0)
h1.SetMarkerStyle(20)
h1.Draw("P same")
# h1.Fit(f1,"RE+","",ptmin,ptmax);
for i in range(20):
     fitr=h1.Fit(f1,"SR0E+","",ptmin,ptmax);
     # fitr=h1.Fit(f1,"SMR0", ptmin,ptmax)
     print "Status=",int(fitr), " is valid=",fitr.IsValid()
     if (fitr.IsValid()==True): break;
fitr.Print()
print "Is valid=",fitr.IsValid()

f2=TF1("f2",s1,ptmin,ptmax);
f2.SetLineStyle(1)
f2.SetLineWidth(2)
f2.SetLineColor(2)
f2.SetParameter(0,0.9)
f2.SetParLimits(0,0.2,2.0)
f2.SetParameter(1,4.58662e-01)
f2.SetParameter(1,4.58662e-01)
f2.SetParLimits(1,0.01,0.8)
f2.FixParameter(1,0)
# f1.FixParameter(2,0)
f2.SetParameter(2,2.02282e-02)
for i in range(20):
     fitr=h1.Fit(f2,"SR0E+","",ptmin,ptmax);
     # fitr=h1.Fit(f1,"SMR0", ptmin,ptmax)
     print "Status=",int(fitr), " is valid=",fitr.IsValid()
     if (fitr.IsValid()==True): break;
fitr.Print()
print "Is valid=",fitr.IsValid()

# f1.Draw("same")
# f2.Draw("same")

par1 = f1.GetParameters()
par2 = f2.GetParameters()

a1='%.2f'%( par1[0] )
b1='%.2f'%( par1[1] )
k1='%.2f'%( par1[2] )
s1="#sqrt{"+a1+" ^{2} /p_{T}^{jet} +"+b1+" ^{2}/p_{T}^{jet,2} +"+k1+"^{2}}" 

print s1
a1='%.1f'%( par1[0]*100 )
k1='%.1f'%( par1[2]*100 )
aPerc= a1
kPerc = k1
s1 ="#scale[1.2]{#frac{"+aPerc+"%}{#sqrt{p_{T}^{jet}}} #oplus "+kPerc+"%}"
print s1

a2='%.2f'%( par2[0] )
b2='%.2f'%( par2[1] )
k2='%.2f'%( par2[2] )
a1='%.1f'%( par2[0]*100 )
k1='%.1f'%( par2[2]*100 )
aPerc= a1
kPerc = k1
s2 ="#scale[1.2]{#frac{"+aPerc+"%}{#sqrt{p_{T}^{jet}}} #oplus "+kPerc+"%}"
print s2

leg1=TLegend(0.5, 0.38, 0.85, 0.6);
leg1.SetBorderSize(0);
leg1.SetTextFont(62);
leg1.SetFillColor(10);
leg1.SetTextSize(0.04);
# leg1.AddEntry(f1,s1,"l")
leg1.Draw("same");

h1=TGraphSkipRange(h1,-0.4, 0.4 )
h2=TGraphSkipRange(h2,-0.4, 0.4 )

h1.Draw("pe same")
h2.SetMarkerColor(2)
h2.SetMarkerStyle(24)
h2.SetMarkerSize(1.0)
h2.Draw("pe same")

leg2=TLegend(0.5, 0.7, 0.85, 0.91);
leg2.SetBorderSize(0);
leg2.SetTextFont(62);
leg2.SetFillColor(10);
leg2.SetTextSize(0.04);
leg2.AddEntry(h1,dlab, "pl")
leg2.AddEntry(h2,nnlab, "pl")
leg2.Draw("same");



myText( 0.2,0.84,4,0.04,"antiKT R=0.4 jets")

txtt="<#mu>=0"
if (tag == "rfast007"): txtt="<#mu>=40"
if (tag == "rfast006"): txtt="<#mu>=140"
myText( 0.2,0.9,2,0.05,txtt)


gPad.RedrawAxis()



ps1.Close()
if (myinput != "-b"):
              if (raw_input("Press any key to exit") != "-9999"): 
                         c1.Close(); sys.exit(1);


