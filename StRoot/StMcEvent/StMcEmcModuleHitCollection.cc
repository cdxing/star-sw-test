/***************************************************************************
 *
 * $Id $
 *
 * Author: Aleksei Pavlinov, May 2000
 ***************************************************************************
 *
 * Description: Monte Carlo Emc Module Hit Collection class
 *
 ***************************************************************************
 *
 * $Log: StMcEmcModuleHitCollection.cc,v $
 * Revision 2.7  2007/12/14 20:23:50  calderon
 * From Adam Kocoloski: fix a memory leak, hits were not deleted in destructor.
 *
 * Revision 2.6  2007/10/05 00:01:21  calderon
 * Changes to include a EMC hit collection that does not care about
 * parent tracks, so that now there are two collections.  This
 * new collection will be useful to compare all the deposited energy in a hit tower
 * in a given event. The information about the track parentage is still
 * kept in the original collection unchanged.
 *
 * Revision 2.5  2005/08/09 03:31:01  perev
 * Cleanup
 *
 * Revision 2.4  2005/01/27 23:40:47  calderon
 * Adding persistency to StMcEvent as a step for Virtual MonteCarlo.
 *
 * Revision 2.3  2001/05/13 21:12:10  calderon
 * Modifications by Aleksei to the Emc Hit Collections on indexing of
 * module numbers
 *
 * Revision 2.2  2000/08/30 14:52:03  calderon
 * New changes made by Aleksei.
 *
 * Revision 2.1  2000/06/06 23:01:09  calderon
 * Inital revision
 *
 *
 **************************************************************************/
#include "StMcEmcModuleHitCollection.hh"
#include "StMcCalorimeterHit.hh"

static const char rcsid[] = "$Id: StMcEmcModuleHitCollection.cc,v 2.7 2007/12/14 20:23:50 calderon Exp $";

ClassImp(StMcEmcModuleHitCollection)

StMcEmcModuleHitCollection::StMcEmcModuleHitCollection(const unsigned int m)  
{init(m);}

StMcEmcModuleHitCollection::StMcEmcModuleHitCollection()  
{init(0);}

void StMcEmcModuleHitCollection::init(const unsigned int m)  
{
  //
  // m - module number
  //
  char name[10];
  sprintf(name,"m%3.3i",m);
  SetName(name);
}

StMcEmcModuleHitCollection::~StMcEmcModuleHitCollection()
{
    int n = mHits.size();
    for (int i=0; i<n; i++) {delete mHits[i];}
    mHits.clear();

    n = mDetectorHits.size();
    for (int i=0; i<n; i++) {delete mDetectorHits[i];}
    mDetectorHits.clear();
}

unsigned long 
StMcEmcModuleHitCollection::numberOfHits() const
{
    return mHits.size();
}

unsigned long
StMcEmcModuleHitCollection::numberOfDetectorHits() const
{
    return mDetectorHits.size();
}

float 
StMcEmcModuleHitCollection::sum() const
{
    float s = 0.0;
    for(unsigned int i=0; i<mHits.size(); i++){
	s += (*mHits[i]).dE();
    }
    return s;
} 

const StSPtrVecMcCalorimeterHit&
StMcEmcModuleHitCollection::hits() const { return mHits; }

StSPtrVecMcCalorimeterHit&
StMcEmcModuleHitCollection::hits() { return mHits; }

const StSPtrVecMcCalorimeterHit&
StMcEmcModuleHitCollection::detectorHits() const { return mDetectorHits; }

StSPtrVecMcCalorimeterHit&
StMcEmcModuleHitCollection::detectorHits() { return mDetectorHits; }

void StMcEmcModuleHitCollection::Browse(TBrowser *b)
{
  int nh = mHits.size();
  if (nh > 0) {
    cout<<" == " << GetName()<<" == hits "<<nh<<" == sum Energy " << sum()<<"\n";
    for (int ih=0; ih<nh; ih++){
      StMcCalorimeterHit *hitEmc = mHits[ih];
      cout<<"   "<<ih+1<<" "<<(*hitEmc);
    }
  }
}
