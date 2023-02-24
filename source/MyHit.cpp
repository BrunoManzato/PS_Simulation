#include "MyHit.h"
#include "G4UnitsTable.hh"
#include "G4RunManager.hh"


//Here we get the data and store in the ROOT file.
//Fell free to add any data you want in the output root file, as like in the examples.
//P.S.: It's a good idea to try to get the data from the options of the aStep object.

MyHit::MyHit(G4Step*aStep) : 
    m_TrackID( aStep->GetTrack()->GetTrackID() ),
    m_PID(  aStep->GetTrack()->GetParticleDefinition()->GetParticleName() ),
    m_EnDep( aStep->GetTotalEnergyDeposit() ),
    m_Position( aStep->GetPreStepPoint()->GetPosition() ),
    m_Direction( aStep->GetPreStepPoint()->GetMomentumDirection() )
{



    const G4VTouchable *touchable = aStep->GetPreStepPoint()->GetTouchable();
    G4int copyNo = touchable->GetCopyNumber();

    G4AnalysisManager *man= G4AnalysisManager::Instance();

    G4int evt = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();

    G4double Itime = aStep->GetPreStepPoint()->GetGlobalTime();


    G4VPhysicalVolume *physvol = touchable->GetVolume();

    G4ThreeVector posDetector = physvol->GetTranslation();

if(m_Position[2]==500 && G4UniformRand()<0.05){

    //man->FillNtupleIColumn(0,m_TrackID);    
    man->FillNtupleIColumn(0,0,evt);
    man->FillNtupleDColumn(0,1,m_Position[0]);
    man->FillNtupleDColumn(0,2,m_Position[1]);
    man->FillNtupleDColumn(0,3,m_Position[2]);
    man->FillNtupleDColumn(0,4,copyNo);
    man->FillNtupleDColumn(0,5,Itime);
    man->FillNtupleDColumn(0,6,posDetector[0]);

    man->AddNtupleRow(0);
}



if(m_Position[2]!=500 && G4UniformRand()<0.05){


    //man->FillNtupleIColumn(0,m_TrackID);    
    man->FillNtupleIColumn(1,0,evt);
    man->FillNtupleDColumn(1,1,m_Position[0]);
    man->FillNtupleDColumn(1,2,m_Position[1]);
    man->FillNtupleDColumn(1,3,m_Position[2]);
    man->FillNtupleDColumn(1,4,copyNo);
    man->FillNtupleDColumn(1,5,Itime);
    man->FillNtupleDColumn(1,6,posDetector[2]);

    man->AddNtupleRow(1);
}


}

void MyHit::Print(){
/*
    G4cout 
    << "TrackID: " <<   m_TrackID 
    << " PID: "    <<   m_PID
    //<< " EnDep: "  <<   std::setw(7) << G4BestUnit( m_EnDep, "Energy" )
    << " Position X: " << std::setw(7) << G4BestUnit( m_Position[0], "Length" )
    << " Position Y: " << std::setw(7) << G4BestUnit( m_Position[1], "Length" )
    << " Position Z: " << std::setw(7) << G4BestUnit( m_Position[2], "Length" )

    << " Direction: " << std::setw(7) << m_Direction
    << G4endl;
    */

    
}
