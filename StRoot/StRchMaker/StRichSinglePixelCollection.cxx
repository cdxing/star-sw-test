/****************************************************************
 * $Id: StRichSinglePixelCollection.cxx,v 1.3 2000/05/23 16:56:01 lasiuk Exp $
 *
 * Description:
 *  Container for cluster finder which allows access in
 *  a 1d or 2d format.
 *  Based on CERES CCollection, but uses a vector instead
 *  of RWPtrOrderedVector.  Uses STL copy and fill, instead
 *  of memset and memcpy
 *
 ****************************************************************
 *
 * $Log: StRichSinglePixelCollection.cxx,v $
 * Revision 1.3  2000/05/23 16:56:01  lasiuk
 * Incorporate new MC info
 * add clone() where necessary
 * accomodate name changes
 *
 * Revision 1.2  2000/05/09 21:55:38  lasiuk
 * deep copy for copy c'tor and assignment (jd)
 *
 * Revision 1.1  2000/04/05 16:39:54  lasiuk
 * Initial Revision
 ****************************************************************/

#include <memory>
//#include <string.h>  // only for memcpy and memset functions

#include "StGlobals.hh"

#include "StRichSinglePixelCollection.h"

StRichSinglePixelCollection::StRichSinglePixelCollection()
{
    mMinX = 0;
    mMaxX = 0;
    mMinY = 0;
    mMaxY = 0;
    mPixelArray = 0;
    mPixelVector.resize(0);
    mZero = 0;
}

StRichSinglePixelCollection::~StRichSinglePixelCollection()
{
    this->clearAndDestroy();
    delete [] mPixelArray;
}

StRichSinglePixelCollection::StRichSinglePixelCollection(const StRichSinglePixelCollection& old)
{
    //cout << "StRichSinglePixelCollection::StRichSinglePixelCollection()" << endl;
    // Correction from jcd.  Make a deep copy if
    // the pixel collection is to be passed by value.
    //mPixelVector = old.mPixelVector;
    mMinX        = old.mMinX;
    mMaxX        = old.mMaxX;
    mMinY        = old.mMinY;
    mMaxY        = old.mMaxY;

    int newx     = int(mMaxX-mMinX+1);
    int newy     = int(mMaxY-mMinY+1);

    mPixelArray  = new StRichSinglePixel*[newx*newy];
    if(!mPixelArray) {
	cerr << "StRichSinglePixelCollection::StRichSinglePixelCollection(const StRichSinglePixelCollection&)\n";
	cerr << "\tFATAL:\n";
	cerr << "\tCannot allocate memory for array.";
	cerr << "\tAborting..." << endl;
	abort();
    }
    
    StRichSinglePixel* fillValue = 0;
    uninitialized_fill_n(mPixelArray,newx*newy,fillValue);
    
    for (size_t ii=0; ii<old.mPixelVector.size(); ii++) {
	// Error handling?
	this->push_back( (old.mPixelVector[ii])->clone() );
        //this->push_back(new StRichSinglePixel(*(old.mPixelVector[ii])));
    }
    
//      memcpy( (void *)mPixelArray,
//  	    (const void *) old.mPixelArray,
//  	    (unsigned long)(newx*newy)*sizeof(StRichSinglePixel*) );
//     copy(old.mPixelArray,old.mPixelArray+(newx*newy),mPixelArray);
}

StRichSinglePixelCollection&
StRichSinglePixelCollection::operator=(const StRichSinglePixelCollection& old)
{
    //cout << "StRichSinglePixelCollection::operator=()" << endl;
    // change from jcd.  Make a deep copy.
    if(this != &old) {
	//
	// clear old stuff...do not destroy
	mPixelVector.clear();
	this->clearThePixelArray();
	delete [] mPixelArray;
	//mPixelVector = old.mPixelVector;
	
	mMinX        = old.mMinX;
	mMaxX        = old.mMaxX;
	mMinY        = old.mMinY;
	mMaxY        = old.mMaxY;
	int newx     = int(mMaxX-mMinX+1);
	int newy     = int(mMaxY-mMinY+1);
	
	mPixelArray  = new StRichSinglePixel* [newx*newy];
	if(!mPixelArray) {
	    cerr << "StRichSinglePixelCollection::operator=(const StRichSinglePixelCollection&)\n";
	    cerr << "\tFATAL:\n";
	    cerr << "\tCannot allocate memory for array.";
	    cerr << "\tAborting..." << endl;
	    abort();
	}
	StRichSinglePixel* fillValue = 0;
	uninitialized_fill_n(mPixelArray,newx*newy,fillValue);

	for (size_t ii=0; ii<old.mPixelVector.size(); ii++) {
	    // Error handling?
	    this->push_back( (old.mPixelVector[ii])->clone() );
	    //this->push_back(new StRichSinglePixel(*(old.mPixelVector[ii])));
	}
    }
// 	memcpy( (void *)mPixelArray,
// 		(const void*) old.mPixelArray,
// 		(unsigned long)(newx*newy)*sizeof(StRichSinglePixel*) );
//     copy(old.mPixelArray,old.mPixelArray+(newx*newy),mPixelArray);
    return *this;
}

void
StRichSinglePixelCollection::resize(size_t x, size_t y)
{
    //cout << "StRichSinglePixelCollection::resize()" << endl;
    if(x<0 || y<0) { //cannot have a -ive array size
	cerr << "StRichSinglePixelCollection::resize():\n"
	     << "\tERROR\n"
	     << "\tIllegal size of (" << x << ", " << y << ")"
	     << "\tRequest Ignored!" << endl;
    }
    if(x<mMaxX || y<mMaxY) { //make array smaller
	cerr << "StRichSinglePixelCollection::resize():\n"
	     << "\tWARNING\n"
	     << "\tResize request of (" << x << ", " << y << ")"
	     << "\tis smaller than existing array"
	     << "\tContinuing..." << endl;
    }

    mMaxX = x+mMinX-1;
    mMaxY = y+mMinY-1;

    long newsize = x*y;

    // clear before deleting?
    this->clearThePixelArray();
    delete [] mPixelArray;

    mPixelArray = new StRichSinglePixel* [newsize];
    if(!mPixelArray) {
	cerr << "StRichSinglePixelCollection::resize():\n"
	     << "\tERROR\n"
	     << "\tCannot allocate memory for 2-d array."
	     << "\tAborting" << endl;
	abort();
    }

    // fill 0 bytes into array...old and STL
    //memset( (void *)mPixelArray, (int)0, (int) (newsize)*sizeof(StRichSinglePixel*) );
    StRichSinglePixel* fillValue = 0;
    uninitialized_fill_n(mPixelArray,newsize,fillValue);
    
    //loop over list to fill array
    for(size_t ii=0; ii<mPixelVector.size(); ii++) {
	StRichSinglePixel* item = dynamic_cast<StRichSinglePixel*>(mPixelVector[ii]);
	int ix = item->pad();
	int iy = item->row();
	if( boundCheck(ix,iy) ) {
	    mPixelArray[where(ix,iy)] = item;
	}
    }
}

void
StRichSinglePixelCollection::resize(size_t i)
{
    mPixelVector.resize(i);
}

// ::clearThePixelArray() is a protected member
// function and is specified to reduce the amount
// of code that appears in other functions.
void
StRichSinglePixelCollection::clearThePixelArray()
{
    for(size_t ii=0; ii<mPixelVector.size(); ii++) {
	StRichSinglePixel* item =
	    dynamic_cast<StRichSinglePixel*>(mPixelVector[ii]);
	int x = item->pad();
	int y = item->row();
	if( boundCheck(x,y) ) {
	    mPixelArray[where(x,y)] = 0;
	}
    }
}

void
StRichSinglePixelCollection::clear()
{
    this->clearThePixelArray();
    mPixelVector.clear();
}

void
StRichSinglePixelCollection::clearAndDestroy()
{
    this->clearThePixelArray();
    
    for(size_t ii=0; ii<mPixelVector.size(); ii++) {
	delete mPixelVector[ii];
    }
    
    mPixelVector.clear();
}

bool
StRichSinglePixelCollection::push_back(StRichSinglePixel* pix)
{
    if(mPixelArray) {
	int x = pix->pad();
	int y = pix->row();
	if(boundCheck(x,y)) {
	    mPixelArray[where(x,y)] = pix;
	    mPixelVector.push_back(pix);
	    return true;
	}
	else {
	    return false;
	}
    }
    else {
	mPixelVector.push_back(pix);
	return true;
    }
}


StRichSinglePixel*
StRichSinglePixelCollection::front() const
{
    return mPixelVector.front();
}

StRichSinglePixel*
StRichSinglePixelCollection::back() const
{
    return mPixelVector.back();
}

size_t
StRichSinglePixelCollection::size() const
{
    return mPixelVector.size();
}

// Access with no bounds check
// if -DRWSTD_BOUNDS_CHECKING is defined, bounds check is always done! (HP)
StRichSinglePixel*
StRichSinglePixelCollection::operator()(size_t i) const
{
    return (mPixelVector[i]);
}

StRichSinglePixel*&
StRichSinglePixelCollection::operator()(size_t i)
{
    return ((StRichSinglePixel*&)mPixelVector[i]);
}

StRichSinglePixel*
StRichSinglePixelCollection::operator()(size_t x, size_t y) const
{
    StRichSinglePixel* ret = 0;
    if(mPixelArray && boundCheck(x,y)) 
	ret = (mPixelArray[where(x,y)]);
    return ret;
}


StRichSinglePixel*&
StRichSinglePixelCollection::operator[](size_t i)
{
    if(i<=mPixelVector.size())
	return(mPixelVector[i]);
    else {
	// should throw exception
	cerr << "StRichSinglePixelCollection::operator[]";
	cerr << "\tERROR:";
	cerr << "\tOut of Bounds";
	return mZero;
    }
}

StRichSinglePixel*
StRichSinglePixelCollection::operator[](size_t i) const
{
    if(i<=mPixelVector.size())
	return(mPixelVector[i]);
    else {
	// should throw exception
	cerr << "StRichSinglePixelCollection::operator[]";
	cerr << "\tERROR:";
	cerr << "\tOut of Bounds";
	return mZero;
    }
}

// StRichSinglePixelCollection::


// both can be inline
size_t
StRichSinglePixelCollection::where(size_t x, size_t y) const
{
    return (y-mMinY)*((mMaxX+1)-mMinX) + (x-mMinX); 
}

bool
StRichSinglePixelCollection::boundCheck(size_t x, size_t y) const
{
    return( (x>=mMinX && x<= mMaxX) &&
	    (y>=mMinY && y<= mMaxY) );
}
