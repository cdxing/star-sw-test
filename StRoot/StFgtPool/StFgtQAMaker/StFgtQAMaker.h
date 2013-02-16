/*!
 * \class  StFgtQAMaker
 * \brief  A FGT QA Class
 * \author Akio
 * \date   Dec2012
 *
 * $Id: StFgtQAMaker.h,v 1.2 2013/02/06 21:17:18 akio Exp $
 *
 */
/* -------------------------------------------------------------------------
 * $Log: StFgtQAMaker.h,v $
 * Revision 1.2  2013/02/06 21:17:18  akio
 * some adjustments & adding macro
 *
 * Revision 1.1  2013/02/06 17:43:19  akio
 * new unified QA maker
 *
 * Revision 1.1  2013/02/05 21:08:01  akio
 * *** empty log message ***
 *
 * -------------------------------------------------------------------------
 */

#ifndef StFgtQAMaker_hh     
#define StFgtQAMaker_hh

#include "TH1F.h"
#include "TH2F.h"

#include "StMaker.h"
#include "TString.h"
#include "StEnumerations.h"
#include "StRoot/StFgtDbMaker/StFgtDbMaker.h"
#include "StRoot/StFgtDbMaker/StFgtDb.h"

class StFgtQAMaker : public StMaker {
public:
  
  StFgtQAMaker(const Char_t *name="fgtqa");     // constructor
  ~StFgtQAMaker() {}                            // destructor
  
  Int_t  Init();                      // called once for initilization
  Int_t  InitRun(Int_t runnum);       // called once per run
  Int_t  Make();                      // invoked for every event
  Int_t  Finish();                    // called once at the end  

  inline void setRunNumber(Int_t v) {mRunNumber=v;}

  virtual const char *GetCVS() const {
    static const char cvs[]="Tag $Name:  $ $Id: StFgtQAMaker.h,v 1.2 2013/02/06 21:17:18 akio Exp $ built "__DATE__" "__TIME__ ; 
    return cvs;
  };
  
protected:    
  void bookHist();
  void fillHist();
  void saveHist();
  
private:
  Int_t   mEventCounter; //!
  Int_t   mRunNumber;    //!  Run# for output file name
  StFgtDb* mDb;          //!

  static const int NHist=3;     //!
  static const int N1dHist=7;   //!
  static const int N2dHist=1;   //!
  static const int NTrkHist=4;  //!
  TH1F *hist0[NHist];                               //! Histos for whole fgt
  TH1F *hist1[kFgtNumDiscs][kFgtNumQuads][N1dHist]; //! 1d histos for each disc/quad
  TH2F *hist2[kFgtNumDiscs][N2dHist];               //! 2d histos for each disc
  TH1F *histTrk[kFgtNumQuads][NTrkHist];            //! Histos for tracks

  ClassDef(StFgtQAMaker,0)
};
#endif