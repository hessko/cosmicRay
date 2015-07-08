#include "TTree.h"
#include "TChain.h"
#include <iostream>
#include <fstream>
#include <string>
#include "TFile.h"
#include <vector>

//#define DEBUG
#define DEBUG2

using namespace std;

void dataReader() {
	string entry;
	TTree *outputTree = new TTree("Event 0", "title");
	TFile *outputFile = new TFile("/home/hessko/cosmicRay/testFile.root", "recreate");
	vector<string> *line = new vector<string>;
	vector<long int> *data = new vector<long int>;
	ifstream dataFile("/home/hessko/cosmicRay/rec_capture_20150701_170501.txt");
	int lineCounter = 0;
	long int channel = 0;
	int eventNum = 0;
	int prevEvt = 0;
	while (dataFile.peek() != EOF && dataFile.good()) {
		if (dataFile.peek() == 13) { //13 is ASCII for enter
			channel = strtol(((*line)[7]).c_str(), NULL, 16);
			eventNum = atoi(((*line)[9]).c_str());
			//When the event changes, write the finished tree to a file. Then create a new tree for the new event.
			if (prevEvt != eventNum || dataFile.peek() == EOF) {
				outputFile->cd();
				outputTree->Write();
#ifdef DEBUG2
				cout << "Wrote to tree" << outputTree->GetName() << endl;
#endif
				string * newTreeName = new string();
				*newTreeName ="Event " + (*line)[9];
				outputTree = new TTree((*newTreeName).c_str(), "title");
			}
			//data starts at index 25
			for (int i = 25; i < (*line).size(); i++) {
				(*data).push_back(strtol(((*line)[i]).c_str(), NULL, 16));
			}
			//adds a new Branch for each channel
			string * newBranchName = new string();
			*newBranchName = "Channel " + (*line)[7];
			outputTree->Branch((*newBranchName).c_str(), &(*data), 32000, 0);
#ifdef DEBUG
			lineCounter++;
#endif		
			//reset placeholer variables
			prevEvt = eventNum;
			data = new vector<long int>;
			line = new vector<string>;
		}
		//skip comment lines
		if (dataFile.peek() == 42) { //42 is ASCII for *
			dataFile.ignore(1000, 10);
			lineCounter++;
			continue;
		}
		dataFile >> entry;
		if (entry.empty()) break;
#ifdef DEBUG
		if (lineCounter > 150) break;
#endif
		(*line).push_back(entry);	
	}
	outputFile->Close();
}
