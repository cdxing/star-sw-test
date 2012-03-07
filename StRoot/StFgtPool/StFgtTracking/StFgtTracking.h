/***************************************************************************
 *
 * $Id: StFgtTracking.h,v 1.1 2012/03/07 15:38:04 sgliske Exp $
 * Author: S. Gliske, March 2012
 *
 ***************************************************************************
 *
 * Description: Abstract base class for FGT tracking.
 *
 ***************************************************************************
 *
 * $Log: StFgtTracking.h,v $
 * Revision 1.1  2012/03/07 15:38:04  sgliske
 * creation
 *
 * Revision 1.1  2012/03/06 01:32:35  sgliske
 * creation
 *
 *
 **************************************************************************/

#ifndef _ST_FGT_TRACKING_
#define _ST_FGT_TRACKING_

#include "StMaker.h"
#include <vector>
#include <TVector3.h>

#include "StRoot/StFgtUtil/StFgtConsts.h"

// reduced amount of data for a cluster
struct StFgtTrClus {
   Int_t idx;
   Float_t pos;

   StFgtTrClus( Int_t i = -1, Float_t p = 0 ) : idx(i), pos(p) { /* */ };
};

// basic space point info for tracking
struct StFgtTrPoint {
   Int_t rIdx, pIdx;
   TVector3 pos;

   StFgtTrPoint( Int_t rI = -1, Int_t pI = -1 ) : rIdx(rI), pIdx(pI) { /* */ };
   StFgtTrPoint( const StFgtTrClus& rClus, const StFgtTrClus& pClus, Float_t z ) : rIdx(rClus.idx), pIdx(pClus.idx), pos(rClus.pos,0,z) {
      pos.SetPhi(pClus.pos);
   };
};


typedef std::vector< StFgtTrPoint > StFgtTrPointVec;
typedef std::vector< StFgtTrClus > StFgtTrClusVec;

class StFgtTracking : public StMaker {
 public:
   // constructors
   StFgtTracking( const Char_t* name = "fgtTracking" );

   // deconstructor
   virtual ~StFgtTracking();

   // default equals operator and copy constructor OK

   virtual Int_t Make();
   virtual Int_t Init();
   virtual void Clear( const Option_t *opt = "" );

 protected:
   // array of clusters and points per disc
   StFgtTrClusVec mRclusVecPerDisc[ kFgtNumDiscs ];
   StFgtTrClusVec mPclusVecPerDisc[ kFgtNumDiscs ];
   StFgtTrPointVec mPointVecPerDisc[ kFgtNumDiscs ];

   // to keep track of total number of points per event
   Int_t mPointsTot;

   // functions
   Int_t computePointsFromStEvent();
   Int_t computePointsFromMuDst();
   Int_t makePoints();

   // virtual function to find the tracks, given that the
   // 'pointVecPerDisc' has been properly set for the event.
   virtual Int_t findTracks() = 0;

 private:   
   ClassDef(StFgtTracking,1);

}; 

#endif
