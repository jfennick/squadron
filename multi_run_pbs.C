#include <fstream>
#include <iomanip>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <cstring>
#include <string>
#include <vector>
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
//double Txsecnc[2*nc]={0}, Txsecwc[2*nc]={0}, TNumEveNC[2*nc]={0}, TNumEveWC[2*nc]={0}, TEff[2*nc]={0}, TJets[2*nc][maxJets]={{0}};
struct Arrs {double Txsecnc[2*nc], Txsecwc[2*nc], TNumEveNC[2*nc], TNumEveWC[2*nc], TEff[2*nc], TJets[2*nc][maxJets];};
const int num_backs = 9;
double back_vals[nc][num_backs]={{0}};
//Global variables are bad ... ... oh well!

void combine_results(Arrs *a, string strresults, string strcombinedresults, int runnum, int backnum) {
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
    a->Txsecnc[i] += xsecnc; a->TNumEveNC[i] += NumEveNC; a->TNumEveWC[i] += NumEveWC;
    double Axsecnc = a->Txsecnc[i] / runnum;
    double Teff;
    if (a->TNumEveNC[i]==0) {Teff=0;} else {Teff = a->TNumEveWC[i] / a->TNumEveNC[i];}
    double Txecwc = Axsecnc * Teff;
    cout << cutname << "\t" << setw(16) << left << Axsecnc << setw(16) << left << Txecwc << setw(16) << left << a->TNumEveNC[i] << a->TNumEveWC[i] << "\t" << setw(16) << left << Teff;
    ofresults << cutname << "\t" << setw(16) << left << Axsecnc << setw(16) << left << Txecwc << setw(16) << left << a->TNumEveNC[i] << a->TNumEveWC[i] << "\t" << setw(16) << left << Teff;
    for (int j=0; j<9; j++) {
      ifresults >> Jet;
      a->TJets[i][j] += Jet;
      cout << setw(5) << left << a->TJets[i][j];//keep the width as small as possible to prevent line-wrapping
      ofresults << a->TJets[i][j] << "\t";
    }
    for (int j=9; j<maxJets; j++) {
      ifresults >> Jet;
      a->TJets[i][j] += Jet;
      cout << a->TJets[i][j] << " ";
      ofresults << a->TJets[i][j] << " ";
    }
    cout << endl;
    ofresults << endl;
  }
  ifresults.close();

  double Tratio=0, nonzero=0, DCxsec=0;
  for (int i=0; i<nc; i++) {
    // now combine the regular and tau-tagged totals, and write out the results and ratio of our results to the values in arxiv 1109.6014
    double Axsecnc = a->Txsecnc[i] / runnum; //same as before
    double Teff;
    if ((a->TNumEveNC[i]+a->TNumEveNC[i+nc])==0) {Teff=0;} else {Teff = (a->TNumEveWC[i]+a->TNumEveWC[i+nc]) / a->TNumEveNC[i];}
    double Txecwc = Axsecnc * Teff;
    DCxsec += Txecwc;
    double ratio;
    if (back_vals[i][backnum]==0) {ratio=0;} else {ratio=Txecwc/back_vals[i][backnum];}
    if (ratio != 0) { Tratio += ratio; nonzero++; }
    cout << "ratio->\t" << setw(16) << left << ratio << setw(16) << left << Txecwc << setw(16) << left << a->TNumEveNC[i] << (a->TNumEveWC[i]+a->TNumEveWC[i+nc]) << "\t" << setw(16) << left << Teff;
    ofresults << "ratio->\t" << setw(16) << left << ratio << setw(16) << left << Txecwc << setw(16) << left << a->TNumEveNC[i] << (a->TNumEveWC[i]+a->TNumEveWC[i+nc]) << "\t" << setw(16) << left << Teff;
    for (int j=0; j<9; j++) {
      cout << setw(5) << left << a->TJets[i][j] + a->TJets[i+nc][j];//keep the width as small as possible to prevent line-wrapping
      ofresults << a->TJets[i][j] + a->TJets[i+nc][j] << "\t";
    }
    for (int j=9; j<maxJets; j++) {
      cout << a->TJets[i][j] + a->TJets[i+nc][j] << " ";
      ofresults << a->TJets[i][j] + a->TJets[i+nc][j] << " ";
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
  if (argc != 9) {
    cout << "arg1: process name (main directory), arg2: run name, arg3: number of runs, arg4: background number (0 for n/a)\n";
    cout << "arg5: recut (1 or 0), arg6: xqcutmin, arg7: xqcutmax, arg8: xqcutstep\n";
    return 1;}
  char* processname = argv[1];
  char* runbasename = argv[2];
  int NumRuns = atoi(argv[3]);
  int backnum = atoi(argv[4]);
  int recut = atoi(argv[5]);
  int xqcutmin = atoi(argv[6]);
  int xqcutmax = atoi(argv[7]);
  int xqcutstep = atoi(argv[8]);

  ifstream ifbackvals("background_values");
  string headers;
  getline(ifbackvals, headers);
  for (int i=0; i<nc; i++) {
    for (int j=0; j<num_backs; j++) {
      ifbackvals >> back_vals[i][j];
    }
  }
  ifbackvals.close();

  stringstream ssmadspin;
  ssmadspin << processname << "/Cards/madspin_card.dat";
  ifstream ifmadspin(ssmadspin.str().c_str());

  vector<string> mejobids;// store all previous mejobids so we can depend on them
  // loop over xqcut
  for (int xqcut=xqcutmin; xqcut <= xqcutmax; xqcut += xqcutstep) {
  stringstream ssrunbasename, ssrunbasenamemlm, ssrunbasenameskt;
  ssrunbasename << runbasename << "_xqcut" << xqcut;
  ssrunbasenamemlm << ssrunbasename.str() << "_mlm";
  ssrunbasenameskt << ssrunbasename.str() << "_skt";
  stringstream sscombinedresultsmlm, sscopymlm;
  stringstream sscombinedresultsskt, sscopyskt;
  sscombinedresultsmlm << processname << "/Events/" << ssrunbasenamemlm.str() << "_results.txt";
  sscombinedresultsskt << processname << "/Events/" << ssrunbasenameskt.str() << "_results.txt";
  sscopymlm << "if [ -e " << sscombinedresultsmlm.str() << " ]; then" << endl;
  sscopymlm << "cp " << sscombinedresultsmlm.str() << " .;" << endl;
  sscopymlm << "fi" << endl;
  sscopyskt << "if [ -e " << sscombinedresultsskt.str() << " ]; then" << endl;
  sscopyskt << "cp " << sscombinedresultsskt.str() << " .;" << endl;
  sscopyskt << "fi" << endl;
  int finishedrunsmlm = 0;int finishedrunsskt = 0;
  //initialize array variables
  Arrs mlm, skt;
  for (int i=0; i<2*nc;i++) {
    mlm.Txsecnc[i]=0; mlm.Txsecwc[i]=0; mlm.TNumEveNC[i]=0; mlm.TNumEveWC[i]=0; mlm.TEff[i]=0;
    skt.Txsecnc[i]=0; skt.Txsecwc[i]=0; skt.TNumEveNC[i]=0; skt.TNumEveWC[i]=0; skt.TEff[i]=0;
    for (int j=0; j<maxJets; j++) {mlm.TJets[i][j]=0; skt.TJets[i][j]=0;}
  }
  // loop over number of 100k event runs
  for (int runnum=1; runnum<=NumRuns; runnum++) {
    stringstream ssrunname;
    stringstream ssrunnamemlm, ssresultsmlm;
    stringstream ssrunnameskt, ssresultsskt;
    ssrunnamemlm << ssrunbasenamemlm.str() << "_" << runnum;
    ssrunnameskt << ssrunbasenameskt.str() << "_" << runnum;
    ssrunname << ssrunbasename.str() << "_" << runnum;
    if (!ifmadspin.fail()) {ssrunname << "_decayed_1";}//accomodate madspin's convoluted directory structure
    stringstream sseventssubdir;
    sseventssubdir << processname << "/Events/" << ssrunname.str();
    ssresultsmlm << processname << "/Events/" << ssrunnamemlm.str() << "/" << ssrunnamemlm.str() << "_results.txt";
    ssresultsskt << processname << "/Events/" << ssrunnameskt.str() << "/" << ssrunnameskt.str() << "_results.txt";
    
    struct stat sb; // check if the events subdirectory exists
    if (stat(sseventssubdir.str().c_str(), &sb) == 0 && S_ISDIR(sb.st_mode)) {
      // mlm
      ifstream ifresultsmlm(ssresultsmlm.str().c_str());
      if (ifresultsmlm.fail()) {
        ifresultsmlm.close();
        cout << "Error! " << ssresultsmlm.str() << " does not exist. " << endl;
        cout << "Calculation might have interrupted and directory not removed." << endl;
        cout << "Delete " << sseventssubdir.str() << " to rerun." << endl;
        return 1;
      } else {
        ifresultsmlm.close();
        cout << ssresultsmlm.str() << " exists. Combining events." << endl;
        if (recut) {
          // re-cut events (i.e. in case cut_parameters file has been modified).
          // Pass parameters as explicit arguments rather than using sed
          stringstream sscut;
          sscut << "cd " << processname << "/Events/" << ssrunnamemlm.str()
                << "; ../../../squadron_batch_files/squadron_cuts_delphes delphes_events.root " << ssrunnamemlm.str();
          system(sscut.str().c_str());
        }
        // combine results
        finishedrunsmlm++;
        combine_results(&mlm, ssresultsmlm.str(), sscombinedresultsmlm.str(), finishedrunsmlm, backnum);
      }
      // skt
      ifstream ifresultsskt(ssresultsskt.str().c_str());
      if (ifresultsskt.fail()) {
        ifresultsskt.close();
        cout << "Error! " << ssresultsskt.str() << " does not exist. " << endl;
        cout << "Calculation might have interrupted and directory not removed." << endl;
        cout << "Delete " << sseventssubdir.str() << " to rerun." << endl;
        return 1;
      } else {
        ifresultsskt.close();
        cout << ssresultsskt.str() << " exists. Combining events." << endl;
        if (recut) {
          // re-cut events (i.e. in case cut_parameters file has been modified).
          // Pass parameters as explicit arguments rather than using sed
          stringstream sscut;
          sscut << "cd " << processname << "/Events/" << ssrunnameskt.str()
                << "; ../../../squadron_batch_files/squadron_cuts_delphes delphes_events.root " << ssrunnameskt.str();
          system(sscut.str().c_str());
        }
        // combine results
        finishedrunsskt++;
        combine_results(&skt, ssresultsskt.str(), sscombinedresultsskt.str(), finishedrunsskt, backnum);
      }
    } else {
      cout << sseventssubdir.str() << " does not exist. Calculating events." << endl;
      /* MadGraph is multithreaded, but pythia, pgs/delphes, and our cuts are not.
         This means that if we allow MadGraph to do the default madevent > pythia
         > pgs/delphes chain, we will have 7 cores sitting idle for ~30% of the time.
         Solution: use torque/pbs to generate events using all 8 cores and schedule
         the remaining work to be done afterwards on one core.  Then pbs can start
         generating events for the next multi_run simultaneously with the follow
         up analysis chain.
      */
      stringstream ssmejobids;
      for (int i=0; i<mejobids.size(); i++) {
        ssmejobids << ":" << mejobids[i];
      }
      stringstream ssmadevent; // generate events
      ssmadevent << "#!/bin/bash" << endl
                 << "#PBS -l nodes=1:ppn=8" << endl
                 << "#PBS -l nice=19" << endl // low priority
                 << "#PBS -j oe" << endl
                 << "#PBS -m n" << endl
                 << "#PBS -N " << ssrunbasename.str() << "_me" << "_" << runnum << endl
                 << "#PBS -S /bin/bash" << endl //not necessary if torque configured correctly...
                 // tell torque/pbs to wait until all previous madevent runs finish okay
                 << "#PBS -W depend=afterok" << ssmejobids.str() << endl << endl
                 << "source ~/.bashrc;" << endl
                 << "cd ~/MG5_aMC_v2_1_2/squadron/" << processname << ";" << endl
                 << "sed -e \"258 s/ 0   = xqcut/ " << xqcut << "   = xqcut/g\" Cards/runtemp > Cards/run_card.dat;" << endl
                 /* NOTE: After generating events for a run, the only new files in the Events     // but it temporarily pollutes Events directory
                    directory are in the *subdirectory* for the run.  It is only when running
                    pythia, etc that the Events directory becomes polluted with files.  The
                    reason the RunWeb lock file exists is to prevent these files from getting
                    overwritten. Rather than try to remove it before each run, modify the if statement to
                    always return false near line 399 of ./bin/internal/common_run_interface.py
                    More importantly, if we modify line 126 of pythia.f like so:
                    sed -e "s|\.\./Cards/pythia_card.dat|pythia_card.dat|g"
                    then we can run several instances of pythia within each event subdirectory
                    in parallel without overwriting files and without worrying about locking.
                    We also need to use run_mg5 to make the same change in ./bin/internal/run_pythia
                 */
                 << "./bin/generate_events -f " << ssrunbasename.str() << "_" << runnum << ";" << endl
                 << "cd Events/" << ssrunbasename.str() << "_" << runnum << ";" << endl // cd ..; cd sseventssubdir.str()
                 << "rm events.lhe.gz;" << endl
                 << "gunzip -f unweighted_events.lhe.gz;" << endl;
      stringstream ssfilename;
      ssfilename << "qsubme_" << ssrunbasename.str() << "_" << runnum;
      ofstream ofqsub(ssfilename.str().c_str());
      ofqsub << ssmadevent.str();
      ofqsub.close();
      stringstream ssqsubin;
      ssqsubin << "mkdir " << sseventssubdir.str() << ";" << endl;
      ssqsubin << "qsub " << ssfilename.str() << " > qsubjobid;" << endl;
      ssqsubin << "mv " << ssfilename.str() << " " << sseventssubdir.str() << ";" << endl;
      system(ssqsubin.str().c_str());

      ifstream ifjobid1("qsubjobid");
      string mejobid;
      ifjobid1 >> mejobid;//get the mejobid so we can wait
      ifjobid1.close();
      mejobids.push_back(mejobid);// store all previous mejobids

      // mlm
      stringstream ssmlm;
      ssmlm << "#!/bin/bash" << endl
            << "#PBS -l nodes=1:ppn=1" << endl
            << "#PBS -j oe" << endl
            << "#PBS -m n" << endl
            << "#PBS -N " << ssrunnamemlm.str() << endl
            << "#PBS -S /bin/bash" << endl 
            // tell torque/pbs to wait until madevent finishes okay
            << "#PBS -W depend=afterok:" << mejobid << endl << endl
            << "source ~/.bashrc;" << endl
            // symlink to the shared events subdirectory
            << "cd ~/MG5_aMC_v2_1_2/squadron/" << processname << "/Events/" << ssrunnamemlm.str() << ";" << endl
            << "ln -s ../" << ssrunname.str() << "/unweighted_events.lhe unweighted_events.lhe;" << endl
            << "cp ../../Cards/pythia_card_default.dat pythia_card.dat;" << endl
            << "cp ../../Cards/delphes_card.dat .;" << endl
            // run pythia
            << "../../bin/internal/run_pythia ../../../../pythia-pgs/src > pythia.log" << endl
            // Get the cross section from pythia's log because when doing jet matching,
            // the cross section after matching is NOT stored in SubProcesses/runname_results.dat
            << "grep \"Cross section (pb):\" pythia.log > xsec;" << endl
            // run delphes
            << "../../../../Delphes/DelphesSTDHEP delphes_card.dat delphes_events.root pythia_events.hep > delphes.log;" << endl
            // remove files
            << "../../../remove_files;" << endl
            // Apply cuts.  Pass parameters as explicit arguments rather than using sed
            << "../../../squadron_batch_files/squadron_cuts_delphes delphes_events.root " << ssrunnamemlm.str() << ";" << endl;

      stringstream ssfilenamemlm;
      ssfilenamemlm << "qsubmlm_" << ssrunname.str();
      ofstream ofqsubmlm(ssfilenamemlm.str().c_str());
      ofqsubmlm << ssmlm.str();
      ofqsubmlm.close();
      stringstream ssqsubmlm;
      ssqsubmlm << "mkdir " << processname << "/Events/" << ssrunnamemlm.str() << ";" << endl;
      ssqsubmlm << "qsub " << ssfilenamemlm.str() << " > qsubjobid;" << endl;
      ssqsubmlm << "mv " << ssfilenamemlm.str() << " " << processname << "/Events/" << ssrunnamemlm.str() << ";" << endl;
      system(ssqsubmlm.str().c_str());

      ifstream ifjobidmlm("qsubjobid");
      string mlmjobid;
      ifjobidmlm >> mlmjobid;//get the mlmjobid
      ifjobidmlm.close();

      // skt
      stringstream ssskt;
      ssskt << "#!/bin/bash" << endl
            << "#PBS -l nodes=1:ppn=1" << endl
            << "#PBS -j oe" << endl
            << "#PBS -m n" << endl
            << "#PBS -N " << ssrunnameskt.str() << endl
            << "#PBS -S /bin/bash" << endl 
            // tell torque/pbs to wait until madevent finishes okay
            << "#PBS -W depend=afterok:" << mejobid << ":" << mlmjobid << endl << endl
//temporarily depend on mlmjobid to reduce massive simultaneous IO. Remove dependency if/when using solid state disk
            << "source ~/.bashrc;" << endl
            // symlink to the shared events subdirectory
            << "cd ~/MG5_aMC_v2_1_2/squadron/" << processname << "/Events/" << ssrunnameskt.str() << ";" << endl
            << "ln -s ../" << ssrunname.str() << "/unweighted_events.lhe unweighted_events.lhe;" << endl
            << "cp ../../Cards/pythia_card_default.dat pythia_card.dat;" << endl
            << "cp ../../Cards/delphes_card.dat .;" << endl
            // turn on skt in pythia_card.dat
            << "echo \"showerkt=T\" >> pythia_card.dat;" << endl
            // run pythia
            << "../../bin/internal/run_pythia ../../../../pythia-pgs/src > pythia.log;" << endl
            // Get the cross section from pythia's log because when doing jet matching,
            // the cross section after matching is NOT stored in SubProcesses/runname_results.dat
            << "grep \"Cross section (pb):\" pythia.log > xsec;" << endl
            // run delphes
            << "../../../../Delphes/DelphesSTDHEP delphes_card.dat delphes_events.root pythia_events.hep > delphes.log;" << endl
            // remove files
            << "../../../remove_files;" << endl
            // Apply cuts.  Pass parameters as explicit arguments rather than using sed
            << "../../../squadron_batch_files/squadron_cuts_delphes delphes_events.root " << ssrunnameskt.str() << ";" << endl;

      stringstream ssfilenameskt;
      ssfilenameskt << "qsubskt_" << ssrunname.str();
      ofstream ofqsubskt(ssfilenameskt.str().c_str());
      ofqsubskt << ssskt.str();
      ofqsubskt.close();
      stringstream ssqsuboutskt;
      ssqsuboutskt << "mkdir " << processname << "/Events/" << ssrunnameskt.str() << ";" << endl;
      ssqsuboutskt << "qsub " << ssfilenameskt.str() << " > qsubjobid;" << endl;
      ssqsuboutskt << "mv " << ssfilenameskt.str() << " " << processname << "/Events/" << ssrunnameskt.str() << ";" << endl;
      system(ssqsuboutskt.str().c_str());

      ifstream ifjobid2("qsubjobid");
      string sktjobid;
      ifjobid2 >> sktjobid;//get the sktjobid
      ifjobid2.close();

      // cleanup job to remove unweighted_events.lhe
      stringstream sslhe;
      sslhe << "#!/bin/bash" << endl
            << "#PBS -l nodes=1:ppn=1" << endl
            << "#PBS -j oe" << endl
            << "#PBS -m n" << endl
            << "#PBS -N " << ssrunbasename.str() << "_lhe" << "_" << runnum << endl
            << "#PBS -S /bin/bash" << endl 
            // tell torque/pbs to wait until mlm and skt finish okay
            << "#PBS -W depend=afterok:" << mlmjobid << ":" << sktjobid << endl << endl
            << "source ~/.bashrc;" << endl
            << "cd ~/MG5_aMC_v2_1_2/squadron/" << sseventssubdir.str() << ";" << endl
            << "rm unweighted_events.lhe;" << endl;

      stringstream ssfilenamelhe;
      ssfilenamelhe << "qsublhe_" << ssrunname.str();
      ofstream ofqsublhe(ssfilenamelhe.str().c_str());
      ofqsublhe << sslhe.str();
      ofqsublhe.close();
      stringstream ssqsuboutlhe;
      ssqsuboutlhe << "qsub " << ssfilenamelhe.str() << " > qsubjobid;" << endl;
      ssqsuboutlhe << "mv " << ssfilenamelhe.str() << " " << sseventssubdir.str() << ";" << endl;
      system(ssqsuboutlhe.str().c_str());
    }
  }
  // Copy the results.txt files to the squadron directory for convenience
  system(sscopymlm.str().c_str());
  system(sscopyskt.str().c_str());
  }
  ifmadspin.close();
  return 0;
}
