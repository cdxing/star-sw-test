/***************************************************************************
 *
 * $Id: StJetOutputMaker.h,v 1.2 2003/05/28 23:46:07 thenry Exp $
 * $Log: StJetOutputMaker.h,v $
 * Revision 1.2  2003/05/28 23:46:07  thenry
 * The file JetEvent.cxx, JetEvent.h has been eliminated.
 *
 * Revision 1.1  2003/05/15 18:11:19  thenry
 * Creation of StJetOutputMaker: this maker takes jets from the StJetMaker, 
 * with general event information, and fills the JetEvent structure, and then
 * saves the JetEvent structure to disk.
 *
 * Revision 1.0  2003/05/09 21:01:34  thenry
 * created
 *
 * Author: Thomas Henry May 2003
 ***************************************************************************
 *
 * Description:  Maker which creates a binary output file of the jets and
 * some other basic information like trigger and event ids.  It contains
 * the jetevent class.  If __ROOT__ is not defined, this file simplifies
 * to only the jetevent class.
 *
 ***************************************************************************/
#ifndef _JetEvent_h
#define _JetEvent_h
#include <map>
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <typeinfo>
using namespace std;

class eofException {
 public:
  static const string Msg;
};

class EventSubStruct {
 public:
  EventSubStruct() : eventId(0), 
    xVertex(0), yVertex(0), zVertex(0), 
    npTracks(0) {};
  EventSubStruct(const EventSubStruct &copy) : eventId(copy.eventId),
    xVertex(copy.xVertex), yVertex(copy.yVertex), zVertex(copy.zVertex),
    npTracks(copy.npTracks) {};
  void clear(void) {};
  ostream& out(ostream &os)
    {
      os << "EventSubStruct" << endl;
      os << "eventId: " << eventId << endl;
      os << "xVertex: " << xVertex << endl;
      os << "yVertex: " << yVertex << endl;
      os << "zVertex: " << zVertex << endl;
      os << "npTracks: " << npTracks << endl;
      return os;
    }

  short eventId;
  double xVertex;
  double yVertex;
  double zVertex;
  short npTracks;
};

class EventStruct {
 public:
  EventStruct() {};
  EventStruct(const EventStruct &copy) : subEvent(copy.subEvent), 
    triggers(copy.triggers) {};
  void clear(void) { triggers.clear(); };
  void setTriggers(vector<int> trigs) {
    triggers.clear();
    for(vector<int>::iterator it = trigs.begin(); it != trigs.end(); ++it)
      triggers.push_back(*it);
  };

  EventSubStruct subEvent;

  vector<short> triggers;
};

class TrackStruct {
 public:
  TrackStruct() {};
  TrackStruct(const TrackStruct &copy) : trackE(copy.trackE), 
    trackPhi(copy.trackPhi), trackEta(copy.trackEta), 
    isTpcTrack(copy.isTpcTrack) {};
  void clear(void) {};  
  ostream& out(ostream &os)
    {
      os << "TrackStruct:" << endl;
      os << "TrackE: " << trackE << endl;
      os << "TrackPhi: " << trackPhi << endl;
      os << "TrackEta: " << trackEta << endl;
      if(isTpcTrack) os << "Track is from TPC" << endl;
      return os;
    }
  
  double trackE;
  double trackPhi;
  double trackEta;
  bool isTpcTrack;
};

typedef vector<TrackStruct> TrackStructVec;

class JetSubStruct {
 public:
  JetSubStruct() {};
  JetSubStruct(const JetSubStruct &copy) : energy(copy.energy), px(copy.px),
    py(copy.py), pz(copy.pz), eta(copy.eta), phi(copy.phi), 
    numCharges(copy.numCharges), charge(copy.charge) {};
  void clear(void) {};
  ostream& out(ostream &os)
    {
      os << "JetSubStruct:" << endl;
      os << "energy: " << energy << endl;
      os << "px: " << px << endl;
      os << "py: " << py << endl;
      os << "pz: " << pz << endl;
      os << "eta: " << eta << endl;
      os << "phi: " << phi << endl;
      os << "numCharges: " << numCharges << endl;
      os << "charge: " << charge << endl;
      return os;
    }

  double energy;
  double px;
  double py;
  double pz;
  double eta;
  double phi;
  short numCharges;
  short charge;
};

class JetStruct {
 public:
  JetStruct() {};
  JetStruct(const JetStruct &copy) : jetname(copy.jetname), jet(copy.jet), 
    tracks(copy.tracks) {};
  void clear(void) { jetname.erase(); tracks.clear(); };

  string jetname;
  JetSubStruct jet;
  TrackStructVec tracks;
};

typedef vector<JetStruct> JetStructVec;

class PJetEvent 
#ifdef __ROOT__
: public TObject
#endif // __ROOT__
{
public:
    PJetEvent() 
#ifndef __ROOT__
      : eventId(event.subEvent.eventId),
      xVertex(event.subEvent.xVertex),
      yVertex(event.subEvent.yVertex),
      zVertex(event.subEvent.zVertex),
      npTracks(event.subEvent.npTracks),
      
      jetname(jet.jetname),
      energy(jet.jet.energy),
      px(jet.jet.px),
      py(jet.jet.py),
      pz(jet.jet.pz),
      eta(jet.jet.eta),
      phi(jet.jet.phi),
      numCharges(jet.jet.numCharges),
      charge(jet.jet.charge),
 
      trackE(track.trackE),
      trackPhi(track.trackPhi),
      trackEta(track.trackEta),
      isTpcTrack(track.isTpcTrack) 
#endif // not __ROOT__
{};

    virtual ~PJetEvent() {};

    void clear(void) { triggers.clear(); event.clear(); 
      jets.clear(); tracks.clear(); };

public:
    EventStruct event;
    vector<int> triggers;

    JetStructVec jets;
    JetStruct jet;    

    TrackStructVec tracks;
    TrackStruct track;

#ifndef __ROOT__
    short &eventId;
    double &xVertex;
    double &yVertex;
    double &zVertex;
    short &npTracks;

    string &jetname;
    double &energy;
    double &px;
    double &py;
    double &pz;
    double &eta;
    double &phi;
    short &numCharges;
    short &charge;

    double &trackE;
    double &trackPhi;
    double &trackEta;
    bool &isTpcTrack;
#endif // not __ROOT__

    void push_track(void){
      tracks.push_back(track);
    }; 
    void push_jet(void){
      jet.tracks = tracks;
      jets.push_back(jet);
      tracks.clear();
    }; 
    void setTriggers(vector<short> newtriggers) {
      triggers.clear();
      for(vector<short>::iterator it = newtriggers.begin(); 
	  it != newtriggers.end(); ++it)
	triggers.push_back(static_cast<short>(*it));
    };

#ifdef __ROOT__
    ClassDef(PJetEvent, 0)
#endif
};

ostream& write(ostream &os, PJetEvent& jets);
istream& read (istream &is, PJetEvent& jets);
ostream& write(ostream &os, EventStruct& event);
istream& read (istream &is, EventStruct& event);
ostream& write(ostream &os, JetStruct& jet);
istream& read (istream &is, JetStruct& jet);
ostream& write(ostream &os, TrackStruct& track);
istream& read (istream &is, TrackStruct& track);
ostream& write(ostream &os, short &toWrite);
istream& read(istream &is, short &toWrite);
ostream& write(ostream &os, const char *toWrite, int size);
istream& read(istream &is, char *toRead, int size);

template <class T>
ostream& write(ostream &os, vector<T> &toWrite)
{
  unsigned short size = toWrite.size();
  write(os, reinterpret_cast<char*>(&size), sizeof(size));
  for(vector<T>::iterator it = toWrite.begin(); 
      it != toWrite.end(); ++it)
    {
      write(os, *it);
    }
  return os;
}

template <class T>
istream& read (istream &is, vector<T> &toRead)
{
  if(is.eof()) throw eofException();
  unsigned short size = 0;
  read(is, reinterpret_cast<char*>(&size), sizeof(size));
  for(int i = 0; i < size; i++)
    {
      static T input;
      read(is, input);
      toRead.push_back(input);
    }
  return is;
}

ostream& operator << (ostream &os, PJetEvent& jets);
ostream& operator << (ostream &os, EventStruct& event);
ostream& operator << (ostream &os, JetStruct& jet);
ostream& operator << (ostream &os, TrackStruct& track);
ostream& operator << (ostream &os, EventSubStruct& subEvent);
ostream& operator << (ostream &os, JetSubStruct& jet);

template <class T>
ostream& operator << (ostream &os, vector<T> &toWrite)
{
  short size = toWrite.size();
  os << typeid(T).name() << " (Size: " << size << ")" << endl;
  for(vector<T>::iterator it = toWrite.begin(); 
      it != toWrite.end(); ++it)
    {
      os << (*it) << " ";
    }
  return os;
}

#endif


#ifndef StJetOutputMaker_h
#define StJetOutputMaker_h

#ifdef __ROOT__
#include "StEmcTpcFourPMaker.h"
#include "StJetMaker.h"

class StJetOutputMaker : public StMaker {
public:
    StJetOutputMaker(const char *name, StMuDstMaker *uDstMaker, 
		     StJetMaker *stjetmaker, const char *outputName,
		     StEmcTpcFourPMaker* emcTpcFourPMaker);
    virtual Int_t Init();
    virtual Int_t Make();
    virtual Int_t Finish();

protected:
    StMuDstMaker *muDstMaker;
    StJetMaker *jetMaker;
    string ofilename;
    StEmcTpcFourPMaker* fourPMaker;
    PJetEvent oJetEvent;
    ofstream* ofile;
    StProjectedTrack tempTrack;
    int eventsProcessed;

    ClassDef(StJetOutputMaker,1)
};
#endif // __ROOT__
#endif // StJetOutputMaker_h







