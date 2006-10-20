// $Id: StiIstDetectorBuilder.h,v 1.3 2006/10/20 18:43:12 wleight Exp $
// 
// $Log: StiIstDetectorBuilder.h,v $
// Revision 1.3  2006/10/20 18:43:12  wleight
// Changes to make perfect hits in the IST work with UPGR05
//
// Revision 1.10  2006/06/28 18:51:46  fisyak
// Add loading of tracking and hit error parameters from DB
//
// Revision 1.9  2005/06/21 16:35:01  lmartin
// DetectorBuilder updated with the correct methods from StIstUtil
//
// Revision 1.8  2005/06/21 15:31:47  lmartin
// CVS tags added
//
/*!
 * \class StiIstDetectorBuilder
 * \author Christelle Roy
 * \date 02/27/04

This class is the description of the StiIstDetectorBuilder
*/
#ifndef StiIstDetectorBuilder_H
#define StiIstDetectorBuilder_H
//#include "StSsdUtil/StSsdGeometry.hh"
//#include "StSsdUtil/StSsdConfig.hh"
#include "Sti/StiDetectorBuilder.h"
#include "Sti/StiHitErrorCalculator.h"
#include "StThreeVector.hh"

class StiIstDetectorBuilder : public StiDetectorBuilder
{

 private :
   //StSsdConfig*   mSsdConfig;    //!
   //StSsdGeometry* mSsdGeom;      //!
 public:
        StiIstDetectorBuilder(bool active,const string & inputFile);
	virtual ~StiIstDetectorBuilder(); 
	virtual void buildDetectors(StMaker& source);
	virtual void AverageVolume(TGeoPhysicalNode *nodeP);
	//virtual void loadDS(TDataSet&);
	//virtual void setDefaults();
	virtual void useVMCGeometry();		
	void    setSiMat(StiMaterial     *m) {_siMat = m;}
	void    setHybridMat(StiMaterial *m) {_hybridMat = m;}
	StiMaterial *getSiMat()    {return _siMat;}
	StiMaterial *getHybridMat(){return _hybridMat;}
 
 protected:
        //float phiForSsdLadder(unsigned int iLadder) const;
        //float radiusForSsdLadder(unsigned int iLadder) const;
	StiMaterial *_siMat;
	StiMaterial *_hybridMat;
	StiPlanarShape * _waferShape[1];
	StiPlanarShape * _hybridShape[1];
	//StSsdConfig   * _config;
	//StSsdGeometry * _geometry;
	//StSsdGeometry * _dimensions;
	StiDefaultHitErrorCalculator _hitCalculator;
};
#endif 
