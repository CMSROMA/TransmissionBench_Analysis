#include "TH1.h"
#include "TF1.h"
#include "TH1F.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include "TTree.h"
#include "TFile.h"
#include "TH2.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TVector.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TStyle.h"
#include "TMath.h"
#include "TError.h"
#include "TLegend.h"
#include "TLatex.h"
#include <cctype>
#include "TLegend.h"



// info running:
//.L spectro_photo_newRange.c++
//TransmissionSpectrum("nomefile",N_dataset=numero sample acquisiti)
// Output: .root con w e T + Tgrapherror T vs w + plot .png average Transmission vs w
// Output da schermo: valore T al picco di emissione;
using namespace std;

void  spettro_photo_newRange(char const file_name[1000],int N_dataset)
{
    std::string str(file_name);
    TString namefileroot = str+".root";
    TFile hfile(namefileroot,"RECREATE");
    TTree *tree_scan = new TTree("data_scan","data_scan");

    float wl;
    float tl;
    float T[N_dataset][801];
    float T_m[801]={}, R[801]={}, W[801]={}, stW[801]={};
    tree_scan->Branch("wl", &wl, "wl/F");
    tree_scan->Branch("T", &tl, "T/F");
    std::ifstream myFile(file_name);
    std::string line;

    
    
  if(!myFile.is_open()) throw std::runtime_error("Could not open file");
    if(myFile.good())
    {
        for (int i=0; i<41; i++) {
            std::getline(myFile, line);
            
        }

         for (int k=0; k<N_dataset; k++) {
           std::getline(myFile, line);
           std::getline(myFile, line);
           
              for (int j=0; j<801; j++) {
              std::getline(myFile, line);
              std::stringstream ss(line);
              ss>>wl;
              W[j]=wl;
              ss.ignore();
              ss>>tl;
              T[k][j]=tl;
              T_m[j]=T_m[j]+tl;
              tree_scan->Fill();
              }
             
           std::getline(myFile, line);
            
         }
    
   }
    
    /*  for (int k=0; k<N_dataset; k++){ */
    /*       for (int j=0; j<801; j++) { */

    /*            if (k<(N_dataset-1)) */
    /*               {R[j]=R[j]+pow((T[k][j]-(T_m[j]/N_dataset)),2); */
    /*            } */
    /*           else {R[j]=sqrt(R[j]+pow((T[k][j]-(T_m[j]/N_dataset)),2)/(N_dataset-1)); */
    /*                 T_m[j]=T_m[j]/N_dataset; */
    /*               //if (W[j]==400)(cout<<"w="<<W[j]<<""<<"T="<<T_m[j]<<""<<"st="<<R[j]<<endl); */
    /*               if (W[j]==400)(cout<<"w="<<W[j]<<" "<<"T="<<T_m[j]<<" "<<"st="<<R[j]<<" "<<"Reproducibility="<<R[j]/T_m[j]<<endl); //Patrizia */


 
    /*           } */
    /*       } */
    /* } */
         
   TGraphErrors *gr1 = new TGraphErrors(800,W,T_m,stW,R);
    
    TCanvas *c1 = new TCanvas("Transmission Vs Wavelenght", "T vs W" , 700,500);
    c1->cd();
    gPad->SetTickx(2);
    gPad->SetTicky(2);
    gPad->SetGrid();
    gStyle->SetOptStat(1111);
    gr1->GetXaxis()->SetTitle("Wavelength (nm) ");
    gr1->GetYaxis()->SetTitle("Transmission (%)");
    gr1->GetYaxis()->SetNdivisions(1020);
    gr1->SetTitle("");
    gr1->SetMarkerStyle(20);
    gr1->SetMarkerSize(0.8);
    gr1->SetLineColor(2);
    gr1->SetLineWidth(2);
    gr1->Draw("ap+L");

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
    char canvas_name[50];
    string canvas_name_string;
    canvas_name_string=str+".png";
    strcpy(canvas_name, canvas_name_string.c_str());
    c1->SaveAs(canvas_name);
   
    
}
    



                                    
                                    
                                    
                                    
                                    
                                    
                                    
                       
