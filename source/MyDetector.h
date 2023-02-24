#pragma once

// The Detector
#include "G4VUserDetectorConstruction.hh"

#include "G4GenericMessenger.hh"

class MyDetector : public G4VUserDetectorConstruction {

public:
    MyDetector();
    ~MyDetector();
    virtual G4VPhysicalVolume* Construct() override;


private:


    G4GenericMessenger *fMessenger;
    G4int FiberNumber;
    G4double Scintwidth,Scintlenght;
};


