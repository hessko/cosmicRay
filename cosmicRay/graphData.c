#include "TTree.h"
#include "TChain.h"
#include "TFile.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
//#include "dataReader.c"
#include "TSystem.h"
#include "TClassTable.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TH1.h"
#include "TIterator.h"

//#define DEBUG
//#define DEBUG2
//#define DEBUG3
//#define DEBUG4

using namespace std;

void graphData() {
	TCanvas *c3 = new TCanvas("c3", "Histo", 200, 10, 700, 500);
	vector<Double_t> *adc = new vector<Double_t>;
#ifdef DEBUG
	cout << "File opened." << endl;
#endif
	///making TChain

	map<string,TChain*> chainsAndTreeNames;
	string treeNames[] = {"Event00", "Event01", "Event02","Event03", "Event04", "Event05","Event06", "Event07","Event08", "Event09", "Event0A"};
//	string treeNames[] = {"Event00", "Event01", "Event02","Event03", "Event04"};
	for(unsigned int i=0; i<5; i++){
#ifdef DEBUG4
		cout << "Starting for loop i = \t" << i << endl;
#endif
		chainsAndTreeNames[treeNames[i] ] = new TChain(treeNames[i].c_str(),"");
		string *fileName = new string();
/*		for (int l = 0; l < 700; l++) {
			int spillNum = 68 + l;
			std::stringstream ss;
			ss << spillNum;
			string str;
			ss >> str;
			*fileName = "./rec_cap_20150716_trees/Spill_" + str + ".root";
			(chainsAndTreeNames[treeNames[i]])->Add((*fileName).c_str());
		}*/
		(chainsAndTreeNames[treeNames[i]])->Add("./rec_cap_20150716_trees/*.root");
#ifdef DEBUG
		cout << "Got tree." << endl;
#endif
		//Loop over all branches on Tree(Event)
		TObjArray *branches = (chainsAndTreeNames[treeNames[i]])->GetListOfBranches();
		TIter myIt(branches);
		TBranch *current;
		string *currentName = new string();
		vector<Long64_t> *vect = new vector<Long64_t>;
		string emptyChannels[] = {"Channel08", "Channel09", "Channel0A", "Channel0B", "Channel0C", "Channel0D", "Channel0E", "Channel0F"};
		int emptyChannelNumbers[] = {8,9,10,11,12,13,14,15};

#ifdef DEBUG
		cout<<"about to enter while loop"<<endl;
#endif
		
		current = (TBranch *)myIt.Next();
		*currentName = string(current->GetName());
		(chainsAndTreeNames[treeNames[i]])->SetBranchAddress((*currentName).c_str(), &vect);
		Long64_t entries = (chainsAndTreeNames[treeNames[i]])->GetEntries();
//		while (current = (TBranch *)myIt.Next() ) {
		for (Long64_t j = 0; j < entries; j++) {
			//if (j % 32 != 3) continue;
#ifdef DEBUG4
			cout << "Starting for loop j = \t" << j << endl;
#endif		
#ifdef DEBUG
			cout<<"started while loop"<<endl;
#endif
			bool empty = false;
#ifdef DEBUG4
			cout << "Checking if channel is empty. j = \t" << j << endl;
			cout << " sz 1 "<<sizeof(emptyChannelNumbers) <<"  sz2 " << sizeof(emptyChannelNumbers[0]) <<endl;
#endif
			for (unsigned int k = 0; k < sizeof(emptyChannelNumbers)/sizeof(emptyChannelNumbers[0]); k++) {
#ifdef DEBUG4
				cout << "j = \t" << j << " k = \t" << k << endl;
#endif
				if (emptyChannelNumbers[k] == j % 32) {
					empty = true;
#ifdef DEBUG4
					cout << "Skipping Channel " << j << endl;
#endif
				}
			}
#ifdef DEBUG4
			cout << "Finished checking for empty. j = \t" << j << endl;
#endif
			if (!empty) {
#ifdef DEBUG4
				cout << "Channel not empty. j = \t" << j << endl;
#endif
#ifdef DEBUG
				cout<<"entries = \t"<< entries << endl;
#endif
				(chainsAndTreeNames[treeNames[i]])->GetEntry(j);
				unsigned int numEntries = (*vect).size();//equals size of vector in branch
//		        cout<<" n of sample = "<<	numEntries <<endl ;	
				//find the average of all entries
				float avg = 0;
				Long64_t total = 0;
#ifdef DEBUG4
				cout << "Looking for average. j = \t" << j << endl;
#endif
				for (unsigned int k = 0; k < (*vect).size(); k++) {
					//printf(" (%d) = %d \n", k, (*vect)[k]) ;
					total+=(*vect)[k];
				}
				avg = total/105.0;
				//cout << "Average is " << avg << endl;
#ifdef DEBUG4
				cout << "Finished finding average. j = \t" << j << endl;
#endif
				//fill the x and y arrays, take the integral, and find max and min values
				Double_t integral = 0;
				Double_t x[numEntries], y[numEntries];
#ifdef DEBUG4
				cout << "Calculating integral. j = \t" << j << endl;
#endif
				for (unsigned int k = 1; k < numEntries - 1; k++) {
				//	x[k] = k;
				    
					y[k] = (*vect)[k] - avg - 5.84;
					if (y[k] > 0) {
						integral += y[k];
					}
//					integral += y[k]; //rectangle has base of 1, height of y[i]
				}
//				cout << "Integral is " << integral << endl;
#ifdef DEBUG4
				cout << "Found integral. j = \t" << j << endl;
#endif
				(*adc).push_back(integral);
				//cout<<" adc size = "<< (*adc).size() <<endl ;
			}
		}
#ifdef DEBUG
		cout<<"left while loop"<<endl;
#endif
#ifdef DEBUG2
		cout << "Branch address set, Tree read." << endl;
		cout << "There are " << count << " branches" << endl;
#endif
	}
#ifdef DEBUG4
	cout << "Size of adc vector is " << (*adc).size() << endl;
#endif
/*	Double_t adcMin = (*adc)[0];
	Double_t adcMax = (*adc)[0];	
	for (unsigned int i = 0; i < (*adc).size(); i++) {
		if ((*adc)[i] < adcMin) {
			adcMin = (*adc)[i];
		}
		if ((*adc)[i] > adcMax) {
			adcMax = (*adc)[i];
		}
	}
	Double_t maxBin = adcMax * 1.02;*/
	//Long64_t adcBins = maxBin - adcMin + 1;
	//Long64_t adcBins = 100;
	TH1F *h2 = new TH1F("h2", "Counts vs ADC value", 100, -20, 100);
	for (unsigned int i = 0; i < (*adc).size(); i++) {
		if ((*adc)[i] > 5) {
			h2->Fill((*adc)[i]);
		}
	}
	h2->Draw();

	/*	//graph the data
		TCanvas *c1 = new TCanvas("c1", "Graph", 200, 10, 700, 500);
		TGraph *gr = new TGraph(numEntries, x, y);
		gr->Draw("AB");
		//make a histogram of the data
		TCanvas *c2 = new TCanvas("c2", "Histo", 1000, 10, 700, 500);
		TH1 *h1 = new TH1F("h1", "Histogram", nbins, min, max + 1);
		for (unsigned int i = 0; i < numEntries; i++) {
			h1->Fill((*vect)[i]);
		}
		h1->Draw();*/

}

