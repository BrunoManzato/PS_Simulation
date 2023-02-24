#pragma once

#include "G4VUserActionInitialization.hh"

#include "run.h"

class MyActionInitialization : public G4VUserActionInitialization{
public:
    virtual void Build() const override;
};
