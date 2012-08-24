#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "TMath.h"
#include "TMatrixD.h"
#include "TVectorD.h"
#include "TCernLib.h"
#include "StvFitter.h"
#include "StvUtil/StvNodePars.h"
#include "StvHit.h"
#include "StvUtil/StvDebug.h"
#include "StvUtil/StvHitErrCalculator.h"
#include "StarVMC/GeoTestMaker/StTGeoHelper.h"

StvFitter *StvFitter::mgFitter=0;
#define VDOT(a,b)   ( a[0]*b[0]+a[1]*b[1]+a[2]*b[2])
#define DIST(a,b)   ((a[0]-b[0])*(a[0]-b[0])+(a[1]-b[1])*(a[1]-b[1])+(a[2]-b[2])*(a[2]-b[2]))
#define DDOT(a,b,c) ((a[0]-b[0])*c[0]+(a[1]-b[1])*c[1]+(a[2]-b[2])*c[2])
#define VADD(a,b)   { a[0]+=b[0];a[1]+=b[1];a[2]+=b[2];}
enum {kDeltaFactor = 3};
static inline double MyXi2(const double G[3],double dA,double dB)  
{
  double Gdet = G[0]*G[2]-G[1]*G[1];
  return  (G[2]*dA*dA-2*G[1]*dA*dB+G[0]*dB*dB)/Gdet;
}
double JoinTwoT(int nP1,const double *P1,const double *E1
               ,int nP2,const double *P2,const double *E2
	               ,      double *PJ,      double *EJ);
//______________________________________________________________________________
double JoinTwo(int nP1,const double *P1,const double *E1
              ,int nP2,const double *P2,const double *E2
	               ,     double *PJ,      double *EJ)
{
static int nCall = 0;  nCall++;
StvDebug::Break(nCall);
  assert(nP1<=nP2);
  int nE1 = nP1*(nP1+1)/2;
  int nE2 = nP2*(nP2+1)/2;
  TArrayD ard(nE2*5+nP2*3);
  double *a = ard.GetArray();  
  double *E1i = a;
  double *E2i = (a+=nE2);
  double *EJi = (a+=nE2);
  double *EJm = (a+=nE2);
  double *ERi = (a+=nE2);
  
  double *Pdif= (a+=nE2);
  double *P1J = (a+=nP2);
  double *P2J = (a+=nP2);

  TCL::trsinv(E1,E1i,nP1);		//E1i = 1/E1
  TCL::trsinv(E2,E2i,nP2);		//E2i = 1/E2
  TCL::vadd (E1i,E2i,EJi ,nE2);		//EJi = E1i+E2i
  TCL::trsinv(EJi,EJm,nP2);		//EJ = 1/EJi
  TCL::vsub (P1 ,P2 ,Pdif,nP1);		//Pdif = P1-P2
  TCL::trqsq(E1i,EJm,ERi ,nP1);		//ERi = E1i*EJm*E1i
  TCL::vsub (E1i,ERi,ERi ,nE1);         //ERi = E1i- E1i*EJi*E1i == 1/(E1+E2)
  double chi2;
  TCL::trasat(Pdif,ERi,&chi2,1,nP1); 

  double PJt[5],EJt[15];
  double chi2T =JoinTwoT(nP1,P1,E1,nP2,P2,E2,PJt,EJt);
  assert(fabs(chi2-chi2T) <1e-4*(chi2+chi2T+1));
  if (!PJ)      return chi2;
  if (chi2>=kExtraBigXi2) return chi2;
  TCL::ucopy(EJm,EJ,nE2);		//EJ = 1/EJi
  
  TCL::trsa(E1i,P1,P1J,nP1,1);		//P1J = E1i*P1
  TCL::trsa(E2i,P2,P2J,nP2,1);		//P2J = E2i*P2
  TCL::vadd(P2J,P1J,P2J,nP1);		//P2J = P1J+P2J
  TCL::trsa(EJ,P2J,PJ,nP2,1);		//PJ  = EJ*P3J

  double dia[5];
  for (int i=0,li=0;i< 5;li+=++i) {
    dia[i]= sqrt(EJt[li+i]);
    assert(fabs(PJ[i]-PJt[i])<1e-4*dia[i]);
    for (int j=0;j<=i; j++) {
      assert( fabs(EJ[li+j]-EJt[li+j])<(dia[i]*dia[j]));}}

  return chi2;
}
//______________________________________________________________________________
double JoinTwoT(int nP1,const double *P1,const double *E1
               ,int nP2,const double *P2,const double *E2
	               ,      double *PJ,      double *EJ)
{
///  Fit track(P2) & errors E2 with track or hit (P1) & errors E1
///  nP1 size of array P1. E1 errors packed as low triangle
///  nP2 size of array P2. E2 errors packed as low triangle
///  PJ output parameters size nP2. EJ according packed errors

  assert(nP1<=nP2);
//  int nE1 = nP1*(nP1+1)/2;
//  int nE2 = nP2*(nP2+1)/2;
  TMatrixD  E1i(nP2,nP2 ),E2i(nP2,nP2);
  for (int i=0,li=0;i< nP1;li+=++i) {
    for (int j=0;j<=i; j++) {E1i[i][j]=E1[li+j]; E1i[j][i]=E1[li+j];}}

  for (int i=0,li=0;i< nP2;li+=++i) {
    for (int j=0;j<=i; j++) {E2i[i][j]=E2[li+j]; E2i[j][i]=E2[li+j];}}

  for (int i=nP1;i<nP2;i++) {E1i[i][i]=1;};
  E1i.Invert();
  for (int i=nP1;i<nP2;i++) {E1i[i][i]=0;};
  E2i.Invert();
  TMatrixD  EJi = E1i+E2i;
  TMatrixD  EJm = EJi; EJm.Invert();
  TVectorD P1v(nP2); TCL::ucopy(P1,P1v.GetMatrixArray(),nP1);
  TVectorD P2v(nP2,P2);
  
  TVectorD PJv = EJm*((E1i*P1v) + (E2i*P2v));
  double chi2 = (PJv-P1v)*(E1i*(PJv-P1v))+(PJv-P2v)*(E2i*(PJv-P2v));
  if (!PJ) return chi2;
  TCL::ucopy(PJv.GetMatrixArray(),PJ,nP2);
  TCL::trpck(EJm.GetMatrixArray(),EJ,nP2);
  return chi2;


}
//______________________________________________________________________________
double JoinVtx(int nP1,const double *P1,const double *E1
              ,int nP2,const double *P2,const double *E2
	               ,     double *PJ,      double *EJ)
{
///  Fit track(P2) & errors E2 with vertex (P1) & errors E1
///  Track must exactly pass thru the vertex. 
///  Vertex errors added afterwards
///  nP1 size of array P1. E1 errors packed as low triangle
///  nP2 size of array P2. E2 errors packed as low triangle
///  PJ output parameters size nP2. EJ according packed errors

  assert(nP1<nP2);
  int nPBig = nP1+nP2;

  TVectorD P1t(nP1,P1),P2t(nP2,P2);
  TVectorD D = (P1t-P2t.GetSub(0,nP1-1));
  TMatrixDSym smaMx(nP1);
  TCL::trupck(E2,smaMx.GetMatrixArray(),nP1);
  smaMx.Invert();
  double Xi2 = D*(smaMx*D);
  if (!PJ) return Xi2;

  TMatrixDSym E2t(nP2);
  TCL::trupck(E2,E2t.GetMatrixArray(),nP2);
  E2t.Invert();

  TVectorD bigB(nPBig),bigP(nPBig);
  bigB.SetSub(0,E2t*P2t);
  bigB.SetSub(nP2,P1t);

  TMatrixDSym bigMx(nPBig);
  bigMx.SetSub(0,E2t);
  for (int i=nP2,j=0;i< nPBig;++i,j++) {bigMx[i][j]=1;bigMx[j][i]=1;}
  bigMx.Invert();
  bigP = bigMx*bigB;
  assert(fabs(bigP[0]-P1[0])<1e-5);
  assert(fabs(bigP[1]-P1[1])<1e-5);

//		set vertex errors (not add because in bigMx they are zeros
  TMatrixDSym E1t(nP1);
  TCL::trupck(E1,E1t.GetMatrixArray(),nP1);
  bigMx.SetSub(0,E1t);

//		To output  
  TCL::ucopy(bigP.GetMatrixArray(),PJ,nP2);
  TCL::trpck(bigMx.GetSub(0,nP2-1,0,nP2-1).GetMatrixArray(),EJ,nP2);
  return Xi2;


}



//______________________________________________________________________________
StvFitter::StvFitter(const char *name):TNamed(name,"")
{
  memset(mBeg,0,mEnd-mBeg+1);
  assert(!mgFitter);
  mgFitter = this;
}
//______________________________________________________________________________
void StvFitter::Set(const StvNodePars *inPars, const StvFitErrs *inErrs
                   ,      StvNodePars *otPars,       StvFitErrs *otErrs)
{
  memset(mBeg,'@',mEnd-mBeg+1);
  mKase = 0;		// track + hit case
  mInPars = inPars; mInErrs = inErrs;
  mOtPars = otPars; mOtErrs = otErrs;
  mJnPars =      0; mJnErrs =      0;
}
//______________________________________________________________________________
void StvFitter::Set(const StvNodePars *inPars, const StvFitErrs *inErrs
                   ,const StvNodePars *jnPars, const StvFitErrs *jnErrs
                   ,      StvNodePars *otPars,       StvFitErrs *otErrs)
{
  memset(mBeg,'@',mEnd-mBeg+1);
  mKase = 1;		// join left & rite part of track
  mInPars = inPars; mInErrs = inErrs;
  mOtPars = otPars; mOtErrs = otErrs;
  mJnPars = jnPars; mJnErrs = jnErrs;
}
//______________________________________________________________________________
void StvFitter::Prep()
{
  mDelta  = mInPars->delta();
  mDelta *= kDeltaFactor;
  mHit   = 0; mHitPlane = 0;
  double myTan = mInPars->_tanl;
  mCos2L = 1./(1+myTan*myTan);
  mCosL = sqrt(mCos2L);
  mSinL = myTan*mCosL;
  mCosP = mInPars->_cosCA;
  mSinP = mInPars->_sinCA;

  mTkPars = *mInPars;
//		Track Frame
  mDcaFrame[0][0] =  mCosL*mCosP;
  mDcaFrame[0][1] =  mCosL*mSinP;
  mDcaFrame[0][2] =  mSinL;

  mDcaFrame[1][0] = -mSinP;
  mDcaFrame[1][1] =  mCosP;
  mDcaFrame[1][2] =  0;

  mDcaFrame[2][0] = -mSinL*mCosP;
  mDcaFrame[2][1] = -mSinL*mSinP;
  mDcaFrame[2][2] =  mCosL;

}
//______________________________________________________________________________
double StvFitter::Xi2(const StvHit *hit)
{
  if (mHit == hit) return mXi2;
  mHit = hit;
  const float *errMtx=mHit->errMtx();
  if (errMtx) mKase=2; 		//Hit is a vertex

  mHitPlane = mHit->detector();

//	restore old parameters for nhits>1  
  mTkPars._x = mInPars->_x; mTkPars._y = mInPars->_y; mTkPars._z = mInPars->_z;

//		Hit position
  const float *hP = mHit->x();

//		Track direction
  double *tD = mDcaFrame[0];
//		Start track position
  double *tP = &mTkPars._x;


//		Distance to DCA along track in xy
//mDeltaL = DDOT(hP,tP,tD);  
//		DCA track position
  switch (mKase) {
    case 0: {
      mHitErrCalc = (StvHitErrCalculator*)mHitPlane->GetHitErrCalc();
      assert(mHitErrCalc);
      mHitErrCalc->SetTrack(tD);
      const StHitPlane *hp = hit->detector(); 
      const Mtx33F_t &hD = hp->GetDir(hit->x());
      mHitErrCalc->CalcDcaErrs(hit->x(),hD,mHitErrs);
       StvDebug::Count("HHhit",sqrt(mHitErrs[0]));
       StvDebug::Count("ZZhit",sqrt(mHitErrs[2]));

    }; break;

    case 1: assert(0 && "Wrong case 1");

    case 2: {
      double d[6]={errMtx[0],errMtx[1],errMtx[2]
                  ,errMtx[3],errMtx[4],errMtx[5]};
      TCL::trasat(mDcaFrame[1],d,mHitErrs,2,3); }
  }
  assert(mHitErrs[0]>0);
  assert(mHitErrs[2]>0);
  assert(mHitErrs[2]*mHitErrs[0]>mHitErrs[1]*mHitErrs[1]);

//		Hit position wrt track 
  double dca[3] = {hP[0]-tP[0],hP[1]-tP[1],hP[2]-tP[2]};

  mDcaT=VDOT(mDcaFrame[0],dca);
  mDcaP=VDOT(mDcaFrame[1],dca);
  mDcaL=VDOT(mDcaFrame[2],dca);
//		small account non zero distance to hit along track
  double dS = mDcaT*mCosL;
  mDcaP-= 0.5*mTkPars._curv*dS*dS;

  double G[3] = {mInErrs->mHH,mInErrs->mHZ,mInErrs->mZZ};
  if (mKase==0) {for (int j=0;j<3;j++) {G[j]+=mHitErrs[j];}}

//  (BB*dX*dX-2*BA*dX*dY+AAdY*dY)/det 
  mXi2 = MyXi2(G,mDcaP,mDcaL);
  return mXi2 ; 
}  
//______________________________________________________________________________
double StvFitter::Xi2()
{
  double inErr = mInErrs->mHH+mInErrs->mZZ;
  double jnErr = mJnErrs->mHH+mJnErrs->mZZ;
  if (jnErr>inErr) {//Not good order
    const StvNodePars *swp = mInPars; mInPars=mJnPars; mJnPars=swp;
    const StvFitErrs  *swe = mInErrs; mInErrs=mJnErrs; mJnErrs=swe;
  }
  StvFitPars F   = (*mInPars-*mJnPars);
  double     Zero[5]= {0};
  double myXi2 = JoinTwo(5,F.Arr()    ,mInErrs->Arr()
                        ,5,Zero       ,mJnErrs->Arr()
		        ,mQQPars.Arr(),mQQErrs.Arr());
  mQQErrs.mHz = mInPars->_hz;
  return myXi2;
}  
//______________________________________________________________________________
int StvFitter::Update()
{
static int nCall=0; nCall++;
StvDebug::Break(nCall);
  int ifail = 0;
  switch (mKase) {
    case 0: ifail = Hpdate(); 	break;		//Hit+Track
    case 1: ifail = Jpdate();	break; 		//Track join
    case 2: ifail = Vpdate();	break;		//Vertex+track
  }
  
  if (ifail) return ifail;
  if (mOtPars->check()) return 2; 
  return 0;
}
//______________________________________________________________________________
int StvFitter::Hpdate()
{
///		this is Update for track+hit fit
		
  mTkErrs = *mInErrs;


//		New Z ortogonal to X (track direction)
  StvFitPars myHitPars(mDcaP, mDcaL );
  StvFitErrs myHitErrs(mHitErrs[0],mHitErrs[1],mHitErrs[2]);
  StvFitPars myTrkPars;
  StvFitPars myJrkPars;

  double myXi2 = JoinTwo(2,myHitPars.Arr(),myHitErrs.Arr()
                        ,5,myTrkPars.Arr(),mTkErrs.Arr()
		        ,  myJrkPars.Arr(),mOtErrs->Arr());
  if (myXi2>kExtraBigXi2) return 1313;
  assert(fabs(myXi2-mXi2)<1e-4*(myXi2+mXi2+1));
  assert(mOtErrs->MaxCorr()<1);
//   assert(fabs(myJrkPars.mH) <3.);
//   assert(fabs(myJrkPars.mZ) <3.);
//  assert(myHitPars.mH*myJrkPars.mH+myHitPars.mZ*myJrkPars.mZ>0);
  int ierr = 0;
  for (int i=0;i<5;i++) {
    if (myJrkPars[i]<-mDelta[i]) {ierr++; myJrkPars[i]= -mDelta[i];}
    if (myJrkPars[i]> mDelta[i]) {ierr++; myJrkPars[i]=  mDelta[i];}
  }
  *mOtPars = mTkPars;
  *mOtPars+= myJrkPars;
   mOtErrs->SetHz(mOtPars->_hz);
  return ierr!=0;
}  
//______________________________________________________________________________
int StvFitter::Vpdate()
{
///		this is Update for track+vertex fit
static int nCall=0; nCall++;

  mTkErrs = *mInErrs;

//		New Z ortogonal to X (track direction)
  StvFitPars myHitPars(mDcaP, mDcaL );
  StvFitPars myTrkPars;
  StvFitPars myJrkPars;

  double myXi2 = JoinVtx(2,myHitPars.Arr(),mHitErrs
                        ,5,myTrkPars.Arr(),mTkErrs.Arr()
		        ,  myJrkPars.Arr(),mOtErrs->Arr());
  if (myXi2){}
//assert(fabs(myXi2-mXi2)<1e-4*(myXi2+mXi2));
  assert(fabs(myJrkPars[0]-myHitPars[0])<1e-6);
  assert(fabs(myJrkPars[1]-myHitPars[1])<1e-6);
  *mOtPars = mTkPars;
  *mOtPars+= myJrkPars;
  mOtErrs->SetHz(mOtPars->_hz);
  for (int i=0;i<3;i++) {mOtErrs->Arr()[i]+=mHitErrs[i];}
  return 0;
}  
//______________________________________________________________________________
int StvFitter::Jpdate()
{
///		this is Update for sub track+sub track fit (join)
  int ierr = 0;
  for (int i=0;i<5;i++) {
    if (mQQPars[i]<-mDelta[i]) {ierr++; mQQPars[i]= -mDelta[i];}
    if (mQQPars[i]> mDelta[i]) {ierr++; mQQPars[i]=  mDelta[i];}
  }
  *mOtPars = *mJnPars; 
  *mOtPars+=  mQQPars;
  *mOtErrs =  mQQErrs;   
   mOtErrs->SetHz(mOtPars->_hz);
  return ierr!=0;
}
//______________________________________________________________________________
double StvFitter::TooBig(StvFitPars &fp, int *mask) const
{
  double fakt = 0;
  int msk=0;
  for (int i=0;i<5;i++) { 
    double f = fabs(fp[i])/mDelta[i];
    if (fakt>f) continue;
    fakt=f; msk|= 1<<i;
  }
  if (mask) *mask = msk;
  return fakt;
}
#if 0
//______________________________________________________________________________
double StvFitter::JoinTwo(int nP1,const double *P1,const double *E1
                         ,int nP2,const double *P2,const double *E2
	                 ,              double *PJ,      double *EJ
			 ,int mode)
{
// 		mode=0 normal case
//		mode=1 assign to vertex where 1st nP1 words of PJ must be == P1
static int nCall =0; nCall++;

  assert(nP1<=nP2);
  int nE1 = nP1*(nP1+1)/2;
  int nE2 = nP2*(nP2+1)/2;
  TArrayD ard(nE2*6+2*nP2*nP2);
  double *a = ard.GetArray();  
  double *sumE 		= (a);
  double *sumEI 	= (a+=nE2);
  double *e2sumEIe2 	= (a+=nE2);
  double *subP 		= (a+=nE2);
  double *sumEIsubP	= (a+=nE2);
  double *E2U           = (a+=nE2);
  double *E2UsumEI      = (a+=nP2*nP2);
  a+=nP2*nP2);
  assert(a-ard.GetArray()<=ard.GetSize());

  double chi2=3e33;


  do {//empty loop
//  	Join errors
    if (!mode) {TCL::vadd (E2,E1,sumE,nE1);}
    else       {TCL::ucopy(E2,   sumE,nE1);}
    TCL::trsinv(sumE,sumEI,nP1);
    TCL::vsub  (P1  ,P2   ,subP   ,nP1);
    TCL::trasat(subP,sumEI,&chi2,1,nP1); 
    if (!EJ) 	break;

    TCL::trqsq (E2  ,sumEI,e2sumEIe2,nP2); 
    TCL::vsub(E2,e2sumEIe2,EJ,nE2);
    if (!mode)  break;
    TCL::trupck(E2,E2U,nP2);
    TCL::trats(E2U,sumEI,E2UsumEI,nP2,nP1);
    TCL::trasat(E2UsumEI,E1,E2U,nP2,nP1);
    TCL::vadd(E2U,EJ,EJ,nE2);
      
  } while(0);
//  	Join params
    double Xi2X,Xi2T;
    if (!mode) {
     Xi2X = JoinTwoX(nP1,P1,E1,nP2,P2,E2,0,0);
     Xi2T = JoinTwoT(nP1,P1,E1,nP2,P2,E2,0,0);
    assert(fabs(Xi2X-Xi2T) < (Xi2X+Xi2T+0.1)*1e-4);
    } else {
     Xi2T = JoinVtx(nP1,P1,E1,nP2,P2,E2,0,0);
    }
    assert(fabs(chi2-Xi2T) < (chi2+Xi2T+0.1)*1e-4);
  if (!PJ) return chi2;
  TCL::tras(subP     ,sumEI,sumEIsubP,1,nP1);
  TCL::tras(sumEIsubP,E2   ,PJ       ,1,nP2);
  TCL::vadd(PJ       ,P2   ,PJ         ,nP2);

  double PJX[5],PJT[5],EJX[15],EJT[15];

    if (!mode) {
      Xi2X = JoinTwoX(nP1,P1,E1,nP2,P2,E2,PJX,EJX);
      Xi2T = JoinTwoT(nP1,P1,E1,nP2,P2,E2,PJT,EJT);
      TCL::ucopy(PJT,PJ,nP2);
      TCL::ucopy(EJT,EJ,nE2);
      for (int i=0;i<5;i++) {
        assert (fabs(PJX[i]-PJT[i])<1e-5*(fabs(PJT[i]+0.1)));
      }
    } else {
      Xi2T = JoinVtx(nP1,P1,E1,nP2,P2,E2,PJT,EJT);
      TCL::ucopy(PJT,PJX,nP2);
      TCL::ucopy(EJT,EJX,nE2);
    }
     
    for (int i=0;i<5;i++) {
      assert (fabs( PJ[i]-PJT[i])<1e-4*(fabs(PJT[i]+0.1)));
    }
    for (int i=0;i<15;i++) {
      assert (fabs(EJX[i]-EJT[i])<1e-5*(fabs(EJT[i]+0.1)));
    }

    for (int i=0;i<15;i++) {
      assert (fabs( EJ[i]-EJT[i])<1e-4*(fabs(EJT[i]+0.1)));
    }
   

  return chi2;
}
#endif
//______________________________________________________________________________
void StvFitter::Test()
{
  double A[5]={1,2,3,4,5};
  double AA[15]=
  {1
  ,0,2
  ,0,0,3
  ,0,0,0,4
  ,0,0,0,0,5};
  double DA[5]={1,2,3,4,5};
  
  double B[5]={6,7,8,9,10};
  double BB[15]=
  {6
  ,0,7
  ,0,0,8
  ,0,0,0,9
  ,0,0,0,0,10};
  double DB[5]={6,7,8,9,10};
  
  double C[5],DC[5],CC[15],myC[5];
  
  for (int i=0;i<5;i++) {
    C[i] = (A[i]/DA[i]+ B[i]/DB[i])/(1/DA[i]+ 1/DB[i]);
    DC[i] = 1/(1/DA[i]+ 1/DB[i]);
    printf("%d C=%g (%g)\n",i,C[i],DC[i]);
  }
  
  
  JoinTwo(5,A,AA,5,B,BB,myC,CC);
  printf("Result =");
  for (int i=0;i<5;i++) {printf(" %g",myC[i]);}
  printf("\nError matrix =\n");
  for (int i=0,li=0;i< 5;li+=++i) {
    for (int j=0;j<=i; j++) { printf("%g\t",CC[li+j]);}
    printf("\n");
  }


 BB[5]=1e3;BB[9]=1e3;BB[14]=1e3;

  printf("  JoinTwo(5,A,AA,5,B,BB,myC,CC)\n");
  JoinTwo(5,A,AA,5,B,BB,myC,CC);
  printf("Result =");
  for (int i=0;i<5;i++) {printf(" %g",myC[i]);}
  printf("\nError matrix =\n");
  for (int i=0,li=0;i< 5;li+=++i) {
    for (int j=0;j<=i; j++) { printf("%g\t",CC[li+j]);}
    printf("\n");
  }
  printf("  JoinTwo(2,B,BB,5,A,AA,myC,CC)\n");
  JoinTwo(2,B,BB,5,A,AA,myC,CC);
  printf("Result =");
  for (int i=0;i<5;i++) {printf(" %g",myC[i]);}
  printf("\nError matrix =\n");
  for (int i=0,li=0;i< 5;li+=++i) {
    for (int j=0;j<=i; j++) { printf("%g\t",CC[li+j]);}
    printf("\n");
  }

}
