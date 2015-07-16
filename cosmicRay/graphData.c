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

#define DEBUG

using namespace std;

void graphData() {
//	string *fileName = new string("/home/hessko/cosmicRay/Spill_6.root");
	TFile *myFile = TFile::Open("/home/hessko/cosmicRay/Spill_6.root");
#ifdef DEBUG
	cout << "File opened." << endl;
#endif
	
/*	if (!TClassTable::GetDict("Data")) {
		gSystem->Load("$ROOTSYS/test/libData.so");
	}*/

	TTree *myTree = (TTree*) (*myFile).Get("Event00");
#ifdef DEBUG
	cout << "Got tree." << endl;
#endif
	//Data *holder = new Data();
	vector<Long64_t> *vect = new vector<Long64_t>;
	//myTree->SetBranchAddress("Channel00", &holder);
	myTree->SetBranchAddress("Channel00", &vect);

	(*myTree).GetEntry(0);
#ifdef DEBUG
	cout << "Branch address set, Tree read." << endl;
#endif
//#ifdef DEBUG
//	cout << "Set Branch." << endl;
//#endif
	//vector<Long64_t> *vect = new vector<Long64_t>;
	//*vect = (*holder).get_contents();
	//cout<<"holder contains a vector with size \t"<< (*holder).get_size() << endl;
/*	TBranch *myBranch = myTree->GetBranch("Channel00");
	int val = myBranch->GetEntry();*/
	unsigned int numEntries = (*vect).size();//equals size of vector in branch
#ifdef DEBUG
	//cout << "GetEntry returns " << val << endl;
	cout << "Number of entries is " << numEntries << endl;
#endif
	//find the average of the first 20 entries
	float avg = 0;
	Long64_t total = 0;
	for (unsigned int i = 0; i < 20; i++) {
		total+=(*vect)[i];
	}
	avg = total/20.0;
#ifdef DEBUG
	cout << "Average of first 20 is " << avg << endl;
#endif
	//fill the x and y arrays
	Double_t x[numEntries], y[numEntries];
	for (unsigned int i = 0; i < numEntries; i++) {
		x[i] = i;
		Long64_t val = (*vect)[i] - avg;
		if (val >= 0) { //subtract the average from all entries before assigning to y. Don't let values be negative
			y[i] = val;
		} else {
			y[i] = 0;
		}
	}
#ifdef DEBUG
	for (int i = 0; i < 20; i++) {
		cout << "Entry num: " << i << "\t" << "X " << x[i] << "\t" << "Y " << y[i] << endl;
	}
#endif
	//

	TCanvas *c1 = new TCanvas("c1", "Graph", 200, 10, 700, 500);
	TGraph *gr = new TGraph(numEntries, x, y);
	gr->Draw("AB");
}

