// $Id: St_trg_Maker.h,v 1.4 2000/01/24 20:35:39 ward Exp $
// $Log: St_trg_Maker.h,v $
// Revision 1.4  2000/01/24 20:35:39  ward
// Access trigger data.
//
// Revision 1.3  1999/07/15 13:58:32  perev
// cleanup
//
// Revision 1.2  1999/03/14 00:25:40  perev
// New makers
//
// Revision 1.1  1999/02/06 01:51:24  yepes
// Add trg maker
//
// Revision 1.7  1998/10/31 00:25:45  fisyak
// Makers take care about branches
//
// Revision 1.6  1998/10/06 18:00:31  perev
// cleanup
//
// Revision 1.5  1998/08/26 12:15:13  fisyak
// Remove asu & dsl libraries
//
// Revision 1.4  1998/08/14 15:25:58  fisyak
// add options
//
// Revision 1.3  1998/08/10 02:32:07  fisyak
// Clean up
//
// Revision 1.2  1998/07/20 15:08:15  fisyak
// Add tcl and tpt
//
#ifndef STAR_St_trg_Maker
#define STAR_St_trg_Maker

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// St_trg_Maker virtual base class for Maker                            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#ifndef StMaker_H
#include "StMaker.h"
#endif
class St_dst_L0_Trigger;
class St_dst_TrgDet;
class StDAQReader;
class StTRGReader;

class St_trg_Maker : public StMaker {
 private:
   Bool_t drawinit;
   StDAQReader *fVictorPrelim; //!
   StTRGReader *fVictor;       //!
// static Char_t  m_VersionCVS = "$Id: St_trg_Maker.h,v 1.4 2000/01/24 20:35:39 ward Exp $";
// Int_t          m_mode;        // mode 1 = primaries;
// St_stk_stkpar *m_stk_stkpar;  //! pointer to stk parameters
   void dumpDataToScreenAndExit();
 
 protected:
 public: 
   void CtbMwcDaq(St_dst_TrgDet *dst1);
   void SecondDstSim(St_dst_L0_Trigger *dst2);
   void SecondDstDaq(St_dst_L0_Trigger *dst2);
   void VpdSim(St_dst_TrgDet *dst);
   void ZdcSim(St_dst_TrgDet *dst);
   void VpdDaq(St_dst_TrgDet *dst);
   void ZdcDaq(St_dst_TrgDet *dst);
   int Daq(St_DataSet *herb,St_dst_TrgDet *dst1,St_dst_L0_Trigger *dst2);
   int Sim(St_dst_TrgDet *dst1,St_dst_L0_Trigger *dst2);
                  St_trg_Maker(const char *name="trg");
   virtual       ~St_trg_Maker();
   virtual Int_t Init();
   virtual Int_t  Make();
// virtual void Set_mode       (Int_t   m =      2){m_mode       = m;} // *MENU*
  virtual const char *GetCVS() const
  {static const char cvs[]="Tag $Name:  $ $Id: St_trg_Maker.h,v 1.4 2000/01/24 20:35:39 ward Exp $ built "__DATE__" "__TIME__ ; return cvs;}

   ClassDef(St_trg_Maker, 1)   //StAF chain virtual base class for Makers
};

#endif
