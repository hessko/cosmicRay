#include <TMath.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <TString.h>
#include <TSystem.h>
#include <TFile.h>
#include <TH2.h>
#include <TH1.h>
#include <TH1D.h>
#include <TF1.h>
#include <stdio.h>
#include <stdlib.h>
//#include <TStyle.h>
//#include <TCanvas.h>
//#include <TFrame.h>
//#include <TFormula.h>
//#include <TGraphErrors.h>
//#include <TGraph.h>
#include <TMath.h>
#include <TChain.h>
#include "TCanvas.h"
#include "TH1.h"
#include "TF1.h"
#include <vector>
#include "TGraph.h"
#include <map>
#include <fstream>
#include <iostream>

void txt2Histo( string fileName, int nEvent ) {
  const float TILE_WIDTH = 1.25; //inches

  // 1. Get txt file and set output root file
  //fileName+="_output.txt" ;
  TString txtFile = fileName ;
  FILE* logfile = fopen( txtFile , "r");
  if ( logfile == NULL ) printf(" file open error \n") ;

  // 2. Create histogram root file and .txt file for muon position
  string hfileName = fileName.substr(0, fileName.size()-4 ) ;
  string outFileName = hfileName + "_output.txt";
  hfileName += ".root" ;
  printf(" histfile : %s \n", hfileName.c_str() ) ;
  TFile* hfile = new TFile( hfileName.c_str() ,"RECREATE");
  std::ofstream outFile (outFileName.c_str(), std::ofstream::out | std::ofstream::trunc);

  // 3. Define Histograms
  TH1D *hADC[32] ;
  TH1F *hPed[32];
  TH1F *hADCSig[32];
/*  TH1F *hADCSig4[32];
  TH1F *hADCSig6[32];
  TH1F *hADCSig7[32];*/
//  TH1F *hADCSig2[32];
  char  ibuf[10] ;
  char pbuf[10];
  char abuf[20];
/*  char abuf4[20];
  char abuf6[20];
  char abuf7[20];*/
//  char abuf2[15];
  for (int i=0; i< 32; i++) {
      sprintf( ibuf, "hADC%2d", i ) ;
	  sprintf(pbuf, "hPed%2d", i);
	  sprintf(abuf, "hADCSig%2d 3sigma 5", i);
/*	  sprintf(abuf4, "hADCSig%2d 3sigma 4", i);
	  sprintf(abuf6, "hADCSig%2d 3sigma 6", i);
	  sprintf(abuf7, "hADCSig%2d 3sigma 7", i);*/
//	  sprintf(abuf2, "hADCSig%2d 5sigma", i);
      hADC[i] = new TH1D( ibuf , "ADC counts", 100, 0, 200  ) ;
	  hPed[i] = new TH1F(pbuf, "Pedestal", 200, 80, 120);
	  hADCSig[i] = new TH1F(abuf, "ADC when Signal Present, 3sigma, 5 Samples", 500, 0, 500);
/*	  hADCSig4[i] = new TH1F(abuf4, "ADC when Signal Present, 3sigma, 4 Samples", 500, 0, 500);
	  hADCSig6[i] = new TH1F(abuf6, "ADC when Signal Present, 3sigma, 6 Samples", 500, 0, 500);
	  hADCSig7[i] = new TH1F(abuf7, "ADC when Signal Present, 3sigma, 7 Samples", 500, 0, 500);*/
//	  hADCSig2[i] = new TH1F(abuf2, "ADC when Signal Present, 5sigma", 400, 0, 400);
  }
  TH1F *hPedTotal = new TH1F("hPedTotal", "Pedestal for All Channels", 200, 80, 120);
  TH1D *hHigh = new TH1D("hHigh", "Channel with Highest Peak", 32, 0, 32);
  TH1F *hMaxADC = new TH1F("hMaxADC", "Max ADC value from each event", 350, 0, 350);

  float x[105];
  for (int i = 0; i < 105; i++) {
	  x[i] = i;
  }
  std::map<int, vector<int>* > channelAndSignalStart;
  std::map<int, float> eventAndAMRatio;
  std::map<int, float> eventAndPMRatio;
  for (int i = 0; i < 32; i++) {
	  channelAndSignalStart[i] = new std::vector<int>;
  }

  // 4. Fill the histograms
  // Data format -> 0:badData, 1: badcap, 2: capID, 3: bx, 4~9: ADC, 10~15: TDC  
  bool eof = false ;
  int  evtId = 0 ;
  int  channel = 0 ;
  int  adc = 0 ;
  int  buf = 0 ;
  char cbuf[600] ;
  int evt0(0), evt1(0), evt2(0) ;
  float adcV[32][105] = {0.} ;

  float sum_adc = 0 ;
  float ped     = 0 ; 
  float ped2    = 0 ;
  float ped15   = 0 ;
  float itg_adc = 0 ;

//  std::vector<float> *event = new std::vector<float>;
  float event[32] = {0.};
  float pedAvg[32] = {98.96, 98.65, 97.59, 97.54, 97.36, 98.05, 98.55, 97.61, 98.34, 99.19, 97.52, 97.6, 97.2, 98.62, 98.16, 97.77, 98.88, 97.98, 97.9, 97.92, 98.36, 97.96, 98.4, 98.23, 97.5, 98, 96.94, 97.22, 97.53, 97.52, 97.62, 97.68};
  float pedStdDev[32] = {0.8578, 1.306, 0.8855, 1.129, 1.039, 1.295, 0.9977, 1.496, 0.4379, 0.4349, 0.5271, 0.7623, 0.8666, 0.5039, 0.4153, 0.4812, 0.8405, 0.6869, 0.498, 0.9666, 0.7696, 1.04, 0.6777, 0.5499, 0.5137, 0.5586, 0.6068, 0.6449, 0.6506, 0.5267, 0.495, 0.5034};

  int k = 0;
  int l = 0;
  int m = 0;
  int nEvt = 0 ;
  int pEvt = -1;
  while ( !eof ) {

     fgets(cbuf, sizeof(cbuf), logfile) ;
     //printf(" read : %c%c%c\n", cbuf[0], cbuf[1], cbuf[1023] ) ;
     if ( cbuf[0] == '*') continue;
     
     adc = 0 ; 
     ped = 0 ;
	 ped2 =0;
	 ped15 = 0;
     sum_adc = 0 ;
     itg_adc = 0 ;
     int fo = -1 ;
     for ( int j=0; j<130; j++ ) {
         if ( j< 4 ) fo = fscanf( logfile, "%d" , &buf ); 
         //printf(" read : %d \n", buf ) ;
         
         if ( j ==4           ) fo = fscanf(logfile, "%x" , &evt2 );
         if ( j ==5           ) fo = fscanf(logfile, "%x" , &evt1 );
         if ( j ==6           ) fo = fscanf(logfile, "%x" , &evt0 );
         if ( j ==7           ) fo = fscanf(logfile, "%x" , &channel );
         if ( j > 7 && j < 25 ) fo = fscanf(logfile, "%x" , &buf ); 
         if ( j >= 25 ) {
            fo = fscanf(logfile, "%x" , &adc );
            adcV[channel][j-25] = (float)adc ;
            sum_adc += (float)adc ;
			if ( j < 40) ped15 += (float)adc;
            if ( j < 45) ped += (float)adc ;
			if ( j >= 110) ped2+=(float)adc;
         }
         if ( fo != 1 ) {
            //printf(" !!! fscan fail %d \n", fo ) ;
            break;
         }
     }
     ped = ped /20. ; //pedestal calculated from first 20 samples
	 ped2 = ped2/20.; //pedestal calculated from last 20 samples
	 ped15 = ped15/15.; //pedestal calculated from first 15 samples
	 
//	 hPed[channel]->Fill(ped);
	 hPed[channel]->Fill(ped15);
	 hPedTotal->Fill(ped15);

	 for (int i = 0; i < 105; i++) {
		 adcV[channel][i] = adcV[channel][i]-ped15;
	 }

//     itg_adc = (sum_adc < (pedAvg[channel]*105) ) ? 0 : (sum_adc - (pedAvg[channel]*105) ) ;
     itg_adc = (sum_adc < (ped15*105) ) ? 0 : (sum_adc - (ped15*105) ) ;
     //printf("ch(%d) ped: %.2f  sum_adc: %.f   itg_adc : %.2f \n", channel, ped, sum_adc, itg_adc ) ;
     hADC[channel]->Fill( itg_adc ) ;
//	 (*event).push_back(itg_adc);

	 if (channel == 31) {
		 int highestChannel = -1;
		 float max = 0;
		 float AM = 0; 
		 float PM = 0;
		 for (int i = 0; i < 32; i++) {
			 if ((i == 0) || (i == 3) || (i == 18) || (i == 23) || (i > 7 && i < 16) || (i > 23)) continue; 
			 Double_t cutoff = 3 * pedStdDev[i];
			 float adcSig = 0;
			 float adcSig4 = 0;
			 for (int j = 15; j < 25; j++) {
				 if (adcV[i][j] > cutoff && adcV[i][j+1] > cutoff && adcV[i][j+2] > cutoff && adcV[i][j+3]) {
				   	 adcSig4 = adcV[i][j] + adcV[i][j+1] + adcV[i][j+2] + adcV[i][j+3];
					 if (adcV[i][j+4] > cutoff) {
						 (*channelAndSignalStart[i]).push_back(j);
					 	 adcSig = adcSig4 + adcV[i][j+4];
						 event[i] = adcSig;
						 event[i-1] = adcV[i-1][j] + adcV[i-1][j+1] + adcV[i-1][j+2] + adcV[i-1][j+3] + adcV[i-1][j+4]; 
						 event[i+1] = adcV[i+1][j] + adcV[i+1][j+1] + adcV[i+1][j+2] + adcV[i+1][j+3] + adcV[i+1][j+4]; 
						 if (event[i-1] < 0) event[i-1] = 0;
						 if (event[i+1] < 0) event[i+1] = 0;
						 if (i == 7) {
							 event[16] = adcV[16][j] + adcV[16][j+1] + adcV[16][j+2] + adcV[16][j+3] + adcV[16][j+4]; 
							 if (event[16] < 0) event[16] = 0;
						 }
						 if ( i == 16) {
							 event[7] = adcV[7][j] + adcV[7][j+1] + adcV[7][j+2] + adcV[7][j+3] + adcV[7][j+4];
							 if (event[7] < 0) event[7] = 0;
						 }
						 if ((i == 7 && l < 10)  && nEvt != pEvt) {
							 pEvt = nEvt;	
 							 TGraph *gr = new TGraph(105, x, adcV[i]);
							 char vName[30];
							 int n = sprintf(vName, "Channel %d, Event %d", i, nEvt); 
							 gr->SetName(vName);
							 gr->SetTitle(vName);
							 gr->GetXaxis()->SetLimits(0, 105);
							 TF1 *fit1 = new TF1("fit1", "landau", 15, 27);
							 gr->Fit(fit1, "QR");
							 gr->Write();
							 l++;
						 }	
//						 When included, the following block will calculate the ADC value for a channel using 6 and 7 samples
/*						 if (adcV[i][j+5] > cutoff) {
						 	adcSig6 = adcSig + adcV[i][j+5];
							 if (adcV[i][j+6] > cutoff) {
							 	adcSig7 = adcSig6 + adcV[i][j+6];
							 }
						 }*/
					 }
					 j=100;
				 }
			 }
			 if (event[i] > max) {
				 max = event[i];
				 highestChannel = i;
				 AM = event[i-1];
				 PM = event[i+1];
				 if (i == 7) PM = event[16];
				 if (i == 16) AM = event[7];
			 }
			 if (adcSig > 0) hADCSig[i]->Fill(adcSig);
		 }
		 if (max > 0) {
			 hMaxADC->Fill(max);
			 eventAndAMRatio[nEvt] = AM/max;
			 eventAndPMRatio[nEvt] = PM/max;
		 }
		 if (highestChannel > -1) {
			 /*Let n represent the channel in which the muon was incident.
			  * If n takes an integer value, the muon hit the tile without hitting a neighboring tile.
			  * Usually, n will take a non-integer value to represent a particle striking a tile off-center.
			  */ 
			 Double_t n = 0; 
			 hHigh->Fill(highestChannel);
			 if (highestChannel > 7) highestChannel = highestChannel - 8; //for calculating distance from edge
			 if (AM == 0 && PM == 0) n = highestChannel;
			 else if (AM != 0 && PM == 0) n = highestChannel - (0.5 * eventAndAMRatio[nEvt]);
			 else if (AM == 0 && PM != 0) n = highestChannel + (0.5 * eventAndPMRatio[nEvt]);
			 else if (AM != 0 && PM != 0) {
				 TH1F *channelFit = new TH1F("channelFit", "Find channel of incident muon", 16, 0, 16);
				 channelFit->Fill(highestChannel, max);
				 channelFit->Fill(highestChannel - 1, AM);
				 channelFit->Fill(highestChannel + 1, PM);
				 n = channelFit->GetMean(1);
				 delete channelFit;
			 }
			 /*The position is calculated as follows because each center of a tile is only half of a tile's width from the last center.
			  * So the center of tile 0 is 1/2 tile width from the edge, while tile 1 is one full width from the edge, and so on.
			  */
			 double position = (0.5 * (n + 1)) * TILE_WIDTH;
			 char *outBuff = new char[65];
//			 char *outBuff = new char[80];
			 int m = sprintf(outBuff, "Event %d: Muon incident %f inches from edge of Channel 0 \n", nEvt, position);
//			 int m = sprintf(outBuff, "Event %d: Channel %d, PreviousToChannel = %f, NextToChannel = %f \n", nEvt, highestChannel, eventAndAMRatio[nEvt], eventAndPMRatio[nEvt]);
			 outFile.write(outBuff, 65);
			 delete[] outBuff;
		 }
//		 printf(" ch:%d evt:%d \n", highestChannel, nEvt) ;
//		 event = new std::vector<float>;
		 for (int k = 0; k < 32; k++) {
			 event[k] = 0.;
		 }
		 nEvt++ ;
	 }

     evtId = (evt2*pow(16,4)) + (evt1*pow(16,2)) + evt0 ;
     if ( nEvt == nEvent ) break ;
     eof = (feof( logfile) != 0) ? true : false ;
     //printf(" event: %d ( %X %X %X), ch: %d , adc( %.f %.f ) \n", evtId, evt2 , evt1, evt0, channel, adcV[0], adcV[2] ) ;
  }

  TH1D *hStart[32];
  char hbuf[20];
  for (int i = 0; i < 32; i++) {
	  if ((*channelAndSignalStart[i]).size() == 0) {
		  continue;
	  }
	  int n = sprintf(hbuf, "Ch %d Start Time", i);
	  hStart[i] = new TH1D(hbuf, "Start Time", 95, 0, 95);
	  for (unsigned int j = 0; j < (*channelAndSignalStart[i]).size(); j++) {
		  hStart[i]->Fill((*channelAndSignalStart[i])[j]);
	  }
	  hStart[i]->Write();
  }

  for (int i=0; i< 32; i++) {
      hADC[i]->Write() ;
  	  hPed[i]->Write();
	  hADCSig[i]->Write();
/*	  hADCSig4[i]->Write();
	  hADCSig6[i]->Write();
	  hADCSig7[i]->Write();*/
//	  hADCSig2[i]->Write();
  }

  hPedTotal->Write();
  hHigh->Write();
  hMaxADC->Write();

  //theTree->Write() ;
  hfile->Close(); 
  outFile.close(); 

  fclose(logfile);
  printf("Ntuple file successfully produced ! \n") ;
}
