#include <cctype>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include "TH1F.h"
#include "TTree.h"
#include "TFile.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TStyle.h"
#include "TString.h"
#include "TMath.h"
#include "TError.h"
#include "TLegend.h"
#include "TLatex.h"

// info running:
//.X spectro_photo_newRange.C++("nomefile")

// Output: .root con w e T + Tgrapherror T vs w + plot .png average Transmission vs w
// Output da schermo: valore T al picco di emissione;

using namespace std;

void  spettro_photo_newRange(char const file_name[1000],int N_dataset=0)
{

  std::ifstream myFile(file_name);
  std::string line;
        
  TString ScanType;
  TString DataMode;
  TString DisplayFormat;
  
  float StartWL=0;
  float EndWL=0;
  float ReadInterval=0;

  if(!myFile.is_open()) throw std::runtime_error("Could not open file");
  if(!myFile.good()) throw std::runtime_error("Could not open file");
    
  std::cout << "Opened " << file_name << std::endl;
  for (int i=0; i<41; i++) {
    std::getline(myFile, line);
    TString ss=TString(line);
    TString tok;
    Ssiz_t from=0;
    ss.Tokenize(tok,from,",");
    if (tok.CompareTo("DataSets")==0)
      {
	ss.Tokenize(tok,from,",");
	N_dataset=tok.Atoi();
	std::cout << "===>Datasets " << N_dataset << std::endl;	
      }
    if (tok.CompareTo("ScanType")==0)
      {
	ss.Tokenize(tok,from,",");
	ScanType=tok;
	std::cout << "===>ScanType " << ScanType << std::endl;	
      }
    if (tok.CompareTo("DataMode")==0)
      {
	ss.Tokenize(tok,from,",");
	DataMode=tok;
	std::cout << "===>DataMode " << DataMode << std::endl;	
      }
    if (tok.CompareTo("DisplayFormat")==0)
      {
	ss.Tokenize(tok,from,",");
	DisplayFormat=tok;
	std::cout << "===>DisplayFormat " << DisplayFormat << std::endl;	
      }
    if (tok.CompareTo("StartWL")==0)
      {
	ss.Tokenize(tok,from,",");
	StartWL=tok.Atof();
	std::cout << "===>StartWL " << StartWL << std::endl;	
      }
    if (tok.CompareTo("EndWL")==0)
      {
	ss.Tokenize(tok,from,",");
	EndWL=tok.Atof();
	std::cout << "===>EndWL " << EndWL << std::endl;	
      }
    if (tok.CompareTo("ReadInterval")==0)
      {
	ss.Tokenize(tok,from,",");
	ReadInterval=tok.Atof();
	std::cout << "===>ReadInterval " << ReadInterval << std::endl;	
      }
  }

  // if (!(ScanType.CompareTo("SCAN_TYPE_WAVELENGTH")==0) ||
  //     !(DataMode.CompareTo("SCAN_DM_PT")==0) ||
  //     !(DisplayFormat.CompareTo("DISPLAY_FORMAT_OVERLAY")==0)
  //     )
  //   {
  //     std::cout << "[ERROR]::Wrong data acquisition format" << std::endl;
  //     exit(-1);
  //   }

  if (N_dataset<1)
    {
      std::cout << "[ERROR]::Datasets not found" << std::endl;
      exit(-1);
    }

  int nMeasure=int((StartWL-EndWL)/ReadInterval);
  if (nMeasure<1)
    {
      std::cout << "[ERROR]::Wrong nMeasure" << std::endl;
      exit(-1);
    }
	
  float T[N_dataset][nMeasure+1];
      
  std::string str(file_name);
  TString namefileroot = str+".root";
  TFile hfile(namefileroot,"RECREATE");
      
  TTree *tree_scan = new TTree("data_scan","data_scan");
  float wl;
  float tl;
  int nd;
  tree_scan->Branch("nd", &nd, "nd/I");
  tree_scan->Branch("wl", &wl, "wl/F");
  tree_scan->Branch("T", &tl, "T/F");
                  
  for (int k=0; k<N_dataset; k++) {
    std::getline(myFile, line);
    std::getline(myFile, line);
    nd=k;
    for (int j=0; j<nMeasure+1; j++) {
      std::getline(myFile, line);
      std::stringstream ss(line);
      ss>>wl;
      ss.ignore();
      ss>>tl;
      T[k][j]=tl;
      tree_scan->Fill();
    }        
    std::getline(myFile, line);        
  }
    
         
  TProfile *gr1 = new TProfile("Transmission","Transmission",nMeasure+1,EndWL-ReadInterval/2,StartWL+ReadInterval/2,"S");
  if (N_dataset>1)
    tree_scan->Draw("T:wl>>Transmission","nd>0","PROFS");
  else
    tree_scan->Draw("T:wl>>Transmission","","PROFS");

  TCanvas *c1 = new TCanvas("Transmission Vs Wavelenght", "T vs W" , 700,500);
  c1->cd();
  gPad->SetTickx(2);
  gPad->SetTicky(2);
  gPad->SetGrid();
  gStyle->SetOptStat(0);
  gr1->GetXaxis()->SetTitle("Wavelength (nm) ");
  gr1->GetYaxis()->SetTitle("Transmission (%)");
  gr1->GetYaxis()->SetNdivisions(1020);
  gr1->SetTitle("");
  gr1->SetMarkerStyle(20);
  gr1->SetMarkerSize(0.8);
  gr1->SetLineColor(2);
  gr1->SetLineWidth(2);
  gr1->Draw("PL");
  char canvas_name[50];
  string canvas_name_string;
  canvas_name_string=str+".png";
  strcpy(canvas_name, canvas_name_string.c_str());
  c1->SaveAs(canvas_name);

  std::cout << "T@400nm: " << gr1->GetBinContent(gr1->GetXaxis()->FindBin(400)) << " +/- " <<  gr1->GetBinError(gr1->GetXaxis()->FindBin(400)) << "(RMS) %" << std::endl;

  //TLegend *leg = new TLegend(.6,.8,.8,.4,"Lab. Lesson 1");
  //leg->SetFillColor(0);
  //gr1->SetFillColor(0);
  //leg->AddEntry("T_m[j]","Exp. Points");
  //leg->AddEntry("R[j]","Th. Law");
  //leg->DrawClone("Same");
  //TLatex *   tex = new TLatex(0.55,0.2,"#T_{400nm}= T_m[j]% #pm R[j]%");
  //TLatex *   tex = new TLatex(0.45,0.3,"#T_{400nm}=", a, "% #pm", b,"%");
  //TLatex *   tex = new TLatex(0.55,0.2,"#T_{400nm}=58.43%#pm0.05%");
  //tex->SetNDC();
  //tex->SetTextFont(42);
  //tex->SetLineWidth(2);
  //tex->Draw();
      
  gr1->Write("Transmission");
  tree_scan->Write();
  hfile.Close();
  std::cout << "Saved " << namefileroot << std::endl;
}
    



                                    
                                    
                                    
                                    
                                    
                                    
                                    
                       
