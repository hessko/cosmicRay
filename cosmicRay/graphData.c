#include "TTree.h"
#include "TFile.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
//#include "dataReader.c"
#include "TSystem.h"
#include "TClassTable.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TH1.h"
#include "TIterator.h"

#define DEBUG
//#define DEBUG2
//#define DEBUG3

using namespace std;

void graphData() {
	vector<Double_t> *adc = new vector<Double_t>;
	string *fileName = new string();
	*fileName ="/home/hessko/cosmicRay/Spill_27.root";
	TFile *myFile = TFile::Open((*fileName).c_str());
#ifdef DEBUG
	cout << "File opened." << endl;
#endif
	
/*	if (!TClassTable::GetDict("Data")) {
		gSystem->Load("$ROOTSYS/test/libData.so");
	}*/

	TTree *myTree = (TTree*) (*myFile).Get("Event04");
#ifdef DEBUG
	cout << "Got tree." << endl;
#endif
	//Loop over all branches on Tree(Event)
	TObjArray *branches = myTree->GetListOfBranches();
	TIter myIt(branches);
	TBranch *current;
	string *currentName = new string();
	//Data *holder = new Data();
	vector<Long64_t> *vect = new vector<Long64_t>;
	string emptyChannels[] = {"Channel08", "Channel09", "Channel0A", "Channel0B", "Channel0C", "Channel0D", "Channel0E", "Channel0F"};
#ifdef DEBUG2	
	int count = 0;
#endif
	while (current = (TBranch *)myIt.Next()) {
		*currentName = string(current->GetName());
		bool empty = false;
		for (unsigned int i = 0; i < sizeof(emptyChannels)/sizeof(emptyChannels[0]); i++) {
			if ((*currentName).compare(emptyChannels[i]) == 0) {
				empty = true;
#ifdef DEBUG
				cout << "Skipping Channel " << *currentName << endl;
#endif
			}
		}
		if (empty) continue;
#ifdef DEBUG2
		cout << "Current Name is " << *currentName << endl;
		count++;
#endif
		//myTree->SetBranchAddress("Channel00", &holder);
		//myTree->SetBranchAddress("Channel00", &vect);
		myTree->SetBranchAddress((*currentName).c_str(), &vect);

		(*myTree).GetEntry(0);
#ifdef DEBUG2
		cout << "Branch address set, Tree read." << endl;
#endif
//#ifdef DEBUG
	//	cout << "Set Branch." << endl;
//#endif
		//vector<Long64_t> *vect = new vector<Long64_t>;
		//*vect = (*holder).get_contents();
		//cout<<"holder contains a vector with size \t"<< (*holder).get_size() << endl;
/*		TBranch *myBranch = myTree->GetBranch("Channel00");
		int val = myBranch->GetEntry();
*/
		unsigned int numEntries = (*vect).size();//equals size of vector in branch
#ifdef DEBUG
		//cout << "GetEntry returns " << val << endl;
		cout << "Number of entries is " << numEntries << endl;
#endif
		//find the average of the first 20 entries
		float avg = 0;
		Long64_t total = 0;
		for (unsigned int i = 0; i < (*vect).size(); i++) {
			total+=(*vect)[i];
		}
		avg = total/105.0;
#ifdef DEBUG
		cout << "Average is " << avg << endl;
#endif
		//fill the x and y arrays, take the integral, and find max and min values
		Double_t integral = 0;
		int min = (*vect)[0];
		int max = (*vect)[0];
		Double_t x[numEntries], y[numEntries];
		for (unsigned int i = 0; i < numEntries; i++) {
			x[i] = i;
			y[i] = (*vect)[i] - avg;
			integral += y[i]; //rectangle has base of 1, height of y[i]
			if ((*vect)[i] < min) {
				min = (*vect)[i];
			}
			if ((*vect)[i] > max) {
				max = (*vect)[i];
			}
		}
		int nbins = max - min + 1;
#ifdef DEBUG3
		for (int i = 0; i < 20; i++) {
			cout << "Entry num: " << i << "\t" << "X " << x[i] << "\t" << "Y " << y[i] << endl;
		}
		cout << "Max value is " << max << "\t" << "Min value is " << min << endl;
#endif
		cout << "Integral is " << integral << endl;
		(*adc).push_back(integral);
	}
#ifdef DEBUG2
	cout << "There are " << count << " branches" << endl;
#endif
	Double_t adcMin = (*adc)[0];
	Double_t adcMax = (*adc)[0];	
	for (unsigned int i = 0; i < (*adc).size(); i++) {
		if ((*adc)[i] < adcMin) {
			adcMin = (*adc)[i];
		}
		if ((*adc)[i] > adcMax) {
			adcMax = (*adc)[i];
		}
	}
	Double_t maxBin = adcMax * 1.02;
	//Long64_t adcBins = maxBin - adcMin + 1;
	Long64_t adcBins = 100;
#ifdef DEBUG
	cout << "ADC max " << adcMax << "\t" << "ADC min " << adcMin << " ADC bins " << adcBins << endl;
#endif
	TCanvas *c3 = new TCanvas("c3", "Histo", 200, 10, 700, 500);
	TH1 *h2 = new TH1F("h2", "Histogram", adcBins, adcMin, maxBin);
	for (unsigned int i = 0; i < (*adc).size(); i++) {
		h2->Fill((*adc)[i]);
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

