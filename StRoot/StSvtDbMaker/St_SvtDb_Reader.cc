/***************************************************************************
 *
 * $Id: St_SvtDb_Reader.cc,v 1.6 2004/01/27 02:37:54 perev Exp $
 *
 * Author: Marcelo Munhoz
 ***************************************************************************
 *
 * Description: SVT DB access Maker
 *
 ***************************************************************************
 *
 * $Log: St_SvtDb_Reader.cc,v $
 * Revision 1.6  2004/01/27 02:37:54  perev
 * LeakOff
 *
 * Revision 1.5  2003/04/14 15:51:53  munhoz
 * reading t0 from DB
 *
 * Revision 1.4  2002/05/06 00:42:51  munhoz
 * adding bad anode list reading
 *
 * Revision 1.3  2002/02/15 22:45:43  munhoz
 * introducing drift velocity reading capability
 *
 * Revision 1.2  2002/02/05 23:30:52  caines
 * fixing configuration bug
 *
 * Revision 1.1  2001/10/29 18:53:14  munhoz
 * starting SVT Data base
 *
 *
 **************************************************************************/

#include "St_SvtDb_Reader.hh"

#include "StMessMgr.h"

#include "StSvtClassLibrary/StSvtConfig.hh"
#include "StSvtClassLibrary/StSvtHybridPed.hh"
#include "StSvtClassLibrary/StSvtHybridDriftVelocity.hh"
#include "StSvtClassLibrary/StSvtHybridCollection.hh"
#include "StSvtClassLibrary/StSvtHybridBadAnodes.hh"
#include "StSvtClassLibrary/StSvtGeometry.hh"
#include "StSvtClassLibrary/StSvtWaferGeometry.hh"
#include "StSvtClassLibrary/StSvtT0.hh"

#include "tables/St_svtConfiguration_Table.h"
#include "tables/St_svtDriftVelAvg_Table.h"
#include "tables/St_svtBadAnodes_Table.h"
#include "tables/St_svtPedestals_Table.h"
#include "tables/St_svtWafersPosition_Table.h"
#include "tables/St_svtDimensions_Table.h"
#include "tables/St_svtElectronics_Table.h"

#ifdef __ROOT__
ClassImp(St_SvtDb_Reader)
#endif

svtElectronics_st *electronic = NULL;

//_____________________________________________________________________________
St_SvtDb_Reader::St_SvtDb_Reader()
{
  mSvtDriftVeloc=0;
  mSvtConfig = 0;
  memset(svtDb,0,sizeof(svtDb));
}

//_____________________________________________________________________________
St_SvtDb_Reader::~St_SvtDb_Reader()
{
  delete mSvtDriftVeloc;
  delete mSvtConfig;
}

//_____________________________________________________________________________
void St_SvtDb_Reader::setDataBase(St_DataSet* input, dbSvtType type)
{
  if (input)
    svtDb[type] = input;    
  else
    gMessMgr->Message("Error setting St_SvtDb_Reader: Need to specify input DataSet","E");  
}

//_____________________________________________________________________________
StSvtConfig* St_SvtDb_Reader::getConfiguration()
{
  cout << "St_SvtDb_Reader::getConfiguration" << endl;

  St_svtConfiguration *configuration;
  const int dbIndex = kGeometry;
  if (svtDb[dbIndex]){
    configuration = (St_svtConfiguration*)svtDb[dbIndex]->Find("svtConfiguration");
    if (!(configuration && configuration->HasData()) ){
     gMessMgr->Message("Error Finding SVT Configuration","E");
     return 0;
    }
  }
  else {
    gMessMgr->Message("Error Finding SVT Geometry Db","E");
    return 0;
  }

  svtConfiguration_st* config = configuration->GetTable();

  cout << "numberOfBarrels = " << config->numberOfBarrels << endl;
  cout << "numberOfLadders = "  << config->numberOfLadders << endl;
  cout << "numberOfWafers = "  << config->numberOfWafers << endl;
  cout << "numberOfHybrids = "  << config->numberOfHybrids << endl;
  
  cout << "numberOfLaddersPerBarrel[0] = "  << config->numberOfLaddersPerBarrel[0] << endl;
  cout << "numberOfLaddersPerBarrel[1] = "  << config->numberOfLaddersPerBarrel[1] << endl;
  cout << "numberOfLaddersPerBarrel[2] = "  << config->numberOfLaddersPerBarrel[2] << endl;
  cout << "numberOfWafersPerLadder[0] = "  << config->numberOfWafersPerLadder[0] << endl;
  cout << "numberOfWafersPerLadder[1] = "  << config->numberOfWafersPerLadder[1] << endl;
  cout << "numberOfWafersPerLadder[2] = "  << config->numberOfWafersPerLadder[2] << endl;
  cout << "numberOfHybridsPerWafer = "  << config->numberOfHybridsPerWafer << endl;

  mSvtConfig = new StSvtConfig();

  mSvtConfig->setNumberOfBarrels(config->numberOfBarrels);

  for (int i=0; i<config->numberOfBarrels; i++) {
    mSvtConfig->setNumberOfLadders(i+1,config->numberOfLaddersPerBarrel[i]);
    mSvtConfig->setNumberOfWafers(i+1, config->numberOfWafersPerLadder[i]);
  }
  mSvtConfig->setNumberOfHybrids(config->numberOfHybridsPerWafer);
  mSvtConfig->setTotalNumberOfHybrids(config->numberOfHybrids);

  //temporary. Must read electronics db and fill these quantities
  mSvtConfig->setNumberOfAnodes(240);
  mSvtConfig->setNumberOfTimeBins(128);

  mSvtConfig->setConfiguration();

  return mSvtConfig;
}

//_____________________________________________________________________________
StSvtHybridCollection* St_SvtDb_Reader::getDriftVelocity()
{
  cout << "St_SvtDb_Reader::getDriftVelocity" << endl;
  assert(!mSvtDriftVeloc);
  mSvtDriftVeloc = new StSvtHybridCollection(mSvtConfig);
  getDriftVelocityAverage(mSvtDriftVeloc);

  return mSvtDriftVeloc;
}

//_____________________________________________________________________________
void St_SvtDb_Reader::getDriftVelocityAverage(StSvtHybridCollection* svtDriftVeloc)
{
  cout << "St_SvtDb_Reader::getDriftVelocityAverage" << endl;

  St_svtDriftVelAvg *driftVelocity;
  const int dbIndex = kCalibration;

  svtDriftVelAvg_st *driftVeloc;
  StSvtHybridDriftVelocity* hybridDriftVeloc;

  char path[100];
  int index;

  for (int barrel = 1;barrel <= mSvtConfig->getNumberOfBarrels();barrel++) {
    for (int ladder = 1;ladder <= mSvtConfig->getNumberOfLadders(barrel);ladder++) {
      for (int wafer = 1;wafer <= mSvtConfig->getNumberOfWafers(barrel);wafer++) {
	for (int hybrid = 1;hybrid <= mSvtConfig->getNumberOfHybrids();hybrid++) {

	  index = mSvtConfig->getHybridIndex(barrel,ladder,wafer,hybrid);

	  if (index < 0) continue;

	  switch (barrel) {
	  case 1:
	    sprintf(path,"InnerBarrel/Ladder_0%d/Wafer_0%d/Hybrid_0%d/svtDriftVelAvg",ladder,wafer,hybrid);
	    break;
	  case 2:
	    if (ladder < 10)
	      sprintf(path,"MiddleBarrel/Ladder_0%d/Wafer_0%d/Hybrid_0%d/svtDriftVelAvg",ladder,wafer,hybrid);
	    else
	      sprintf(path,"MiddleBarrel/Ladder_%d/Wafer_0%d/Hybrid_0%d/svtDriftVelAvg",ladder,wafer,hybrid);
	    break;
	  case 3:
	    if (ladder < 10)
	      sprintf(path,"OuterBarrel/Ladder_0%d/Wafer_0%d/Hybrid_0%d/svtDriftVelAvg",ladder,wafer,hybrid);
	    else
	      sprintf(path,"OuterBarrel/Ladder_%d/Wafer_0%d/Hybrid_0%d/svtDriftVelAvg",ladder,wafer,hybrid);
	    break;
	  }

	  //cout << path << endl;

	  // get wafers position table
	  if (svtDb[dbIndex]){
	    driftVelocity = (St_svtDriftVelAvg*)svtDb[dbIndex]->Find(path);
	    if (!(driftVelocity && driftVelocity->HasData()) ){
	      gMessMgr->Message("Error Finding SVT drift velocity average","E");
	      return;
	    }
	  }
	  else {
	    gMessMgr->Message("Error Finding SVT Calibration Db","E");
	    return;
	  }

	  driftVeloc = driftVelocity->GetTable();

	  hybridDriftVeloc = (StSvtHybridDriftVelocity*)svtDriftVeloc->at(index);
	  if (!hybridDriftVeloc)
	    hybridDriftVeloc = new StSvtHybridDriftVelocity(barrel,ladder,wafer,hybrid);
	  
	  // loop over anodes
	  for (int anode=1;anode<=mSvtConfig->getNumberOfAnodes();anode++) {
	    hybridDriftVeloc->setV3(driftVeloc->averageDriftVelocity,anode);
	  }

	  cout << "index = " << index << ", averageDriftVelocity = " << driftVeloc->averageDriftVelocity << endl;

	  svtDriftVeloc->put_at(hybridDriftVeloc,index);

	} // end of loop over hybrids
      } // end of loop over wafers
    } // end of loop over ladders
  } // end of loop over barrels

}

//_____________________________________________________________________________
StSvtHybridCollection* St_SvtDb_Reader::getPedestals()
{
  cout << "St_SvtDb_Reader::getPedestals" << endl;

  St_svtPedestals *pedestals;
  const int dbIndex = kCalibration;
  if (svtDb[dbIndex]){
    pedestals = (St_svtPedestals*)svtDb[dbIndex]->Find("svtPedestals");
    if (!(pedestals && pedestals->HasData()) ){
     gMessMgr->Message("Error Finding SVT Pedestals","E");
     return 0;
    }
  }
  else {
    gMessMgr->Message("Error Finding SVT Calibration Db","E");
    return 0;
  }

  svtPedestals_st *pedestal = pedestals->GetTable();

  // Create all pedestal objects
  StSvtHybridCollection* svtPed = new StSvtHybridCollection(mSvtConfig);
  StSvtHybridPed* hybridPed;
  int index;

  for (int barrel = 1;barrel <= mSvtConfig->getNumberOfBarrels();barrel++) {
    for (int ladder = 1;ladder <= mSvtConfig->getNumberOfLadders(barrel);ladder++) {
      for (int wafer = 1;wafer <= mSvtConfig->getNumberOfWafers(barrel);wafer++) {
	for (int hybrid = 1;hybrid <= mSvtConfig->getNumberOfHybrids();hybrid++) {

	  index = mSvtConfig->getHybridIndex(barrel,ladder,wafer,hybrid);

	  if (index < 0) continue;

	  hybridPed = new StSvtHybridPed(barrel,ladder,wafer,hybrid);
	  hybridPed->reset();

	  // loop over anodes
	  for (int anode = 1; anode <= mSvtConfig->getNumberOfAnodes(); anode++)
	    for (int time = 0; time < mSvtConfig->getNumberOfTimeBins(); time++) {
	      hybridPed->addToPixel(anode,time,pedestal[index].pedestal[anode-1][time]);
	      cout << anode << "  " << time << "  " << pedestal[index].pedestal[anode-1][time] << endl;
	    }

	  svtPed->put_at(hybridPed,index);

	} // end of loop over hybrids
      } // end of loop over wafers
    } // end of loop over ladders
  } // end of loop over barrels

  return svtPed;
}

//_____________________________________________________________________________
StSvtGeometry* St_SvtDb_Reader::getGeometry()
{
  // get svt dimensions table
  St_svtDimensions *dimensions;
  const int dbIndex = kGeometry;
  if (svtDb[dbIndex]){
    dimensions = (St_svtDimensions*)svtDb[dbIndex]->Find("svtDimensions");
    if (!(dimensions && dimensions->HasData()) ){
     gMessMgr->Message("Error Finding SVT Dimensions","E");
     return 0;
    }
  }
  else {
    gMessMgr->Message("Error Finding SVT Geometry Db","E");
    return 0;
  }

  svtDimensions_st *dimension = dimensions->GetTable();

  // Create all pedestal objects
  StSvtGeometry* svtGeom = new StSvtGeometry(mSvtConfig);
  StSvtWaferGeometry* waferGeom;
  int index;

  svtGeom->setBarrelRadius(dimension->barrelRadius);
  svtGeom->setWaferLength(dimension->waferLength);
  svtGeom->setWaferThickness(dimension->waferThickness);
  svtGeom->setWaferWidth(dimension->waferWidth);
  //svtGeom->setAnodePitch(dimension->anodePitch);
  svtGeom->setAnodePitch(0.025);
  svtGeom->setFocusRegionLength(dimension->focusRegionLength);
  svtGeom->setDistanceInjector(dimension->distanceInjector);
  svtGeom->setLaserPosition(dimension->laserPosition);

  char path[100];
  St_svtWafersPosition *positions;
  svtWafersPosition_st *position;

  for (int barrel = 1;barrel <= svtGeom->getNumberOfBarrels();barrel++) {
    for (int ladder = 1;ladder <= svtGeom->getNumberOfLadders(barrel);ladder++) {
      for (int wafer = 1;wafer <= svtGeom->getNumberOfWafers(barrel);wafer++) {

	  index = svtGeom->getWaferIndex(barrel,ladder,wafer);

	  if (index < 0) continue;

	  switch (barrel) {
	  case 1:
	    sprintf(path,"InnerBarrel/Ladder_0%d/Wafer_0%d/svtWafersPosition",ladder,wafer);
	    break;
	  case 2:
	    if (ladder < 10)
	      sprintf(path,"MiddleBarrel/Ladder_0%d/Wafer_0%d/svtWafersPosition",ladder,wafer);
	    else
	      sprintf(path,"MiddleBarrel/Ladder_%d/Wafer_0%d/svtWafersPosition",ladder,wafer);
	    break;
	  case 3:
	    if (ladder < 10)
	      sprintf(path,"OuterBarrel/Ladder_0%d/Wafer_0%d/svtWafersPosition",ladder,wafer);
	    else
	      sprintf(path,"OuterBarrel/Ladder_%d/Wafer_0%d/svtWafersPosition",ladder,wafer);
	    break;
	  }

	  // get wafers position table
	  if (svtDb[dbIndex]){
	    positions = (St_svtWafersPosition*)svtDb[dbIndex]->Find(path);
	    if (!(positions && positions->HasData()) ){
	      gMessMgr->Message("Error Finding SVT Positions","E");
	      return 0;
	    }
	  }
	  else {
	    gMessMgr->Message("Error Finding SVT Geometry Db","E");
	    return 0;
	  }
	  
	  position = positions->GetTable();

	  // fill StSvtGeometry object
	  waferGeom = new StSvtWaferGeometry(barrel,ladder,wafer);

	  waferGeom->setDriftDirection(position->driftDirection[0],position->driftDirection[1],position->driftDirection[2]);
	  waferGeom->setNormalDirection(position->normalDirection[0],position->normalDirection[1],position->normalDirection[2]);
	  waferGeom->setTransverseDirection(position->transverseDirection[0],position->transverseDirection[1],position->transverseDirection[2]);
	  waferGeom->setCenterPosition(position->centerPosition[0],position->centerPosition[1],position->centerPosition[2]);

	  cout << "index = "  << index << ", x = " << position->centerPosition[0] << endl;

	  svtGeom->put_at(waferGeom,index);

      } // end of loop over wafers
    } // end of loop over ladders
  } // end of loop over barrels

  return svtGeom;
}

//_____________________________________________________________________________
StSvtHybridCollection* St_SvtDb_Reader::getBadAnodes()
{
  // get svt dimensions table
  St_svtBadAnodes *badAnodes;
  svtBadAnodes_st *badAnode;
  const int dbIndex = kCalibration;

  // Create all pedestal objects
  StSvtHybridCollection* svtBadAnodes = new StSvtHybridCollection(mSvtConfig);
  StSvtHybridBadAnodes* hybridBadAnodes;

  char path[100];
  int index;

  for (int barrel = 1;barrel <= mSvtConfig->getNumberOfBarrels();barrel++) {
    for (int ladder = 1;ladder <= mSvtConfig->getNumberOfLadders(barrel);ladder++) {
      for (int wafer = 1;wafer <= mSvtConfig->getNumberOfWafers(barrel);wafer++) {
	for (int hybrid = 1;hybrid <= mSvtConfig->getNumberOfHybrids();hybrid++) {

	  index = mSvtConfig->getHybridIndex(barrel,ladder,wafer,hybrid);

	  if (index < 0) continue;

	  switch (barrel) {
	  case 1:
	    sprintf(path,"InnerBarrel/Ladder_0%d/Wafer_0%d/Hybrid_0%d/svtBadAnodes",ladder,wafer,hybrid);
	    break;
	  case 2:
	    if (ladder < 10)
	      sprintf(path,"MiddleBarrel/Ladder_0%d/Wafer_0%d/Hybrid_0%d/svtBadAnodes",ladder,wafer,hybrid);
	    else
	      sprintf(path,"MiddleBarrel/Ladder_%d/Wafer_0%d/Hybrid_0%d/svtBadAnodes",ladder,wafer,hybrid);
	    break;
	  case 3:
	    if (ladder < 10)
	      sprintf(path,"OuterBarrel/Ladder_0%d/Wafer_0%d/Hybrid_0%d/svtBadAnodes",ladder,wafer,hybrid);
	    else
	      sprintf(path,"OuterBarrel/Ladder_%d/Wafer_0%d/Hybrid_0%d/svtBadAnodes",ladder,wafer,hybrid);
	    break;
	  }

	  // get wafers position table
	  if (svtDb[dbIndex]){
	    badAnodes = (St_svtBadAnodes*)svtDb[dbIndex]->Find(path);
	    if (!(badAnodes && badAnodes->HasData()) ){
	      gMessMgr->Message("Error Finding SVT bad anodes","E");
	      return 0;
	    }
	  }
	  else {
	    gMessMgr->Message("Error Finding SVT Calibration Db","E");
	    return 0;
	  }

	  badAnode = badAnodes->GetTable();

	  hybridBadAnodes = new StSvtHybridBadAnodes(barrel,ladder,wafer,hybrid);
	  
	  // loop over anodes
	  for (int anode=1;anode<=mSvtConfig->getNumberOfAnodes();anode++) {
	    if (badAnode->isBadAnode[anode-1]) {
	      hybridBadAnodes->setBadAnode(anode);
	      //cout << "hybrid = "<< index << ", anode = " << anode << endl;
	    }
	  }

	  svtBadAnodes->put_at(hybridBadAnodes,index);

	} // end of loop over hybrids
      } // end of loop over wafers
    } // end of loop over ladders
  } // end of loop over barrels

  return svtBadAnodes;
}

//_____________________________________________________________________________
int St_SvtDb_Reader::getElectronics()
{
  // get svt electronics table
  St_svtElectronics *electronics;
  const int dbIndex = kCalibration;
  if (svtDb[dbIndex]){
    electronics = (St_svtElectronics*)svtDb[dbIndex]->Find("svtElectronics");
    if (!(electronics && electronics->HasData()) ){
     gMessMgr->Message("Error Finding SVT Electronics","E");
     return kFALSE;
    }
  }
  else {
    gMessMgr->Message("Error Finding SVT Geometry Db","E");
    return kFALSE;
  }

  electronic = electronics->GetTable();
  return kTRUE;
}

//_____________________________________________________________________________
StSvtT0* St_SvtDb_Reader::getT0()
{
  StSvtT0* svtT0 = new StSvtT0();

  if (getElectronics()) {
    for (int i=0;i<24;i++)
      svtT0->setT0(electronic->tZero[i],i+1);
    svtT0->setFsca(electronic->samplingFrequency);

    cout << "t0 = " << svtT0->getT0(1) << ", fsca =  " << svtT0->getFsca() << endl;

  }

  return svtT0;
}
