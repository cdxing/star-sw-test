/***************************************************************************
 *
 * $Id: StHit.cxx,v 2.9 2001/03/24 03:34:50 perev Exp $
 *
 * Author: Thomas Ullrich, Sept 1999
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StHit.cxx,v $
 * Revision 2.9  2001/03/24 03:34:50  perev
 * clone() -> clone() const
 *
 * Revision 2.8  2001/03/06 21:04:30  ullrich
 * Modified detector() method. Replaced switch
 * statement by simple static_cast.
 *
 * Revision 2.7  2000/12/08 20:21:07  genevb
 * Changed kTofPatchId -> kTofId
 *
 * Revision 2.6  2000/07/28 23:29:42  calderon
 * Added handling of Fit Flag: use this flag to tell if the point
 * is used in the fit.
 *
 * Revision 2.5  2000/07/28 19:49:28  akio
 * Change in Detector Id for Endcap SMD
 *
 * Revision 2.4  2000/06/07 09:43:17  ullrich
 * Changed return type of flag() to UInt_t
 *
 * Revision 2.3  2000/06/01 21:38:53  ullrich
 * Added member mFlag and access member flag() and setFlag().
 *
 * Revision 2.2  2000/05/19 18:33:14  ullrich
 * Minor changes (add const) to cope with modified StArray.
 *
 * Revision 2.1  1999/10/28 22:25:47  ullrich
 * Adapted new StArray version. First version to compile on Linux and Sun.
 *
 * Revision 2.0  1999/10/12 18:42:17  ullrich
 * Completely Revised for New Version
 *
 **************************************************************************/
#include "StHit.h"
#include "StTrack.h"
#include "StTrackNode.h"
#include "StTrackDetectorInfo.h"

static const char rcsid[] = "$Id: StHit.cxx,v 2.9 2001/03/24 03:34:50 perev Exp $";

ClassImp(StHit)

StHit::StHit()
{
    mHardwarePosition = 0;
    mCharge = 0;
    mTrackRefCount = 0;
    mFlag = mFitFlag = 0;
}

StHit::StHit(const StThreeVectorF& p,
             const StThreeVectorF& e,
             ULong_t hp, Float_t q, UChar_t c)
    : StMeasuredPoint(p), mHardwarePosition(hp),
      mCharge(q), mTrackRefCount(c), mPositionError(e)
{
    mFlag = mFitFlag = 0;
}

StHit::~StHit() { /* noop */ }

StObject*
StHit::clone() const { return new StHit(*this); }
   
Int_t
StHit::operator==(const StHit& h) const
{
    return h.mPosition         == mPosition &&
           h.mPositionError    == h.mPositionError &&
           h.mCharge           == mCharge &&
           h.mHardwarePosition == mHardwarePosition &&
	   h.mFlag             == mFlag;
}

Int_t
StHit::operator!=(const StHit& h) const
{
    return !(*this == h);  // use operator==()
}

void
StHit::setCharge(Float_t val) { mCharge = val; }

void
StHit::setTrackReferenceCount(UChar_t val) { mTrackRefCount = val; }
    
void
StHit::setFlag(UChar_t val) { mFlag = val; }
    
void
StHit::setHardwarePosition(ULong_t val) { mHardwarePosition = val; }

void
StHit::setPositionError(const StThreeVectorF& e) { mPositionError = e; }
    
Float_t
StHit::charge() const { return mCharge; }

UInt_t
StHit::flag() const { return static_cast<UInt_t>(mFlag); }

Int_t
StHit::usedInFit() const { return static_cast<Int_t>(mFitFlag); } 

UInt_t
StHit::trackReferenceCount() const { return static_cast<UInt_t>(mTrackRefCount); }

StDetectorId
StHit::detector() const
{
    return static_cast<StDetectorId>(bits(0, 4));
}

StThreeVectorF
StHit::positionError() const { return mPositionError; }
   
StMatrixF
StHit::covariantMatrix() const
{
    // for now the diagonal elements is all we have
    StMatrixF m(3,3);
    m(1,1) = mPositionError.x()*mPositionError.x();
    m(2,2) = mPositionError.y()*mPositionError.y();
    m(3,3) = mPositionError.z()*mPositionError.z();
    return m;
}

StPtrVecTrack
StHit::relatedTracks(const StSPtrVecTrackNode& nodes, StTrackType type)
{
    StPtrVecTrack vec;
    StPtrVecHit hvec; 
    StDetectorId id = this->detector();

    for (unsigned int i=0; i<nodes.size(); i++) {
        const StTrackNode *node = nodes[i];
        unsigned int ntracks = node->entries(type);
        for (unsigned int k=0; k<ntracks; k++) {
            const StTrack *track = node->track(type, k);
            hvec = track->detectorInfo()->hits(id);
            for (unsigned int j=0; j<hvec.size(); j++)
                if (hvec[j] == this) vec.push_back(const_cast<StTrack*>(track));
        }
    }
    return vec;
}

