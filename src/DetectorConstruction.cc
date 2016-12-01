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

#include "DetectorConstruction.hh"

#include "G4Box.hh"

#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"

#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"

#include "G4VisAttributes.hh"

#include "SensitiveDetector.hh"

//For Scintillator material
#include "G4Material.hh"
#include "G4MaterialPropertiesTable.hh"

//For Surface property
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"

//Variable Container
#include "VariableContainer_wjcheon.hh"





DetectorConstruction::DetectorConstruction()
:G4VUserDetectorConstruction()
{
	fN = fO = NULL;
	fLXe = fAir = fDRZ_high = NULL;
	fLXe_mt = fAir_mt = fDRZ_high_mt = NULL;
	pv_World = pv_Scint = NULL;
	lv_Scint = NULL;
	WorldSzX = WorldSzY = WorldSzZ = 0.0;
	ScintSzX = ScintSzY = ScintSzZ =0.0;

}

DetectorConstruction::~DetectorConstruction()
{

}

G4VPhysicalVolume* DetectorConstruction::Construct()
{

	SetMaterial();
	SetDimension();


	// World
	G4VSolid* sol_World = new G4Box("World", WorldSzX*0.5, WorldSzY*0.5, WorldSzZ*0.5);
	G4LogicalVolume* lv_World = new G4LogicalVolume(sol_World, fAir, "World");
	pv_World =
			new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, 0.0), lv_World, "World", 0, false, 0);

	// User geometry
	G4VSolid* sol_Scint = new G4Box("Scint", ScintSzX*0.5, ScintSzY*0.5, ScintSzZ*0.5);

	lv_Scint = new G4LogicalVolume(sol_Scint, fDRZ_high, "Scint");
	pv_Scint = new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, -0.5*(ScintSzZ)), lv_Scint, "Scint", lv_World, false, 10);


	G4VSolid *sol_RepX = new G4Box("RepX",(ScintSzX/REPLICA_NUM)*0.5,ScintSzY*0.5,ScintSzZ*0.5);
	G4LogicalVolume *lv_RepX = new G4LogicalVolume(sol_RepX,fDRZ_high,"RepX");
	G4VPhysicalVolume *pv_RepX = new G4PVReplica("RepX",lv_RepX,lv_Scint,kXAxis,REPLICA_NUM,(ScintSzX/REPLICA_NUM));

	G4VSolid *sol_RepY = new G4Box("RepY",(ScintSzX/REPLICA_NUM)*0.5,(ScintSzY/REPLICA_NUM)*0.5,ScintSzZ*0.5);
	G4LogicalVolume *lv_RepY = new G4LogicalVolume(sol_RepY,fDRZ_high,"RepY");
	G4VPhysicalVolume *pv_RepY = new G4PVReplica("RepY",lv_RepY,lv_RepX,kYAxis,REPLICA_NUM,(ScintSzY/REPLICA_NUM));

	//SolidWater Phantom
	G4VSolid *sol_WaterBox = new G4Box("WaterBox",WaterBoxX*0.5,WaterBoxY*0.5,WaterBoxZ*0.5);
	G4Material* WATER = G4NistManager::Instance()->FindOrBuildMaterial("G4_WATER");
	G4LogicalVolume *lv_WaterBox = new G4LogicalVolume(sol_WaterBox,WATER,"WaterBox");
	G4VPhysicalVolume *pv_WaterBox = new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, 0.5*(WaterBoxZ)), lv_WaterBox, "WaterBox",
			lv_World, false, 300);



	// Visualization
	G4VisAttributes* va_World = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0));
	va_World->SetForceWireframe(true);
	lv_World->SetVisAttributes(va_World);

	G4VisAttributes* va_Scint = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0));
	va_Scint->SetForceWireframe(true);
	lv_Scint->SetVisAttributes(va_Scint);

	G4VisAttributes* va_WaterBox = new G4VisAttributes(G4Colour(0.0,0.0,1.0));
	va_WaterBox->SetForceSolid(true);
	lv_WaterBox->SetVisAttributes(va_WaterBox);


	G4VisAttributes* va_Rep = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0,0.3));
	va_Rep->SetForceWireframe(true);
	lv_RepX->SetVisAttributes(va_Rep);
	lv_RepY->SetVisAttributes(va_Rep);



	SetSurfaceProperty();

	return pv_World;
}

void DetectorConstruction::ConstructSDandField()
{
	// Set sensitive detector on "Geom"
	SetSensitiveDetector("RepY", new SensitiveDetector("detector"));
}


void DetectorConstruction::SetMaterial()
{

	G4double a;  // atomic mass
	G4double z;  // atomic number
	G4double density;

	//Geom
	fN = new G4Element("N", "N", z=7., a= 14.01*g/mole);
	fO = new G4Element("O", "O", z=8., a= 16.00*g/mole);
	fAir = new G4Material("Air", density= 1.29*mg/cm3, 2);
	fAir->AddElement(fN, 70*perCent);
	fAir->AddElement(fO, 30*perCent);


	// Liquid Xenon (in Geant4 example)
	//	fLXe = new G4Material("LXe",z=54.,a=131.29*g/mole,density=3.020*g/cm3);
	//
	//
	//	const G4int lxenum = 3;
	//	G4double DRZ_Energy[lxenum]    = { 7.0*eV , 7.07*eV, 7.14*eV };
	//
	//	G4double DRZ_SCINT[lxenum] = { 0.1, 1.0, 0.1 };
	//	G4double DRZ_RIND[lxenum]  = { 1.59 , 1.57, 1.54 };
	//	G4double DRZ_ABSL[lxenum]  = { 35.*cm, 35.*cm, 35.*cm};
	//	fLXe_mt = new G4MaterialPropertiesTable();
	//	fLXe_mt->AddProperty("FASTCOMPONENT", DRZ_Energy, DRZ_SCINT, lxenum);
	//	fLXe_mt->AddProperty("SLOWCOMPONENT", DRZ_Energy, DRZ_SCINT, lxenum);
	//	fLXe_mt->AddProperty("RINDEX",        DRZ_Energy, DRZ_RIND,  lxenum);
	//	fLXe_mt->AddProperty("ABSLENGTH",     DRZ_Energy, DRZ_ABSL,  lxenum);
	//	//fLXe_mt->AddConstProperty("SCINTILLATIONYIELD",12000./MeV);
	//	fLXe_mt->AddConstProperty("SCINTILLATIONYIELD",100./MeV);
	//	fLXe_mt->AddConstProperty("RESOLUTIONSCALE",1.0);
	//	fLXe_mt->AddConstProperty("FASTTIMECONSTANT",20.*ns);
	//	fLXe_mt->AddConstProperty("SLOWTIMECONSTANT",45.*ns);
	//	fLXe_mt->AddConstProperty("YIELDRATIO",1.0);
	//	fLXe->SetMaterialPropertiesTable(fLXe_mt);



	fDRZ_high = new G4Material("DRZ-High",z=59.5,a=378.6*g/mole,density=7.3*g/cm3);
	const G4int drznum = 354;
	G4double DRZ_Energy[drznum]    = {1.902565*eV,1.905483*eV,1.908410*eV,1.911346*eV,1.914291*eV,1.917245*eV,1.920209*eV,1.923181*eV,1.926163*eV,1.929154*eV,1.932154*eV,1.935164*eV,1.938183*eV,1.941211*eV,1.944249*eV,1.947296*eV,1.950353*eV,1.953420*eV,1.956496*eV,1.959582*eV,1.962678*eV,1.965783*eV,1.968899*eV,1.972024*eV,1.975159*eV,1.978304*eV,1.981459*eV,1.984625*eV,1.987800*eV,1.990986*eV,1.994181*eV,1.997387*eV,2.000604*eV,2.003831*eV,2.007068*eV,2.010316*eV,2.013574*eV,2.016843*eV,2.020122*eV,2.023412*eV,2.026713*eV,2.030025*eV,2.033347*eV,2.036680*eV,2.040025*eV,2.043380*eV,2.046746*eV,2.050124*eV,2.053512*eV,2.056912*eV,2.060323*eV,2.063746*eV,2.067180*eV,2.070625*eV,2.074082*eV,2.077550*eV,2.081030*eV,2.084522*eV,2.088025*eV,2.091540*eV,2.095067*eV,2.098606*eV,2.102157*eV,2.105720*eV,2.109295*eV,2.112883*eV,2.116482*eV,2.120094*eV,2.123718*eV,2.127354*eV,2.131003*eV,2.134665*eV,2.138339*eV,2.142026*eV,2.145725*eV,2.149438*eV,2.153163*eV,2.156901*eV,2.160652*eV,2.164416*eV,2.168194*eV,2.171984*eV,2.175788*eV,2.179605*eV,2.183436*eV,2.187280*eV,2.191138*eV,2.195009*eV,2.198894*eV,2.202793*eV,2.206705*eV,2.210632*eV,2.214572*eV,2.218527*eV,2.222496*eV,2.226478*eV,2.230476*eV,2.234487*eV,2.238514*eV,2.242554*eV,2.246609*eV,2.250679*eV,2.254764*eV,2.258864*eV,2.262978*eV,2.267108*eV,2.271252*eV,2.275412*eV,2.279587*eV,2.283778*eV,2.287983*eV,2.292205*eV,2.296442*eV,2.300694*eV,2.304963*eV,2.309247*eV,2.313547*eV,2.317864*eV,2.322196*eV,2.326545*eV,2.330910*eV,2.335291*eV,2.339689*eV,2.344104*eV,2.348535*eV,2.352983*eV,2.357448*eV,2.361930*eV,2.366429*eV,2.370945*eV,2.375478*eV,2.380029*eV,2.384597*eV,2.389183*eV,2.393786*eV,2.398407*eV,2.403046*eV,2.407703*eV,2.412379*eV,2.417072*eV,2.421784*eV,2.426514*eV,2.431262*eV,2.436029*eV,2.440815*eV,2.445620*eV,2.450444*eV,2.455287*eV,2.460149*eV,2.465030*eV,2.469930*eV,2.474851*eV,2.479790*eV,2.484750*eV,2.489729*eV,2.494729*eV,2.499748*eV,2.504788*eV,2.509848*eV,2.514929*eV,2.520030*eV,2.525152*eV,2.530295*eV,2.535459*eV,2.540644*eV,2.545850*eV,2.551078*eV,2.556327*eV,2.561598*eV,2.566890*eV,2.572205*eV,2.577541*eV,2.582900*eV,2.588281*eV,2.593685*eV,2.599111*eV,2.604560*eV,2.610032*eV,2.615526*eV,2.621044*eV,2.626586*eV,2.632150*eV,2.637739*eV,2.643351*eV,2.648987*eV,2.654647*eV,2.660332*eV,2.666041*eV,2.671774*eV,2.677532*eV,2.683315*eV,2.689123*eV,2.694957*eV,2.700815*eV,2.706699*eV,2.712609*eV,2.718545*eV,2.724507*eV,2.730495*eV,2.736509*eV,2.742550*eV,2.748617*eV,2.754712*eV,2.760833*eV,2.766982*eV,2.773158*eV,2.779362*eV,2.785594*eV,2.791854*eV,2.798142*eV,2.804458*eV,2.810803*eV,2.817177*eV,2.823580*eV,2.830011*eV,2.836473*eV,2.842963*eV,2.849484*eV,2.856034*eV,2.862615*eV,2.869226*eV,2.875868*eV,2.882541*eV,2.889244*eV,2.895979*eV,2.902745*eV,2.909543*eV,2.916373*eV,2.923235*eV,2.930130*eV,2.937057*eV,2.944017*eV,2.951010*eV,2.958036*eV,2.965095*eV,2.972189*eV,2.979317*eV,2.986478*eV,2.993675*eV,3.000906*eV,3.008172*eV,3.015473*eV,3.022810*eV,3.030183*eV,3.037592*eV,3.045037*eV,3.052518*eV,3.060037*eV,3.067593*eV,3.075186*eV,3.082816*eV,3.090485*eV,3.098192*eV,3.105937*eV,3.113722*eV,3.121545*eV,3.129408*eV,3.137311*eV,3.145253*eV,3.153236*eV,3.161260*eV,3.169324*eV,3.177430*eV,3.185577*eV,3.193766*eV,3.201997*eV,3.210271*eV,3.218588*eV,3.226948*eV,3.235352*eV,3.243799*eV,3.252291*eV,3.260827*eV,3.269408*eV,3.278034*eV,3.286706*eV,3.295424*eV,3.304189*eV,3.313000*eV,3.321858*eV,3.330764*eV,3.339718*eV,3.348720*eV,3.357770*eV,3.366870*eV,3.376019*eV,3.385218*eV,3.394467*eV,3.403767*eV,3.413118*eV,3.422521*eV,3.431975*eV,3.441482*eV,3.451042*eV,3.460655*eV,3.470321*eV,3.480042*eV,3.489817*eV,3.499648*eV,3.509534*eV,3.519476*eV,3.529474*eV,3.539530*eV,3.549643*eV,3.559814*eV,3.570043*eV,3.580331*eV,3.590679*eV,3.601087*eV,3.611555*eV,3.622085*eV,3.632675*eV,3.643328*eV,3.654044*eV,3.664823*eV,3.675666*eV,3.686573*eV,3.697545*eV,3.708582*eV,3.719686*eV,3.730856*eV,3.742093*eV,3.753399*eV,3.764773*eV,3.776216*eV,3.787729*eV,3.799312*eV,3.810966*eV,3.822692*eV,3.834491*eV,3.846362*eV,3.858307*eV,3.870327*eV,3.882422*eV,3.894592*eV,3.906840*eV,3.919164*eV,3.931566*eV,3.944048*eV,3.956608*eV,3.969249*eV,3.981971*eV,3.994775*eV,4.007661*eV,4.020631*eV,4.033685*eV,4.046824*eV,4.060049*eV,4.073361*eV,4.086760*eV,4.100248*eV,4.113825*eV,4.127492*eV,4.141250*eV};

	G4double DRZ_SCINT[drznum] = {0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.050794,0.111111,0.126984,0.139683,0.130159,0.079365,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.034921,0.063492,0.076190,0.082540,0.085714,0.161905,0.184127,0.139683,0.041270,0.041270,0.028571,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.025397,0.126984,0.339683,0.441270,0.498413,0.498413,0.479365,0.600000,0.806349,0.955556,1.000000,1.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.025397,0.053968,0.139683,0.149206,0.146032,0.133333,0.146032,0.168254,0.168254,0.041270,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.047619,0.050794,0.047619,0.038095,0.038095,0.031746,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.025397,0.066667,0.092063,0.092063,0.088889,0.092063,0.085714,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.028571,0.047619,0.057143,0.057143,0.053968,0.050794,0.053968,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000};

	G4double DRZ_RIND[drznum]  = {2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2,2.2};
	G4double DRZ_ABSL[drznum]  = {1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m,1*m};

	fDRZ_high_mt= new G4MaterialPropertiesTable();
	fDRZ_high_mt->AddProperty("FASTCOMPONENT", DRZ_Energy, DRZ_SCINT, drznum);
	fDRZ_high_mt->AddProperty("SLOWCOMPONENT", DRZ_Energy, DRZ_SCINT, drznum);
	fDRZ_high_mt->AddProperty("RINDEX",        DRZ_Energy, DRZ_RIND,  drznum);
	fDRZ_high_mt->AddProperty("ABSLENGTH",     DRZ_Energy, DRZ_ABSL,  drznum);
	//fLXe_mt->AddConstProperty("SCINTILLATIONYIELD",12000./MeV);
	fDRZ_high_mt->AddConstProperty("SCINTILLATIONYIELD",1./MeV);
	fDRZ_high_mt->AddConstProperty("RESOLUTIONSCALE",1.0);
	fDRZ_high_mt->AddConstProperty("FASTTIMECONSTANT",0.5*ms);
	fDRZ_high_mt->AddConstProperty("SLOWTIMECONSTANT",3.*ms);
	fDRZ_high_mt->AddConstProperty("YIELDRATIO",1.0);
	fDRZ_high->SetMaterialPropertiesTable(fDRZ_high_mt);

	const G4int airnum = 3;
	G4double Air_Energy[airnum]={2.0*eV,7.0*eV,7.14*eV};
	G4double Air_RIND[airnum]={1.,1.,1.};
	G4double Air_AbsLength[airnum]={10.0*m,10.0*m,10.0*m};
	fAir_mt = new G4MaterialPropertiesTable();
	fAir_mt->AddProperty("ABSLENGTH",Air_Energy,Air_AbsLength,drznum);
	fAir_mt->AddProperty("RINDEX", Air_Energy, Air_RIND,drznum);
	fAir->SetMaterialPropertiesTable(fAir_mt);//Give air the same rindex


	fC = new G4Element("N", "N", z=6., a= 12.011*g/mole);
	fH = new G4Element("O", "O", z=1., a= 1.008*g/mole);
	fPolystyrene_SMC = new G4Material("SolidWater", density= 0.950*g/cm3, 2);
	fPolystyrene_SMC->AddElement(fC, 85.7*perCent);
	fPolystyrene_SMC->AddElement(fH, 14.3*perCent);

}

void DetectorConstruction::SetSurfaceProperty(){

	//Scintillator to World
	G4OpticalSurface* Scint2World = new G4OpticalSurface("ScintWrap");

	new G4LogicalBorderSurface("Scint2World", pv_Scint,
			pv_World,
			Scint2World);


	Scint2World->SetType(dielectric_metal);
	Scint2World->SetFinish(polishedair);
	Scint2World->SetModel(glisur);


	const G4int num = 2;

	G4double pp[num] = {2.0*eV, 3.5*eV};
	G4double reflectivity[num] = {0., 0.};
	G4double efficiency[num] = {0.0, 0.0};

	G4MaterialPropertiesTable* WrapProperty
	= new G4MaterialPropertiesTable();

	WrapProperty->AddProperty("REFLECTIVITY",pp,reflectivity,num);
	WrapProperty->AddProperty("EFFICIENCY",pp,efficiency,num);
	Scint2World->SetMaterialPropertiesTable(WrapProperty);

	new G4LogicalSkinSurface("MirrorSurface",lv_Scint,Scint2World);


}


void DetectorConstruction::SetDimension(){

	WorldSzX = 1.0*m;
	WorldSzY = 1.0*m;
	WorldSzZ = 2.0*m;

	ScintSzX = 30.0*cm;
	ScintSzY = 30.0*cm;
	ScintSzZ = 0.64*cm;  // Measured size

	WaterBoxX = 30.0*cm;
	WaterBoxY = 30.0*cm;
	WaterBoxZ = 20.0*cm;

}
