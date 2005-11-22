// $Id: StSpaStrip.hh,v 1.3 2005/11/22 03:56:46 bouchet Exp $
//
// $Log: StSpaStrip.hh,v $
// Revision 1.3  2005/11/22 03:56:46  bouchet
// id_mctrack is using for setIdTruth
//
// Revision 1.2  2005/05/13 08:39:33  lmartin
// CVS tags added
//

#ifndef STSPASTRIP_HH
#define STSPASTRIP_HH
#include <Stiostream.h>
#include <stdlib.h>
#include <math.h>

class StSpaStrip
{
 public:
  StSpaStrip(int rNStrip, int rMcStrip, int rDigitSig, float rAnalogSig, int *rIdMcHit);
  StSpaStrip(int rNStrip, int rMcStrip, float rAnalogSig);
  StSpaStrip(int rNStrip, int rDigitSig);
  ~StSpaStrip();
  void        setNStrip(int rNStrip);
  void        setMcStrip(int rMcStrip);
  void        setDigitSig(int rDigitSig);
  void        setAnalogSig(float rAnalogSig);
  void        setIdMcHit(int rIdMcHit, int iR);
  void        setIdMcTrack(int rIdMcTrack, int iR);
  void        setPrevStrip(StSpaStrip *rPrevStrip);
  void        setNextStrip(StSpaStrip *rNextStrip);
  int         getNStrip();
  int         getMcStrip();
  int         getDigitSig();
  float       getAnalogSig();
  int         getIdMcHit(int iR);
  int         getIdMcTrack(int iR);
  StSpaStrip* getPrevStrip();
  StSpaStrip* getNextStrip();
  StSpaStrip* giveCopy();
  
private:
  int         mNStrip;
  int         mMcStrip;
  int         mDigitSig;
  float       mAnalogSig;
  int        *mIdMcHit;
  int        *mIdMcTrack;
  StSpaStrip *mPrevStrip;
  StSpaStrip *mNextStrip;
};

#endif
