/***************************************************************************
 *
 * $Id: StTrsSector.cc,v 1.4 1999/02/12 01:26:37 lasiuk Exp $
 *
 * Author: 
 ***************************************************************************
 *
 * Description: 
 *
 ***************************************************************************
 *
 * $Log: StTrsSector.cc,v $
 * Revision 1.4  1999/02/12 01:26:37  lasiuk
 * Limit debug output
 *
 * Revision 1.4  1999/02/12 01:26:37  lasiuk
 * Limit debug output
 *
 * Revision 1.3  1999/01/18 21:02:57  lasiuk
 * comment diagnostics
 *
 * Revision 1.2  1998/11/16 14:49:06  lasiuk
 * make sure of index when filling sector
 *
 * Revision 1.1  1998/11/10 17:12:26  fisyak
 * Put Brian trs versin into StRoot
 *
 * Revision 1.3  1998/11/08 17:06:41  lasiuk
 * use resize() for LINUX compatibiltiy
 * allocator specification for vector<>
 *
 * Revision 1.2  1998/11/04 18:49:40  lasiuk
 * modify constructors
 * macro ST_SECTOR_BOUND_CHECK
 * addEntry()
 * addTimeBins()
 *
 * Revision 1.1  1998/06/30 22:46:50  lasiuk
 * Initial Revision
 *
 **************************************************************************/
#include "StTrsSector.hh"

StTrsSector::StTrsSector(StTpcGeometry* geoDb)
{
    tpcTimeBins  timeBins;
    tpcPadRow    padRow;
    int          irow;

    for(irow=0; irow< geoDb->numberOfRows(); irow++) {
	//cout << " NumberOfPadsAtRow(" << irow << "): " << geoDb->numberOfPadsAtRow(irow+1) << endl;
	//padRow.assign(geoDb->numberOfPadsAtRow(irow+1), timeBins);
	padRow.resize(geoDb->numberOfPadsAtRow(irow+1), timeBins);
	mSector.push_back(padRow);
    }

    
    // tmp
    // check size at creation?
//     cout << "  NumberOfRows in Sector: " << mSector.size() << endl;
//     for(int ii=0; ii<mSector.size(); ii++) {
//  	cout << "  PadsInRow(" << ii << "): " << mSector[ii].size() << endl;
//     }
}

StTrsSector::~StTrsSector() {/* nopt */}

void StTrsSector::clear() // clears only the time bins
{
    cout << "in StTrsSector::clear()" << endl;
    for(int irow=0; irow<mSector.size(); irow++) {
	for(int ipad=0; ipad<mSector[irow].size(); ipad++) {
	    mSector[irow][ipad].clear();
	}
    }
}

// Caution: rowN specifies rowNumber 1..45
// Below, rowIndex specifies index 0..44
void StTrsSector::addEntry(int rowN, int padN, StTrsAnalogSignal& signl)
{
    //cout << "rowN " << rowN << " padN " << padN << endl;
#ifdef ST_SECTOR_BOUNDS_CHECK
    if( (rowN > 0 && row <= mSector.size()) )
	if( (padN > 0 && pad <= mSector[(rowN-1)].size()) )
#endif
	    mSector[(rowN-1)][(padN-1)].push_back(signl);
}

void StTrsSector::addEntry(StTpcPadCoordinate& coord, StTrsAnalogSignal& sig)
{
    addEntry(coord.row(), coord.pad(), sig);
}

// Caution: rowIndex specifies index 0..44
// Above: rowN specifies rowNumber 1..45
void StTrsSector::assignTimeBins(int rowN, int padN, tpcTimeBins& tbins)
{
#ifdef ST_SECTOR_BOUNDS_CHECK
    if( (rowIndex > 0 && rowIndex <= mSector.size()) )
	if( (padIndex > 0 && padIndex <= mSector[rowIndex].size()) )
#endif
	    mSector[(rowN-1)][(padN-1)] = tbins;
}

void StTrsSector::assignTimeBins(StTpcPadCoordinate& coord, tpcTimeBins& tbins)
{
    assignTimeBins(coord.row(), coord.pad(), tbins);
}
