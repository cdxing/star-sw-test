#ifndef StHbtMcEventReader_hh
#define StHbtMcEventReader_hh

#include <ctime>
#include "StHbtMaker/Base/StHbtEventReader.hh"
//#include "StChain.h"
#include "StMaker.h"

class StHbtMcEventReader : public StHbtEventReader{

private:
  StMaker* mTheMcEventMaker;      //! this is the chain where the StEventReaderMaker is
  time_t timeStamp;

public:
  StHbtMcEventReader();
  //  ~StHbtMcEventReader();

  virtual StHbtEvent* ReturnHbtEvent();
  virtual StHbtString Report();


  void SetTheMcEventMaker(StMaker*);
  StMaker* TheMcEventMaker();

  ClassDef(StHbtMcEventReader, 1)

};

inline void StHbtMcEventReader::SetTheMcEventMaker(StMaker* mcMaker){mTheMcEventMaker=mcMaker;}
inline StMaker* StHbtMcEventReader::TheMcEventMaker(){return mTheMcEventMaker;}

#endif
