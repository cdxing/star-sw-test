#include "StarChairDefs.h"
#include "St_db_Maker/St_db_Maker.h"
//________________________________________________________________________________
#include "StiHitErrorCalculator.h"
ClassImp(StiHitErrorCalculator);
//________________________________________________________________________________
void StiHitErrorCalculator::calculateError(Double_t _z,  Double_t _eta, Double_t _tanl, Double_t &ecross, Double_t &edip) const {
  static const Double_t tenMicrons = 1e-3;
  static const Double_t min2Err = tenMicrons*tenMicrons;
  static const Double_t max2Err = 1.;
  const Double_t *Coeff = ((StiHitErrorCalculator *) this)->coeff();
  Double_t dz = (200.-fabs(_z))/100.;
  Double_t cosCA = cos(_eta);
  Double_t sinCA = sin(_eta);
  if (cosCA<0.01) cosCA=0.01;
  Double_t tanCA = sinCA/cosCA;
  ecross=Coeff[0]+Coeff[1]*dz/(cosCA*cosCA) +Coeff[2]*tanCA*tanCA;
  if (ecross< min2Err) ecross = min2Err;
  if (ecross> max2Err) ecross = max2Err;
  Double_t tanDip=_tanl;
  Double_t cosDipInv2=1+tanDip*tanDip;
         edip=Coeff[3]+Coeff[4]*dz*cosDipInv2+Coeff[5]*tanDip*tanDip;
  if (edip< min2Err) edip = min2Err;
  if (edip> max2Err) edip = max2Err;
}
//________________________________________________________________________________
#include "StiTrackingParameters.h"
ClassImp(StiTrackingParameters)
#include "StiDefaultTrackingParameters.h"
MakeChairInstance2(TrackingParameters,StiDefaultTrackingParameters,Calibrations/tracker/DefaultTrackingParameters);
#include "StiLocalTrackSeedFinderParameters.h"
MakeChairInstance2(LocalTrackSeedFinder,StiLocalTrackSeedFinderParameters,Calibrations/tracker/LocalTrackSeedFinder);
#include "StiKalmanTrackFitterParameters.h"
MakeChairInstance2(KalmanTrackFitterParameters,StiKalmanTrackFitterParameters,Calibrations/tracker/KalmanTrackFitterParameters);
#include "StiKalmanTrackFinderParameters.h"
MakeChairInstance2(KalmanTrackFinderParameters,StiKalmanTrackFinderParameters,Calibrations/tracker/KalmanTrackFinderParameters);
