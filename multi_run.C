#include <fstream>
#include <iomanip>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <cstring>
#include <sys/stat.h>
using namespace std;

/* The multi_run functionality built-in to madgraph basically does NOT work.
 It generates events for all runs at the parton level, sends each run of
 parton events through pythia, but then combines the resulting events into
 one file at the *parton* level!  Also, it does NOT run pgs/delphes.

 This script does what madgraph should have done.  It generates events,
 sends them to pythia, sends them to delphes, applies cuts, and combines
 the *results.txt files. It also implements checkpoints, allowing you
 to interrupt a calculation and restart it without recalculating existing
 events. This script assumes run_mg5 has already been executed
 (so the directory arg1 already exists).*/

const int nc = 15; // *half* the number of cut_parameters (assumes regular cuts then tau tagged cuts)
const int maxJets = 25; // maximum number of jets; if using pgs, change this to kMaxJet=20
double Txsecnc[2*nc]={0}, Txsecwc[2*nc]={0}, TNumEveNC[2*nc]={0}, TNumEveWC[2*nc]={0}, TEff[2*nc]={0}, TJets[2*nc][maxJets]={{0}};
const int num_backs = 9;
double back_vals[nc][num_backs]={{0}};
//Global variables are bad ... ... oh well!

void combine_results(string strresults, string strcombinedresults, int runnum, int backnum) {
  double xsecnc, xsecwc, NumEveNC, NumEveWC, Eff, Jet;
  string headers, cutname;
  ifstream ifresults(strresults.c_str());
  ofstream ofresults(strcombinedresults.c_str());
  getline(ifresults, headers);
  ofresults << headers << endl;
  cout << headers << endl;
  double Cxsecnc[nc]={0}, Cxsecwc[nc]={0}, CNumEveNC[nc]={0}, CNumEveWC[nc]={0}, CEff[nc]={0}, CJets[nc][maxJets]={{0}};
  //Arrays for the combined results for regular cuts & tau tagged cuts
  for (int i=0; i<2*nc; i++) {
    // Read in the individual results, tally up the totals, and write out the combined results
    ifresults >> cutname >> xsecnc >> xsecwc >> NumEveNC >> NumEveWC >> Eff;
    Txsecnc[i] += xsecnc; TNumEveNC[i] += NumEveNC; TNumEveWC[i] += NumEveWC;
    double Axsecnc = Txsecnc[i] / runnum;
    double Teff;
    if (TNumEveNC[i]==0) {Teff=0;} else {Teff = TNumEveWC[i] / TNumEveNC[i];}
    double Txecwc = Axsecnc * Teff;
    cout << cutname << "\t" << setw(16) << left << Axsecnc << setw(16) << left << Txecwc << TNumEveNC[i] << "\t" << TNumEveWC[i] << "\t" << setw(16) << left << Teff;
    ofresults << cutname << "\t" << setw(16) << left << Axsecnc << setw(16) << left << Txecwc << TNumEveNC[i] << "\t" << TNumEveWC[i] << "\t" << setw(16) << left << Teff;
    for (int j=0; j<9; j++) {
      ifresults >> Jet;
      TJets[i][j] += Jet;
      cout << setw(5) << left << TJets[i][j];//keep the width as small as possible to prevent line-wrapping
      ofresults << TJets[i][j] << "\t";
    }
    for (int j=9; j<maxJets; j++) {
      ifresults >> Jet;
      TJets[i][j] += Jet;
      cout << TJets[i][j] << " ";
      ofresults << TJets[i][j] << " ";
    }
    cout << endl;
    ofresults << endl;
  }
  ifresults.close();

  double Tratio=0, nonzero=0, DCxsec=0;
  for (int i=0; i<nc; i++) {
    // now combine the regular and tau-tagged totals, and write out the results and ratio of our results to the values in arxiv 1109.6014
    double Axsecnc = Txsecnc[i] / runnum; //same as before
    double Teff;
    if ((TNumEveNC[i]+TNumEveNC[i+nc])==0) {Teff=0;} else {Teff = (TNumEveWC[i]+TNumEveWC[i+nc]) / TNumEveNC[i];}
    double Txecwc = Axsecnc * Teff;
    DCxsec += Txecwc;
    double ratio;
    if (back_vals[i][backnum]==0) {ratio=0;} else {ratio=Txecwc/back_vals[i][backnum];}
    if (ratio != 0) { Tratio += ratio; nonzero++; }
    cout << "ratio->\t" << setw(16) << left << ratio << setw(16) << left << Txecwc << TNumEveNC[i] << "\t" << (TNumEveWC[i]+TNumEveWC[i+nc]) << "\t" << setw(16) << left << Teff;
    ofresults << "ratio->\t" << setw(16) << left << ratio << setw(16) << left << Txecwc << TNumEveNC[i] << "\t" << (TNumEveWC[i]+TNumEveWC[i+nc]) << "\t" << setw(16) << left << Teff;
    for (int j=0; j<9; j++) {
      cout << setw(5) << left << TJets[i][j] + TJets[i+nc][j];//keep the width as small as possible to prevent line-wrapping
      ofresults << TJets[i][j] + TJets[i+nc][j] << "\t";
    }
    for (int j=9; j<maxJets; j++) {
      cout << TJets[i][j] + TJets[i+nc][j] << " ";
      ofresults << TJets[i][j] + TJets[i+nc][j] << " ";
    }
    cout << endl;
    ofresults << endl;
  }
  cout << "average ratio\t" << Tratio / nonzero << "\ttotal \"double counted\" cross section\t" << DCxsec << endl;
  ofresults << "average ratio\t" << Tratio / nonzero << "\ttotal \"double counted\" cross section\t" << DCxsec << endl;
  ofresults.close();
  return;
}

int main (int argc, char *argv[]) {
  if (argc != 6) {cout << "arg1: process name (main directory), arg2: run name, arg3: number of runs, arg4: background number, arg5: recut (1 or 0)\n"; return 1;}
  char* processname = argv[1];
  char* runbasename = argv[2];
  int NumRuns = atoi(argv[3]);
  int backnum = atoi(argv[4]);
  int recut = atoi(argv[5]);

  ifstream ifbackvals("background_values");
  string headers;
  getline(ifbackvals, headers);
  for (int i=0; i<nc; i++) {
    for (int j=0; j<num_backs; j++) {
      ifbackvals >> back_vals[i][j];
    }
  }
  ifbackvals.close();

  stringstream sscombinedresults, sscopy;
  sscombinedresults << processname << "/Events/" << runbasename << "_results.txt";
  sscopy << "cp " << sscombinedresults.str() << " .";
  stringstream ssmadspin;
  ssmadspin << processname << "/Cards/madspin_card.dat";
  ifstream ifmadspin(ssmadspin.str().c_str());
  
  for (int runnum=1; runnum<=NumRuns; runnum++) {
    stringstream ssrunname, ssrundir, ssfulldirpath, ssresults;
    ssrunname << runbasename << "_" << runnum;
    ssrundir << ssrunname.str();
    if (!ifmadspin.fail()) {ssrundir << "_decayed_1";}//accomodate madspin's convoluted directory structure
    ssfulldirpath << processname << "/Events/" << ssrundir.str();
    ssresults << ssfulldirpath.str() << "/" << ssrundir.str() << "_results.txt";
    
    struct stat sb; // check if the process name directory exists
    if (stat(ssfulldirpath.str().c_str(), &sb) == 0 && S_ISDIR(sb.st_mode)) {
      ifstream ifresults(ssresults.str().c_str());
      if (ifresults.fail()) {
        ifresults.close();
        cout << "Error! " << ssresults.str() << " does not exist. " << endl;
        cout << "Calculation might have interrupted and directory not removed." << endl;
        cout << "Delete " << ssfulldirpath.str() << " to rerun." << endl;
        return 1;
      } else {
        ifresults.close();
        cout << ssresults.str() << " exists. Only combining events." << endl;
        if (recut) {
          // re-cut events (i.e. in case cut_parameters file has been modified).
          // Pass parameters as explicit arguments rather than using sed
          stringstream sscut;
          sscut << "cd " << processname << "/Events; " << "../../squadron_batch_files/squadron_cuts_delphes "
                << ssrundir.str() << "/tag_1_delphes_events.root " << processname << " " << ssrundir.str();
          system(sscut.str().c_str());
        }
        // combine results
        combine_results(ssresults.str(), sscombinedresults.str(), runnum, backnum);
      }
    } else {
      cout << ssfulldirpath.str() << " does not exist. Calculating and combining events." << endl;

      // calculate events
      stringstream ssgen;
      ssgen << "cd " << processname << "; ./bin/generate_events -f " << ssrunname.str();
      system(ssgen.str().c_str());

      // remove files
      stringstream ssrem;// use ssrundir.str() here
      ssrem << "./remove_files " << processname << " " << ssrundir.str();
      system(ssrem.str().c_str());

      // Get the cross section from pythia's log because when doing jet matching,
      // the cross section after matching is NOT stored in SubProcesses/runname_results.dat
      stringstream ssgrep;
      ssgrep << "cd " << processname << "/Events/" << ssrundir.str() << "; grep \"Cross section (pb):\" tag_1_pythia.log > xsec";
      system(ssgrep.str().c_str());

      // Apply cuts.  Pass parameters as explicit arguments rather than using sed
      stringstream sscut;
      sscut << "cd " << processname << "/Events; " << "../../squadron_batch_files/squadron_cuts_delphes "
            << ssrundir.str() << "/tag_1_delphes_events.root " << processname << " " << ssrundir.str();
      system(sscut.str().c_str());
      // combine results
      combine_results(ssresults.str(), sscombinedresults.str(), runnum, backnum);
    }
  }
  ifmadspin.close();
// Copy the results.txt file to the squadron directory for convenience
  system(sscopy.str().c_str());
  return 0;
}
