// A filename has been added to the constructor so we can use this analysis file for any root file
//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Jul 29 13:47:14 2014 by ROOT version 5.34/14
// from TTree LHCO/Analysis tree
// found on file: tree_reader_lhco/tag_1_pgs_events.root
//////////////////////////////////////////////////////////

#ifndef tree_reader_lhco_h
#define tree_reader_lhco_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include <TClonesArray.h>
#include <TObject.h>
#include "../../ExRootAnalysis/ExRootAnalysis/ExRootClasses.h"

// Fixed size dimensions of array or collections stored in the TTree if any.
const Int_t kMaxEvent = 1;
const Int_t kMaxPhoton = 2;
const Int_t kMaxElectron = 3;
const Int_t kMaxMuon = 3;
const Int_t kMaxTau = 4;
const Int_t kMaxJet = 20;
const Int_t kMaxMissingET = 1;

class tree_reader_lhco {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           Event_;
   UInt_t          Event_fUniqueID[kMaxEvent];   //[Event_]
   UInt_t          Event_fBits[kMaxEvent];   //[Event_]
   Long64_t        Event_Number[kMaxEvent];   //[Event_]
   Int_t           Event_Trigger[kMaxEvent];   //[Event_]
   Int_t           Event_size;
   Int_t           Photon_;
   UInt_t          Photon_fUniqueID[kMaxPhoton];   //[Photon_]
   UInt_t          Photon_fBits[kMaxPhoton];   //[Photon_]
   Double_t        Photon_PT[kMaxPhoton];   //[Photon_]
   Double_t        Photon_Eta[kMaxPhoton];   //[Photon_]
   Double_t        Photon_Phi[kMaxPhoton];   //[Photon_]
   Double_t        Photon_EhadOverEem[kMaxPhoton];   //[Photon_]
   Int_t           Photon_size;
   Int_t           Electron_;
   UInt_t          Electron_fUniqueID[kMaxElectron];   //[Electron_]
   UInt_t          Electron_fBits[kMaxElectron];   //[Electron_]
   Double_t        Electron_PT[kMaxElectron];   //[Electron_]
   Double_t        Electron_Eta[kMaxElectron];   //[Electron_]
   Double_t        Electron_Phi[kMaxElectron];   //[Electron_]
   Double_t        Electron_Charge[kMaxElectron];   //[Electron_]
   Double_t        Electron_Ntrk[kMaxElectron];   //[Electron_]
   Double_t        Electron_EhadOverEem[kMaxElectron];   //[Electron_]
   Int_t           Electron_size;
   Int_t           Muon_;
   UInt_t          Muon_fUniqueID[kMaxMuon];   //[Muon_]
   UInt_t          Muon_fBits[kMaxMuon];   //[Muon_]
   Double_t        Muon_PT[kMaxMuon];   //[Muon_]
   Double_t        Muon_Eta[kMaxMuon];   //[Muon_]
   Double_t        Muon_Phi[kMaxMuon];   //[Muon_]
   Double_t        Muon_Charge[kMaxMuon];   //[Muon_]
   Double_t        Muon_Ntrk[kMaxMuon];   //[Muon_]
   Double_t        Muon_PTiso[kMaxMuon];   //[Muon_]
   Double_t        Muon_ETiso[kMaxMuon];   //[Muon_]
   Int_t           Muon_JetIndex[kMaxMuon];   //[Muon_]
   Int_t           Muon_size;
   Int_t           Tau_;
   UInt_t          Tau_fUniqueID[kMaxTau];   //[Tau_]
   UInt_t          Tau_fBits[kMaxTau];   //[Tau_]
   Double_t        Tau_PT[kMaxTau];   //[Tau_]
   Double_t        Tau_Eta[kMaxTau];   //[Tau_]
   Double_t        Tau_Phi[kMaxTau];   //[Tau_]
   Double_t        Tau_Charge[kMaxTau];   //[Tau_]
   Double_t        Tau_Ntrk[kMaxTau];   //[Tau_]
   Double_t        Tau_EhadOverEem[kMaxTau];   //[Tau_]
   Int_t           Tau_size;
   Int_t           Jet_;
   UInt_t          Jet_fUniqueID[kMaxJet];   //[Jet_]
   UInt_t          Jet_fBits[kMaxJet];   //[Jet_]
   Double_t        Jet_PT[kMaxJet];   //[Jet_]
   Double_t        Jet_Eta[kMaxJet];   //[Jet_]
   Double_t        Jet_Phi[kMaxJet];   //[Jet_]
   Double_t        Jet_Mass[kMaxJet];   //[Jet_]
   Double_t        Jet_Ntrk[kMaxJet];   //[Jet_]
   Double_t        Jet_BTag[kMaxJet];   //[Jet_]
   Double_t        Jet_EhadOverEem[kMaxJet];   //[Jet_]
   Int_t           Jet_Index[kMaxJet];   //[Jet_]
   Int_t           Jet_size;
   Int_t           MissingET_;
   UInt_t          MissingET_fUniqueID[kMaxMissingET];   //[MissingET_]
   UInt_t          MissingET_fBits[kMaxMissingET];   //[MissingET_]
   Double_t        MissingET_MET[kMaxMissingET];   //[MissingET_]
   Double_t        MissingET_Phi[kMaxMissingET];   //[MissingET_]
   Int_t           MissingET_size;

   // List of branches
   TBranch        *b_Event_;   //!
   TBranch        *b_Event_fUniqueID;   //!
   TBranch        *b_Event_fBits;   //!
   TBranch        *b_Event_Number;   //!
   TBranch        *b_Event_Trigger;   //!
   TBranch        *b_Event_size;   //!
   TBranch        *b_Photon_;   //!
   TBranch        *b_Photon_fUniqueID;   //!
   TBranch        *b_Photon_fBits;   //!
   TBranch        *b_Photon_PT;   //!
   TBranch        *b_Photon_Eta;   //!
   TBranch        *b_Photon_Phi;   //!
   TBranch        *b_Photon_EhadOverEem;   //!
   TBranch        *b_Photon_size;   //!
   TBranch        *b_Electron_;   //!
   TBranch        *b_Electron_fUniqueID;   //!
   TBranch        *b_Electron_fBits;   //!
   TBranch        *b_Electron_PT;   //!
   TBranch        *b_Electron_Eta;   //!
   TBranch        *b_Electron_Phi;   //!
   TBranch        *b_Electron_Charge;   //!
   TBranch        *b_Electron_Ntrk;   //!
   TBranch        *b_Electron_EhadOverEem;   //!
   TBranch        *b_Electron_size;   //!
   TBranch        *b_Muon_;   //!
   TBranch        *b_Muon_fUniqueID;   //!
   TBranch        *b_Muon_fBits;   //!
   TBranch        *b_Muon_PT;   //!
   TBranch        *b_Muon_Eta;   //!
   TBranch        *b_Muon_Phi;   //!
   TBranch        *b_Muon_Charge;   //!
   TBranch        *b_Muon_Ntrk;   //!
   TBranch        *b_Muon_PTiso;   //!
   TBranch        *b_Muon_ETiso;   //!
   TBranch        *b_Muon_JetIndex;   //!
   TBranch        *b_Muon_size;   //!
   TBranch        *b_Tau_;   //!
   TBranch        *b_Tau_fUniqueID;   //!
   TBranch        *b_Tau_fBits;   //!
   TBranch        *b_Tau_PT;   //!
   TBranch        *b_Tau_Eta;   //!
   TBranch        *b_Tau_Phi;   //!
   TBranch        *b_Tau_Charge;   //!
   TBranch        *b_Tau_Ntrk;   //!
   TBranch        *b_Tau_EhadOverEem;   //!
   TBranch        *b_Tau_size;   //!
   TBranch        *b_Jet_;   //!
   TBranch        *b_Jet_fUniqueID;   //!
   TBranch        *b_Jet_fBits;   //!
   TBranch        *b_Jet_PT;   //!
   TBranch        *b_Jet_Eta;   //!
   TBranch        *b_Jet_Phi;   //!
   TBranch        *b_Jet_Mass;   //!
   TBranch        *b_Jet_Ntrk;   //!
   TBranch        *b_Jet_BTag;   //!
   TBranch        *b_Jet_EhadOverEem;   //!
   TBranch        *b_Jet_Index;   //!
   TBranch        *b_Jet_size;   //!
   TBranch        *b_MissingET_;   //!
   TBranch        *b_MissingET_fUniqueID;   //!
   TBranch        *b_MissingET_fBits;   //!
   TBranch        *b_MissingET_MET;   //!
   TBranch        *b_MissingET_Phi;   //!
   TBranch        *b_MissingET_size;   //!

   tree_reader_lhco(char* rootFile, TTree *tree=0);
   virtual ~tree_reader_lhco();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef tree_reader_lhco_hxx

#endif // #ifdef tree_reader_lhco_hxx
