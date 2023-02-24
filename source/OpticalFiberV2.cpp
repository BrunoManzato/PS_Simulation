#include "MyDetector.h"

#include "G4Box.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"

#include "G4Tubs.hh"

#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4OpticalParameters.hh"

#include "G4VisAttributes.hh"

#include "MySensorDetector.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "G4MaterialPropertiesTable.hh"

#include "G4LogicalSkinSurface.hh"

using namespace CLHEP;

MyDetector::MyDetector(){}

MyDetector::~MyDetector(){}

G4VPhysicalVolume* MyDetector::Construct(){


    G4NistManager *nist= G4NistManager::Instance();

    const int N_DATA = 2;
    double photonenergies[N_DATA] = {1.65*eV, 3.10*eV};

//Air
    G4Material *worldMat = nist->FindOrBuildMaterial("G4_AIR");


    double AirRIndex[N_DATA]={1.0,1.0};

    G4MaterialPropertiesTable *AirPropertiesTable = new G4MaterialPropertiesTable();
    AirPropertiesTable->AddProperty("RINDEX",photonenergies,AirRIndex,N_DATA);

    worldMat->SetMaterialPropertiesTable(AirPropertiesTable);


//World Volume
    G4Box *solidWorld = new G4Box("solidWorld", 0.75*m, 0.3*m, 0.75*m);

    G4LogicalVolume *logicWorld = new G4LogicalVolume(solidWorld,worldMat,"logicWorld");

    G4VPhysicalVolume *physWorld = new G4PVPlacement(0, {0,0,0}, logicWorld, "physWorld",0,false,0,true);

//Materials

    G4Element *C= nist->FindOrBuildElement("C");
    G4Element *H= nist->FindOrBuildElement("H");


    //Polystyrene

    G4Material *PST= nist->FindOrBuildMaterial("G4_POLYSTYRENE");

    double PSTRIndex[N_DATA]={1.59,1.59};

    G4MaterialPropertiesTable *PSTPropertiesTable = new G4MaterialPropertiesTable();
    PSTPropertiesTable->AddProperty("RINDEX",photonenergies,PSTRIndex,N_DATA);

    double PSTAbsLenght[N_DATA]={4.4*m,4.4*m};

    PSTPropertiesTable->AddProperty("ABSLENGTH",photonenergies,PSTAbsLenght,N_DATA);

    //PSTPropertiesTable->AddConstProperty("SCINTILLATIONYIELD",12000.0/MeV);
    PSTPropertiesTable->AddConstProperty("SCINTILLATIONYIELD",120.0/MeV);

    PSTPropertiesTable->AddConstProperty("YIELDRATIO",1.0);
    PSTPropertiesTable->AddConstProperty("RESOLUTIONSCALE",1.0);

    PSTPropertiesTable->AddConstProperty("SCINTILLATIONRISETIME1",0.0*ns);
    //PSTPropertiesTable->AddConstProperty("SCINTILLATIONTIMECONSTANT1",0.0*ns);

    double PhotEnergiesFiber[3]={2.06*eV,2.6*eV,2.88*eV};
    double PSTScint[3]={0.1,1.0,0.1};
    PSTPropertiesTable->AddProperty("SCINTILLATIONCOMPONENT1",PhotEnergiesFiber,PSTScint,11);


    PSTPropertiesTable->AddProperty("FASTCOMPONENT",PhotEnergiesFiber,PSTScint,11);
    PSTPropertiesTable->AddConstProperty("FASTTIMECONSTANT",6.9*ns);


    PST->SetMaterialPropertiesTable(PSTPropertiesTable);

    //Acrylic->(C502H8)n


    G4Material *Acrylic= new G4Material("Acrylic",1.18*g/cm3,3);

    G4Element *O= nist->FindOrBuildElement("O");
/*
    Acrylic->AddElement(C,5);
    Acrylic->AddElement(H,8);
    Acrylic->AddElement(O,2);
*/


    Acrylic->AddElement(C,(36*6)/(36*6+57+14*16));
    Acrylic->AddElement(H,57/(36*6+57+14*16));
    Acrylic->AddElement(O,(14*16)/(36*6+57+14*16));

    double AcrylicRIndex[N_DATA]={1.495,1.495};

    G4MaterialPropertiesTable *AcrylicPropertiesTable = new G4MaterialPropertiesTable();
    AcrylicPropertiesTable->AddProperty("RINDEX",photonenergies,AcrylicRIndex,N_DATA);
    Acrylic->SetMaterialPropertiesTable(AcrylicPropertiesTable);


    //Fluorinated polymer (A polymer constituted by C,H and F. 
    //Here I'm using one generic CHF formula based on POLYVINYLIDENE_FLUORIDE from 
    //GEANT4 NIST table due to lack of description by the producer)
    //Obs.: The material itself is not so important as it's optical properties for us now.


    G4Material *FP= new G4Material("FP",1.43*g/cm3,3);

    G4Element *F= nist->FindOrBuildElement("F");

    FP->AddElement(C,0.375141);
    FP->AddElement(H,0.03148);
    FP->AddElement(F,0.593379);

    double FPRIndex[N_DATA]={1.42,1.42};

    G4MaterialPropertiesTable *FPPropertiesTable = new G4MaterialPropertiesTable();
    FPPropertiesTable->AddProperty("RINDEX",photonenergies,FPRIndex,N_DATA);
    FP->SetMaterialPropertiesTable(FPPropertiesTable);



    //Detector

//Fiber 
    double FiberLenght=1*m, FiberRadius = 1.0*mm; 

    //G4Tubs *Fiber = new G4Tubs("Fiber",0,FiberRadius/2,FiberLenght/2,360*degree,360*degree);

    G4Box *Fiber = new G4Box("Fiber",(0.96*FiberRadius)/2,(0.96*FiberRadius)/2,FiberLenght/2);

    G4LogicalVolume *LogicFiber = new G4LogicalVolume(Fiber,PST,"LogicScintillator");

    G4VPhysicalVolume *PhysFiber = new G4PVPlacement(0,{0.,0.,0.},LogicFiber,"PhysFiber",logicWorld,false,0,true);



//Fiber Acrylic cladding

    G4Box *AcrylicFiberCladding = new G4Box("AcrylicFiberCladding",(0.98*FiberRadius)/2,(0.98*FiberRadius)/2,FiberLenght/2);

    G4Box *AcrylicFiberHole = new G4Box("AcrylicFiberHole",(0.96*FiberRadius)/2,(0.96*FiberRadius)/2,FiberLenght/2);

    G4SubtractionSolid *AcrylicCladding = new G4SubtractionSolid("AcrylicCladding",AcrylicFiberCladding,AcrylicFiberHole);

    G4LogicalVolume *LogicAcrylicCladding = new G4LogicalVolume(AcrylicCladding,Acrylic,"LogicAcrylicCladding");

    G4VPhysicalVolume *PhysAcrylicCladding = new G4PVPlacement(0,{0.,0.,0.},LogicAcrylicCladding,"PhysAcrylicCladding",logicWorld,false,0,true);

    LogicAcrylicCladding->SetVisAttributes( new G4VisAttributes( G4Color::Green() ) );



//Fiber FP cladding

    G4Box *FPCoverCladding = new G4Box("FPCoverCladding",(1.0*FiberRadius)/2,(1.0*FiberRadius)/2,FiberLenght/2);

    G4Box *FPFiberHole = new G4Box("FPFiberHole",(0.98*FiberRadius)/2,(0.98*FiberRadius)/2,FiberLenght/2);

    G4SubtractionSolid *FPCladding = new G4SubtractionSolid("Cladding",FPCoverCladding,FPFiberHole);

    G4LogicalVolume *LogicFPCladding = new G4LogicalVolume(FPCladding,FP,"LogicFPCladding");

    G4VPhysicalVolume *PhysFPCladding = new G4PVPlacement(0,{0.,0.,0.},LogicFPCladding,"PhysFPCladding",logicWorld,false,0,true);
    
    LogicFPCladding->SetVisAttributes( new G4VisAttributes( G4Color::Blue() ) );




///OPTICAL SKIN SURFACE

/*
    double MirrorReflectivity[N_DATA] ={0.9,0.9}; 

    G4OpticalSurface *mirrorSurface = new G4OpticalSurface("mirrorSurface");

    mirrorSurface->SetType(dielectric_metal);
    mirrorSurface->SetFinish(ground);
    mirrorSurface->SetModel(unified);

    G4MaterialPropertiesTable *mptMirror = new G4MaterialPropertiesTable();

    mptMirror->AddProperty("RELECTIVITY",photonenergies,MirrorReflectivity,N_DATA);

    mirrorSurface->SetMaterialPropertiesTable(mptMirror);

    G4LogicalSkinSurface *skin = new G4LogicalSkinSurface("skin",LogicFiber,mirrorSurface);

*/


    auto *surfaceFiberAir= new G4OpticalSurface("Fiber->Air_Surface");
    surfaceFiberAir->SetModel(unified);
    surfaceFiberAir->SetType(dielectric_metal);
//    surfaceAirFPC->SetFinish(polishedtyvekair);
    surfaceFiberAir->SetFinish(polished);

    double surfaceFiberAirAbsorption[N_DATA]={.0,.0};

    auto surfaceFiberAirPropertiesTable = new G4MaterialPropertiesTable();
    surfaceFiberAir->SetMaterialPropertiesTable(surfaceFiberAirPropertiesTable);

    surfaceFiberAirPropertiesTable->AddProperty("ABSORPTION",photonenergies,surfaceFiberAirAbsorption,N_DATA);


    auto *logBorderFiberAir= new G4LogicalBorderSurface("Fiber->Air_interface",PhysFiber,PhysAcrylicCladding,surfaceFiberAir);
 
 ////////////
    //auto *surfaceAirFiber= new G4OpticalSurface("Air->Fiber_Surface");


    //auto *logBorderFPCAir= new G4LogicalBorderSurface("FPC->Air_interface",physWorld,PhysFiber,surfaceAirFiber);
 







    //Sensitive Detector

//Sensitive material


    G4Material *StSteel = nist->FindOrBuildMaterial("G4_F");


    double StSteelRIndex[N_DATA]={1.0,1.0};

    G4MaterialPropertiesTable *StSteelPropertiesTable = new G4MaterialPropertiesTable();
    StSteelPropertiesTable->AddProperty("RINDEX",photonenergies,StSteelRIndex,N_DATA);

    double StSteelAbsLenght[N_DATA]={1*nm,1*nm};
    StSteelPropertiesTable->AddProperty("ABSLENGHT",photonenergies,StSteelAbsLenght,N_DATA);

    StSteel->SetMaterialPropertiesTable(StSteelPropertiesTable);

    //Sensitive volume

    double SiPMSize=0.5*cm;
    auto *SiPM = new G4Tubs("SiPM",0,FiberRadius/2,SiPMSize/2,360*degree,360*degree);
    //auto *SiPM = new G4Tubs("SiPM",0,5*cm           ,SiPMSize,360.0*degree,360.0*degree);

    auto *logicDetector = new G4LogicalVolume(SiPM,StSteel,"logicDetector");

    G4VPhysicalVolume* PhysSiPM;

    PhysSiPM = new G4PVPlacement(0,{0,0,FiberLenght/2+SiPMSize/2},logicDetector,"PhysSiPM",logicWorld,false,0,true);


    logicDetector->SetVisAttributes( new G4VisAttributes( G4Color::Red() ) );

    logicDetector->SetSensitiveDetector( new MySensorDetector("PhotonCounter") );

    return physWorld;


}