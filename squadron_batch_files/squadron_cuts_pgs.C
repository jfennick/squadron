/*
How to run the script using the CINT interpreter:
First comment out the first two includes below, then run
root -l -q "squadron_cuts_pgs.C(\"tag_1_pgs_events.root\",\"processname\",\"runname_num\")"

How to compile the script:
First add ~/MG5_aMC_v2_1_2/ExRootAnalysis/lib to your LDLIBRARYPATH
g++ `root-config --cflags` -I ~/MG5_aMC_v2_1_2/ExRootAnalysis/ tree_reader_lhco.C squadron_cuts_pgs2.C `root-config --glibs` -L ~/MG5_aMC_v2_1_2/ExRootAnalysis/lib -lExRootAnalysis -o squadron_cuts_pgs -Ofast
How to run the compiled script:
./squadron_cuts_pgs tag_1_pgs_events.root processname runname_num
*/
#include "tree_reader_lhco.h"

#include "TH1F.h"
#include "TLorentzVector.h"
#include "TSystem.h"

#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

#define NUMHISTOS 4 // Number of histograms
tree_reader_lhco *tr; //Make this global so all args are strings so we can choose to use root interactively

void squadron_cuts_pgs(const char *rootFile, const char *processname, const char *runname_num);

int main (int argc, char *argv[]) {
  if (argc != 4) {cout << "arg1: rootFile, arg2: processname, arg3: runname_num\n"; return 1;}
  gSystem->Load("../../../ExRootAnalysis/lib/libExRootAnalysis.so");
  tr = new tree_reader_lhco(argv[1]);
  squadron_cuts_pgs(argv[1], argv[2], argv[3]);
  delete tr;
  return 0;
}

void squadron_cuts_pgs(const char *rootFile, const char *processname, const char *runname_num) {
   
   ofstream resultsfile;// All-in-one output file
   stringstream sseff;
   sseff << runname_num << "/" << runname_num << "_results.txt";
   resultsfile.open(sseff.str().c_str());
   resultsfile << "CutName\txsec no cuts\txsec w/ cuts\t#Eve nc\t#Eve wc\tEfficiency\t#EventsNJets" << endl;
//TODO Need to modify gg_xsecs_out.dat code in cycle_squadron_pgs.

//Read in the cross section before cuts
stringstream ssdat;
ssdat << "../SubProcesses/" << runname_num << "_results.dat";
ifstream datfile(ssdat.str().c_str());
if (datfile.fail()) {cout << "Error! " << ssdat.str() << " does not exist." << endl; return;}
Double_t crossx;
datfile >> crossx;//ignore the rest of the file
datfile.close();
crossx *= 1000.0;//convert cross section to femtobarns

int numcuts;
string cutname;
Int_t NPTCUTJETS, NJETSMIN;
Double_t CUTJETETA, CUTJETPTHI, CUTJETPTLO, CUTMETMIN, DELTARMIN, DELTAPHIMINJ3MET, HTMIN, HT5MIN, TAUTAGMIN;
ifstream paramfile("../../cut_parameters");// Input file containing all the combinations of cut parameters
paramfile.ignore(numeric_limits<streamsize>::max(), '\n'); //skip the first line
if (paramfile.fail()) {cout << "Error! ../../cut_parameters does not exist." << endl; return;}

   paramfile >> numcuts;
   //cout << numcuts << "\n";
   // Loop over all sets of cut parameters
   for (int i=0; i<numcuts; i++)  {
   paramfile >> cutname >> CUTJETETA >> CUTJETPTHI >> CUTJETPTLO >> CUTMETMIN >> DELTARMIN;
   paramfile >> DELTAPHIMINJ3MET >> NPTCUTJETS >> NJETSMIN >> HTMIN >> HT5MIN >> TAUTAGMIN;
   cout << cutname << " " << CUTJETETA << " " << CUTJETPTHI << " " << CUTJETPTLO << " ";
   cout << CUTMETMIN << " " << DELTARMIN << " " << DELTAPHIMINJ3MET << " " << NPTCUTJETS << " ";
   cout << NJETSMIN << " " << HTMIN << " " << HT5MIN << " " << TAUTAGMIN << endl;

   Int_t numEventsPassCuts = 0,numEventsWithNJets[kMaxJet];
   Double_t HT,HT5;

   TH1F*  Histos[NUMHISTOS];// Define histograms
   Histos[0] = new TH1F("NJets","Number of Jets", 20,0,20);
   Histos[1] = new TH1F("MissET","Missing Transverse Energy",100,0,500);
   Histos[2] = new TH1F("HT","H_T from All Objects", 200,0,800);
   Histos[3] = new TH1F("HT5","H_T from Five Leading Jets",200,0,800);

   for(Int_t ijet=0; ijet<kMaxJet; ijet++){ numEventsWithNJets[ijet] = 0; }

   ofstream eventfile;// Output file which lists the number of every event which passes the cuts
   stringstream ssevent;
   ssevent << runname_num << "/" << runname_num << "_" << cutname << "_event_numbers.txt";
   eventfile.open(ssevent.str().c_str());
   eventfile << "The following events passed all cuts:\n";

   Long64_t numberOfEvents = tr->fChain->GetEntriesFast();

   for(Long64_t eventNum = 0; eventNum < numberOfEvents; ++eventNum) {
     Long64_t ientry = tr->LoadTree(eventNum);
     if (ientry < 0) break;
     tr->fChain->GetEntry(eventNum);

     Double_t MissET = tr->MissingET_MET[0];

// We are only interested in fully hadronic decays of Taus, so keep events with
// enough jets and ignore / "veto" all events with charged leptons of any sort
     if((tr->Jet_size >= NJETSMIN) && (tr->Electron_size + tr->Muon_size == 0)) {
// assign a counter (numJetsPassPtCut) for jets that meet the p_T and eta cuts
// and set initial values for H_T and H_T5 before the jet p_T are added to the sum
       //Long64_t* jetidx = new Long64_t[kMaxJet];
       TMath::Sort(tr->Jet_size,tr->Jet_PT,tr->Jet_Index);//sort jets by PT

       Int_t numJetsPassPtCut = 0, numTauTag = 0;
       HT = MissET;
       HT5 = 0.0;
       Bool_t jetsok = true;
// cycle through jets and apply Delta R isolation cuts for each jet-jet combination, DeltaPhi cuts for 
// the three leading jets and the MET vector, and p_T cuts on the leading NPTCUTJETS jets, where
// NPTCUTJETS is some integer specified in the definitions above.  H_T and H_T5 are also calculated: the
// first is the sum of all jets' p_T and MET, the second is the sum of the leading five jets' p_T
       for (Int_t ijet=0; ijet<tr->Jet_size; ijet++){
//         if (jet1->TauTag) { numTauTag++; }         // Tau Tagging
         TLorentzVector vjet1;
         vjet1.SetPtEtaPhiM(tr->Jet_PT[tr->Jet_Index[ijet]], tr->Jet_Eta[tr->Jet_Index[ijet]], tr->Jet_Phi[tr->Jet_Index[ijet]], tr->Jet_Mass[tr->Jet_Index[ijet]]);
         HT += tr->Jet_PT[tr->Jet_Index[ijet]];                       // add jet p_T to H_T
         if (ijet < 5) { HT5 += tr->Jet_PT[tr->Jet_Index[ijet]]; }    // add jet p_T to H_T5
         if (tr->Jet_PT[tr->Jet_Index[ijet]] >= CUTJETPTLO) { numJetsPassPtCut++; }  // count the jet
         for (Int_t jjet=ijet+1; jjet<tr->Jet_size; jjet++){
           TLorentzVector vjet2, vmet;
           vjet2.SetPtEtaPhiM(tr->Jet_PT[tr->Jet_Index[jjet]], tr->Jet_Eta[tr->Jet_Index[jjet]], tr->Jet_Phi[tr->Jet_Index[jjet]], tr->Jet_Mass[tr->Jet_Index[jjet]]);
           vmet.SetPtEtaPhiM(MissET,0,tr->MissingET_Phi[0],0);// use met->Eta?
           Double_t Pi = 3.14159265; Double_t Degree = Pi / 180.0;
           if (vjet1.DeltaR(vjet2) < DELTARMIN ||                                      // apply Delta R_jj criterion
              (tr->Jet_PT[tr->Jet_Index[ijet]]<CUTJETPTHI && ijet<NPTCUTJETS) ||                // apply p_T criterion
              (TMath::Abs(vjet1.DeltaPhi(vmet)) < DELTAPHIMINJ3MET * Degree && ijet<3))// apply Delta phi criterion
             {jetsok=false;} // end if
         } // end jjet for
       } // end ijet for
//Now we can apply the final cuts and record the event
       if (jetsok && // verifies that jet isolation cuts are satisfied
//           numTauTag >= TAUTAGMIN &&                // apply TauTag cut
           MissET >= CUTMETMIN &&                   // apply MET cut
           HT >= HTMIN &&                           // apply HT cut
           HT5 >= HT5MIN &&                         // apply HT5 cut 
           numJetsPassPtCut >= NJETSMIN){           // apply jet number cut 
         numEventsPassCuts++;
         numEventsWithNJets[numJetsPassPtCut-1]++;  // -1 for zero based array indexing
         Histos[0]->Fill(numJetsPassPtCut);         // Number of jets in the event
         Histos[1]->Fill(MissET);                   // Missing Energy
         Histos[2]->Fill(HT);                       // True H_T
         Histos[3]->Fill(HT5);                      // H_T from leading five jets
// list event numbers of events passing all cuts in the output text file	      
         eventfile << eventNum << "\n";
       } // end final cut if
     } // end event veto if
   } // end main event loop
   eventfile.close();
   
   Double_t sigeff = 1.0*numEventsPassCuts/numberOfEvents; // efficiency of the cuts
   //always print 16 characters for floating-point values so our output can be tab aligned
   resultsfile << cutname << "\t" << setw(16) << left << crossx << setw(16) << left << crossx*sigeff
               << numberOfEvents << "\t" << numEventsPassCuts << "\t" << setw(16) << left << sigeff;
   
   for(Int_t ijet=0; ijet<9; ijet++){
     resultsfile << numEventsWithNJets[ijet] << "\t";
   }
   for(Int_t ijet=9; ijet<kMaxJet; ijet++){
     resultsfile << numEventsWithNJets[ijet] << " ";
   }
   resultsfile << endl;

   TDirectory *curDir = gDirectory;
   stringstream ssroot;
   ssroot << runname_num << "/" << runname_num << "_" << cutname << "_out.root";
   TFile* fOut = new TFile(ssroot.str().c_str(),"RECREATE");
   
   fOut->cd();
   
   for (Int_t idx=0;idx<NUMHISTOS;idx++)
      Histos[idx]->Write();
   curDir->cd();
   fOut->Close();
      
   for (Int_t idx=0;idx<NUMHISTOS;idx++)
      delete Histos[idx];
   delete fOut;
} // end of cut parameters loop
resultsfile.close();
paramfile.close();
} // end of squadron_cuts_pgs function
