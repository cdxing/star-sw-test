/*********************************************************************
 * $Id: StRichIonization.cxx,v 1.7 2000/03/12 23:56:34 lasiuk Exp $
 *
 * Description:
 *  StRichIonization simulates the charged particle track through the gas.
 *  The main parameters of the process are the average number of
 *  ionization clusters per length unit and the distribution of
 *  ionized electrons in each of those clusters.
 *
 *  The function first randomly (poisson) choses a number of
 *  interaction clusters on the electrons path through the gas
 *  and then determines also randomly (e_distribut) how
 *  many electrons were ionized at each cluster. Each electron
 *  is then passed to the InduceSignal algorithm.
 *
 *  e_distribut: p is chosen and then compared to the Nth
 *  column of e_distribut. If p fails the "greater than" test
 *  then the number of electrons ionized is given by N. If not
 *  the same check is done with the N+1 column.
 *
 * Implementation of the StRichIonization function object.
 *
 *********************************************************************
 * $Log: StRichIonization.cxx,v $
 * Revision 1.7  2000/03/12 23:56:34  lasiuk
 * new coordinate system
 * exchange MyRound with inline templated funtion
 *
 * Revision 1.8  2000/03/17 14:54:44  lasiuk
 * Large scale revisions after ROOT dependent memory leak
 *
 * Revision 1.7  2000/03/12 23:56:34  lasiuk
 * new coordinate system
 * exchange MyRound with inline templated funtion
 *
 * Revision 1.6  2000/02/14 01:14:06  lasiuk
 * add track_p to GHit c'tor
 *
 * Revision 1.5  2000/02/11 21:11:37  lasiuk
 * use new name in physics db interface
 *
 * Revision 1.4  2000/02/08 23:51:13  lasiuk
 * removal of rrs macro---CC4.2 cannot handle it!
 *
 * Revision 1.3  2000/02/08 16:27:34  lasiuk
 * change to class.  Put dbs and random generators into
 * class as data members
 *
 * Revision 1.2  2000/01/25 22:02:21  lasiuk
 * Second Revision
 *
 *********************************************************************/
//namespace StRichRawData {
#include "PhysicalConstants.h"

#ifndef ST_NO_NAMESPACES
#endif
#include "StRichInduceSignal.h"
#include "StRichGHit.h"
#include "StRichIonization.h"
#include "StRichPhysicsDb.h"

#ifdef RICH_WITH_VIEWER
#include "StRichViewer.h"
#endif

StRichIonization::StRichIonization()
{
	log(1.649*mIonize/eV/(28.8*sqrt(mDensity*centimeter3/gram*mZa)));

    mF = 4.606*(mIntersectionOfRiseAndPlateau-mFirstCorner)/(pow((mPlateau-mFirstCorner),mShapeOfRise));   
}
void StRichIonization::operator()(const StRichGHit& hit ) 
StRichIonization::~StRichIonization() { /* nopt */ }
    StRichInduceSignal induceSignal;


void StRichIonization::splitSegment(const StRichGHit* hit, list<StRichMiniHit*>& aList) const
{
    // locals
	
    int numberOfInteractions = mRandom.Poisson( nearestInteger(hit.ds() * mAverageNumberOfInteractions) );
    //PR(numberOfInteractions);
//     PR(numberOfInteractions);
//     sleep(1);

	t = hit.ds() * ( mRandom.Flat(1.0) - 0.5 );
	//PR(t);
	//
	// t between -.5 and .5
	x = hit.position().x()  +  t * hit.cosX();
	y = hit.position().y()  +  t * hit.cosY();
	z = hit.position().z()  +  t * hit.cosZ();
	x = hit->position().x()  +  t * hit->cosX();
	y = hit->position().y()  +  t * hit->cosY();
	z = hit->position().z()  +  t * hit->cosZ();
	
	//p = random.Flat(1.0) * mPhysicsDb->e_max;
	//
	// random selector from 0 to e_max
	//
	p = mRandom.Flat(1.0) * mMaximumElectronEnergyProbability;
	
	// number of electrons in this cluster
	//
	n = 1;
	while ( n <= mPhysicsDb->e_distribut.size() && p >= mPhysicsDb->e_distribut[n-1] )
	    n++;
	
#ifdef RICH_WITH_VIEWER
	if (StRichViewer::histograms )
	    StRichViewer::getView()->mClusterElectrons->Fill(n);
#endif
	    StRichGHit aGHit(x,y,z,hit.trackp(), hit.id());
	    induceSignal(aGHit);
					      hit->id(),
					      hit->mass(),
					      eCharged));
	}
    }
#ifndef ST_NO_NAMESPACES
//}
#endif

    return (I/Io);
}
