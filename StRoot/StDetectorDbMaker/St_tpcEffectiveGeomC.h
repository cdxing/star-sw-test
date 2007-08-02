#ifndef St_tpcEffectiveGeomC_h
#define St_tpcEffectiveGeomC_h

#include "TChair.h"
#include "tables/St_tpcEffectiveGeom_Table.h"

class St_tpcEffectiveGeomC : public TChair {
 public:
  static St_tpcEffectiveGeomC* 	instance();
  tpcEffectiveGeom_st 	*Struct(Int_t i = 0) 	  {return ((St_tpcEffectiveGeom*) Table())->GetTable()+i;}
  UInt_t     	getNumRows()                	  {return GetNRows();}
  Double_t 	drift_length_correction(Int_t i=0){return Struct(i)->drift_length_correction;}
  Double_t 	z_inner_offset(Int_t i = 0) 	  {return Struct(i)->z_inner_offset;}
  Double_t 	z_outer_offset(Int_t i = 0) 	  {return Struct(i)->z_outer_offset;}
 protected:
  St_tpcEffectiveGeomC(St_tpcEffectiveGeom *table=0) : TChair(table) {}
  virtual ~St_tpcEffectiveGeomC() {SafeDelete(fgInstance);}
 private:
  static St_tpcEffectiveGeomC* fgInstance;
  ClassDefChair(St_tpcEffectiveGeom, tpcEffectiveGeom_st )
  ClassDef(St_tpcEffectiveGeomC,1) //C++ TChair for tpcEffectiveGeom table class
};
#endif
