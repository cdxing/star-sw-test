/***************************************************************************
 * $Id: StFmsDbMaker.h,v 1.5 2015/02/26 23:48:29 yuxip Exp $
 * \author: akio ogawa
 ***************************************************************************
 *
 * Description: FMS DB access Maker
 * Please visit http://drupal.star.bnl.gov/STAR/subsys/fms/database/stfmsdbmaker for more information
 *
 ***************************************************************************
 *
 * $Log: StFmsDbMaker.h,v $
 * Revision 1.5  2015/02/26 23:48:29  yuxip
 * fps functionalities added, interface to fms_rec table added
 *
 * Revision 1.4  2014/08/06 11:43:15  jeromel
 * Suffix on literals need to be space (later gcc compiler makes it an error) - first wave of fixes
 *
 * Revision 1.3  2014/04/24 21:15:04  tpb
 * Change default name to fmsDb to match BFC
 *
 * Revision 1.2  2010/01/11 20:35:30  jgma
 * Added reversed map and some other minor updates
 *
 * Revision 1.1  2009/10/28 16:11:15  jgma
 * This is the first check in of the code.
 *
 **************************************************************************/

#ifndef STFMSDBMAKER_H
#define STFMSDBMAKER_H

#ifndef StMaker_H
#include "StMaker.h"
#endif
#include "StThreeVectorF.hh"
#include "StRoot/StFmsUtil/StFmsDbConfig.h"

struct fmsDetectorPosition_st;
struct fmsChannelGeometry_st;
struct fmsMap_st;
struct fmsPatchPanelMap_st;
struct fmsQTMap_st;
struct fmsGain_st;
struct fmsGainCorrection_st;
struct fmsRec_st;
struct fpsConstant_st;
struct fpsChannelGeometry_st;
struct fpsSlatId_st;
struct fpsPosition_st;
struct fpsMap_st;
struct fpsGain_st;

class StFmsDbMaker : public StMaker {
 public: 
  StFmsDbMaker(const Char_t *name="fmsDb");
  virtual       ~StFmsDbMaker();
  virtual Int_t  Init();
  virtual Int_t  InitRun(Int_t runNumber);
  virtual Int_t  Make();
  virtual Int_t  Finish();
  virtual void   Clear(const Char_t *opt);

  void setDebug(Int_t debug); ///< debug mode, 0 for minimal message, >0 for more debug messages

  //! getting the whole table
  fmsChannelGeometry_st*  ChannelGeometry();
  fmsDetectorPosition_st* DetectorPosition();
  fmsMap_st*              Map();
  fmsPatchPanelMap_st*    PatchPanelMap();
  fmsQTMap_st*            QTMap();
  fmsGain_st*             Gain();
  fmsGainCorrection_st*   GainCorrection();
  fmsRec_st*              RecPar(); //reconstruction related parameters
  fpsConstant_st*         FpsConstant();
  fpsChannelGeometry_st** FpsChannelGeometry();
  fpsSlatId_st*           FpsSlatId();
  fpsPosition_st*         FpsPosition();
  fpsMap_st*              FpsMap();
  fpsGain_st*             FpsGain();

  //! Utility functions related to FMS ChannelGeometry
  Int_t maxDetectorId(); //! maximum value of detector Id
  Int_t detectorId(Int_t ew, Int_t ns, Int_t type);  //! convert to detector Id
  Int_t eastWest(Int_t detectorId); //! east or west to the STAR IP
  Int_t northSouth(Int_t detectorId); //! north or south side
  Int_t type(Int_t detectorId); //! type of the detector
  Int_t nRow(Int_t detectorId); //! number of rows
  Int_t nColumn(Int_t detectorId); //! number of column
  Int_t maxChannel(Int_t detectorId); //! maximum number of channels
  Int_t getRowNumber(Int_t detectorId, Int_t ch); //! get the row number for the channel
  Int_t getColumnNumber(Int_t detectorId, Int_t ch); //! get the column number for the channel
  Int_t getChannelNumber(Int_t detectorId, Int_t row, Int_t column); //! get the channel number

  //! Utility functions related to DetectorPosition
  StThreeVectorF getDetectorOffset(Int_t detectorId);  //! get the offset of the detector
  Float_t getXWidth(Int_t detectorId); //! get the X width of the cell
  Float_t getYWidth(Int_t detectorId); //! get the Y width of the cell
  StThreeVectorF getStarXYZ(Int_t detectorId,Float_t FmsX, Float_t FmsY); //! get the STAR frame coordinates
  Float_t getPhi(Int_t detectorId,Float_t FmsX, Float_t FmsY); //! get the STAR frame phi angle
  Float_t getEta(Int_t detectorId,Float_t FmsX, Float_t FmsY, Float_t Vertex); //! get the STAR frame pseudo rapidity assuming vertex is at (0,0,0)

  //! fmsMap related
  Int_t maxMap();
  void getMap(Int_t detectorId, Int_t ch, Int_t* qtCrate, Int_t* qtSlot, Int_t* qtChannel);
  void getReverseMap(Int_t qtCrate, Int_t qtSlot, Int_t qtChannel, Int_t* detectorId, Int_t* ch);

  //! fmsPatchPanelMap related
  Int_t maxModule();

  //! fmsQTMap related
  Int_t maxNS();

  //! fmsGain/GainCorrection related
  Int_t maxGain();
  Int_t maxGainCorrection();
  Float_t getGain(Int_t detectorId, Int_t ch); //! get the gain for the channel
  Float_t getGainCorrection(Int_t detectorId, Int_t ch); //! get the gain correction for the channel
  
  //reference to StFmsDbConfig
  StFmsDbConfig& getRecConfig();  

  void forceUniformGain(float v)           {mForceUniformGain=v;          } //! force gain to be specified value                                       
  void forceUniformGainCorrection(float v) {mForceUniformGainCorrection=v;} //! force gaincorr to be specified value                                   

  //! FPS related
  Int_t   fpsNQuad();
  Int_t   fpsNLayer();
  Int_t   fpsMaxSlat();
  Int_t   fpsMaxQTaddr();
  Int_t   fpsMaxQTch();
  Int_t   fpsMaxSlatId();
  Int_t   fpsNSlat(int quad, int layer);
  Int_t   fpsSlatId(int quad, int layer, int slat);
  void    fpsQLSfromSlatId(int slatid, int* quad, int* layer, int* slat);
  void    fpsPosition(int slatid, float xyz[3], float dxyz[3]);
  void    fpsPosition(int quad, int layer, int slat, float xyz[3], float dxyz[3]);
  void    fpsQTMap(int slatid, int* QTaddr, int* QTch);
  Int_t   fpsSlatidFromQT(int QTaddr, int QTch);
  void    fpsQLSFromQT(int QTaddr, int QTch, int* quad, int* layer, int* slat);
  Float_t fpsGain(int slatid);
  Float_t fpsGain(int quad, int layer, int slat);
  Int_t   fpsSlatIdFromG2t(int g2tvolid);

  //! text dump for debugging
  void dumpFmsChannelGeometry (const Char_t* filename="dumpFmsChannelGeometry.txt");
  void dumpFmsDetectorPosition(const Char_t* filename="dumpFmsDetectorPosition.txt");
  void dumpFmsMap             (const Char_t* filename="dumpFmsMap.txt");
  void dumpFmsPatchPanelMap   (const Char_t* filename="dumpFmsPatchPanelMap.txt");
  void dumpFmsQTMap           (const Char_t* filename="dumpFmsQTMap.txt");
  void dumpFmsGain            (const Char_t* filename="dumpFmsGain.txt");
  void dumpFmsGainCorrection  (const Char_t* filename="dumpFmsGainCorrection.txt");
  void dumpFpsConstant        (const Char_t* filename="dumpFpsConstant.txt");
  void dumpFpsChannelGeometry (const Char_t* filename="dumpFpsChannelGeometry.txt");
  void dumpFpsSlatId          (const Char_t* filename="dumpSlatId.txt"); 
  void dumpFpsPosition        (const Char_t* filename="dumpFpsPosition.txt");
  void dumpFpsMap             (const Char_t* filename="dumpFpsMap.txt");
  void dumpFpsGain            (const Char_t* filename="dumpFpsGain.txt");
  void dumpFmsRec             (const Char_t* filename="dumpFmsRec.txt");

 private:
  void                  deleteArrays();
  Int_t                 mDebug; //! >0 dump tables to text files

  fmsChannelGeometry_st *mChannelGeometry;  //! channel configuration for each detector
  Int_t                 mMaxDetectorId;  //! max detector Id

  fmsDetectorPosition_st *mDetectorPosition;  //! position (in STAR frame) of each detector

  fmsMap_st             *mMap;    //! detector map
  fmsMap_st             **mmMap;
  Int_t                  mMaxMap;
  enum {mMaxCrate=8, mMaxSlot=17, mMaxCh=32};
  Int_t                  mReverseMapDetectorId[mMaxCrate][mMaxSlot][mMaxCh]; //!
  Int_t                  mReverseMapChannel[mMaxCrate][mMaxSlot][mMaxCh];    //!

  fmsPatchPanelMap_st   *mPatchPanelMap; //! patch panel map
  Int_t                  mMaxModule;

  fmsQTMap_st           *mQTMap;  //! Qt map
  Int_t                  mMaxNS;

  fmsGain_st            *mGain;   //! gain table
  fmsGain_st            **mmGain; 
  Int_t                  mMaxGain;

  fmsGainCorrection_st  *mGainCorrection; //! gain correction table
  fmsGainCorrection_st  **mmGainCorrection;
  Int_t                   mMaxGainCorrection;

  fmsRec_st             *mRecPar; //! rec. parameters table
  Int_t                 mMaxRecPar;
  StFmsDbConfig&        mRecConfig; //reference to StFmsDbConfig singleton, for accessing rec. parameter values by name

  Float_t                 mForceUniformGain; //!                                                                                                       
  Float_t                 mForceUniformGainCorrection; //!                                                                                             

  fpsConstant_st*         mFpsConstant;
  Int_t                   mMaxSlatId;
  fpsChannelGeometry_st** mFpsChannelGeometry;
  fpsSlatId_st*           mFpsSlatId;
  Int_t***                mFpsReverseSlatId;
  fpsPosition_st*         mFpsPosition;
  fpsMap_st*              mFpsMap;
  Int_t**                 mFpsReverseMap;
  fpsGain_st*             mFpsGain;
  
  virtual const Char_t *GetCVS() const {static const Char_t cvs[]="Tag " __DATE__ " " __TIME__ ; return cvs;}
  ClassDef(StFmsDbMaker,2)   //StAF chain virtual base class for Makers
};

//! Global pointners:
R__EXTERN StFmsDbMaker* gStFmsDbMaker;
#endif


