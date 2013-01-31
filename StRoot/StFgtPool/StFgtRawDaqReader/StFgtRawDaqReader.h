/*
 *
 * \class StFgtRawMaker
 * \author S. Gliske (sgliske@anl.gov) based on StFgtComsicReader v1.15 written by A. Vossen (avossen@indiana.edu)
 *
 * Reads in a DAQ or SFS file taken on the cosmic test stand or from
 * the actual DAQ machine, and fills the StEvent FGT containers.
 * Should replace the StFgtComsicReader.
 *
 */

#ifndef STAR_StFgtRawDaqReader_HH
#define STAR_StFgtRawDaqReader_HH

#include "StRoot/St_base/Stypes.h"
#include "StMaker.h"
class daqReader;
class StFgtDbMaker;
class StFgtDb;

class StFgtCollection;

class StFgtRawDaqReader : public StMaker {
 public: 
   StFgtRawDaqReader( const Char_t* name = "fgtRawDaqReader", const Char_t *daqFileName = "", const Char_t* dbMkrName = "" );
   virtual ~StFgtRawDaqReader();

   void setFilename( std::string filename );

   virtual Int_t Init();
   virtual Int_t Make();
   virtual void Clear( Option_t *opts = "" );
   virtual Int_t prepareEnvironment();

   void cutShortEvents( Bool_t doIt = 1 );
   void setIsCosmic( Bool_t itIs = 1 );

   void setAlldata()       {mDataType=0;};
   void setNoneZSdataOnly(){mDataType=1;};
   void setZSdataOnly()    {mDataType=2;};

   // Get CVS
   virtual const char *GetCVS() const;

 protected:
   Bool_t mCutShortEvents, mIsCosmic;
   Int_t mDate, mTime;
   StFgtCollection *mFgtCollectionPtr;

 private:
   std::string mDaqFileName, mDbMkrName;
   daqReader *mRdr;
   StFgtDbMaker *mFgtDbMkr;
   int mDataType; //!  0=adc, and if not there zs. 1=adc only, 2=zs only

   ClassDef(StFgtRawDaqReader,1);
};

// inline functions

inline void StFgtRawDaqReader::setFilename( std::string filename ){ mDaqFileName = filename; };
inline void StFgtRawDaqReader::cutShortEvents( Bool_t doIt ){ mCutShortEvents = doIt; };
inline void StFgtRawDaqReader::setIsCosmic( Bool_t itIs ){ mIsCosmic = itIs; };
inline const char *StFgtRawDaqReader::GetCVS() const {
   static const char cvs[] = "Tag $Name:  $ $Id: StFgtRawDaqReader.h,v 1.4 2013/01/31 20:00:32 akio Exp $ built "__DATE__" "__TIME__ ;
   return cvs;
};

#endif

/*
 * $Id: StFgtRawDaqReader.h,v 1.4 2013/01/31 20:00:32 akio Exp $
 * $Log: StFgtRawDaqReader.h,v $
 * Revision 1.4  2013/01/31 20:00:32  akio
 * adding obtaining number of timebins from meta data
 * adding options for zero suppressed data
 *
 * Revision 1.3  2012/01/31 11:23:34  sgliske
 * No longer requires passing name of StFgtDbMaker
 *
 * Revision 1.2  2012/01/31 09:16:55  sgliske
 * fixed cvs caption
 *
 * Revision 1.1  2012/01/31 09:15:34  sgliske
 * Moved to StFgtPool
 *
 * Revision 1.2  2012/01/26 11:38:33  sgliske
 * added GetCVS()
 *
 * Revision 1.1  2012/01/17 20:10:54  sgliske
 * creation
 *
 *
 */
