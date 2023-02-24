#include "run.h"

//Here we'll generate the output ROOT file and it's structure.
//Fell free to explore, add columms in the Ntuple or, in the line 37, change the output name. 

MyRunAction::MyRunAction()
{

    G4AnalysisManager *man = G4AnalysisManager::Instance();


    man->CreateNtuple("Hits_X","Hits_X");

    man->CreateNtupleIColumn("fEvent");

    man->CreateNtupleDColumn("fX");
    man->CreateNtupleDColumn("fY");
    man->CreateNtupleDColumn("fZ");
    man->CreateNtupleDColumn("Detector_Number_X");
    man->CreateNtupleDColumn("Time");
    man->CreateNtupleDColumn("Detector_Position_X");


    man->FinishNtuple(0);




    man->CreateNtuple("Hits_Y","Hits_Y");

    man->CreateNtupleIColumn("fEvent");

    man->CreateNtupleDColumn("fX");
    man->CreateNtupleDColumn("fY");
    man->CreateNtupleDColumn("fZ");
    man->CreateNtupleDColumn("Detector_Number_Y");
    man->CreateNtupleDColumn("Time");
    man->CreateNtupleDColumn("Detector_Position_Y");


    man->FinishNtuple(1);

}

MyRunAction::~MyRunAction(){}

void MyRunAction::BeginOfRunAction(const G4Run*run)
{

    G4int runNumber = run->GetRunID();

    std::stringstream strRunID;
    strRunID<<runNumber;

    G4AnalysisManager *man = G4AnalysisManager::Instance();

    man->OpenFile("./OUTPUT/output"+strRunID.str()+".root");

}

void MyRunAction::EndOfRunAction(const G4Run*)
{


    G4AnalysisManager *man = G4AnalysisManager::Instance();

    man->Write();
    man->CloseFile("./OUTPUT/output.root");


}
