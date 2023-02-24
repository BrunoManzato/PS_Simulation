#include "MyPrimaryGenerator.h"

#include "G4Geantino.hh"

MyPrimaryGenerator::MyPrimaryGenerator() : 
	G4VUserPrimaryGeneratorAction(), 
	m_ParticleGun( new G4ParticleGun( G4Geantino::Definition() ) ) 
{

};

MyPrimaryGenerator::~MyPrimaryGenerator(){

	delete m_ParticleGun;
};

void MyPrimaryGenerator::GeneratePrimaries(G4Event* anEvent){

	int nP = 1; // number of primaries, you can change this number, bit it's better to change in the .mac file.
	for(int i=0; i<nP; i++) m_ParticleGun->GeneratePrimaryVertex(anEvent);

};