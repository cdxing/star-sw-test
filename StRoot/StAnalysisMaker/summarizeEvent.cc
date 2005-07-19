/***************************************************************************
 *
 * $Id: summarizeEvent.cc,v 2.6 2005/07/19 20:08:17 perev Exp $
 *
 * Author: Torre Wenaus, BNL,
 *         Thomas Ullrich, Nov 1999
 ***************************************************************************
 *
 * Description:  This is an example of a function which performs
 *               some simple analysis using StEvent.
 *               Use this as a template and customize it for your
 *               studies.
 *
 ***************************************************************************
 *
 * $Log: summarizeEvent.cc,v $
 * Revision 2.6  2005/07/19 20:08:17  perev
 * MultiVertex corr
 *
 * Revision 2.5  2005/06/22 22:19:37  fisyak
 * Add protection for absence svtHitCollection
 *
 * Revision 2.4  2005/06/22 16:09:01  fisyak
 * Add summary of quality
 *
 * Revision 2.3  2000/07/12 05:24:39  ullrich
 * Minor updates to cope with revised StAnalysisMaker.
 *
 * Revision 2.2  2000/01/05 16:06:27  ullrich
 * Added SSD hits.
 *
 * Revision 2.1  1999/11/16 12:28:44  ullrich
 * Corrected typo and added print-out of number of primary tracks.
 *
 * Revision 2.0  1999/11/04 16:10:11  ullrich
 * Revision for new StEvent
 *
 **************************************************************************/
#include "StContainers.h"
#include "StEventTypes.h"
#include "StMessMgr.h"

static const char rcsid[] = "$Id: summarizeEvent.cc,v 2.6 2005/07/19 20:08:17 perev Exp $";

void
summarizeEvent(StEvent& event, const int &nevents)
{
  static const UInt_t NoFitPointCutForGoodTrack = 15;
    gMessMgr->QAInfo() << "StAnalysisMaker,  Reading Event: " << nevents
		       << "  Type: " << event.type()
		       << "  Run: " << event.runId() << endm;
    
    StSPtrVecTrackNode& trackNode = event.trackNodes();
    UInt_t nTracks = trackNode.size();
    StTrackNode *node = 0;
    UInt_t nGoodTracks = 0;
    for (unsigned int i=0; i < nTracks; i++) {
      node = trackNode[i]; if (!node) continue;
      StGlobalTrack* gTrack = static_cast<StGlobalTrack*>(node->track(global));
      if (! gTrack) continue;
      if (gTrack->fitTraits().numberOfFitPoints() <  NoFitPointCutForGoodTrack) continue;
      nGoodTracks++;
    }
    gMessMgr->QAInfo() << "# track nodes:   \t"
		       <<  nTracks << ":\tglobals with NFitP>="<< NoFitPointCutForGoodTrack << ":\t" << nGoodTracks << endm;
    StPrimaryVertex *pVertex=0;
    for (int ipr=0;(pVertex=event.primaryVertex(ipr));ipr++) {
      StThreeVectorD primPos = pVertex->position();
      int nDaughters = pVertex->numberOfDaughters();
      nGoodTracks = 0;
      for (unsigned int i=0; i < nDaughters; i++) {
        StPrimaryTrack* pTrack = (StPrimaryTrack*)pVertex->daughter(i);
        if (pTrack->fitTraits().numberOfFitPoints() <  NoFitPointCutForGoodTrack) continue;
        nGoodTracks++;
      }
      gMessMgr->QAInfo() << "# primary vertex("<<ipr<<"): \t"<<primPos<<endm;

      gMessMgr->QAInfo() << "# primary tracks:\t"
		         << nDaughters << ":\tones    with NFitP(>="<< NoFitPointCutForGoodTrack << "):\t" << nGoodTracks << endm;
    }// end prim vtx    
    
    gMessMgr->QAInfo() << "# V0 vertices:       "
		       << event.v0Vertices().size() << endm;

    gMessMgr->QAInfo() << "# Xi vertices:       "
		       << event.xiVertices().size() << endm;
    
    gMessMgr->QAInfo() << "# Kink vertices:       "
		       << event.kinkVertices().size() << endm;

    UInt_t TotalNoOfTpcHits = 0, noBadTpcHits = 0, noTpcHitsUsedInFit = 0;
    StTpcHitCollection* TpcHitCollection = event.tpcHitCollection();
    if (TpcHitCollection) {
      UInt_t numberOfSectors = TpcHitCollection->numberOfSectors();
      for (UInt_t i = 0; i< numberOfSectors; i++) {
	StTpcSectorHitCollection* sectorCollection = TpcHitCollection->sector(i);
	if (sectorCollection) {
	  Int_t numberOfPadrows = sectorCollection->numberOfPadrows();
	  for (int j = 0; j< numberOfPadrows; j++) {
	    StTpcPadrowHitCollection *rowCollection = sectorCollection->padrow(j);
	    if (rowCollection) {
	      StSPtrVecTpcHit &hits = rowCollection->hits();
	      UInt_t NoHits = hits.size();
	      for (UInt_t k = 0; k < NoHits; k++) {
		StTpcHit *tpcHit = static_cast<StTpcHit *> (hits[k]);
		if (tpcHit) {
		  TotalNoOfTpcHits++;
		  if ( tpcHit->flag()) noBadTpcHits++;
		  if (tpcHit->usedInFit()) noTpcHitsUsedInFit++;
		}
	      }
	    }
	  }
	}
      }
    }
    gMessMgr->QAInfo() << "# TPC hits:          " << TotalNoOfTpcHits 
		       << ":\tdeconvoluted:     " << noBadTpcHits 
		       << ":\tUsed in Fit:      " << noTpcHitsUsedInFit << endm;
    
    UInt_t TotalNoOfSvtHits = 0, noBadSvtHits = 0, noSvtHitsUsedInFit = 0;
    StSvtHitCollection* svthits = event.svtHitCollection();
    if (svthits) {
      StSvtHit* hit;
      for (unsigned int barrel=0; barrel<svthits->numberOfBarrels(); ++barrel) {
	StSvtBarrelHitCollection* barrelhits = svthits->barrel(barrel);
	if (!barrelhits) continue;
	for (unsigned int ladder=0; ladder<barrelhits->numberOfLadders(); ++ladder) {
	  StSvtLadderHitCollection* ladderhits = barrelhits->ladder(ladder);
	  if (!ladderhits) continue;
	  for (unsigned int wafer=0; wafer<ladderhits->numberOfWafers(); ++wafer) {
	    StSvtWaferHitCollection* waferhits = ladderhits->wafer(wafer);
	    if (!waferhits) continue;
	    const StSPtrVecSvtHit& hits = waferhits->hits();
	    for (const_StSvtHitIterator it=hits.begin(); it!=hits.end(); ++it) {
	      hit = static_cast<StSvtHit*>(*it);
	      if (!hit) continue;
	      TotalNoOfSvtHits++;
	      if (hit->flag() > 5) noBadSvtHits++;
	      if (hit->usedInFit()) noSvtHitsUsedInFit++;
	    }
	  }
	}
      }
    }
    gMessMgr->QAInfo() << "# SVT hits:          " << TotalNoOfSvtHits 
		       << ":\tBad ones(flag>5): " << noBadSvtHits 
		       << ":\tUsed in Fit:      " << noSvtHitsUsedInFit << endm;
    
    gMessMgr->QAInfo() << "# SSD hits:       "
		       << (event.ssdHitCollection() ? event.ssdHitCollection()->numberOfHits() : 0) << endm;
    
    gMessMgr->QAInfo() << "# FTPC hits:      "
		       << (event.ftpcHitCollection() ? event.ftpcHitCollection()->numberOfHits() : 0) << endm;
    
}
