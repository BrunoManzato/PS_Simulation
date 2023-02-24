#pragma once 
#include "G4VSensitiveDetector.hh"
#include "g4root.hh"
#include "G4RunManager.hh"

class MySensorDetector : public G4VSensitiveDetector{
public:

    MySensorDetector(const G4String &name);
    ~MySensorDetector();

    virtual G4bool ProcessHits(G4Step*aStep,G4TouchableHistory*ROhist) override;

};
