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



#ifndef SensitiveDetector_hh_
#define SensitiveDetector_hh_

#include "G4VSensitiveDetector.hh"
#include "G4Step.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "VariableContainer_wjcheon.hh"


#include <string.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <time.h>
#include <stdio.h>

using namespace std;

class G4HCofThisEvent;
class G4TouchableHistory;

class SensitiveDetector: public G4VSensitiveDetector
{
public:
	SensitiveDetector(G4String name);
	virtual ~SensitiveDetector();

	void Initialize(G4HCofThisEvent*);
	G4bool ProcessHits(G4Step* aStep, G4TouchableHistory*);
	void EndOfEvent(G4HCofThisEvent*);
private:
	std::ofstream ofs;
	std::ofstream ofs_hist;
	G4double DEMatrix[REPLICA_NUM][REPLICA_NUM];
	G4double Counter;
	string filenameForSave;
};

#endif
