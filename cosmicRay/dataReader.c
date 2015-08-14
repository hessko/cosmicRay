#include "TSystem.h"
#include "TClassTable.h"
#include "TTree.h"
#include "TChain.h"
#include <iostream>
#include <fstream>
#include <string>
#include "TFile.h"
#include <vector>
//#include "Data.h"

//#define DEBUG
//#define DEBUG2

using namespace std;


class Data : public TObject {
		vector<Long64_t> container;
	public:
		Data();
		void add_val(Long64_t);
		vector<Long64_t> get_contents();
		ClassDef(Data,1);
};

Data::Data() {
	//container = new vector<Long64_t>;
}

void Data::add_val(Long64_t newVal) {
	container.push_back(newVal);
}

vector<Long64_t> Data::get_contents() {
	return container;
}


void dataReader() {
	string entry;
	TTree *outputTree = new TTree("Event00", "title");
	TFile *outputFile = NULL;
	vector<string> *line = new vector<string>;
	vector<Long64_t> *data = new vector<Long64_t>;
	Int_t nElems = 4;
	//Long64_t testArr[200];
	//outputTree->Branch("testArr", testArr,"testArr[nElems]/L");
	ifstream dataFile("/home/hessko/cosmicRay/rec_capture_20150716_142724.txt");
	int lineCounter = 0;
	long int channel = 0;
	long int eventNum = 0;
	string *newFileName = new string();
#ifdef DEBUG2
	int fileCount = 0;
#endif
	string *spillNum = new string();

	/*if (!TClassTable::GetDict("Data")) {
		gSystem->Load("$ROOTSYS/test/libData.so");
	}*/

	while (dataFile.peek() != EOF && dataFile.good()) {
		if (dataFile.peek() == 13) { //13 is ASCII for enter
			//skip comment lines
			if ((*line).at(0) == "***" && (*line).at(1) != "starting") { //42 is ASCII for *
				dataFile.ignore(1000, 10);
#ifdef DEBUG			
				lineCounter++;
#endif			
			}
			//Change spill number when necessary
			else if ((*line).at(1) == "starting") {
				if(outputFile != NULL) {
					(*outputFile).Close();
				}
				*spillNum = (*line)[4];
#ifdef DEBUG
				cout << "New Spill Number is " << *spillNum << endl;
#endif			
				*newFileName = "/home/hessko/cosmicRay/rec_cap_20150716_trees/Spill_" + *spillNum + ".root";
#ifdef DEBUG
				cout << "New File Name will be " << *newFileName << endl;
#endif
				outputFile = new TFile((*newFileName).c_str(), "recreate");
				outputFile->cd();
#ifdef DEBUG			
				lineCounter++;
#endif			
#ifdef DEBUG2
				fileCount++;
#endif
			} 
			else {
				channel = strtol(((*line)[7]).c_str(), NULL, 16);
				eventNum = strtol(((*line)[9]).c_str(), NULL, 16);
#ifdef DEBUG				
				cout << "Line " << lineCounter << "\t" << "Channel "<< (*line)[7] << "\t" << "Event " << (*line)[9] <<endl;
#endif
				//When a new event begins, create a new tree for the new event.
				if ((*line)[7] == "00") {
					string * newTreeName = new string();
					*newTreeName ="Event" + (*line)[9];
					outputTree = new TTree((*newTreeName).c_str(), "title");
				}
				//data starts at index 25
				for (unsigned int i = 25; i < (*line).size(); i++) {
					(*data).push_back(strtol(((*line)[i]).c_str(), NULL, 16));
				}
				//adds a new Branch for each channel
/*				Data *temp = new Data();
				for (unsigned int i = 0; i < (*data).size(); i++) {
					(*temp).add_val((*data)[i]);
				}*/
				string * newBranchName = new string();
				*newBranchName = "Channel" + (*line)[7];
				outputTree->Branch((*newBranchName).c_str(), &(*data));
				//outputTree->Branch((*newBranchName).c_str(), "Data", &(*temp));
				
				outputTree->Fill();
#ifdef DEBUG
				lineCounter++;
#endif	
				//When and event ends, write the finished tree to the file and close it.
				if ((*line)[7] == "1F") {
					outputTree->Write();
#ifdef DEBUG
					cout << "Wrote to tree " << outputTree->GetName() << endl;
#endif
				}
			}
			//reset placeholer variables
			data = new vector<Long64_t>;
			line = new vector<string>;	
		}
		dataFile >> entry;
		if (entry.empty()) break;
#ifdef DEBUG
		if (lineCounter > 100) break;
#endif
		(*line).push_back(entry);	
	}
#ifdef DEBUG2
	cout << "Number of files created: " << fileCount << endl;
#endif
	outputFile->Close();
}
