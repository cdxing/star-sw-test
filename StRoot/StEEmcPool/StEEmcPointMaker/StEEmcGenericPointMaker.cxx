#include "StEEmcGenericPointMaker.h"

#include "TH1F.h"
#include "TH2F.h"

ClassImp(StEEmcGenericPointMaker);

// ----------------------------------------------------------------------------
StEEmcGenericPointMaker::StEEmcGenericPointMaker( const Char_t *name, StEEmcA2EMaker *a2e, StEEmcGenericClusterMaker *cl ):StMaker(name)
{

  mEEanalysis=a2e;
  mEEclusters=cl;

  mEEtow=new EEmcGeomSimple();
  mEEsmd=EEmcSmdGeom::instance();
  mEEmap=EEmcSmdMap::instance(); 

  mKey=0;

}

//----------------------------------------------------------------------------
Int_t StEEmcGenericPointMaker::Init()
{

  //  hNumberOfPoints=new TH1F("hNumberOfPoints","N points",20,0.,20.);
  //  hEnergyOfPoints=new TH1F("hEnergyOfPoints","Energy spectrum of points",50,0.,25.);
  //  hDistributionOfPoints=new TH2F("hDistributionOfPoints",225,-225,225,225,-225,225);
  return StMaker::Init();
}

// ----------------------------------------------------------------------------
Int_t StEEmcGenericPointMaker::Make()
{

  return kStOK;
}

// ----------------------------------------------------------------------------
void StEEmcGenericPointMaker::addPoint( StEEmcPoint &p )
{

  p.key(nextPointId());
  mPoints.push_back(p);

  
  StEEmcClusterVec_t myclusters = p.clusters(0);
  StEEmcCluster mycluster = myclusters[0];
  //  printf("gen pointmaker adding key=%i E=%5.3f\n",mycluster.key(),mycluster.energy());

  mPoint2cluster[ p.key() ] = mycluster;



  // get key to the tower cluster associated with this point                                             
  Int_t key=mycluster.key();
  // if it's the first time this key is referenced, add a point vector to the map
  StEEmcPointVec_t mypoints;
  if ( !mCluster2points.count(key) ) mCluster2points[ key ] = mypoints;
  // add this point to the list of points associated with this cluster
  mCluster2points[ key ].push_back(p);

}

void StEEmcGenericPointMaker::addSmdPoint( StEEmcPoint &p )
{
  p.key(nextPointId());
  mSmdPoints.push_back(p);


  /**** do not count smd points in maps

  StEEmcClusterVec_t myclusters = p.clusters(0);
  StEEmcCluster mycluster = myclusters[0];
  //  printf("gen pointmaker addingsmd key=%i E=%5.3f\n",mycluster.key(),mycluster.energy());

  mPoint2cluster[ p.key() ] = mycluster;

  // get key to the tower cluster associated with this point
  Int_t key=p.clusters(0)[0].key();
  // if it's the first time this key is referenced, add a point vector to the map
  StEEmcPointVec_t mypoints;
  if ( !mCluster2points.count(key) ) mCluster2points[ key ] = mypoints;
  // add this point to the list of points associated with this cluster
  mCluster2points[ key ].push_back(p);

  ****/
  
}

void StEEmcGenericPointMaker::addTowerPoint( StEEmcPoint &p )
{
  p.key(nextPointId());
  mTowerPoints.push_back(p);
  mPoint2cluster[ p.key() ] = p.clusters(0)[0];

  /**** do not count tower points in maps

  // get key to the tower cluster associated with this point
  Int_t key=p.clusters(0)[0].key();
  // if it's the first time this key is referenced, add a point vector to the map
  StEEmcPointVec_t mypoints;
  if ( !mCluster2points.count(key) ) mCluster2points[ key ] = mypoints;
  // add this point to the list of points associated with this cluster
  mCluster2points[ key ].push_back(p);

  ****/

}

// ----------------------------------------------------------------------------
void StEEmcGenericPointMaker::Clear(Option_t *opts)
{
  mKey=0;
  mPoints.clear();
  mSmdPoints.clear();
  mTowerPoints.clear();
  mPoint2cluster.clear();
  mCluster2points.clear();
}

// ----------------------------------------------------------------------------
StEEmcPointVec_t StEEmcGenericPointMaker::buildPoints( StEEmcClusterVec_t &tower, StEEmcSmdClusterVec_t &smdu, StEEmcSmdClusterVec_t &smdv )
{
  StEEmcPointVec_t points;
  return points;
}

// ----------------------------------------------------------------------------
StEEmcPointVec_t StEEmcGenericPointMaker::buildSmdPoints( Int_t sector, StEEmcSmdClusterVec_t &u, StEEmcSmdClusterVec_t &v)
{
  StEEmcPointVec_t points;
  return points;
}

// ----------------------------------------------------------------------------
StEEmcPointVec_t StEEmcGenericPointMaker::buildTowerPoints(Int_t sector, StEEmcClusterVec_t &c )
{
  StEEmcPointVec_t points;
  return points;
}

// ----------------------------------------------------------------------------
void StEEmcGenericPointMaker::fillStEvent()
{

}
