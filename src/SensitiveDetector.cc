// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// 	Author: wjcheon (Medical physics Lab, Sungkyunkwan University, Seoul, Republic of Korea)
//  GitHub: http://github.com/wjcheon
//


#include "SensitiveDetector.hh"

#include "G4SystemOfUnits.hh"
#include "G4Threading.hh"

// For Filename
#include <string.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <time.h>
#include <stdio.h>
#include <string>

using namespace std;

SensitiveDetector::SensitiveDetector(G4String name)
:G4VSensitiveDetector(name)
{

	//// Time
	time_t now;
	struct tm t;


	time(&now);
	t = *localtime(&now);
	G4int year = t.tm_year + 1900;
	G4int month = t.tm_mon +1;
	G4int day = t.tm_mday;
	G4int hour = t.tm_hour;
	G4int min = t.tm_min;
	G4int sec = t.tm_sec;

	// File name
	stringstream ss_filename;
	ss_filename<< year << month << day << "_" << hour << "_" << min <<"_" <<sec <<"_ThreadNum_" << G4Threading::G4GetThreadId();
	filenameForSave = ss_filename.str();

	ofs.open(filenameForSave.c_str());

	Counter = 1;
	ofs_hist.open("ScintHistogram.out");

	//Initialization.
	for(G4int i=0;i<REPLICA_NUM;i++){
		for(G4int j=0;j<REPLICA_NUM;j++){
			DEMatrix[i][j] = 0;
		}
	}

}

SensitiveDetector::~SensitiveDetector()
{

	for(G4int i=0;i<REPLICA_NUM;i++){
		for(G4int j=0;j<REPLICA_NUM;j++){
			ofs<< i << "\t"<<j<<"\t"<<DEMatrix[i][j]<<G4endl;
		}
		ofs<<G4endl;
	}


	ofs.close();
	ofs_hist.close();
}

void SensitiveDetector::Initialize(G4HCofThisEvent*)
{

}

G4bool SensitiveDetector::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
	G4String ParName = aStep->GetTrack()->GetParticleDefinition()->GetParticleName();

	if(ParName == "opticalphoton"){
		G4int RepZNo = aStep->GetPreStepPoint()->GetTouchable()->GetReplicaNumber(0);
		G4int RepXNo = aStep->GetPreStepPoint()->GetTouchable()->GetReplicaNumber(1);

		//optical photon doesn't have Deposit Energy
		G4double dE = aStep->GetPreStepPoint()->GetKineticEnergy();
		ofs_hist<<dE<<G4endl;

		DEMatrix[RepZNo][RepXNo] += Counter;
	}
	//  G4cout<< "Sensitive Detector is Activated"<<G4endl;
	//  G4cout<<"ParName is "<< ParName<<G4endl;

	return true;
}

void SensitiveDetector::EndOfEvent(G4HCofThisEvent*)
{

}
