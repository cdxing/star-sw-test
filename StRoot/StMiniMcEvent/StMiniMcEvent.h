/**
 * $Id $
 * \file  StMiniMcEvent.h
 * \brief Top level class for the MiniMcTree, containing event-wise information and the McTrack, and all TrackPair
 *        collections.
 * 
 *
 * \author Bum Choi
 * \date   March 2001
 *  
 *
 *  used jeff reid and raimond's makers as references.
*/

#ifndef StMiniMcEvent_H
#define StMiniMcEvent_H

#include "TObject.h"
#include "TClonesArray.h"
class StTinyRcTrack;
class StTinyMcTrack;
class StMiniMcPair;
class StContamPair;

enum Category { MC,MATCHED,MERGED,SPLIT,CONTAM,GHOST};

class StMiniMcEvent : public TObject {
 public:
  StMiniMcEvent();
  virtual ~StMiniMcEvent();
  void Clear(Option_t *option=""); // clear the tracks

  void setCentrality(Int_t nTrack);

  void addMcTrack(StTinyMcTrack*);
  void addTrackPair(StMiniMcPair*, Category);

  TClonesArray* tracks(Category);

  //
  // data members
  //
  Int_t         mEventId;
  Int_t         mRunId;       // set to 0 for simulations
  Int_t         mOriginMult;  // uncorrected n primary track
  Int_t         mCentralMult; // corrected n primary track
  Int_t         mCentrality;  // same as the flow paper
  Int_t         mNUncorrectedNegativePrimaries; //
  Int_t         mNUncorrectedPrimaries; //
  Int_t         mMcMult;      // embedding: n mc tracks;
                              // simulation: same as mOriginMult 
  Float_t       mVertexX;  
  Float_t       mVertexY;     // 
  Float_t       mVertexZ;
  Float_t       mMcVertexX;
  Float_t       mMcVertexY;
  Float_t       mMcVertexZ;
    
  Float_t       mMagField;    // in kGauss

  Float_t       mCTB;
  Float_t       mZDCe;
  Float_t       mZDCw;

  //
  // for root reasons, even though ghosts are just the rc tracks,
  // save it as a StMiniMcPair ... 

  Int_t mNMcTrack;     
  Int_t mNMatchedPair;
  Int_t mNMergedPair;
  Int_t mNSplitPair;
  Int_t mNGhostPair;
  Int_t mNContamPair;

  TClonesArray* mMcTracks;
  TClonesArray* mMatchedPairs;
  TClonesArray* mMergedPairs;
  TClonesArray* mSplitPairs;
  TClonesArray* mGhostPairs; 
  TClonesArray* mContamPairs;

  static Int_t mSFirst; //!

  ClassDef(StMiniMcEvent,1)
};

#endif

  
//
// $Log: StMiniMcEvent.h,v $
// Revision 1.3  2003/05/08 02:09:20  calderon
// Added data members for svt and ftpc fit points for StTinyRcTrack.
// Added data members for svt and ftpc hits for StTinyMcTrack.
// Added methods to calculate px, py, and p from the available pt,  phi and pz, for
// global and primary momenta and also for monte carlo momentum.
// Cleaned up includes in StMiniMcEvent.
//
// Revision 1.2  2002/06/06 15:14:13  calderon
// the comment about the magnetic field said it is in tesla, but it is actually
// in kGauss, so I modified the comment to reflect this
//
// Revision 1.1  2002/05/30 01:20:57  calderon
// Classes for use in a general framework for extracting efficiencies
// from both embedding and full simulations
// (after GSTAR+TRS+StEvent+StMcEvent+StAssociationMaker)
// so that the information of the track matches gets stored persistently.
//
//
