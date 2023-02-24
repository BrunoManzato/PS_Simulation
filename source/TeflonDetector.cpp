
/////////////////////////////////////////////

////////CONSTRUCTION OF OUR DETECTOR////////

////////////////////////////////////////////

#include "MyDetector.h"

#include "G4Box.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"

#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"

#include "G4VisAttributes.hh"

#include "MySensorDetector.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "G4MaterialPropertiesTable.hh"

#include "G4LogicalSkinSurface.hh"


#include "G4VUserDetectorConstruction.hh"
#include "G4SystemOfUnits.hh"
#include "G4Scintillation.hh"

#include "G4IntersectionSolid.hh"
#include "G4Tubs.hh"
#include "G4MultiUnion.hh"

#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"

#include "G4VisAttributes.hh"

using namespace CLHEP;


////////////////////////////////////
//Here we define the construction of our detector materials, geometry and Placement
//For a better and detailed explanation about each part of the code, 
//I strongly recommend to see the tutorials of Professor  Mustafa Schmidt at channel "Physics Matters":
// https://www.youtube.com/playlist?list=PLLybgCU6QCGWgzNYOV0SKen9vqg4KXeVL
//In other hand, you can always take a look at the GEANT4 documentation:
// https://geant4.web.cern.ch/support/user_documentation   -->All documentation
//https://geant4-userdoc.web.cern.ch/UsersGuides/ForApplicationDeveloper/BackupVersions/V10.7/html/index.html 
// (-->Aplication documentation

MyDetector::MyDetector(){
    fMessenger = new G4GenericMessenger(this, "/detector/","Detector Construction");
    fMessenger->DeclareProperty("FiberNumber",FiberNumber,"Number of Optical Fibers in Detector");
    fMessenger->DeclareProperty("Thickness",Scintwidth,"Thickness of Detector");
    fMessenger->DeclareProperty("Lenght",Scintlenght,"Lenght of Detector (It's always squared)");

    FiberNumber=40;
    Scintwidth=3.0*cm;
    Scintlenght=1.0*m;

}



MyDetector::~MyDetector(){}


G4VPhysicalVolume* MyDetector::Construct(){


//Add NIST material's table
   G4NistManager *nist= G4NistManager::Instance();

    const int N_DATA = 2;
    double photonenergies[N_DATA] = {1.65*eV, 3.10*eV};


//Here we define air properties and the World volume (where the simulation occurs)

//Vacuum

G4double atomicNumber = 1.;
G4double massOfMole = 1.008*g/mole;
G4double density = 1.e-25*g/cm3;
G4double temperature = 2.73*kelvin;
G4double pressure = 3.e-18*pascal;
G4Material* Vacuum =
new G4Material("interGalactic", atomicNumber,
massOfMole, density, kStateGas,
temperature, pressure);


//Air
    G4Material *worldMat = nist->FindOrBuildMaterial("G4_AIR");


    double AirRIndex[N_DATA]={1.0,1.0};

    G4MaterialPropertiesTable *AirPropertiesTable = new G4MaterialPropertiesTable();
    AirPropertiesTable->AddProperty("RINDEX",photonenergies,AirRIndex,N_DATA);


    worldMat->SetMaterialPropertiesTable(AirPropertiesTable);



//Teflon
    G4Material *Teflon = nist->FindOrBuildMaterial("G4_TEFLON");


    double TefRIndex[N_DATA]={1.0,1.0}; //We are just adding RIndex to Teflon because
                                        //GEANT4 will kill all photons if we do not add it

    G4MaterialPropertiesTable *TefPropertiesTable = new G4MaterialPropertiesTable();
    TefPropertiesTable->AddProperty("RINDEX",photonenergies,TefRIndex,N_DATA);


    Teflon->SetMaterialPropertiesTable(TefPropertiesTable);



//Real World Volume
    G4Box *BigsolidWorld = new G4Box("BigsolidWorld", 1.5*m, 0.5*m, 1.5*m);

    G4LogicalVolume *BiglogicWorld = new G4LogicalVolume(BigsolidWorld,Vacuum,"BiglogicWorld");

    G4VPhysicalVolume *BigWorld = new G4PVPlacement(0, {0,0,0}, BiglogicWorld, "BigWorld",0,false,0,true);


//Small World Volume (cover)
    G4Box *solidWorld = new G4Box("solidWorld", 0.5*m + 5*cm, 5.0*cm, 0.5*m + 5*cm);

    G4LogicalVolume *logicWorld = new G4LogicalVolume(solidWorld,Teflon,"logicWorld");

    G4VPhysicalVolume *physWorld = new G4PVPlacement(0, {0,0,0}, logicWorld, "physWorld",BiglogicWorld,false,0,true);





//Here we define the materials to the simulation:

//Materials

    //EJ-200

    G4Material *EJ200= new G4Material("EJ200",1.023*g/cm3,2);

    G4Element *C= nist->FindOrBuildElement("C");
    G4Element *H= nist->FindOrBuildElement("H");

    EJ200->AddElementByMassFraction(C,(4.69*12.)/(4.69*12. + 5.17*1.0));
    EJ200->AddElementByMassFraction(H,(5.17*1.0)/(4.69*12. + 5.17*1.0));

        //Physical quantities - EJ-200

    double EJ200RIndex[N_DATA]={1.58,1.58};
    double EJ200AbsLenght[N_DATA]={3.8*m,3.8*m};


    //Aproximated data took from Eljen plot of the catalog:
    double photonenergiesScintPlot[11]={3.1,3.03,3.0,2.95,2.92,2.85,2.77,2.70,2.66,2.59,2.48};
    double EJ200Scint[11]={0.0145,0.2029,0.4010,0.8841,1.0,0.8744,0.5845,0.4300,0.3768,0.1908,0.0556};


    G4MaterialPropertiesTable *EJ200PropertiesTable = new G4MaterialPropertiesTable();
    EJ200PropertiesTable->AddProperty("RINDEX",photonenergies,EJ200RIndex,N_DATA);

    EJ200PropertiesTable->AddProperty("ABSLENGTH",photonenergies,EJ200AbsLenght,N_DATA);

//Here you can use the option 10.0/MeV for SCINTILLATIONYIELD property 
//to visualize the scintillation process in OPENGL
//10000 photons/MeV will saturate the visualization and will not be plotted.
//P.S.: REMEMBER to use 10000.0/MeV option to run the proper simulation, it's very important
    //EJ200PropertiesTable->AddConstProperty("SCINTILLATIONYIELD",10000.0/MeV);
    EJ200PropertiesTable->AddConstProperty("SCINTILLATIONYIELD",10.0/MeV);

    EJ200PropertiesTable->AddConstProperty("YIELDRATIO",1.0);
    EJ200PropertiesTable->AddConstProperty("RESOLUTIONSCALE",1.0);

    EJ200PropertiesTable->AddConstProperty("SCINTILLATIONRISETIME1",0.9*ns);
    EJ200PropertiesTable->AddConstProperty("SCINTILLATIONTIMECONSTANT1",2.1*ns);

    EJ200PropertiesTable->AddProperty("SCINTILLATIONCOMPONENT1",photonenergiesScintPlot,EJ200Scint,11);


    EJ200PropertiesTable->AddProperty("FASTCOMPONENT",photonenergiesScintPlot,EJ200Scint,11);
    EJ200PropertiesTable->AddConstProperty("FASTTIMECONSTANT",0.9*ns);


    EJ200->SetMaterialPropertiesTable(EJ200PropertiesTable);



    //Polystyrene

    G4Material *PST= nist->FindOrBuildMaterial("G4_POLYSTYRENE");

    double PSTRIndex[N_DATA]={1.59,1.59};

    G4MaterialPropertiesTable *PSTPropertiesTable = new G4MaterialPropertiesTable();
    PSTPropertiesTable->AddProperty("RINDEX",photonenergies,PSTRIndex,N_DATA);

    //For PolyStyrene, we are using 70% of absorption lenght of EJ-200 as reazonable value
    double PSTAbsLenght[N_DATA]={0.7*3.8*m,0.7*3.8*m};

    PSTPropertiesTable->AddProperty("ABSLENGTH",photonenergies,PSTAbsLenght,N_DATA);

    PSTPropertiesTable->AddConstProperty("SCINTILLATIONYIELD",0.0/MeV);
    //Please comment the line above and uncomment the line bellow 
    //to use Polystyrene as scintillation material
 //   PSTPropertiesTable->AddConstProperty("SCINTILLATIONYIELD",12000.0/MeV);

    PSTPropertiesTable->AddConstProperty("YIELDRATIO",1.0);
    PSTPropertiesTable->AddConstProperty("RESOLUTIONSCALE",1.0);

    PSTPropertiesTable->AddConstProperty("SCINTILLATIONRISETIME1",0.9*ns);
    PSTPropertiesTable->AddConstProperty("SCINTILLATIONTIMECONSTANT1",2.1*ns);

    double PhotEnergiesFiber[3]={2.06*eV,2.6*eV,2.88*eV};
    double PSTScint[3]={0.1,1.0,0.1};
    PSTPropertiesTable->AddProperty("SCINTILLATIONCOMPONENT1",PhotEnergiesFiber,PSTScint,11);


    PSTPropertiesTable->AddProperty("FASTCOMPONENT",PhotEnergiesFiber,PSTScint,11);
    PSTPropertiesTable->AddConstProperty("FASTTIMECONSTANT",6.9*ns);


    PST->SetMaterialPropertiesTable(PSTPropertiesTable);

    //Acrylic->(C502H8)n


    G4Material *Acrylic= new G4Material("Acrylic",1.18*g/cm3,3);

    G4Element *O= nist->FindOrBuildElement("O");

    Acrylic->AddElement(C,5);
    Acrylic->AddElement(H,8);
    Acrylic->AddElement(O,2);

    double AcrylicRIndex[N_DATA]={1.495,1.495};

    G4MaterialPropertiesTable *AcrylicPropertiesTable = new G4MaterialPropertiesTable();
    AcrylicPropertiesTable->AddProperty("RINDEX",photonenergies,AcrylicRIndex,N_DATA);
    Acrylic->SetMaterialPropertiesTable(AcrylicPropertiesTable);



    //Fluorinated

    //Fluorinated polymer is a polymer constituted by C,H and F. 
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



//Here we'll define the detector geometry and materials
    //Detector

//Box body for scintillator(will generate a 1m x 1m x 3cm detector, you always can change those parameters) 
    //double Scintwidth=3.0*cm, Scintlenght = 1.0*m; 

    //FiberNumber is the number of fibers that will be added in the detector surface.
    //Fell free to use the many fibers you want. The code is made to adjust the detector automatically
    //int FiberNumber=40; 
    double gap;

    gap=Scintlenght/(FiberNumber);

    G4Box *BoxScint = new G4Box("BoxScint",gap/2.,(Scintwidth/2)/2.,Scintlenght/2.);

//Constructing the fiber layers (here, the fibers are cilynders):
//Fiber 
    double fiberradius=1*mm, fiberLenght=Scintlenght;
    G4Tubs*Fiber= new G4Tubs("Fiber",0,0.96*fiberradius/2.0,fiberLenght/2.0,0.0*degree,360.0*degree);

//Acrylic Cladding (2% of fiber radius)
        G4Tubs*AcrylicCladding= new G4Tubs("AcrylicCladding",0.96*fiberradius/2.0,0.98*fiberradius/2.0,fiberLenght/2.0,0.0*degree,360.0*degree);


//FP Cladding (2% of fiber radius, outisde cladding)
        G4Tubs*FPCladding= new G4Tubs("FPCladding",0.98*fiberradius/2.0,1.0*fiberradius/2.0,fiberLenght/2.0,0.0*degree,360.0*degree);



//To do grooves in the scintillator:

    auto *Groove = new G4Tubs("Groove",0,fiberradius/2.0,(fiberLenght/2.0)+1*cm,180.0*degree,180.0*degree);

    auto*SolidScintillator = new G4SubtractionSolid("SolidScintillator",BoxScint,Groove,0,{0,Scintwidth/4,0});



//Logical AND physical placement of detector
    G4LogicalVolume *LogicScintillator = new G4LogicalVolume(SolidScintillator,EJ200,"LogicScintillator");

    G4LogicalVolume *LogicFibers = new G4LogicalVolume(Fiber,PST,"LogicFibers");

    G4LogicalVolume *LogicAcrylicCladding = new G4LogicalVolume(AcrylicCladding,Acrylic,"LogicAcrylicCladding");

    G4LogicalVolume *LogicFPCladding = new G4LogicalVolume(FPCladding,FP,"LogicFPCladding");


    //Here we set the *inner* fiber in Blue to better visualization
    LogicFibers->SetVisAttributes(new G4VisAttributes(G4Color::Blue));


////////////////////Simple scint box
    auto *PlastScintFull = new G4Box("PlastScintFull",Scintlenght/2.,Scintwidth/2.,Scintlenght/2.);
    auto *LogicPlastScintFull = new G4LogicalVolume(PlastScintFull,EJ200,"LogicPlastScintFull");


//////////Air gap to photons reach fibers
    auto *airgap = new G4Box("airgap",fiberradius,fiberradius/2.,Scintlenght/2.);
    G4Tubs*GapFiber= new G4Tubs("GapFiber",0,fiberradius/2.0,fiberLenght/2.0,0.0*degree,360.0*degree);

    auto*FullAirGap = new G4SubtractionSolid("FullAirGap",airgap,GapFiber,0,{0.,0.,0.});

    auto *LogicAirGap = new G4LogicalVolume(FullAirGap,worldMat,"LogicAirGap");


/////////////////////////////Translate of Fibers to not touch the plastic
    double moveY = fiberradius/2.;

    G4VPhysicalVolume *PhysScintillatorFull = new G4PVPlacement(0,{0.,0.,0.},LogicPlastScintFull,"PhysScintillatorFull",logicWorld,false,0,true);;

    G4VPhysicalVolume *PhysAirGap[FiberNumber];

    for(int i=0;i<FiberNumber;i++){
    PhysAirGap[i] = new G4PVPlacement(0,{-Scintlenght/2+gap/2+(i*gap),Scintwidth/2. + moveY,0.},LogicAirGap,"PhysAirGap",logicWorld,false,0,true);
    }

    G4VPhysicalVolume *PhysicalFPCladding[FiberNumber];
    G4VPhysicalVolume *PhysicalAcrylicCladding[FiberNumber];
    G4VPhysicalVolume *PhysicalFiber[FiberNumber];

    for(int i=0;i<FiberNumber;i++){
    PhysicalAcrylicCladding[i] = new G4PVPlacement(0,{-Scintlenght/2+gap/2+(i*gap),Scintwidth/2 + moveY,0.},LogicAcrylicCladding,"PhysicalAcrylicCladding",logicWorld,false,0,true);

    PhysicalFPCladding[i]      = new G4PVPlacement(0,{-Scintlenght/2+gap/2+(i*gap),Scintwidth/2 + moveY,0.},LogicFPCladding,"PhysicalFPCladding",logicWorld,false,0,true);

    PhysicalFiber[i]           = new G4PVPlacement(0,{-Scintlenght/2+gap/2+(i*gap),Scintwidth/2 + moveY,0.},LogicFibers,"PhysicalFiber",logicWorld,false,0,true);

    }


/////////////////////////////Doing the Y direction



    G4VPhysicalVolume *PhysAirGapYDirection[FiberNumber];

    for(int i=0;i<FiberNumber;i++){
        G4Rotate3D rotY(180*degree,G4ThreeVector(0.,0.,1.));
        G4Rotate3D SecondrotrotY(90*degree,G4ThreeVector(0.,1.,0.));
        G4Translate3D transY(G4ThreeVector(-Scintlenght/2+gap/2+(i*gap),Scintwidth/2. + fiberradius/2.,0.));
//        G4Translate3D transY(G4ThreeVector(-Scintlenght/2+(i+1)*gap,5*Scintwidth,0.));

        G4Transform3D transformY = (SecondrotrotY*rotY)*(transY);


    PhysAirGapYDirection[i] = new G4PVPlacement(transformY,"PhysAirGapY",LogicAirGap,physWorld,0,0,true);
        //transformY,{-Scintlenght/2+(i+1)*gap,0.,0.},LogicScintillator,"PhysScintillator",logicWorld,false,0,true);
    }

    G4VPhysicalVolume *PhysicalFPCladdingY[FiberNumber];
    G4VPhysicalVolume *PhysicalAcrylicCladdingY[FiberNumber];
    G4VPhysicalVolume *PhysicalFiberY[FiberNumber];


    for(int i=0;i<FiberNumber;i++){
        G4Rotate3D rotFiberY(180*degree,G4ThreeVector(0.,0.,1.));
        G4Rotate3D SecondrotFiberY(90*degree,G4ThreeVector(0.,1.,0.));

        G4Translate3D transFiberY(G4ThreeVector(-Scintlenght/2+gap/2+(i*gap),Scintwidth/2 + moveY,0.));

        G4Transform3D transformFiberY = (SecondrotFiberY*rotFiberY)*(transFiberY);

    PhysicalAcrylicCladdingY[i] = new G4PVPlacement(transformFiberY,"PhysicalAcrylicCladdingY",LogicAcrylicCladding,physWorld,0,0,true);
//                                                 (rotFiberY,{-Scintlenght/2+(i+1)*gap,Scintwidth/2,0.},LogicAcrylicCladding,"PhysicalAcrylicCladding",logicWorld,false,0,true);

    PhysicalFPCladdingY[i] = new G4PVPlacement(transformFiberY,"PhysicalFPCladdingY",LogicFPCladding,physWorld,0,0,true);
//    PhysicalFPCladdingY[i] = new G4PVPlacement(rotFiberY,{-Scintlenght/2+(i+1)*gap,Scintwidth/2,0.},LogicFPCladding,"PhysicalFPCladding",logicWorld,false,0,true);

    PhysicalFiberY[i] = new G4PVPlacement(transformFiberY,"PhysicalFiberY",LogicFibers,physWorld,0,0,true);
//    PhysicalFiberY[i] = new G4PVPlacement(rotFiberY,{-Scintlenght/2+(i+1)*gap,Scintwidth/2,0.},LogicFibers,"PhysicalFiber",logicWorld,false,0,true);

    }



//////Surface to capture photons in Optical fiber

    auto *surfaceFiberAir= new G4OpticalSurface("Fiber->Air_Surface");
    surfaceFiberAir->SetModel(unified);
    surfaceFiberAir->SetType(dielectric_metal);
//    surfaceAirFPC->SetFinish(polishedtyvekair);
    surfaceFiberAir->SetFinish(polished);

    //double surfaceFiberAirReflectivity[N_DATA]={1.0,1.0};

    //auto surfaceFiberAirPropertiesTable = new G4MaterialPropertiesTable();
    //surfaceFiberAir->SetMaterialPropertiesTable(surfaceFiberAirPropertiesTable);

    //surfaceFiberAirPropertiesTable->AddProperty("REFLECTIVITY",photonenergies,surfaceFiberAirReflectivity,N_DATA);

    G4LogicalBorderSurface *logBorderFiberAir[FiberNumber];
    G4LogicalBorderSurface *logBorderFiberAirY[FiberNumber];


    for(int i=0;i<FiberNumber;i++){
    logBorderFiberAir[i]= new G4LogicalBorderSurface("Fiber->Air_interface",PhysicalFiber[i],PhysicalAcrylicCladding[i],surfaceFiberAir);
    }


    for(int i=0;i<FiberNumber;i++){
    logBorderFiberAirY[i]= new G4LogicalBorderSurface("YFiber->Air_interface",PhysicalFiberY[i],PhysicalAcrylicCladdingY[i],surfaceFiberAir);
    }




//Here we'll construct the most relevant surfaces in simulation
    //Surfaces

    //Materials: Acrylic, FP, PST, EJ200, Air
/*
    //Air->EJ200

    auto *surfaceAirEJ200= new G4OpticalSurface("Air->EJ200_Surface");
    surfaceAirEJ200->SetModel(unified);
    surfaceAirEJ200->SetType(dielectric_dielectric);

    //Here you can choose the better finish for the detector surface. 
    //Polishedtyvekair option seens more realist
//    surfaceAirEJ200->SetFinish(polishedtyvekair);
    surfaceAirEJ200->SetFinish(polished);

    double SurfAirEJ200Reflectivity[N_DATA]={0.0,0.0};
    double SurfAirEJ200Absorb[N_DATA]={1.0,1.0};

    G4MaterialPropertiesTable *surfaceAirEJ200PropertiesTable = new G4MaterialPropertiesTable();
    surfaceAirEJ200->SetMaterialPropertiesTable(surfaceAirEJ200PropertiesTable);

    //surfaceAirEJ200PropertiesTable->AddProperty("REFLECTIVITY",photonenergies,SurfAirEJ200Reflectivity,N_DATA);
    surfaceAirEJ200PropertiesTable->AddProperty("ABSORPTION",photonenergies,SurfAirEJ200Absorb,N_DATA);


    auto *logBorderAirEJ200 = new G4LogicalBorderSurface("Air->EJ200_interface",physWorld,PhysScintillator,surfaceAirEJ200);
    auto *logBorderEJ200Air = new G4LogicalBorderSurface("EJ200->Air_interface",PhysScintillator,physWorld,surfaceAirEJ200);



    //Air->Acrylic

    auto *surfaceAirAcrylic= new G4OpticalSurface("Air->Acrylic_Surface");
    surfaceAirAcrylic->SetModel(unified);
    surfaceAirAcrylic->SetType(dielectric_dielectric);
//    surfaceAirAcrylic->SetFinish(polishedtyvekair);
    surfaceAirAcrylic->SetFinish(polished);

    double surfaceAirAcrylicReflectivity[N_DATA]={0.8,0.8};

    G4MaterialPropertiesTable *surfaceAirAcrylicPropertiesTable = new G4MaterialPropertiesTable();
    surfaceAirAcrylic->SetMaterialPropertiesTable(surfaceAirAcrylicPropertiesTable);

    surfaceAirAcrylicPropertiesTable->AddProperty("REFLECTIVITY",photonenergies,surfaceAirAcrylicReflectivity,N_DATA);


    auto *logBorderAirAcrylic= new G4LogicalBorderSurface("Air->Acrylic_interface",physWorld,PhysicalAcrylicCladding,surfaceAirAcrylic);
    auto *logBorderAcrylicAir = new G4LogicalBorderSurface("Acrylic->Air_interface",PhysicalAcrylicCladding,physWorld,surfaceAirAcrylic);




    //Air->PST

    auto *surfaceAirPST= new G4OpticalSurface("Air->PST_Surface");
    surfaceAirPST->SetModel(unified);
    surfaceAirPST->SetType(dielectric_dielectric);
//    surfaceAirPST->SetFinish(polishedtyvekair);
    surfaceAirPST->SetFinish(polished);

    double SurfAirPSTReflectivity[N_DATA]={0.8,0.8};

    G4MaterialPropertiesTable *surfaceAirPSTPropertiesTable = new G4MaterialPropertiesTable();
    surfaceAirPST->SetMaterialPropertiesTable(surfaceAirPSTPropertiesTable);

    surfaceAirPSTPropertiesTable->AddProperty("REFLECTIVITY",photonenergies,SurfAirPSTReflectivity,N_DATA);


    auto *logBorderAirPST = new G4LogicalBorderSurface("Air->PST_interface",physWorld,PhysicalFiber,surfaceAirPST);
    auto *logBorderPSTAir = new G4LogicalBorderSurface("PST->Air_interface",PhysicalFiber,physWorld,surfaceAirPST);




/*
    //PST->Acrylic

    auto *surfacePSTAcrylic= new G4OpticalSurface("Air->EJ200_Surface");
    surfacePSTAcrylic->SetModel(unified);
    surfacePSTAcrylic->SetType(dielectric_dielectric);
 //   surfacePSTAcrylic->SetFinish(polishedtyvekair);
    surfacePSTAcrylic->SetFinish(polished);

    double surfacePSTAcrylicReflectivity[N_DATA]={0.99,0.99};

    G4MaterialPropertiesTable *surfacePSTAcrylicPropertiesTable = new G4MaterialPropertiesTable();
    surfacePSTAcrylic->SetMaterialPropertiesTable(surfacePSTAcrylicPropertiesTable);

    surfacePSTAcrylicPropertiesTable->AddProperty("REFLECTIVITY",photonenergies,surfacePSTAcrylicReflectivity,N_DATA);


    auto *logBorderPSTAcrylic = new G4LogicalBorderSurface("PST->Acrylic_interface",PhysicalFiber,PhysicalAcrylicCladding,surfacePSTAcrylic);
    auto *logBorderAcrylicPST= new G4LogicalBorderSurface("Acrylic->PST_interface",PhysicalAcrylicCladding,PhysicalFiber,surfacePSTAcrylic);

*/

//Reflective surface to cover the detector
//Simulates a thin Aluminium foil covering the detector.
//The ground finish allows that some photons can go outside the detector, 
//even that it's less than 1% of the photons that come out.

    double MirrorReflectivity[N_DATA] ={1.0,1.0}; 

    G4OpticalSurface *mirrorSurface = new G4OpticalSurface("mirrorSurface");

    mirrorSurface->SetType(dielectric_metal);
    mirrorSurface->SetFinish(groundteflonair);
    mirrorSurface->SetModel(unified);

    G4MaterialPropertiesTable *mptMirror = new G4MaterialPropertiesTable();

    mptMirror->AddProperty("RELECTIVITY",photonenergies,MirrorReflectivity,N_DATA);

    mirrorSurface->SetMaterialPropertiesTable(mptMirror);

    G4LogicalSkinSurface *skin = new G4LogicalSkinSurface("skin",logicWorld,mirrorSurface);



/////////To keep photons inside fibers

    auto *CladingKeep= new G4OpticalSurface("Cladding_Surface");
    CladingKeep->SetModel(unified);
    CladingKeep->SetType(dielectric_metal);
//    surfaceAirAcrylic->SetFinish(polishedtyvekair);
    CladingKeep->SetFinish(polished);

    //double CladingKeepReflectivity[N_DATA]={1.0,1.0};

    //G4MaterialPropertiesTable *CladingKeepPropertiesTable = new G4MaterialPropertiesTable();
    //CladingKeep->SetMaterialPropertiesTable(CladingKeepPropertiesTable);

    //CladingKeepPropertiesTable->AddProperty("REFLECTIVITY",photonenergies,CladingKeepReflectivity,N_DATA);


    G4LogicalBorderSurface *logBorderCladingKeepinside[FiberNumber];
    G4LogicalBorderSurface *logBorderCladingKeepOutside[FiberNumber];

    
    for(int i=0;i<FiberNumber;i++){
        logBorderCladingKeepinside[i] = new G4LogicalBorderSurface("CladingKeep_interface",PhysAirGap[i],PhysicalFiber[i],CladingKeep);
        logBorderCladingKeepOutside[i] = new G4LogicalBorderSurface("CladingKeepOUT_interface",PhysAirGap[i],PhysicalAcrylicCladding[i],CladingKeep);

    }
    //auto *logBorderAcrylicAir = new G4LogicalBorderSurface("Acrylic->Air_interface",PhysicalAcrylicCladding,physWorld,surfaceAirAcrylic);



//Here we'll construct our sensitive detector material, geometry and placement:

    //Sensitive Detector

//Sensitive material
//P.S.: In fact, the material of the sensitive detector is not relevant here, since it's a PMT.

    G4Material *StSteel = nist->FindOrBuildMaterial("G4_F");


    double StSteelRIndex[N_DATA]={1.0,1.0};

    G4MaterialPropertiesTable *StSteelPropertiesTable = new G4MaterialPropertiesTable();
    StSteelPropertiesTable->AddProperty("RINDEX",photonenergies,StSteelRIndex,N_DATA);

    double StSteelAbsLenght[N_DATA]={1*nm,1*nm};//Just for future uses, where we can see the 
                                                //energy deposited by each photon.
    StSteelPropertiesTable->AddProperty("ABSLENGHT",photonenergies,StSteelAbsLenght,N_DATA);

    StSteel->SetMaterialPropertiesTable(StSteelPropertiesTable);

    //Sensitive volume geometry construction

    double SiPMSize=0.05*cm;
    G4Tubs *SiPM = new G4Tubs("SiPM",0,fiberradius/2.0,SiPMSize,360.0*degree,360.0*degree);

    auto* logicDetector = new G4LogicalVolume(SiPM,StSteel,"logicDetector");

    G4VPhysicalVolume* PhysSiPM;

    for(int i=0;i<FiberNumber;i++){

    PhysSiPM = new G4PVPlacement(0,{-Scintlenght/2+gap/2+i*gap,Scintwidth/2 + moveY,fiberLenght/2.0+SiPMSize},logicDetector,"PhysSiPM",logicWorld,false,i,true);

    }

    //Here we make the SiPM's Red to better visualization.
    logicDetector->SetVisAttributes( new G4VisAttributes( G4Color::Red() ) );


    //Atributing the geometry as a sensor
    logicDetector->SetSensitiveDetector( new MySensorDetector("PhotonCounter") );



////////////Y-axis detectors


    G4VPhysicalVolume* PhysSiPMY;

    for(int i=0;i<FiberNumber;i++){

        G4Rotate3D rotFiberY(180*degree,G4ThreeVector(0.,0.,1.));
        G4Rotate3D SecondrotFiberY(90*degree,G4ThreeVector(0.,1.,0.));

        G4Translate3D transFiberY(G4ThreeVector(-Scintlenght/2+gap/2+i*gap,Scintwidth/2 + moveY,fiberLenght/2.0+SiPMSize));

        G4Transform3D transformFiberY = (SecondrotFiberY*rotFiberY)*(transFiberY);

    PhysSiPMY = new G4PVPlacement(transformFiberY,"PhysSiPMY",logicDetector,physWorld,false,FiberNumber+i,true);


    }


    return BigWorld;
    //return physWorld;


}
