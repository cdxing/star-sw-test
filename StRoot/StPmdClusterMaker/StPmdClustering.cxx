/***********************************************************
 *
 * $Id: StPmdClustering.cxx,v 1.1 2002/08/27 12:06:10 subhasis Exp $
 *
 * Author: based on original routine written by S. C. Phatak.
 *
 ***********************************************************
 *
 * Description: Base class for PMD clusters
 *
 ***********************************************************
 *
 * $Log: StPmdClustering.cxx,v $
 * Revision 1.1  2002/08/27 12:06:10  subhasis
 * First version
 *
 *
 ***********************************************************/

#include<iostream.h>
#include<assert.h>
#include<math.h>
#include"TROOT.h"
#include<TRandom.h>
#include<TBrowser.h>
#include<TPad.h>
#include<StMessMgr.h>
#include<TFile.h>


#include <TTableSorter.h>

#include "StBFChain.h"
#include "StPmdUtil/StPmdGeom.h"
#include "StPmdUtil/StPmdHit.h"
#include "StPmdClustering.h"
#include "StPmdClusterMaker.h"
#include "StPmdUtil/StPmdClusterCollection.h"
#include "StPmdUtil/StPmdCluster.h"
#include "StPmdUtil/StPmdModule.h"
#include "StPmdUtil/StPmdDetector.h"



ClassImp(StPmdClustering)

Double_t d1[96][72], clusters[4][2000], coord[2][96][72];

Int_t iord[2][6912], infocl[2][96][72], inford[3][6912], clno;
const Int_t nmx=6912;   //! (72 x 96)maximum number of cells in a supermodule
const Double_t pi=3.141593, sqrth=sqrt(3.)/2.;

StPmdGeom *geom=new StPmdGeom(); //! utility class
//-------------------------------------------------
//! constructor for getting PMD and CPV detectors

  StPmdClustering::StPmdClustering(StPmdDetector *pmd_det, StPmdDetector *cpv_det):StPmdAbsClustering(pmd_det,cpv_det){
// m_pmd_det=pmd_det;
// m_cpv_det=cpv_det;
}
//------------------------------
StPmdClustering::~StPmdClustering()
{
}
//---------------------------------
//! finding Pmd clusters

void StPmdClustering::findPmdClusters()
{
  if(m_pmd_det){  //! getting Pmd detector
    StPmdClusterCollection * pmdclus = new StPmdClusterCollection();
    m_pmd_det->setCluster(pmdclus);
    
      Int_t i, i1, i2, j,xpad, ypad,nmx1, incr,idet;
      Int_t gsuper;  
      Double_t  edep, cutoff, ave;
      for(i=0; i<96; i++){
	for(j=0;j<72;j++){
	  coord[0][i][j]=i+j/2.; coord[1][i][j]=sqrth*j; 
	}
      }
      i=0;
      for(Int_t id=1;id<=12;id++){   //! loop for supermodule

	//!  id has to be 1 to 12, not 0 to 11
	for(Int_t j=0;j<96;j++){
	  for(Int_t k=0;k<72;k++){
            d1[j][k]=0;  //! Initialize Pmd arrays
	  }
	}
	
	StPmdModule * pmd_mod=m_pmd_det->module(id);  //! getting module(id)
      //      TObjArray* pmd_hit=pmd_mod->Hits();
      
      if(m_pmd_det->module_hit(id)>0){
	Int_t nmh=m_pmd_det->module_hit(id);  //! total no.of hits in the supermodule  

	TIter next(pmd_mod->Hits());
	StPmdHit *spmcl;   //! pointer for hits
	for(Int_t im=0; im<nmh; im++)
	  {
	    spmcl = (StPmdHit*)next();
	    if(spmcl){
	      ypad=spmcl->Row();          //! row of the hit
	      xpad=spmcl->Column();       //! column of the hit
	      edep=spmcl->Edep();         //! edep
	      gsuper = spmcl->Gsuper();   //! supermodule
	      idet=spmcl->SubDetector();  //! detector(= 1) for Pmd
	      xpad = xpad -1;ypad = ypad -1; 
	      if(idet==1){
		d1[xpad][ypad]=d1[xpad][ypad]+edep;  //! edep added for each cell
	      }
	    }
	  }

	idet=1;
	order(idet);  //! order the data according to edep ( largest to smallest )
	//! cutoff is the threshold above which value the data is analysed.
	cutoff=0.000000;
	ave=0.; nmx1=-1;
	for(Int_t jj=0;jj<nmx; jj++){
	  i1=iord[0][jj]; i2=iord[1][jj];
	  if(d1[i1][i2] > 0.){nmx1=nmx1+1;ave=ave+d1[i1][i2];}
	}

	ave=ave/nmx1;
	//!compare cutoff with the average energy deposited. Has no use in calc.

	cutoff=0.0; //! cutoff is the threshold above which value the data is analysed.

	/*! crude clusters. superclusters are formed. These are separated from each
	 * other by cells having edep smaller than cutoff. */
	incr=crclust(ave, cutoff, nmx1, idet);

	arrange(incr);  //! arrange cells in each supercluster
	
	refclust(incr, id, idet,pmdclus);  //! resolve superclusters into clusters
      }
//	cout << " supermodule (PMD)" << id<< " done"<<endl;
    }
  }
}
//-----------------------------------------
//! finding Cpv clusters 

void StPmdClustering::findCpvClusters()
{
  if(m_cpv_det){
      Int_t idet,gsuper;
      Double_t  edep;

    StPmdClusterCollection * pmdclus = new StPmdClusterCollection();
    m_cpv_det->setCluster(pmdclus);
    
    for(Int_t id=1;id<=12;id++){    //! loop for supermodule

      StPmdModule * pmd_mod=m_cpv_det->module(id);
      
      if(m_cpv_det->module_hit(id)>0){
	Int_t nmh=m_cpv_det->module_hit(id);  //! total number of hits in the supermodule
	TIter next(pmd_mod->Hits());
	StPmdHit *spmcl;
	for(Int_t im=0; im<nmh; im++)
	  {
	    spmcl = (StPmdHit*)next();
	    if(spmcl){
             Float_t clueta,cluphi,x,y;
	     Int_t ypad=spmcl->Row();       //! row of the hit
	     Int_t xpad=spmcl->Column();    //! column of the hit
	     edep=spmcl->Edep();            //! edep 
	     gsuper = spmcl->Gsuper();      //! supermodule
	     idet=spmcl->SubDetector();    //!  detector(= 2) for Cpv

	     //! x,y,eta,phi of the hits has been calculated
	     geom->DetCell_xy(gsuper,Float_t(ypad),Float_t(xpad),x,y,clueta,cluphi); 

	     //! Hits of CPV are written as clusters here 

             StPmdCluster *pclust = new StPmdCluster();
             Int_t members=1; //! As we are taking only one CPV cell
             pclust->setModule(id);
             pclust->setCluEdep(edep);
             pclust->setCluEta(clueta);
             pclust->setCluPhi(cluphi);
             pclust->setNumofMems(Int_t(members));    
             pmdclus->addCluster(pclust);
	    }
	  }
      }
    }
  }
}
//-----------------------------------------
void StPmdClustering::printclust(Int_t i, Int_t clno, Int_t idet,StPmdClusterCollection *pmdclus)
{

  //! for printing cluster details ( position ( x and y ), and strength
  //! info about # of cells in a cluster to be added. 
  Float_t x0, y0, x, y,xc,yc,zc, cells;
  for(Int_t m=0;m<=clno;m++){
    xc = clusters[0][m];yc = clusters[1][m];  //!cluster position
    zc = clusters[2][m];   //! cluster strength
    cells = clusters[3][m];  //! number of cells in the cluster	
    Float_t clusigma = clusters[4][m];	 //! sigma of the cluster
    Float_t cluedep=zc; y0 = yc/sqrth; x0 = xc - y0/2.;
    Float_t clueta,cluphi;

    geom->DetCell_xy(i,y0+1,x0+1,x,y,clueta,cluphi);

    StPmdCluster *pclust = new StPmdCluster();
    Int_t members=(Int_t)cells;

    if(idet==1){
    pclust->setModule(i);
    pclust->setCluEdep(cluedep);
    pclust->setCluEta(clueta);
    pclust->setCluPhi(cluphi);
    pclust->setCluSigma(clusigma);
    pclust->setNumofMems(Int_t(members));    
    pmdclus->addCluster(pclust);
    }
  }
}
//------------------------------------------------------

//! order the data according to edep ( largest to smallest )
void StPmdClustering::order(Int_t idet)
{
  // arranging in descending order. Simple sort used. The array itself 
  // is not sorted but an index array is sorted. 
  Double_t d[nmx], dd[nmx], adum;
  Int_t i, j, i1, i2, iord1[nmx], itst, idum;
  if(idet==1){
    for(i1=0; i1 < 96; i1++){
      for(i2=0; i2 < 72; i2++){
	i=i1+i2*96;

	d[i]=d1[i1][i2];iord1[i]=i; dd[i]=d[i];
      }
    }
  }

  for(j=1; j < nmx; j++){

    itst=0; adum=d[j]; idum=iord1[j];
    for(i1=0; i1 < j ; i1++){
      if(adum > d[i1] && itst == 0){
        itst=1;
        for(i2=j-1; i2 >= i1 ; i2=i2--){
          d[i2+1]=d[i2]; iord1[i2+1]=iord1[i2];
        }
        d[i1]=adum; iord1[i1]=idum;
      }
    }
  }
  for(i=0; i < nmx; i++){
    j=iord1[i]; i2=j/96; i1=j-i2*96; iord[0][i]=i1; iord[1][i]=i2;
  }
}
//---------------------------------------------

//! arrange cells in each supercluster
void StPmdClustering::arrange(Int_t incr)
{
  Int_t i, j, k, i1, itest, ihld1, ihld2, ihld3;
  i1=-1; 
  for(i=0; i<96; i++){
    for(j=0; j<72; j++){
      if(infocl[0][i][j] == 1){i1=i1+1;
      inford[0][i1]=infocl[1][i][j]; inford[1][i1]=i; inford[2][i1]=j;
      }
      if(infocl[0][i][j] == 2){i1=i1+1;
      inford[0][i1]=infocl[1][i][j]; inford[1][i1]=i; inford[2][i1]=j;
      }
    }
  }
  for(i=1; i < incr; i++){
    itest=0; ihld1=inford[0][i]; ihld2=inford[1][i]; ihld3=inford[2][i];
    for(j=0; j<i; j++){
      if(itest == 0 && inford[0][j] > ihld1){
        itest=1; 
        for(k=i-1; k>=j; k--){
          inford[0][k+1]=inford[0][k]; inford[1][k+1]=inford[1][k]; 
          inford[2][k+1]=inford[2][k];
        }
        inford[0][j]=ihld1; inford[1][j]=ihld2; inford[2][j]=ihld3;
      }
    }
  }
}

//-------------------------------------------------
/*! Breaking of super clusters into smaller clusters,
 * Cluster positions,strengths,and widths has has been calculated
 * by adopting minimization process. */
 
void StPmdClustering::refclust(Int_t incr, Int_t supmod, Int_t idet,StPmdClusterCollection *pmdclus)
{
  Int_t clno, i, j, k, i1, i2, id, icl, ncl[2000], iord[2000], itest, ihld;
  Int_t ig, nsupcl,lev1[20],lev2[20];
  Double_t x[2000], y[2000], z[2000], x1, y1, z1, x2, y2, z2, rr,dist;
  Double_t xc[2000], yc[2000], zc[2000], d[96][72], cells[2000],sum,rc[2000];
  //! clno counts the final clusters
  //! nsupcl =  # of superclusters; ncl[i]= # of cells in supercluster i
  clno=-1;
  nsupcl=-1;
  if(idet == 1){
    for(i1=0; i1<96; i1++){
      for(i2=0; i2<72; i2++){
	d[i1][i2]=d1[i1][i2];
      }
    }
  }
  for(i=0; i<1000; i++){ncl[i]=-1;}
  for(i=0; i<incr; i++){
    if(inford[0][i] != nsupcl){ nsupcl=nsupcl+1; }
    ncl[nsupcl]=ncl[nsupcl]+1;
  }
//  cout<<"# of super cells "<<incr<<" # of supercluster"<<nsupcl<<endl;
  id=-1;
  icl=-1;

  for(i=0; i<=nsupcl; i++){
    if(ncl[i] == 0){ id=id+1; icl=icl+1;

    //! one and two cell super-clusters --> single cluster

    clno=clno+1; i1=inford[1][id]; i2=inford[2][id];
    clusters[0][clno]=coord[0][i1][i2]; clusters[1][clno]=coord[1][i1][i2];
    clusters[2][clno]=d[i1][i2]; clusters[3][clno]=1.;clusters[4][clno]=0.5;
    }else if(ncl[i] == 1){
      id=id+1; icl=icl+1;
      clno=clno+1; i1=inford[1][id]; i2=inford[2][id]; x1=coord[0][i1][i2];
      y1=coord[1][i1][i2]; z1=d[i1][i2];
      id=id+1; i1=inford[1][id]; i2=inford[2][id];
      x2=coord[0][i1][i2]; y2=coord[1][i1][i2]; z2=d[i1][i2];
      clusters[0][clno]=(x1*z1+x2*z2)/(z1+z2); 
      clusters[1][clno]=(y1*z1+y2*z2)/(z1+z2);
      clusters[2][clno]=z1+z2; clusters[3][clno]=2.; clusters[4][clno]=0.5;
    }else{
      id=id+1; iord[0]=0;
      /*! super-cluster of more than two cells - broken up into smaller clusters
       * gaussian centers computed. (peaks separated by > 1 cell). Start from top */
      i1=inford[1][id]; i2=inford[2][id];
      x[0]=coord[0][i1][i2]; y[0]=coord[1][i1][i2]; z[0]=d[i1][i2];iord[0]=0;
      for(j=1;j<=ncl[i];j++){
        id=id+1;
        i1=inford[1][id]; i2=inford[2][id];iord[j]=j;
        x[j]=coord[0][i1][i2]; y[j]=coord[1][i1][i2]; z[j]=d[i1][i2];
      }
      //!ordering
      for(j=1;j<=ncl[i];j++){
        itest=0; ihld=iord[j];
        for(i1=0;i1<j;i1++){
          if(itest == 0 && z[iord[i1]] < z[ihld]){
            itest=1;
            for(i2=j-1;i2>=i1;i2--){
              iord[i2+1]=iord[i2];
            }
            iord[i1]=ihld;
          }
        }
      }
      //! compute the number of Gaussians and their centers ( first guess )
      ig=0;
      xc[ig]=x[iord[0]]; yc[ig]=y[iord[0]]; zc[ig]=z[iord[0]];
      for(j=1;j<=ncl[i];j++){itest=-1; x1=x[iord[j]]; y1=y[iord[j]];
      for(k=0;k<=ig;k++){x2=xc[k]; y2=yc[k]; rr=Dist(x1,y1,x2,y2);
      if( rr >= 1.1 && rr < 1.8 && z[iord[j]] > zc[k]/4.)itest=itest+1;
      if( rr >= 1.8 && rr < 2.1 && z[iord[j]] > zc[k]/10.)itest=itest+1;
      if( rr >= 2.1)itest=itest+1;
      } 
      if(itest == ig){
	ig=ig+1; xc[ig]=x1; yc[ig]=y1; zc[ig]=z[iord[j]];
      }
      }
      gaussfit(ncl[i], ig, x[0], y[0] ,z[0], xc[0], yc[0], zc[0], rc[0]);
      icl=icl+ig+1;
      for(j=0; j<=ig; j++){
        cells[j]=0.;
      }
      if(ig > 0){
	for(j=0; j<=ncl[i]; j++){
	  lev1[0]=0; lev2[0]=0;
	  for(k=0; k<=ig; k++){
	    dist=Dist(x[j], y[j], xc[k], yc[k]);
	    if(dist < sqrt(3.) ){
	      lev1[0]++; i1=lev1[0]; lev1[i1]=k;
	    }else{
	      if(dist < 2.1){
		lev2[0]++; i1=lev2[0]; lev2[i1]=k;
	      }
	    }
	  }
	  if(lev1[0] != 0){
	    if(lev1[0] == 1){cells[lev1[1]]=cells[lev1[1]]+1.;}
	    else{
	      sum=0.;
	      for(k=1; k<=lev1[0]; k++){
		sum=sum+zc[lev1[k]];
	      }
	      for(k=1; k<=lev1[0]; k++){
		cells[lev1[k]]=cells[lev1[k]]+zc[lev1[k]]/sum;
	      }
	    }
	  }else{
	    if(lev2[0] == 0){cells[lev2[1]]=cells[lev2[1]]+1.;}
	    else{
	      sum=0.;
	      for(k=1; k<=lev2[0]; k++){
		sum=sum+zc[lev2[k]];
	      }
	      for(k=1; k<=lev2[0]; k++){
		cells[lev2[k]]=cells[lev2[k]]+zc[lev2[k]]/sum;
	      }
	    }
	  }
	}
      }
      for(j=0; j<=ig; j++){
        clno=clno+1; clusters[0][clno]=xc[j]; clusters[1][clno]=yc[j]; 
        clusters[2][clno]=zc[j];clusters[4][clno]=rc[j];
	if(ig == 0){
	  clusters[3][clno]=ncl[i];
	}else{
	  clusters[3][clno]=cells[j];
	}
      }
    }
  }
  //!  cout<<"CLUSTER NUMBER IS "<<clno<<"supmod**"<<supmod<<endl;
  printclust(supmod, clno, idet,pmdclus);

}
//-------------------------------------------------------

//! minimization has been done by varying the cluster position,strength,width
void StPmdClustering::gaussfit(Int_t ncell, Int_t nclust, Double_t &x, Double_t &y ,Double_t &z, Double_t &xc, Double_t &yc, Double_t &zc, Double_t &rc) 
{
  Int_t i, j, i1, i2, jmax, novar, idd, jj;
  Double_t xx[2000], yy[2000], zz[2000], xxc[1000], yyc[1000], zzc[1000], 
    rrc[1000];
  Double_t a[1000], b[1000], c[1000], d[1000], ha[1000], hb[1000], hc[1000], 
    hd[1000];
  Int_t neib[1000][100];
  Double_t sum, dx, dy, str, str1, aint, sum1, rr, dum;
  Double_t x1, x2, y1, y2;
  str=0.; str1=0.; rr=0.3; novar=0;
  for(i=0; i<=ncell; i++){
    xx[i]=*(&x+i); yy[i]=*(&y+i); zz[i]=*(&z+i);str=str+zz[i];
  }
  for(i=0; i<=nclust; i++){
    xxc[i]=*(&xc+i); yyc[i]=*(&yc+i); zzc[i]=*(&zc+i); str1=str1+zzc[i]; 
    rrc[i]=0.5;
  }
  for(i=0; i<=nclust; i++){
    zzc[i]=str/str1*zzc[i];
    ha[i]=xxc[i]; hb[i]=yyc[i]; hc[i]=zzc[i]; hd[i]=rrc[i];x1=xxc[i]; y1=yyc[i];
  }
  for(i=0; i<=ncell; i++){
    idd=0; x1=xx[i]; y1=yy[i];
    for(j=0; j<=nclust; j++){
      x2=xxc[j]; y2=yyc[j];
      if(Dist(x1,y1,x2,y2) <= 3.){ idd=idd+1; neib[i][idd]=j; }
    }
    neib[i][0]=idd;
  }
  sum=0.;
  for(i1=0; i1<=ncell; i1++){
    aint=0.; idd=neib[i1][0];
    for(i2=1; i2<=idd; i2++){
      jj=neib[i1][i2];
      dx=xx[i1]-xxc[jj]; dy=yy[i1]-yyc[jj]; dum=rrc[j]*rrc[jj]+rr*rr;
      aint=aint+exp(-(dx*dx+dy*dy)/dum)*zzc[idd]*rr*rr/dum;
    }
    sum=sum+(aint-zz[i1])*(aint-zz[i1])/str;
  }
  jmax=nclust*1000; if(nclust > 20)jmax=20000;
  for(j=0; j<jmax; j++){
    str1=0.;
    for(i=0; i<=nclust; i++){
      a[i]=xxc[i]+0.6*(ranmar()-0.5); b[i]=yyc[i]+0.6*(ranmar()-0.5);
      c[i]=zzc[i]*(1.+(ranmar()-0.5)*0.2); str1=str1+zzc[i];
      d[i]=rrc[i]*(1.+(ranmar()-0.5)*0.1);if(d[i] < 0.25)d[i]=0.25;
    }
    for(i=0; i<=nclust; i++){ c[i]=c[i]*str/str1; }
    sum1=0.;
    for(i1=0; i1<=ncell; i1++){
      aint=0.; idd=neib[i1][0];
      for(i2=1; i2<=idd; i2++){
        jj=neib[i1][i2];
        dx=xx[i1]-a[jj]; dy=yy[i1]-b[jj]; dum=d[jj]*d[jj]+rr*rr;
        aint=aint+exp(-(dx*dx+dy*dy)/dum)*c[i2]*rr*rr/dum;
      }
      sum1=sum1+(aint-zz[i1])*(aint-zz[i1])/str;
    }
    if(sum1 < sum){
      for(i2=0; i2<=nclust; i2++){
        xxc[i2]=a[i2]; yyc[i2]=b[i2]; zzc[i2]=c[i2]; rrc[i2]=d[i2]; sum=sum1;
      }
    }
  }
  for(j=0; j<=nclust; j++){
    *(&xc+j)=xxc[j]; *(&yc+j)=yyc[j]; *(&zc+j)=zzc[j]; *(&rc+j)=rrc[j];
  }
}

//----------------------------------------------

//! distance between centre of two clusters
Double_t StPmdClustering::Dist(Double_t x1, Double_t y1, Double_t x2, Double_t y2)
{
  return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}
//----------------------------------------------
/*! Super clusters are constructed by using crclust function and
 * complete algorithm for clustering has been adopted */

Int_t StPmdClustering::crclust(Double_t ave, Double_t cutoff, Int_t nmx1, Int_t idet)
{
   Double_t xx, yy, d[96][72];
   Int_t i,j,k,id1,id2,icl,clust[2][3000], numcell, cellcount;
   Int_t jd1,jd2,icell;

   //! constants used for calculating coordinate of neighbouring cells
   static Int_t neibx[6]={1,0,-1,-1,0,1}, neiby[6]={0,1,1,0,-1,-1};


   if(idet==1){
     for (j=0; j < 96; j++){
       for(k=0; k < 72; k++){
	 d[j][k]=d1[j][k];
       }
     }
   }
   for (j=0; j < 96; j++){
     for(k=0; k < 72; k++){
       for (i=0; i < 2; i++){infocl[i][j][k] = 0;} //! initialize infocl[2][96][72]
     }
   }
   cellcount=0;
   for(i=0; i < nmx; i++){
     id1=iord[0][i]; id2=iord[1][i];
     if(d[id1][id2] <= cutoff){infocl[0][id1][id2]=-1;}
   }
   /*! crude clustering begins. Start with cell having largest adc count and 
    * loop over the cells in descending order of adc count */
   icl=-1;
   for(icell=0; icell <= nmx1; icell++){

     id1=iord[0][icell]; id2=iord[1][icell]; xx=id1+id2/2.; yy=sqrth*id2;
     if(infocl[0][id1][id2] == 0 ){

       /*! icl --cluster #, numcell -- # of cells in a cluster, clust -- stores 
	* coordinates of the cells in a cluster, infocl[0][i1][i2] is 1 
	*for primary and 2 for secondary cells, infocl[1][i1][i2] stores cluster # */

       icl=icl+1; numcell=0;
       for(i=0; i < 3000; i++){
         clust[0][i]=0; clust[1][i]=0;
       }
       clust[0][numcell]=id1; clust[1][numcell]=id2;
       infocl[0][id1][id2]=1; infocl[1][id1][id2]=icl;
       //! check for adc count in neighbouring cells. If ne 0 include them in this clust
       for(i=0; i<6; i++){
         jd1=id1+neibx[i]; jd2=id2+neiby[i];
         if( (jd1 >= 0 && jd1 < 96) && (jd2 >= 0 && jd2 < 72) && d[jd1][jd2] > 
             cutoff && infocl[0][jd1][jd2] == 0){
           numcell=numcell+1; clust[0][numcell]=jd1; clust[1][numcell]=jd2;
           infocl[0][jd1][jd2]=2; infocl[1][jd1][jd2]=icl;
           xx=jd1+jd2/2.; yy=sqrth*jd2; 
         }
       }
       for(i=1;i < 3000;i++){
         if(clust[0][i] != 0){
           id1=clust[0][i]; id2=clust[1][i];
           for(j=0; j<6 ; j++){
             jd1=id1+neibx[j]; jd2=id2+neiby[j];
             if( (jd1 >= 0 && jd1 < 96) && (jd2 >= 0 && jd2 < 72) && 
                 d[jd1][jd2] >  cutoff && infocl[0][jd1][jd2] == 0 ){
               infocl[0][jd1][jd2]=2; infocl[1][jd1][jd2]=icl;
               numcell=numcell+1;
               clust[0][numcell]=jd1; clust[1][numcell]=jd2;
               xx=jd1+jd2/2.; yy=sqrth*jd2;
             }
           }
         }
       }
       cellcount=cellcount+numcell+1;
     }
   }
   //   cout<<"CRCLUST**, icl  "<<icl<<endl;
   return cellcount;
}
//------------------------------------------
/*!Universal random number generator proposed by Marsaglia and Zaman
 * in report FSU-SCRI-87-50 
 * converted to C by S. C. Phatak
 * Seeding is done by using system clock.*/
 
Double_t StPmdClustering::ranmar()
{
  //  clock_t start;
  Int_t ii, jj;
  static Int_t i=96, j=32, itest=0, i1, i2, i3, i4, i5;
  static Double_t u[97], c, cd, cm, s, t;
  static Double_t uni;
  Int_t count1,count2,idum;
  /*    $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$  */
  if (itest == 0) {
    
    /*    start = time(NULL); //! used for random value of ii & jj
    ii=start;
    jj=start;
    if(ii > 31328 ) ii = ii - ( ii / 31328 ) * 31328;
    if(jj > 30081 ) jj = jj - ( jj / 30081 ) * 30081;*/

    ii = 1253;  //!  random value
    jj = 3035;  //!  ramdom value
    itest=itest+1;
    if((( ii > 0 ) &&  ( ii <= 31328 )) && (( jj > 0 ) && ( jj <= 30081 ))){
      i1=ii/177+2; i2=ii-(i1-2)*177+2; i3=jj/169+1; i4=jj-(i3-1)*169; 
      i4 = jj - (i3-1)*169;
      count1=0;
      while ( count1 < 97 ){
        s=0.;
        t=0.5;
        count2=0;
        while( count2 < 24 ){
          idum=i1*i2/179;
          idum=( i1*i2 - (i1*i2/179)*179 ) * i3;
          i5=idum-(idum/179)*179;
          i1=i2; i2=i3; i3=i5; idum=53*i4+1; i4=idum-(idum/169)*169;
          if( i4*i5-((i4*i5)/64)*64 >= 32 ) s=s+t;
          t=0.5*t;
          count2=count2+1;
        }
        u[count1] = s;
        count1 = count1 +1;
      }
      c = 362436./16777216.;  cd = 7654321./16777216.; cm = 16777213./16777216.;
    }
    else{
      cout << " wrong initialization " << endl;
    }
  }
  else{
    uni = u[i] - u[j]; if( uni < 0.) uni = uni + 1; u[i] = uni; i = i -1;
    if( i < 0 ) i = 96; j = j - 1; if ( j < 0 ) j = 96; c = c - cd;
    if( c < 0. ) c = c + cm; uni = uni - c ; if( uni < 0. ) uni = uni + 1.;
    return uni;
  }
  return kStOK;
}        
//-----------------------------------------------------









