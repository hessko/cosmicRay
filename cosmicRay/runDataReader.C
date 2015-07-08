void runDataReader(){
	gROOT->ProcessLine(".L dataReader.c");
	dataReader();

}
