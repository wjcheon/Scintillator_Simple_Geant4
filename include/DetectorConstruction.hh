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


#ifndef DetectorConstruction_hh_
#define DetectorConstruction_hh_

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4VPhysicalVolume.hh"

#include "G4Material.hh"
#include "G4MaterialPropertiesTable.hh"

#include "G4LogicalVolume.hh"
class DetectorConstruction: public G4VUserDetectorConstruction
{
public:
	DetectorConstruction();
	virtual ~DetectorConstruction();

	virtual G4VPhysicalVolume* Construct();
	virtual void ConstructSDandField();
	void SetMaterial();
	void SetSurfaceProperty();
	void SetDimension();

private:

	//Geometry
	G4LogicalVolume* lv_Scint;

	G4VPhysicalVolume* pv_World;
	G4VPhysicalVolume* pv_Scint;


	//Material

	G4Element *fN;
	G4Element *fO;
	G4Material *fAir;
	G4MaterialPropertiesTable* fAir_mt;
	G4Material *fLXe;
	G4MaterialPropertiesTable* fLXe_mt;
	G4Material *fDRZ_high;
	G4MaterialPropertiesTable* fDRZ_high_mt;

	G4Element *fC;
	G4Element *fH;
	G4Material *fPolystyrene_SMC;


	//time

	//Dimension
	G4double WorldSzX;
	G4double WorldSzY;
	G4double WorldSzZ;

	G4double ScintSzX;
	G4double ScintSzY;
	G4double ScintSzZ;

	G4double WaterBoxX;
	G4double WaterBoxY;
	G4double WaterBoxZ;




};

#endif
