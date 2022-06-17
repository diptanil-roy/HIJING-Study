void ReadTree(){
	gROOT->ProcessLine(".L Test.C");
	gROOT->ProcessLine("Test t");
	gROOT->ProcessLine("t.Loop()");	
}