///////////////////////////////////////////////////////////////////////////////
//
// StHbtTagMaker.hh
//
// Author List: 
//  Frank Laue, The Ohio State University, laue@bnl.gov
//
///////////////////////////////////////////////////////////////////////////////
//
// Description: 
//  Maker to fill Tag database for hbt analysis
//
//
//////////////////////////////////////////////////////////////////////


#ifndef StHbtTagMaker_H
#define StHbtTagMaker_H
#include <iostream.h>
#include <stdlib.h>
#include "StMaker.h"
#include "tables/St_HbtTag_Table.h"
#include "StHbtMaker/Infrastructure/StHbtTagWriter.hh"

class StHbtEvent;
class TH1F;
class TH1D;
class TProfile;

class StHbtTagMaker
#ifdef __ROOT__
 : public StMaker 
#endif
{

public:

  StHbtTagMaker(const Char_t* name = "HbtTag");
  virtual ~StHbtTagMaker();

  Int_t        Init();
  Int_t        Clear();
  Int_t        Make();
  void            PrintInfo();
  void            SetTagWriter(const StHbtTagWriter* tagWriter);
  StHbtTagWriter* TagWriter() const;
  Int_t        Finish();
  HbtTag_st*  TagPointer() const;           // returns pointer to the tag table
  void        Fill(StHbtTagWriter*);  // copy data from the StHbtTagWriter
private:

  void         PrintTag(ostream& = cout);  // output Tag info to screen

  St_HbtTag*  mSt_HbtTag; //! the StHbtTag table header
  HbtTag_st*  mHbtTag;    //! the StHbtTag table structure to fill
  StHbtTagWriter* mTagWriter;


#ifdef __ROOT__
  ClassDef(StHbtTagMaker, 0)                     // macro for rootcint
#endif
};

//inline void StHbtTagMaker::SetTagWriter(const StHbtTagWriter* tagWriter) {mTagWriter = tagWriter;}
inline StHbtTagWriter* StHbtTagMaker::TagWriter() const {return mTagWriter;}
inline HbtTag_st* StHbtTagMaker::TagPointer() const { return mHbtTag; }

#endif
