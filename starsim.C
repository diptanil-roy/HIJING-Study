// macro to instantiate the Geant3 from within
// STAR  C++  framework and get the starsim prompt
// To use it do
//  root4star starsim.C

#include <typeinfo>

class St_geant_Maker;
St_geant_Maker *geant_maker = 0;

class StarGenEvent;
StarGenEvent   *event       = 0;

class StarPrimaryMaker;
StarPrimaryMaker *_primary = 0;

class StarHijing;
StarHijing *hijing = 0; 
// ----------------------------------------------------------------------------
void geometry( TString tag, bool agml=true )
{
  TString cmd = "DETP GEOM "; cmd += tag;
  if ( !geant_maker ) geant_maker = (St_geant_Maker *)chain->GetMaker("geant");
  geant_maker -> LoadGeometry(cmd);
  
  //  if ( agml ) command("gexec $STAR_LIB/libxgeometry.so");
}
// ----------------------------------------------------------------------------
void command( TString cmd )
{
  if ( !geant_maker ) geant_maker = (St_geant_Maker *)chain->GetMaker("geant");
  geant_maker -> Do( cmd );
}
// ----------------------------------------------------------------------------
void trig( int n=0 )
{
  for ( int i=0; i<n+1; i++ ) {
    chain->Clear();
    chain->Make();
  }
}
// ----------------------------------------------------------------------------
void Hijing()
{
  hijing = new StarHijing(); 
  hijing->SetTitle("Hijing 1.383");

  // Setup collision frame, energy and beam species
  hijing->SetFrame("CMS",200.0);
  hijing->SetBlue("Au");
  hijing->SetYell("Au");  
  hijing->SetImpact(0.0, 3.0);       // Impact parameter min/max (fm)    0.   30.



  _primary -> AddGenerator(hijing);
  _primary -> SetCuts( 1.0E-6 , -1., -2.5, +2.5 );
  _primary -> SetEtaRange(-1., 1.);
  
}
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void starsim( const char* filename = "hijing.starsim.root", int nevents=10, int rngSeed=0, int quenching = 1, const char* tag="y2018" )
{ 

  gROOT->ProcessLine(".L bfc.C");
  {
    TString simple = Form("%s geant gstar usexgeom agml ",tag);
    bfc(0, simple );
  }

  gSystem->Load( "libVMC.so");

  gSystem->Load( "StarGeneratorUtil.so" );
  gSystem->Load( "StarGeneratorEvent.so" );
  gSystem->Load( "StarGeneratorBase.so" );
  gSystem->Load( "libMathMore.so"   );  
  gSystem->Load( "libHijing1_383.so");
  gSystem->Load( "xgeometry.so"     );

  // Setup RNG seed and map all ROOT TRandom here
  StarRandom::seed( rngSeed );
  StarRandom::capture();
  
  //
  // Create the primary event generator and insert it
  // before the geant maker
  //
  _primary = new StarPrimaryMaker();
  {
    _primary -> SetFileName( filename );
    chain -> AddBefore( "geant", _primary );
  }


  //
  // Setup an event generator
  //
  Hijing();

  //
  // Initialize primary event generator and all sub makers
  //
  _primary -> Init();

  // Configure HIJING simulation
  HiParnt_t &hiparnt = hijing->hiparnt();
  {
    hiparnt.ihpr2(4) = quenching;     // Jet quenching (1=yes/0=no)       0
    hiparnt.ihpr2(3) = 1;     // Hard scattering (1=yes/0=no)
    hiparnt.hipr1(8) = 15.0;   // Minimum PT transfer in hard or semihard scatterings
    hiparnt.hipr1(10) = -2.5;  //    pT jet (negative indicates lower limit)
    hiparnt.ihpr2(8)  = 10;   // Max number of jets / nucleon
    hiparnt.ihpr2(11) = 1;    // Set baryon production
    hiparnt.ihpr2(12) = 1;    // Turn on/off decay of particles [1=recommended]
    hiparnt.ihpr2(18) = 0;    // 1=B quark production.  0=C quark production.
    // hiparnt.hipr1(7) = 5.35;  // Set B production ???? Not really used... Really ????

  // For more configuration options, see the HIJING manual
  // http://ntc0.lbl.gov/~xnwang/hijing/doc.html
  }
  //
  // Setup geometry and set starsim to use agusread for input
  //
  //geometry("y2012");
  command("gkine -4 0");
  // command("gfile o hijing.starsim.root");
  
  //
  // Trigger on nevents
  //
  trig( nevents );
  command("gprint kine");

  command("call agexit");

  // if ( gROOT->IsBatch() ) {
  //     command("call agexit");  // Make sure that STARSIM exits properly
  // }
  // else {
  //     std::cout << "Interactive mode.  Please call AgExit() before .q" << std::endl; 
  // }

}
// ----------------------------------------------------------------------------
void AgExit() { command("call agexit"); } 
