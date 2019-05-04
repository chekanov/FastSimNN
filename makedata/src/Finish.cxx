#include "Ana.h"

// Finish all calculations at the end   
Int_t Ana::Finish() { 


    cout << "\n\n-- Write output file=" << ffile << endl;
    cout << "\n\n";

    RootFile->Write();
    RootFile->Print();
    RootFile->Close();




   return 0;

}

