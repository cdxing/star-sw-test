#ifndef trgStructures2019_h
#define trgStructures2019_h
/******
*
*     Layout of new Trigger Data Block
*
*     J.M. Nelson        30 January 2009
*
*     Notes:  The event descriptor will describe data from either
*     the Mk1 or Mk2 TCUs.   The variable TCU_Mark will be 1 for Mk1
*     and 2 for the Mk2 TCU.  Variables not used by one or other of the
*     TCUs will be zero.   
*
*     The data block structure will always begin with a 4 character
*     name, followed by the byte-count of data following.  The structure of
*     data will depend on the configuration of particular crates.  
*
*     Note:  PrePost data will only be available on local trigger disks and
*     will not be present in event files.
*
*     8Dec16: JMN changed FORMAT_VERSION
*     3Oct17: je - removed DAQ10k TPCpreMask from MIX added EPDlayer# to BBC
*     6Dec18: je - modified BBCBlock structure for added EPD DSMs
*     4Jan19: JMN removed redundant info from BBCBlock
*
******************************************************************************/
#define y19FORMAT_VERSION        0x18120646      /* Format: yymmddvv */
#define y19MAX_TRG_BLK_SIZE          122896      /* Current total: 113.25k bytes for pre/post non-zero suppressed data.  Allow 120k */
#define y19MAX_OFFLEN                    20      /* Depends on the number of crates in the system */

#define y19MAX_CONF_NUM      20
#define y19RCC_CONF_NUM       0
#define y19L1_CONF_NUM        1
#define y19BC1_CONF_NUM       2
#define y19MXQ_CONF_NUM       3
#define y19MIX_CONF_NUM       4
#define y19BCW_CONF_NUM       5
#define y19BCE_CONF_NUM       6
#define y19EQ3_CONF_NUM       7
#define y19BBC_CONF_NUM       8
#define y19BBQ_CONF_NUM       9
#define y19FMS_CONF_NUM      10
#define y19QT1_CONF_NUM      11
#define y19QT2_CONF_NUM      12
#define y19QT3_CONF_NUM      13
#define y19QT4_CONF_NUM      14
#define y19EQ1_CONF_NUM      15
#define y19EQ2_CONF_NUM      16
#define y19INF_CONF_NUM      20

#define y19ADD_BIT_PREPILEUP      0              /* pileup present */
#define y19ADD_BIT_POSTPILEUP     1
#define y19ADD_BIT_FORCE          5              /* Force store of this event */
#define y19ADD_BIT_L2_5           6              /* Level 2.5 abort */
#define y19ADD_BIT_SIM            7              /* Simulated event - used by DAQ */

    /* Event Descriptor Data Structures */
    
//#pragma pack(1)

typedef struct {
    char           name[3];                     /* Contains  EVD */
    char           TrgDataFmtVer;               /* Exception for use by DAQ (LS byte of FORMAT_VERSION) */
    int            length;                      /* Byte count of data that follows */
    unsigned int   bunchXing_hi;
    unsigned int   bunchXing_lo;                /* Two parts of RHIC bunch crossing number */
    unsigned short actionWdDetectorBitMask;     /* from Fifo 1 */
    unsigned char  actionWdTrgCommand;          /* from Fifo 1 */
    unsigned char  actionWdDaqCommand;          /* from Fifo 1 */  
    unsigned short TrgToken;                    /* from Fifo 2 */
    unsigned short addBits;                     /* used by trigger/daq: bit 5=Force store; bit 6=L2.5 abort; bit 7=1 is fake data */
    unsigned short DSMInput;                    /* only for use with Mk1 TCU.  0 if Mk2 TCU is used */
    unsigned short externalBusy;                /* from Fifo 9 (Fifo 3 Mk1 TCU) */
    unsigned short internalBusy;                /* from Fifo 9 (Mk2 TCU) */
    unsigned short trgDetMask;                  // After 11/8/16
    unsigned short tcuCtrBunch_hi;               // After 11/8/16
    unsigned short DSMAddress;                  /* from Fifo 10 (Fifo 6 Mk1 TCU) */
    unsigned short TCU_Mark;                    /* TCU_Mark Mk1=1 Mk2=2 */
    unsigned short npre;        // (crate_mask & 0xfff) << 4 | npre      (after 11/8/16)
    unsigned short npost;       // (crate_mask & 0xfff000)>>8| npost     (after 11/8/16)
    unsigned short res1;        // (crate_mask & 0xff000000)>>20 | res1&0xf    (after 11/8/16)
} EvtDescData2019;

//#pragma pack()

      /* L1 DSM data structures */

typedef struct {
  char               name[4];                 /* Contains  L1DS */
  int                length;                  /* Byte count of data that follows */
  unsigned short     TOF[8];                  /* TOF and MTD data */
  unsigned short     VTX[8];                  /* Separate VPD, ZDC and BBC DSMs have been replaced with this one */
  unsigned short     EMC[8];                  /* Contents of 1 EMC IB - results of separate BEMC and EEMC DSMs */
  unsigned short     TPCMask[8];              /* TPC mask for DAQ10K */
  unsigned short     BCdata[16];              /* Contents of 2 Bunch Crossing DSMs IB's */       
  unsigned short     specialTriggers[8];      /* Contents of 1 Special Trigger DSM - all the special trigger requests */
  unsigned short     FPD[8];                  /* Contents of 1 FMS and FPD IB */
  unsigned short     lastDSM[8];              /* Contents of last DSM IB - results of all DSM trees */
} L1_DSM_Data2019;

      /* Trigger Summary Data Structures */

typedef struct {
  char           name[4];                     /* Contains  TSUM */
  int            length;                      /* Byte count of data that follows */
  unsigned int   L1Sum[2];                    /* L1 Summary */
  unsigned int   L2Sum[2];                    /* L2 Summary */
  unsigned int   L1Result[32];                /* Result from L1 CPU */
  unsigned int   L2Result[64];                /* Result from L2 CPU */
  unsigned int   C2Result[64];                /* Result from last algorithm */
  unsigned int   LocalClocks[32];	      /* localClock values from RCC2*/
} TrgSumData2019;

typedef struct {
  char name[4];
  int length;                                 /* Byte count of data that follows */
  unsigned int data[1];                       /* NB: this definition is generic but would vary depending on actual data */
} DataBlock2019;

typedef struct {
  char name[4];                               /* Contains BBC */
  int length;                                 /* Byte count of data that follows */
  unsigned short BBClayer1[8];                /* BBC-small layer1 DSM feeding VT201 DSM */
  unsigned short EPDlayer1a[8];		      /* layer1 DSM feeding QT32C TAC info to VT201 DSM */
  unsigned short ZDClayer1[8];                /* layer1 ZDC DSM that feeds the VT201 DSM */
  unsigned short VPD[8];                      /* layer1 VPD DSM feeding ADC & TAC values to VT201*/
  unsigned short EPDlayer0t[16];	      /* layer0 EPD DSM feeding east & west TAC to EP101 */
  unsigned short EPDlayer1b[8];		      /* 2nd layer1 EPD DSM taking EPD QT ADC data to VT201 */
  unsigned short EPDlayer0a[16];              /* layer0 EPD DSM feeding east & west QT32C adcs to EP102 */
  unsigned char EPDlayer0h[32]; 	      /* layer0 EPD DSM feeding east & west QT32B to EP102 */
} BBCBlock2019;


typedef struct {
  char name[4];                               /* Contains MIX */
  int length;                                 /* Byte count of data that follows */
  unsigned short FPDEastNSLayer1[8];          /* FPD east north/south layer 1  */  
  unsigned char  MTD_P2PLayer1[16];           /* Data from MTD and PP2PP */
  unsigned short TOFLayer1[8];                /* This is TOF Layer 1 */
  unsigned short TOF[48];                     /* TOF data */
/*  unsigned short TPCpreMask[24]; */         /* EMC, MTD, & TOF TPC Grid Masks je-removed starting run 18 */
} MIXBlock2019;

typedef struct  {
  char name[4];
  int length;                                 /* Byte count of data that follows */
  int dataLoss;                               /* Byte count of data truncated due to buffer limitations */
  unsigned int data[1];                       /* NB: this definition is generic but would vary depending on actual data */
} QTBlock2019;

typedef struct {
  char name[4];
  int length;
  unsigned char BEMCEast[240];                /* 15 DSMs covering the East half of BEMC */
} BEastBlock2019; 

typedef struct {
  char name[4];
  int length;
  unsigned char BEMCWest[240];                /* 15 DSMs covering the West half of BEMC */
} BWestBlock2019;

typedef struct {
  char name[4];
  int length;
  unsigned short BEMClayer1[48];              /* 6 DSMs for BEMC at layer1 */
  unsigned short EEMClayer1[16];              /* 2 DSMs for EEMC at layer1 */
  unsigned char  EEMC[144];                   /* 9 DSMs for EEMC at layer0 */
} BELayerBlock2019;

typedef struct {
  char name[4];
  int length;
  unsigned char FMS[256];                     /* 16 DSMs for FMS */
} FMSBlock2019;

typedef struct {
  int offset;                                 /* Offset (in bytes) from the start of Trigger block to data */
  int length;                                 /* Length (in bytes) of data */
} TrgOfflen2019; 

typedef struct {
  int FormatVersion;                          /* Trigger Data Definition Version yymmddvv */
  int totalTriggerLength;                     /* Total length (bytes) of complete Trigger Block */
  int eventNumber;                            /* Event number in this run */
  TrgOfflen2019 EventDesc_ofl;                    /* Offset/length pair to Event Descriptor */
  TrgOfflen2019 L1_DSM_ofl;                       /* Offset/length pair to L1 DSM Data */
  TrgOfflen2019 Summary_ofl;                      /* Offset/length pair to Summary Data */
  TrgOfflen2019 MainX[y19MAX_OFFLEN];                /* Offset/length pairs for main crossing */
  int PrePostList[10];                        /* Offsets to offset/length pairs to Pre and Post crossing */
  int raw_data[y19MAX_TRG_BLK_SIZE/4];           /* Storage for raw data */
} TriggerDataBlk2019;

#endif


