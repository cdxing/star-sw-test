#include "StFgtCosmicMaker.h"
#include "StRoot/StFgtUtil/geometry/StFgtGeomDefs.h"
#include "StRoot/StFgtUtil/geometry/StFgtCosmicTestStandGeom.h"
#include "RTS/src/DAQ_FGT/daq_fgt.h"
#include "RTS/src/DAQ_READER/daq_dta.h"
#include "StRoot/StEvent/StFgtEvent/StFgtEvent.h"

StFgtCosmicMaker::StFgtCosmicMaker( const Char_t* name, const Char_t *daqFileName ) :
   StFgtRawBase(), StMaker( name ), mDaqFileName( daqFileName ), mRdr(0)
{
   //LOG_INFO << "OK?" << endm;

   // nothing else to do
};

StFgtCosmicMaker::~StFgtCosmicMaker(){
   if( mRdr )
      delete mRdr;
};

Int_t StFgtCosmicMaker::Init(){
   cerr << "initializing" << endl;

   Int_t ierr = constructFgtEvent();

   cerr << "event constructed" << endl;

   if( ierr || !mFgtEventPtr )
      {
         cerr << "error" << endl;
         LOG_FATAL << "Error constructing FgtEvent" << endm;
         ierr = kStFatal;
      };

  cerr << "constructing daqReader" << endl;

  // unfortunately, the daqReader has some constness issues to be
  // fixed.  Until they are, must remove constness of the filename.
  mRdr = new daqReader( const_cast< Char_t* >( mDaqFileName.data() ) ); 	

  cerr << "done with init" << endl;

  return ierr;
};

//read next event from daq file and fill the fgtevent
Int_t StFgtCosmicMaker::Make()
{

   //Short_t quadrant=0;      
   //Char_t layer=0;
   //Double_t ordinate=0;
   //Double_t lowerSpan=0;
   //Double_t upperSpan=0;
  Int_t rdo=0;
  Int_t arm=0;
  Int_t apv=0;
  Int_t channel=0;
  Short_t adc=0;
  Short_t timebin=0;

  //char *ret =
  mRdr->get(0,EVP_TYPE_ANY);
  if(mRdr->status == EVP_STAT_EOR) {
     //LOG_DEBUG <<"End of File reached..."<<endm;
     //return kStEOF;	
     cout <<"End of File reached..."<<endl;
     mEOF = 1;
     return kStOk;
  }
  daq_dta *dd = 0;
  dd = mRdr->det("fgt")->get("adc");
      
  //cout << "z starting event " << GetEventNumber() << ", dd at " << dd << endl;

  while(dd && dd->iterate()) 
    {
      fgt_adc_t *f = (fgt_adc_t *) dd->Void ;

//       cout << "y Arm, Apv, Rdo = " << arm << ' ' << apv << ' ' << rdo
//            << ", number of hits " << dd->ncontent << endl;

      for(u_int i=0;i<dd->ncontent;i++) 
	{
	  channel=f[i].ch;
	  adc=f[i].adc;
	  arm=dd->sec;
	  apv=dd->pad;
	  rdo=dd->rdo;
	  timebin=f[i].tb;
          Short_t discIdx=0;  // will be set with getNaivePhysCoordFromElecCoord

	  Short_t geoId = StFgtCosmicTestStandGeom::getNaiveGeoIdFromElecCoord(rdo,arm,apv,channel);

          // remove hack
//        Short_t quad=0;
//        Short_t strip=0;
//        Char_t layer=0;
// 	  StFgtGeom::decodeGeoId( geoId, discIdx, quad, layer, strip );
// 	  StFgtGeom::getNaivePhysCoordFromElecCoord(rdo,arm,apv,channel,discIdx,quadrant,layer,ordinate,lowerSpan,upperSpan);
//        if( arm == 0 ){
//           discIdx = 0;
//        } else if ( apv < 12 ) {
//           discIdx = 1;
//        } else {
//           discIdx = 2;
//           apv -= 12;
//        };
//        geoId = StFgtGeom::encodeGeoId( discIdx, 0, layer, strip );

          Char_t type = 0;    // raw adc, no correction yet.
	  StFgtRawHit hit(geoId,adc,type,timebin);
	  StFgtDisc* pDisc=mFgtEventPtr->getDiscPtr(discIdx);

          // HACK: since don't yet have geoId->APV/channel mapping,
          // encode in the charge field for now
          // hit.setCharge( 128*apv + channel );

	  if(pDisc)
	    pDisc->getRawHitArray().PushBack(hit);
	  else
	    { LOG_WARN <<"Could not access disc "<<endm; }
	}
    }

  // clear events that do not have a complete set of channels
  Bool_t eventOK = 1;
  for( UInt_t i = 0; i < mNumDiscs && eventOK; ++i ){
     StFgtDisc* pDisc=mFgtEventPtr->getDiscPtr( i );
     if( pDisc )
        eventOK = ( pDisc->getNumRawHits() == 7*1280 );
  };
  if( !eventOK ){
     for( UInt_t i = 0; i < mNumDiscs; ++i ){
        StFgtDisc* pDisc=mFgtEventPtr->getDiscPtr( i );
        if( pDisc )
           pDisc->ClearRawHitArray();
     };
  };

  return kStOk;
};

void StFgtCosmicMaker::Clear( Option_t *opts )
{
   if( mFgtEventPtr )
      mFgtEventPtr->Clear( opts );
};

ClassImp(StFgtCosmicMaker);
