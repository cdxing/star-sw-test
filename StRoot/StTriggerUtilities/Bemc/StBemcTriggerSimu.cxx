#include "StBemcTriggerSimu.h"

//General
#include <TH2.h>
#include <StMessMgr.h>

//Bemc
#include "StDaqLib/EMC/StEmcDecoder.h"
#include "StEmcUtil/geometry/StEmcGeom.h"
#include "StEmcUtil/others/emcDetectorName.h"
#include "StEmcRawMaker/defines.h"
#include "StEmcRawMaker/StBemcRaw.h"
#include "StEmcRawMaker/StBemcTables.h"
#include "StEmcRawMaker/StEmcRawMaker.h"
#include "StBemcTriggerDbThresholds.h"

//StEvent
#include "St_DataSetIter.h"
#include "StEvent/StEvent.h"
#include "StEvent/StEventTypes.h"
#include "StEvent/StEmcTriggerDetector.h"
#include "StEvent/StL0Trigger.h"

//MuDst
#include "StMuDSTMaker/COMMON/StMuDstMaker.h"
#include "StMuDSTMaker/COMMON/StMuDst.h"
#include "StMuDSTMaker/COMMON/StMuEvent.h"
#include "StMuDSTMaker/COMMON/StMuEmcCollection.h"

//Db
#include "St_db_Maker/St_db_Maker.h"
  

ClassImp(StBemcTriggerSimu);

//==================================================

StBemcTriggerSimu::StBemcTriggerSimu() {
  
  LOG_INFO<<"StBemcTriggerSimu::constructor"<<endm; 

  mMuDstMaker=new StMuDstMaker();
  muDst=new StMuDst();
  mEvent=new StEvent();
  mDecoder = new StEmcDecoder();
  mDbThres = new StBemcTriggerDbThresholds();
  mGeo=StEmcGeom::getEmcGeom("bemc");
  
  mHList=0;
  
}

//==================================================
StBemcTriggerSimu::~StBemcTriggerSimu(){ 

  LOG_INFO<<"StBemcTriggerSimu::deconstructor"<<endl;

}


//==================================================
//==================================================
void StBemcTriggerSimu::Init(){
 
  LOG_INFO <<"StBemcTriggerSimu::Init()"<<endm;
}


 
//==================================================
//==================================================
void StBemcTriggerSimu::InitRun(){
 
  LOG_INFO<<"StBemcTriggerSimu::InitRun()"<<endm;

  assert(starDb);

  int yyyy=starDb->GetDateTime().GetYear();   

  HT_FEE_Offset=mDbThres->GetHtFEEbitOffset(yyyy);

}


//==================================================
//==================================================
void  
StBemcTriggerSimu::Clear(){
  
  LOG_INFO <<"StBemcTriggerSimu::Clear()"<<endm;

  //set all adcs and pedestals to 0
  for (int i=0;i<kNTowers;i++){
    adc08[i]=0;
    adc10[i]=0;
    adc12[i]=0;
    ped08[i]=0;
    ped10[i]=0;
    ped12[i]=0;
  }

  //for offline config all TP status is good by definition. 
  //Only individual towers are masked out
  if (config=="offline") {
    for (int i=0;i<kNPatches;i++){
      PatchStatus[i]=1;
    }
  }

  //for online config it is possible to mask TP from DSM side
  if (config=="online"){
    for (int i=0;i<kNPatches;i++){
      PatchStatus[i]=1;
    }
  }

  if (config=="expert"){
    //you set here exactly your patch status - default to all good
    for (int i=0;i<kNPatches;i++){
      PatchStatus[i]=1;
    }
  }


}
  
//==================================================
//==================================================
void  
StBemcTriggerSimu::addTriggerList( void * adr){

}


     
//==================================================
//==================================================
void StBemcTriggerSimu::Make(){

  LOG_INFO<<"StBemcTriggerSimu::Maker()"<<endl;

  Clear();
  
  if(!mEvent)
    {
      LOG_WARN << "StBemcTriggerSimu -- no StEvent!" << endm;
    }
  
  StEmcCollection *emc = mEvent->emcCollection();
  if(!emc)
    {
      LOG_WARN << "StBemcTriggerSimu -- no StEmcCollection!" << endm;
    }
  
  StEmcDetector* detector=emc->detector(kBarrelEmcTowerId);
  if(!detector)
    {
      LOG_WARN << "StBemcTriggerSimu -- no StEmcDetector!" << endm;
    }

 //loop through BEMC hits and store 8,10,12 bit pedestal adjusted adcs for all hits
  if(detector)
    {
      for(Int_t m = 1; m <= 120; ++m)
        {
	  StEmcModule* module = detector->module(m);
	  if(module)
            {
	      StSPtrVecEmcRawHit& rawHit=module->hits();
	      for(UInt_t k = 0; k < rawHit.size(); ++k)
                {
		  if(rawHit[k])
                    {

		      Int_t mod=rawHit[k]->module();
		      Int_t e=rawHit[k]->eta();
		      Int_t s=abs(rawHit[k]->sub());
		      mGeo->getId(mod,e,s,did);

		      if (config == "offline") mTables->getPedestal(BTOW,did,0,ped,rms);
		      if (config == "online") mTables->getPedestal(BTOW,did,0,ped,rms);
		      adc=rawHit[k]->adc();

		      adc12[did-1]=(Int_t)adc;
		      adc10[did-1]=adc12[did-1] >> 2;
		      adc08[did-1]=adc10[did-1] >> 2;

		      ped12[did-1]=(Int_t)ped;
		      ped10[did-1]=ped12[did-1] >> 2;
		      ped08[did-1]=ped10[did-1] >> 2;

		    }
		}
	    }
	}
    }


  //Loop through Trigger Patches and find 6 bit HT and 6 bit TP FEE ADC 
  for(int tpid = 0; tpid < kNPatches; ++tpid) 
    {      
      if(PatchStatus[tpid]==1) 
        {
        
            int crate = 0;
            int seq  = 0;
            //int HT = 0;
            //int PA = 0;
            //int HTID = -1;
            int id;
            //int patchPed = 0;

            mDecoder->GetCrateAndSequenceFromTriggerPatch(tpid,crate,seq);
            
            //loop over each tower(j) in the trigger patch (tpid)
            for(int j = seq; j < seq + 16; ++j)
            {
            
                int stat = mDecoder->GetTowerIdFromCrate(crate,j,id);
                if(stat == 1)
                {
            
		  /* if(adc10[id-1]>=HT)
                    {
		      HT = adc10[id-1];
		      HTID = id;
                    }
		  
		  patchPed += ped10[id-1] >> 2;
		  PA += adc08[id-1];
		  */
                }
		
            }
	    
            // now HT=10 bits and patch=12 bits
            // convert patch sum to 6 bits using LUT
            // during 2006 LUT's looked like this:
            // 0,0,0,...,0,1,2,3,...,63,63,63 -- total 4096 entries
            // <-- ped -->
            // the number of 0's is equal to patchPed_12bit
	    /*
            if(PA >= patchPed){
	      mTrigger.Patch[i] = PA - (patchPed - 1);
	      if(mTrigger.Patch[i] > 62)  mTrigger.Patch[i] = 62;
            }
	    
            else
	      {
                mTrigger.Patch[i] = 1;
	      }
            
	    // for HT need to:
            //1) drop lowest bits (depends on calibration)
            //2) take next 6 LSB as HT 6 bit adc
            //3) if 6 or higher bit ==1 need to set all bits high (63)
            
	    HT = HT >> mTrigger.HTBits - 1;
	    int HTL = HT & 0x1F;//5 LSB
            int HTH = HT >> 5;  //>= 6 LSB
            int B5  = 0;
            if(HTH>0) B5 = 1;
            mTrigger.HT[i] = HTL+(B5<<5);
            mTrigger.HTID[i] = HTID;
            
	    {
	      LOG_DEBUG <<"Patch number "<<i<<" Tower id = "<<mTrigger.HTID[i]<<" adc12 = "<<adc12[HTID-1]
			<<" adc10 = "<<adc10[HTID-1]<<" adc08 = "<<adc08[HTID-1]<<" HT10 = "<<HT<<" PA12 = "<<PA
			<<" HT = "<<mTrigger.HT[i]<<" PA = "<<mTrigger.Patch[i]<<endm; 
            }
            
            if(mTrigger.HT[i]>HTmax)
            {
	      HTmax=mTrigger.HT[i];
	      HTmaxID=HTID;
            }
    
	    for (int matrix=0; matrix<6; matrix++){
	      
	      if (HTID>2400) {//East
		if(mTrigger.HT[i]>HT2EAST_TH_2006[matrix])
		  {
		    HT2_2006_array[matrix][numHT2_2006[matrix]]=HTID;
		    numHT2_2006[matrix]++;
		  }
	      }
	      
	      if (HTID<=2400) {//West
		if(mTrigger.HT[i]>HT2WEST_TH_2006[matrix])
		  {
		    HT2_2006_array[matrix][numHT2_2006[matrix]]=HTID;
		    numHT2_2006[matrix]++;
		  }
	      }
	    }
	    */
	}
    }            
}

/*
//copied from Oleksandr's BEMC_DSM_decoder.cxx
//is input 12 bit ped and 12 bit adc?
void StBemcTriggerSimu::simulateFEEaction(int adc, int ped, int bitConv, int &ht, int &pa) {

  //12 bit ADC comes into FEE and first need to drop 2 lowest bits
  int adc1 = adc >> 2; 

  //if 5th bit of ped written in binary ==1(0) then operation bit == 16(0)
  //specifically  if int{ped/16} = odd(even) operationBit==16(0)
  //if ped=0-15,32-47,... operationBit==0
  //if ped=16-31,48-63,... operationBit==16
  int operationBit = ped & 0x10;  
  
  //pedestal = remainder of ped/16
  int pedestal = ped & 0x0F;
  
  //if operationBit ==0 adc2 =adc1 + pedestal
  //if operationBit ==16 adc2 = adc1 - pedestal
  int adc2 = operationBit ? (adc1 - pedestal) : (adc1 + pedestal);
  
  // drop 2 lowest bits to give 8 bits total
  int adc3 = adc2 >> 2;   
  pa = adc3;

  //ht needs a 6 bit window determined by year and calibration
  if (bitConv == 0) {
    ht = adc2;
  } else if (bitConv == 1) {
    int adc4 = ((adc2 >> 1) & 0x1F) | ((adc2 & 0x03C0) ? 0x20 : 0);
    ht = adc4;
  } else if (bitConv == 2) {
    int adc4 = ((adc2 >> 2) & 0x1F) | ((adc2 & 0x0380) ? 0x20 : 0);
    ht = adc4;
  } else if (bitConv == 3) {
    int adc4 = ((adc2 >> 3) & 0x1F) | ((adc2 & 0x0300) ? 0x20 : 0);
    ht = adc4;
  }
  cout << "Simulating FEE: adc = " << adc << ", ped = " << ped << ", bitConv = " << bitConv<<endl;
  cout << "pedestal = " << pedestal << ", adc2 = " << adc2<<endl;
  cout << "HT = " << ht << ", PA = " << pa << endl;
}
*/
