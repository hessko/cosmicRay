void runGraphData(){
	gROOT->ProcessLine(".L graphData.c+");
	graphData();

}
