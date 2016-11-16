/*****************************************************************************
 *   2008-2013, RIGOL Technologies,Inc.  All Rights Reserved.                     *
 *****************************************************************************/

/*****************************************************************************
 *  RIGOL rgdm3k Digital Multimeter Instrument Driver
 *  LabWindows/CVI Instrument Driver
 *  Original Release: 2008-8-16
 *  By: LiJiansheng, RIGOL Technologies,Inc.
 *
 *  Modification History:
 *   Rev  1.1, 07/2009
 *		- Changed  some SCPI command for DM3058.
 *              - Changed  attrResolutionDCRangeTableForRGDM3K,
 *                         attrMathDbmReferenceRangeTable,
 *                         attrMathOperationRangeTable,
 *                         attrMathLimit_Freq_RangeTable,
 *                         attrMathLimit_Resist_RangeTable,
 *                         attrMathLimit_Current_RangeTable,
 *                         attrMathLimit_CAP_RangeTable.
 *              - Add attrTriggerDelayRangeTable.
 *              - Add delay time in some function.
 *
 *   2008-8-16  Instrument Driver Created.
 *   2013-9-29  Porting driver to 64 bit.
 *
 *****************************************************************************/


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <crtdbg.h>
#include "RGDM3K.h"


/*****************************************************************************
 *---------------------Attribute Declarations -----------------------*
 *****************************************************************************/

#define RGDM3K_ATTR_RANGE_VDC                 (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 2L)
#define RGDM3K_ATTR_RANGE_VAC                 (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 4L)
#define RGDM3K_ATTR_RANGE_ADC                 (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 5L)
#define RGDM3K_ATTR_RANGE_AAC                 (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 6L)
#define RGDM3K_ATTR_RANGE_RES                 (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 7L)
#define RGDM3K_ATTR_INVALIDATE_NULL_OFFSET    (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 15L)
#define RGDM3K_ATTR_INVALIDATE_DB_REFERENCE   (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 16L)
#define RGDM3K_ATTR_RANGE_CAP             	(IVI_SPECIFIC_PRIVATE_ATTR_BASE + 17L)
#define RGDM3K_ATTR_RESOLUTION_VDC          (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 18L)
#define RGDM3K_ATTR_RESOLUTION_FRES         (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 19L)
#define RGDM3K_ATTR_RESOLUTION_ADC          (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 20L)
#define RGDM3K_ATTR_RESOLUTION_CAP          (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 21L)
#define RGDM3K_ATTR_RESOLUTION_RES          (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 22L)
#define RGDM3K_ATTR_RANGE_FREQ              (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 23L)
#define RGDM3K_ATTR_RANGE_PERIOD            (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 24L)
#define RGDM3K_ATTR_RANGE_FRES              (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 25L)


/*****************************************************************************
 *---------------------------- Useful Macros --------------------------------*
 *****************************************************************************/

/*- I/O buffer size -----------------------------------------------------*/
#define BUFFER_SIZE                             512L
#define EXTENDED_BUFFER_SIZE                    8300L        /*  For Multi-Point Fetch function */

/*- 488.2 Event Status Register (ESR) Bits ------------------------------*/
//#define IEEE_488_2_QUERY_ERROR_BIT              0x04
//#define IEEE_488_2_DEVICE_DEPENDENT_ERROR_BIT   0x08
//#define IEEE_488_2_EXECUTION_ERROR_BIT          0x10
//#define IEEE_488_2_COMMAND_ERROR_BIT            0x20

/*- List of channels passed to the Ivi_BuildChannelTable function -------*/
#define CHANNEL_LIST                            "1"



static IviRangeTableEntry attrTriggerDelayRangeTableEntries[] = {
    {400, 2000, 400, "", 0},
    {IVI_RANGE_TABLE_LAST_ENTRY}
};

static IviRangeTable attrTriggerDelayRangeTable = {
    IVI_VAL_RANGED,
    VI_TRUE,
    VI_TRUE,
    VI_NULL,
    attrTriggerDelayRangeTableEntries,
};



/*****************************************************************************
 *----------------------------range table entry --------------------------------*
 *****************************************************************************/

static IviRangeTableEntry attrTriggerSlopeRangeTableEntries[] = {
    {RGDM3K_VAL_POSITIVE, 0, 0, "POS", 0},
    {RGDM3K_VAL_NEGATIVE, 0, 0, "NEG", 0},
    {IVI_RANGE_TABLE_LAST_ENTRY}
};

static IviRangeTable attrTriggerSlopeRangeTable = {
    IVI_VAL_DISCRETE,
    VI_FALSE,
    VI_FALSE,
    VI_NULL,
    attrTriggerSlopeRangeTableEntries,
};

static IviRangeTableEntry attrMathLimit_CAP_RangeTableEntries[] = {
    { -0.012, 1.2e-2, 0, "", 0},
    {IVI_RANGE_TABLE_LAST_ENTRY}
};

static IviRangeTable attrMathLimit_CAP_RangeTable = {
    IVI_VAL_RANGED,
    VI_TRUE,
    VI_TRUE,
    VI_NULL,
    attrMathLimit_CAP_RangeTableEntries,
};

static IviRangeTableEntry attrMathLimit_Current_RangeTableEntries[] = {
    { -12, 12, 0, "", 0},
    {IVI_RANGE_TABLE_LAST_ENTRY}
};

static IviRangeTable attrMathLimit_Current_RangeTable = {
    IVI_VAL_RANGED,
    VI_TRUE,
    VI_TRUE,
    VI_NULL,
    attrMathLimit_Current_RangeTableEntries,
};

static IviRangeTableEntry attrMathLimit_Period_RangeTableEntries[] = {
    { -1.2, 1.2, 0, "", 0},
    {IVI_RANGE_TABLE_LAST_ENTRY}
};

static IviRangeTable attrMathLimit_Period_RangeTable = {
    IVI_VAL_RANGED,
    VI_TRUE,
    VI_TRUE,
    VI_NULL,
    attrMathLimit_Period_RangeTableEntries,
};



static IviRangeTableEntry attrMathLimit_Resist_RangeTableEntries[] = {
    { -1.20E8, 1.20E8, 0, "", 0},
    {IVI_RANGE_TABLE_LAST_ENTRY}
};

static IviRangeTable attrMathLimit_Resist_RangeTable = {
    IVI_VAL_RANGED,
    VI_TRUE,
    VI_TRUE,
    VI_NULL,
    attrMathLimit_Resist_RangeTableEntries,
};

static IviRangeTableEntry attrMathLimit_Volts_RangeTableEntries[] = {
    { -1200.0, 1200.0, 0, "", 0},
    {IVI_RANGE_TABLE_LAST_ENTRY}
};

static IviRangeTable attrMathLimit_Volts_RangeTable = {
    IVI_VAL_RANGED,
    VI_TRUE,
    VI_TRUE,
    VI_NULL,
    attrMathLimit_Volts_RangeTableEntries,
};



static IviRangeTableEntry attrMathLimit_Freq_RangeTableEntries[] = {
    { -1.2E6, 1.2E6, 0, "", 0},
    {IVI_RANGE_TABLE_LAST_ENTRY}
};

static IviRangeTable attrMathLimit_Freq_RangeTable = {
    IVI_VAL_RANGED,
    VI_TRUE,
    VI_TRUE,
    VI_NULL,
    attrMathLimit_Freq_RangeTableEntries,
};



static IviRangeTableEntry attrMathDbmReferenceRangeTableEntries[] = {
    {2, 8000, 0, "", 0},
    {IVI_RANGE_TABLE_LAST_ENTRY}
};

static IviRangeTable attrMathDbmReferenceRangeTable = {
    IVI_VAL_RANGED,
    VI_TRUE,
    VI_TRUE,
    VI_NULL,
    attrMathDbmReferenceRangeTableEntries,
};

static IviRangeTableEntry attrMathDbReferenceRangeTableEntries[] = {
    { -120, 120, 0, "", 0},
    {IVI_RANGE_TABLE_LAST_ENTRY}
};

static IviRangeTable attrMathDbReferenceRangeTable = {
    IVI_VAL_RANGED,
    VI_TRUE,
    VI_TRUE,
    VI_NULL,
    attrMathDbReferenceRangeTableEntries,
};

static IviRangeTableEntry attrMathOperationRangeTableEntries[] = {
    {RGDM3K_VAL_MATH_OFF, 0, 0, "NONE", 0},
    {RGDM3K_VAL_MATH_MATH_NULL, 0, 0, "REL", 0},
    {RGDM3K_VAL_MATH_DB, 0, 0, "DB", 0},
    {RGDM3K_VAL_MATH_DBM, 0, 0, "DBM", 0},
    {RGDM3K_VAL_MATH_MIN_MAX, 0, 0, "AVERAGE", 0},
    {RGDM3K_VAL_MATH_LIMIT, 0, 0, "PF", 0},
    {IVI_RANGE_TABLE_LAST_ENTRY}
};

static IviRangeTable attrMathOperationRangeTable = {
    IVI_VAL_DISCRETE,
    VI_TRUE,
    VI_TRUE,
    VI_NULL,
    attrMathOperationRangeTableEntries,
};





static IviRangeTableEntry attrThresholdVoltageRangeTableEntries[] = {
    {RGDM3K_VAL_AUTO_THRESHOLD_ON, RGDM3K_VAL_AUTO_THRESHOLD_ON, RGDM3K_VAL_AUTO_THRESHOLD_ON, "", 0},
    {RGDM3K_VAL_AUTO_THRESHOLD_OFF, RGDM3K_VAL_AUTO_THRESHOLD_OFF, RGDM3K_VAL_AUTO_THRESHOLD_OFF, "", 0},
    {RGDM3K_VAL_AUTO_THRESHOLD_ONCE, RGDM3K_VAL_AUTO_THRESHOLD_ONCE, RGDM3K_VAL_AUTO_THRESHOLD_ONCE, "", 0},
    {0.0, 0.1, 0.1, "", 0},
    {0.1, 1.0, 1.0, "", 0},
    {1.0, 10.0, 10.0, "", 0},
    {10.0, 100.0, 100.0, "", 0},
    {100.0, 750.0, 750.0, "", 0},
    {IVI_RANGE_TABLE_LAST_ENTRY}
};

static IviRangeTable attrThresholdVoltageRangeTable = {
    IVI_VAL_COERCED,
    VI_FALSE,
    VI_TRUE,
    VI_NULL,
    attrThresholdVoltageRangeTableEntries,
};

static IviRangeTableEntry AAC_RangeTableEntries[] = {
    {RGDM3K_VAL_AUTO_RANGE_ON, RGDM3K_VAL_AUTO_RANGE_ON, RGDM3K_VAL_AUTO_RANGE_ON, "", 0},
    {RGDM3K_VAL_AUTO_RANGE_OFF, RGDM3K_VAL_AUTO_RANGE_OFF, RGDM3K_VAL_AUTO_RANGE_OFF, "", 0},
    {RGDM3K_VAL_AUTO_RANGE_ONCE, RGDM3K_VAL_AUTO_RANGE_ONCE, RGDM3K_VAL_AUTO_RANGE_ONCE, "", 0},
    {0, 0.001, 0.0, "", 0},
    {1, 0.01, 0.001, "", 0},
    {2, 0.1, 0.01, "", 0},
    {3, 1.0, 0.1, "", 0},
    {4, 10.0, 1.0, "", 0},
    {RGDM3K_VAL_MINAAC, 0, 0, "", 0},
    {RGDM3K_VAL_DEFAAC, 0, 0, "", 0},
    {RGDM3K_VAL_MAXAAC, 0, 0, "", 0},
    {IVI_RANGE_TABLE_LAST_ENTRY}
};

static IviRangeTable AAC_RangeTable = {
    IVI_VAL_DISCRETE,
    VI_FALSE,
    VI_TRUE,
    VI_NULL,
    AAC_RangeTableEntries,
};



static IviRangeTableEntry VAC_RangeTableEntries[] = {
    {RGDM3K_VAL_AUTO_RANGE_ON, RGDM3K_VAL_AUTO_RANGE_ON, RGDM3K_VAL_AUTO_RANGE_ON, "", 0},
    {RGDM3K_VAL_AUTO_RANGE_OFF, RGDM3K_VAL_AUTO_RANGE_OFF, RGDM3K_VAL_AUTO_RANGE_OFF, "", 0},
    {RGDM3K_VAL_AUTO_RANGE_ONCE, RGDM3K_VAL_AUTO_RANGE_ONCE, RGDM3K_VAL_AUTO_RANGE_ONCE, "", 0},
    {0, 0.1, 0, "", 0},
    {1, 1, 0.1, "", 0},
    {2, 10, 1, "", 0},
    {3, 100, 10, "", 0},
    {4, 750, 100, "", 0},
    {RGDM3K_VAL_MAXVAC, 0, 0, "", 0},
    {RGDM3K_VAL_DEFVAC, 0, 0, "", 0},
    {RGDM3K_VAL_MINVAC, 0, 0, "", 0},
    {IVI_RANGE_TABLE_LAST_ENTRY}
};

static IviRangeTable VAC_RangeTable = {
    IVI_VAL_DISCRETE,
    VI_FALSE,
    VI_TRUE,
    VI_NULL,
    VAC_RangeTableEntries,
};

static IviRangeTableEntry VDC_RangeTableEntries[] = {
    {RGDM3K_VAL_AUTO_RANGE_ON, RGDM3K_VAL_AUTO_RANGE_ON, RGDM3K_VAL_AUTO_RANGE_ON, "", 0},
    {RGDM3K_VAL_AUTO_RANGE_OFF, RGDM3K_VAL_AUTO_RANGE_OFF, RGDM3K_VAL_AUTO_RANGE_OFF, "", 0},
    {RGDM3K_VAL_AUTO_RANGE_ONCE, RGDM3K_VAL_AUTO_RANGE_ONCE, RGDM3K_VAL_AUTO_RANGE_ONCE, "", 0},
    {0, 0.2, 0, "", 0},
    {1, 2, 0.2, "", 0},
    {2, 20, 2, "", 0},
    {3, 200, 20, "", 0},
    {4, 1000, 200, "", 0},
    {RGDM3K_VAL_MAXVDC, 0, 0, "", 0},
    {RGDM3K_VAL_DEFVDC, 0, 0, "", 0},
    {RGDM3K_VAL_MINVDC, 0, 0, "", 0},
    {IVI_RANGE_TABLE_LAST_ENTRY}
};

static IviRangeTable VDC_RangeTable = {
    IVI_VAL_DISCRETE,
    VI_FALSE,
    VI_TRUE,
    VI_NULL,
    VDC_RangeTableEntries,
};

static IviRangeTableEntry attrAcMinFreqTable1Entries[] = {
    {200.0, 300000.0, 200.0, "", 0},
    {20.0, 200.0, 20.0, "", 0},
    {3.0, 20.0, 3.0, "", 0},
    {IVI_RANGE_TABLE_LAST_ENTRY}
};

static IviRangeTable attrAcMinFreqTable1 = {
    IVI_VAL_COERCED,
    VI_TRUE,
    VI_TRUE,
    VI_NULL,
    attrAcMinFreqTable1Entries,
};




/*****************************************************************************
 *-------------- Utility Function Declarations (Non-Exported) ---------------*
 *****************************************************************************/
static ViStatus RGDM3K_InitAttributes ( ViSession vi, ViInt32 modelNum );
static ViStatus RGDM3K_AdjustAttrsTriggerSourceRngTbl ( ViSession vi );
static ViStatus RGDM3K_DefaultInstrSetup ( ViSession openInstrSession );
static ViStatus RGDM3K_CheckStatus ( ViSession vi );
//static ViStatus RGDM3K_GetActualRange (ViSession vi, ViSession io, ViReal64 *value);
static ViStatus RGDM3K_GetDriverSetupOption ( ViSession vi,
        ViConstString optionTag,
        ViInt32 optionValueSize,
        ViChar optionValue[] );


/*****************************************************************************
 *----------------- Callback Declarations (Non-Exported) --------------------*
 *****************************************************************************/

/*- Global Session Callbacks --------------------------------------------*/

//static ViStatus _VI_FUNC RGDM3K_CheckStatusCallback (ViSession vi, ViSession io);

/*- Attribute callbacks -------------------------------------------------*/

static ViStatus _VI_FUNC RGDM3KAttrSpecificDriverRevision_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        const ViConstString cacheValue );
static ViStatus _VI_FUNC RGDM3KAttrIdQueryResponse_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        const ViConstString cacheValue );
static ViStatus _VI_FUNC RGDM3KAttrInstrumentFirmwareRevision_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        const ViConstString cacheValue );
static ViStatus _VI_FUNC RGDM3KAttrInstrumentManufacturer_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        const ViConstString cacheValue );
static ViStatus _VI_FUNC RGDM3KAttrInstrumentModel_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        const ViConstString cacheValue );
static ViStatus _VI_FUNC RGDM3KAttrFunction_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViInt32 *value );
static ViStatus _VI_FUNC RGDM3KAttrFunction_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViInt32 value );
static ViStatus _VI_FUNC RGDM3KAttrRange_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value );
static ViStatus _VI_FUNC RGDM3KAttrRange_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value );
static ViStatus _VI_FUNC RGDM3KAttrTriggerSource_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViInt32 *value );
static ViStatus _VI_FUNC RGDM3KAttrTriggerSource_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViInt32 value );
static ViStatus _VI_FUNC RGDM3KAttrAcMaxFreq_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value );
static ViStatus _VI_FUNC RGDM3KAttrTriggerDelay_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value );
static ViStatus _VI_FUNC RGDM3KAttrTriggerDelay_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value );
static ViStatus _VI_FUNC RGDM3KAttrRangeVdc_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value );
static ViStatus _VI_FUNC RGDM3KAttrRangeVdc_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value );
static ViStatus _VI_FUNC RGDM3KAttrRangeAac_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value );
static ViStatus _VI_FUNC RGDM3KAttrRangeAac_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value );
static ViStatus _VI_FUNC RGDM3KAttrRangeVac_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value );
static ViStatus _VI_FUNC RGDM3KAttrRangeVac_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value );
static ViStatus _VI_FUNC RGDM3KAttrRangeAdc_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value );
static ViStatus _VI_FUNC RGDM3KAttrRangeAdc_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value );
static ViStatus _VI_FUNC RGDM3KAttrRangeFres_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value );
static ViStatus _VI_FUNC RGDM3KAttrRangeFres_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value );
static ViStatus _VI_FUNC RGDM3KAttrRangeRes_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value );
static ViStatus _VI_FUNC RGDM3KAttrRangeRes_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value );



static ViStatus _VI_FUNC RGDM3KAttrResolutionAbsolute_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value );

static ViStatus _VI_FUNC RGDM3KAttrResolutionAbsolute_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value );

static ViStatus _VI_FUNC RGDM3KAttrRangeCap_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value );

static ViStatus _VI_FUNC RGDM3KAttrRangeCap_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value );

static ViStatus _VI_FUNC RGDM3KAttrFreqVoltageRange_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value );

static ViStatus _VI_FUNC RGDM3KAttrFreqVoltageRange_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value );

static ViStatus _VI_FUNC RGDM3KAttrAcMinFreq_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value );

static ViStatus _VI_FUNC RGDM3KAttrAcMinFreq_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value );


static ViStatus _VI_FUNC RGDM3KAttrResolutionVdc_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value );

static ViStatus _VI_FUNC RGDM3KAttrResolutionVdc_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value );

static ViStatus _VI_FUNC RGDM3KAttrResolutionFres_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value );

static ViStatus _VI_FUNC RGDM3KAttrResolutionFres_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value );

static ViStatus _VI_FUNC RGDM3KAttrResolutionAdc_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value );

static ViStatus _VI_FUNC RGDM3KAttrResolutionAdc_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value );

static ViStatus _VI_FUNC RGDM3KAttrResolutionCap_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value );

static ViStatus _VI_FUNC RGDM3KAttrResolutionCap_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value );

static ViStatus _VI_FUNC RGDM3KAttrResolutionRes_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value );

static ViStatus _VI_FUNC RGDM3KAttrResolutionRes_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value );

static ViStatus _VI_FUNC RGDM3KAttrRangeFreq_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value );

static ViStatus _VI_FUNC RGDM3KAttrRangeFreq_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value );

static ViStatus _VI_FUNC RGDM3KAttrRangePeriod_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value );

static ViStatus _VI_FUNC RGDM3KAttrRangePeriod_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value );

static ViStatus _VI_FUNC RGDM3KAttrAcMaxFreq_RangeTableCallback ( ViSession vi,
        ViConstString channelName,
        ViAttr attributeId,
        IviRangeTablePtr *rangeTablePtr );

static ViStatus _VI_FUNC RGDM3KAttrAcMinFreq_RangeTableCallback ( ViSession vi,
        ViConstString channelName,
        ViAttr attributeId,
        IviRangeTablePtr *rangeTablePtr );

static ViStatus _VI_FUNC RGDM3KAttrFreqThresholdVoltage_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value );

static ViStatus _VI_FUNC RGDM3KAttrFreqThresholdVoltage_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value );

static ViStatus _VI_FUNC RGDM3KAttrPeriodThresholdVoltage_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value );

static ViStatus _VI_FUNC RGDM3KAttrPeriodThresholdVoltage_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value );



static ViStatus _VI_FUNC RGDM3KAttrMathOperation_CheckCallback ( ViSession vi,
        ViConstString channelName,
        ViAttr attributeId,
        ViInt32 value );

static ViStatus _VI_FUNC RGDM3KAttrMathOperation_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViInt32 *value );

static ViStatus _VI_FUNC RGDM3KAttrMathOperation_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViInt32 value );

static ViStatus _VI_FUNC RGDM3KAttrMathDbReference_CheckCallback ( ViSession vi,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value );

static ViStatus _VI_FUNC RGDM3KAttrMathDbReference_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value );

static ViStatus _VI_FUNC RGDM3KAttrMathDbReference_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value );

static ViStatus _VI_FUNC RGDM3KAttrMathDbmReference_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViInt32 *value );

static ViStatus _VI_FUNC RGDM3KAttrMathDbmReference_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViInt32 value );

static ViStatus _VI_FUNC RGDM3KAttrMathLimitUpper_RangeTableCallback ( ViSession vi,
        ViConstString channelName,
        ViAttr attributeId,
        IviRangeTablePtr *rangeTablePtr );

static ViStatus _VI_FUNC RGDM3KAttrMathLimitUpper_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value );

static ViStatus _VI_FUNC RGDM3KAttrMathLimitUpper_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value );

static ViStatus _VI_FUNC RGDM3KAttrMathLimitLower_RangeTableCallback ( ViSession vi,
        ViConstString channelName,
        ViAttr attributeId,
        IviRangeTablePtr *rangeTablePtr );

static ViStatus _VI_FUNC RGDM3KAttrMathLimitLower_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value );

static ViStatus _VI_FUNC RGDM3KAttrMathLimitLower_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value );

static ViStatus _VI_FUNC RGDM3KAttrMathNullOffset_CheckCallback ( ViSession vi,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value );

static ViStatus _VI_FUNC RGDM3KAttrMathNullOffset_RangeTableCallback ( ViSession vi,
        ViConstString channelName,
        ViAttr attributeId,
        IviRangeTablePtr *rangeTablePtr );

static ViStatus _VI_FUNC RGDM3KAttrMathNullOffset_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value );

static ViStatus _VI_FUNC RGDM3KAttrMathNullOffset_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value );



static ViStatus _VI_FUNC RGDM3KAttrTriggerSlope_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViInt32 *value );

static ViStatus _VI_FUNC RGDM3KAttrTriggerSlope_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViInt32 value );


/******************************************************************************
*--------------------------TIME DELAY--------------------------------*
*********************************************************************************/
static ViStatus RGDM3K_delaytime()
{
    ViInt32 n, m;

    for ( n = 0; n < 1000; n++ ) {
        for ( m = 0; m < 1000; m++ ) {

        }

    }

    return 0;
}

/*****************************************************************************
 *------------ User-Callable Functions (Exportable Functions) ---------------*
 *****************************************************************************/

/*****************************************************************************
 * Function: RGDM3K_init
 * Purpose:  VXIplug&play required function.  Calls the
 *           RGDM3K_InitWithOptions function.
 *****************************************************************************/
ViStatus _VI_FUNC RGDM3K_init ( ViRsrc resourceName, ViBoolean IDQuery,
                                ViBoolean resetDevice, ViSession *newVi )
{
    ViStatus    error = VI_SUCCESS;

    if ( newVi == VI_NULL ) {
        Ivi_SetErrorInfo ( VI_NULL, VI_FALSE, IVI_ERROR_INVALID_PARAMETER,
                           VI_ERROR_PARAMETER4, "Null address for Instrument Handle" );
        checkErr ( IVI_ERROR_INVALID_PARAMETER );
    }

    checkErr ( RGDM3K_InitWithOptions ( resourceName, IDQuery, resetDevice,
                                        "", newVi ) );

Error:
    return error;
}


/*****************************************************************************
 * Function: RGDM3K_InitWithOptions
 * Purpose:  This function creates a new IVI session and calls the
 *           IviInit function.
 *****************************************************************************/
ViStatus _VI_FUNC RGDM3K_InitWithOptions ( ViRsrc resourceName, ViBoolean IDQuery,
        ViBoolean resetDevice, ViConstString optionString,
        ViSession *newVi )
{
    ViStatus    error = VI_SUCCESS;
    ViSession   vi = VI_NULL;
    ViChar      newResourceName[IVI_MAX_MESSAGE_BUF_SIZE];
    ViChar      newOptionString[IVI_MAX_MESSAGE_BUF_SIZE];
    ViBoolean   isLogicalName;

    if ( newVi == VI_NULL ) {
        Ivi_SetErrorInfo ( VI_NULL, VI_FALSE, IVI_ERROR_INVALID_PARAMETER,
                           VI_ERROR_PARAMETER5, "Null address for Instrument Handle" );
        checkErr ( IVI_ERROR_INVALID_PARAMETER );
    }

    *newVi = VI_NULL;
    checkErr ( Ivi_GetInfoFromResourceName ( resourceName, ( ViString ) optionString, newResourceName,
               newOptionString, &isLogicalName ) );

    /* create a new interchangeable driver */
    checkErr ( Ivi_SpecificDriverNew ( "RGDM3K", newOptionString, &vi ) );

    if ( !isLogicalName ) {
        ViInt32 oldFlag = 0;

        checkErr ( Ivi_GetAttributeFlags ( vi, IVI_ATTR_IO_RESOURCE_DESCRIPTOR, &oldFlag ) );
        checkErr ( Ivi_SetAttributeFlags ( vi, IVI_ATTR_IO_RESOURCE_DESCRIPTOR, oldFlag & 0xfffb | 0x0010 ) );
        checkErr ( Ivi_SetAttributeViString ( vi, "", IVI_ATTR_IO_RESOURCE_DESCRIPTOR, 0, newResourceName ) );
    }

    /* init the driver */
    checkErr ( RGDM3K_IviInit ( newResourceName, IDQuery, resetDevice, vi ) );

    if ( isLogicalName )
    { checkErr ( Ivi_ApplyDefaultSetup ( vi ) ); }

    *newVi = vi;

Error:

    if ( error < VI_SUCCESS )
    { Ivi_Dispose ( vi ); }

    return error;
}


/*****************************************************************************
 * Function: RGDM3K_IviInit
 * Purpose:  This function is called by RGDM3K_InitWithOptions
 *           or by an IVI class driver.  This function initializes the I/O
 *           interface, optionally resets the device, optionally performs an
 *           ID query, and sends a default setup to the instrument.
 *****************************************************************************/
ViStatus _VI_FUNC RGDM3K_IviInit ( ViRsrc resourceName, ViBoolean IDQuery,
                                   ViBoolean reset, ViSession vi )
{
    ViStatus    error = VI_SUCCESS;
    ViSession   io = VI_NULL;
    ViUInt16    interface = VI_INTF_GPIB;
    ViChar      modelStr[20] = "";
    ViInt32     modelNum = 1;

    checkErr ( Ivi_BuildChannelTable ( vi, CHANNEL_LIST, VI_FALSE, VI_NULL ) );

    if ( !Ivi_Simulating ( vi ) ) {
        ViSession   rmSession = VI_NULL;

        /* Open instrument session */
        checkErr ( Ivi_GetAttributeViSession ( vi, VI_NULL, IVI_ATTR_VISA_RM_SESSION, 0,
                                               &rmSession ) );
        viCheckErr ( viOpen ( rmSession, resourceName, VI_NULL, VI_NULL, &io ) );
        /* io session owned by driver now */
        checkErr ( Ivi_SetAttributeViSession ( vi, VI_NULL, IVI_ATTR_IO_SESSION, 0, io ) );

        /* Configure VISA Formatted I/O */
        viCheckErr ( viSetAttribute ( io, VI_ATTR_TMO_VALUE, 50000 ) );
        viCheckErr ( viSetBuf ( io, VI_READ_BUF | VI_WRITE_BUF, 4000 ) );
        viCheckErr ( viSetAttribute ( io, VI_ATTR_WR_BUF_OPER_MODE, VI_FLUSH_ON_ACCESS ) );
        viCheckErr ( viSetAttribute ( io, VI_ATTR_RD_BUF_OPER_MODE, VI_FLUSH_ON_ACCESS ) );

        /* Configure VISA Serial I/O */
        viCheckErr ( viGetAttribute ( io, VI_ATTR_INTF_TYPE, &interface ) );

        if ( interface == VI_INTF_ASRL ) {
            viCheckErr ( viSetAttribute ( io, VI_ATTR_TERMCHAR, '\n' ) );
            viCheckErr ( viSetAttribute ( io, VI_ATTR_TERMCHAR_EN, VI_TRUE ) );

            viCheckErr ( viSetAttribute ( io, VI_ATTR_ASRL_END_IN, VI_ASRL_END_TERMCHAR ) );
            viCheckErr ( viSetAttribute ( io, VI_ATTR_ASRL_END_OUT, VI_ASRL_END_TERMCHAR ) );

            viCheckErr ( viSetAttribute ( io, VI_ATTR_ASRL_FLOW_CNTRL, VI_ASRL_FLOW_DTR_DSR ) );
            viCheckErr ( viSetAttribute ( io, VI_ATTR_ASRL_BAUD, 9600 ) );
            viCheckErr ( viSetAttribute ( io, VI_ATTR_ASRL_PARITY, VI_ASRL_PAR_EVEN ) );
            viCheckErr ( viSetAttribute ( io, VI_ATTR_ASRL_STOP_BITS, VI_ASRL_STOP_TWO ) );
            viCheckErr ( viSetAttribute ( io, VI_ATTR_ASRL_DATA_BITS, 7 ) );


        }


        /*- Identification Query ------------------------------------------------*/
        if ( IDQuery ) {
            ViChar rdBuffer[BUFFER_SIZE];
            ViChar *queryResponse1 = "Rigol Technologies,DM3";

            viCheckErr ( viQueryf ( io, "*IDN?\n", "%256[^\n]", rdBuffer ) );

            if ( strncmp ( rdBuffer, queryResponse1, strlen ( queryResponse1 ) ) == 0 )
            { modelNum = RGDM3K_VAL_MODEL_3058; }

            else
            { viCheckErr ( VI_ERROR_FAIL_ID_QUERY ); }
        }
    }

    /*- Simulation or IDQuery == VI_FALSE ---------------------------------------*/
    if ( ( Ivi_Simulating ( vi ) ) || ( !IDQuery ) ) {
        checkErr ( RGDM3K_GetDriverSetupOption ( vi, "Model", 20, modelStr ) );

        if ( ( strncmp ( modelStr, "3058", strlen ( "3058" ) ) == 0 ) || ( modelStr[0] == 0 ) )
        { modelNum = RGDM3K_VAL_MODEL_3058; }

        else
        { viCheckErrElab ( IVI_ERROR_INVALID_VALUE, "The model specified is not valid" ); }
    }

    /* Add attributes */
    checkErr ( RGDM3K_InitAttributes ( vi, modelNum ) );

    /*- Reset instrument ----------------------------------------------------*/
    if ( reset )
    { checkErr ( RGDM3K_reset ( vi ) ); }

    else  /*- Send Default Instrument Setup ---------------------------------*/
    { checkErr ( RGDM3K_DefaultInstrSetup ( vi ) ); }

    checkErr ( RGDM3K_CheckStatus ( vi ) );

Error:

    if ( error < VI_SUCCESS ) {
        if ( !Ivi_Simulating ( vi ) && io )
        { viClose ( io ); }
    }

    return error;
}


/*****************************************************************************
 * Function: RGDM3K_close
 * Purpose:  This function closes the instrument.
 *
 *           Note:  This function must unlock the session before calling
 *           Ivi_Dispose.
 *****************************************************************************/
ViStatus _VI_FUNC RGDM3K_close ( ViSession vi )
{
    ViStatus    error = VI_SUCCESS;

    checkErr ( Ivi_LockSession ( vi, VI_NULL ) );

    checkErr ( RGDM3K_IviClose ( vi ) );

Error:
    Ivi_UnlockSession ( vi, VI_NULL );
    Ivi_Dispose ( vi );

    return error;
}


/*****************************************************************************
 * Function: RGDM3K_IviClose
 * Purpose:  This function performs all of the drivers clean-up operations
 *           except for closing the IVI session.  This function is called by
 *           RGDM3K_close or by an IVI class driver.
 *
 *           Note:  This function must close the I/O session and set
 *           IVI_ATTR_IO_SESSION to 0.
 *****************************************************************************/
ViStatus _VI_FUNC RGDM3K_IviClose ( ViSession vi )
{
    ViStatus error = VI_SUCCESS;
    ViSession io = VI_NULL;

    /* Do not lock here.  The caller manages the lock. */
    checkErr ( Ivi_GetAttributeViSession ( vi, VI_NULL, IVI_ATTR_IO_SESSION, 0, &io ) );

Error:
    Ivi_SetAttributeViSession ( vi, VI_NULL, IVI_ATTR_IO_SESSION, 0, VI_NULL );

    if ( io ) {
        viClose ( io );
    }

    return error;
}


/*****************************************************************************
 * Function: RGDM3K_reset
 * Purpose:  This function resets the instrument.
 * Note: *RST is not be implemented.
 *****************************************************************************/
ViStatus _VI_FUNC RGDM3K_reset ( ViSession vi )
{
    ViStatus    error = VI_SUCCESS;

    checkErr ( Ivi_LockSession ( vi, VI_NULL ) );

    if ( !Ivi_Simulating ( vi ) ) {         /* call only when locked */
        ViSession   io = Ivi_IOSession ( vi ); /* call only when locked */

        checkErr ( Ivi_SetNeedToCheckStatus ( vi, VI_TRUE ) );
        viCheckErr ( viPrintf ( io, "*RST\n" ) );
    }

    checkErr ( RGDM3K_DefaultInstrSetup ( vi ) );

    checkErr ( RGDM3K_CheckStatus ( vi ) );

Error:
    Ivi_UnlockSession ( vi, VI_NULL );
    return error;
}


/*****************************************************************************
 * Function: RGDM3K_self_test
 * Purpose:  This function executes the instrument self-test and returns the
 *           result.
 * Note: Self test is not supported by instrumnet, so this function is not be
         implemented.
 *****************************************************************************/
ViStatus _VI_FUNC RGDM3K_self_test ( ViSession vi, ViInt16 *testResult,
                                     ViChar testMessage[] )
{
    ViStatus    error = VI_SUCCESS;
    ViSession   io = VI_NULL;
    ViUInt32    oldTimeout = 0;
    ViBoolean   needToRestoreTimeout = VI_FALSE;

    checkErr ( Ivi_LockSession ( vi, VI_NULL ) );

    if ( testResult == VI_NULL )
    { viCheckParm ( IVI_ERROR_INVALID_PARAMETER, 2, "Null address for Test Result" ); }

    if ( testMessage == VI_NULL )
    { viCheckParm ( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Test Message" ); }

    if ( !Ivi_Simulating ( vi ) ) {         /* call only when locked */
        io = Ivi_IOSession ( vi ); /* call only when locked */

        checkErr ( Ivi_SetNeedToCheckStatus ( vi, VI_TRUE ) );

        /* Store the old timeout so that it can be restored later */
        viCheckErr ( viGetAttribute ( io, VI_ATTR_TMO_VALUE, &oldTimeout ) );
        viCheckErr ( viSetAttribute ( io, VI_ATTR_TMO_VALUE, 10000 ) );
        needToRestoreTimeout = VI_TRUE;

        viCheckErr ( viPrintf ( io, "*TST?" ) );

        viCheckErr ( viScanf ( io, "%hd", testResult ) );

        strcpy ( testMessage, *testResult ? "Failed self-test." : "Passed self-test." );

    } else {
        /* Simulate Self Test */
        *testResult = 0;
        strcpy ( testMessage, "No error." );
    }

    checkErr ( Ivi_InvalidateAllAttributes ( vi ) ); /* Invalidate all attributes */

    checkErr ( RGDM3K_CheckStatus ( vi ) );

Error:

    if ( needToRestoreTimeout ) {
        /* Restore the original timeout */
        viSetAttribute ( io, VI_ATTR_TMO_VALUE, oldTimeout );
    }

    Ivi_UnlockSession ( vi, VI_NULL );
    return error;
}
/*****************************************************************************
 * Function: RGDM3K_error_query
 * Purpose:  This function queries the instrument error queue and returns
 *           the result.
 * Note: Error Query is not supported by instrument, so this function is not be
         implemented.
 *****************************************************************************/
ViStatus _VI_FUNC RGDM3K_error_query ( ViSession vi, ViInt32 *errCode,
                                       ViChar errMessage[] )
{
    ViStatus    error = VI_SUCCESS;

    checkErr ( Ivi_LockSession ( vi, VI_NULL ) );

    if ( errCode == VI_NULL )
    { viCheckParm ( IVI_ERROR_INVALID_PARAMETER, 2, "Null address for Error Code" ); }

    if ( errMessage == VI_NULL )
    { viCheckParm ( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Error Message" ); }

    if ( !Ivi_Simulating ( vi ) ) {         /* call only when locked */
        ViSession   io = Ivi_IOSession ( vi ); /* call only when locked */

        checkErr ( Ivi_SetNeedToCheckStatus ( vi, VI_TRUE ) );
        viCheckErr ( viPrintf ( io, ":SYST:ERR?" ) );

        viCheckErr ( viScanf ( io, "%ld,\"%256[^\"]", errCode, errMessage ) );

    } else {
        /* Simulate Error Query */
        *errCode = 0;
        strcpy ( errMessage, "No error." );
    }

Error:
    Ivi_UnlockSession ( vi, VI_NULL );
    return error;
}


/*****************************************************************************
 * Function: RGDM3K_error_message
 * Purpose:  This function translates the error codes returned by this
 *           instrument driver into user-readable strings.
 *
 * Note:  Error Message is not supported by instrument,so this function is not
          be implemented.
 *****************************************************************************/
ViStatus _VI_FUNC RGDM3K_error_message ( ViSession vi, ViStatus errorCode,
        ViChar errorMessage[256] )
{
    ViStatus    error = VI_SUCCESS;

    static      IviStringValueTable errorTable = {
        IVIDMM_ERROR_CODES_AND_MSGS,
        {VI_NULL,                               VI_NULL}
    };

    if ( vi )
    { Ivi_LockSession ( vi, VI_NULL ); }

    /* all VISA and IVI error codes are handled as well as codes in the table */
    if ( errorMessage == VI_NULL )
    { viCheckParm ( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Error Message" ); }

    checkErr ( Ivi_GetSpecificDriverStatusDesc ( vi, errorCode, errorMessage, errorTable ) );

Error:

    if ( vi )
    { Ivi_UnlockSession ( vi, VI_NULL ); }

    return error;
}


/*****************************************************************************
 * Function: RGDM3K_revision_query
 * Purpose:  This function returns the driver and instrument revisions.
 * note: this function is not  be implemented.
 *****************************************************************************/
ViStatus _VI_FUNC RGDM3K_revision_query ( ViSession vi, ViChar driverRev[],
        ViChar instrRev[] )
{
    ViStatus    error = VI_SUCCESS;

    checkErr ( Ivi_LockSession ( vi, VI_NULL ) );

    if ( driverRev == VI_NULL )
    { viCheckParm ( IVI_ERROR_INVALID_PARAMETER, 2, "Null address for Driver Revision" ); }

    if ( instrRev == VI_NULL )
    { viCheckParm ( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Instrument Revision" ); }

    checkErr ( Ivi_GetAttributeViString ( vi, VI_NULL, RGDM3K_ATTR_SPECIFIC_DRIVER_REVISION,
                                          0, 256, driverRev ) );
    checkErr ( Ivi_GetAttributeViString ( vi, "", RGDM3K_ATTR_INSTRUMENT_FIRMWARE_REVISION,
                                          0, 256, instrRev ) );
    checkErr ( RGDM3K_CheckStatus ( vi ) );

Error:
    Ivi_UnlockSession ( vi, VI_NULL );
    return error;
}


/*****************************************************************************
 * Function: RGDM3K_ConfigureMeasurement
 * Purpose:  Configures the common attributes of the DMM.  These attributes
 *           are RGDM3K_ATTR_FUNCTION,
 *               RGDM3K_ATTR_RANGE,
 *               RGDM3K_ATTR_RESOLUTION_ABSOLUTE

 RGDM3K_VAL_DC_VOLTS       - DC Volts
 RGDM3K_VAL_DC_CURRENT     - DC Current
 RGDM3K_VAL_AC_VOLTS       - AC Volts
 RGDM3K_VAL_AC_CURRENT     - AC Current
 RGDM3K_VAL_2_WIRE_RES     - 2 Wire Resistance
 RGDM3K_VAL_4_WIRE_RES     - 4 Wire Resistance
 RGDM3K_VAL_CAPACITANCE    - Capacitance
 RGDM3K_VAL_DIODE          - Diode
 RGDM3K_VAL_CONTINUITY     - Continuity
 RGDM3K_VAL_FREQ           - Frequency
 RGDM3K_VAL_PERIOD         - Period
 Input
 Name Type Description
 instrumentHandle ViSession The ViSession handle that you obtain from the RGDM3K_init or RGDM3K_InitWithOptions function.  The handle identifies a particular instrument session.
 Default Value:  None

 measurementFunction ViInt32 Specifies the measurement function you want the DMM to perform. The driver sets the RGDM3K_ATTR_FUNCTION attribute to this value.
 Defined Values:
 RGDM3K_VAL_DC_VOLTS       - DC Volts
 RGDM3K_VAL_DC_CURRENT     - DC Current
 RGDM3K_VAL_AC_VOLTS       - AC Volts
 RGDM3K_VAL_AC_CURRENT     - AC Current
 RGDM3K_VAL_2_WIRE_RES     - 2 Wire Resistance
 RGDM3K_VAL_4_WIRE_RES     - 4 Wire Resistance
 RGDM3K_VAL_CAPACITANCE    - Capacitance
 RGDM3K_VAL_DIODE          - Diode
 RGDM3K_VAL_CONTINUITY     - Continuity
 RGDM3K_VAL_FREQ           - Frequency
 RGDM3K_VAL_PERIOD         - Period

 range ViReal64 Specifies the measurement range you want to use. The driver sets the RGDM3K_ATTR_RANGE attribute to this value. Use positive values to specify the absolute value of the maximum expected measurement.
 The value of the measurement range must be in units appropriate for the RGDM3K_ATTR_FUNCTION attribute. For example, when you set the Measurement Function parameter to RGDM3K_VAL_DC_VOLTS, you must specify the range in volts.
 The driver reserves the following negative values for controlling the auto-ranging capability of the DMM.
 Defined Values:
 RGDM3K_VAL_AUTO_RANGE_OFF  (-2.0)  - Auto-Range Off
 RGDM3K_VAL_AUTO_RANGE_ON   (-1.0)  - Auto-Range On
 RGDM3K_VAL_AUTO_RANGE_ONCE (-3.0)  - Auto-Range Once
 Valid Manual Range :
 The valid manual range depends on the measurement function.  The following table lists the valid manual ranges.
 for vdc
 0   200mV   100 nV
 1   2V  1 ¦ÌV
 2   20V 10 ¦ÌV
 3   200V    100 ¦ÌV
 4   1000V   1 mV
 MIN 200mV   100 nV
 MAX 1000V   1 mV
 DEF 20V 10 ¦ÌV
 for vac
 0   200mV
 1   2.0V
 2   20.0V
 3   200.0V
 4   750.0V
 MIN 200mV
 MAX 750V
 DEF 20.0V
 for
 0   2mA 1 nA
 1   20mA    10nA
 2   200mA   100nA
 3   1A  1¦ÌA
 4   10A 10¦ÌA
 MIN 2mA 1 nA
 MAX 10A 10¦ÌA
 DEF 200mA   100nA
 Default Value: RGDM3K_VAL_AUTO_RANGE_ON
 Notes:
 (1) You must set this parameter to RGDM3K_VAL_AUTO_RANGE_ON when the RGDM3K_ATTR_FUNCTION attribute is set to Diode, Continuity, Frequency, or Period.
 (2) Setting this parameter to RGDM3K_VAL_AUTO_RANGE_ON configures the DMM to automatically calculate the range before each measurement.
 (3) Setting this parameter to RGDM3K_VAL_AUTO_RANGE_OFF configures the DMM to stop auto-ranging and keep the range fixed at the current maximum range.
 (4) Setting this parameter to RGDM3K_VAL_AUTO_RANGE_ONCE performs an immediate auto-range and then sets auto-ranging to OFF. The query therefore returns "0".

 resolution_absolute ViReal64 Specifies the measurement resolution in absolute units. The driver sets the RGDM3K_ATTR_RESOLUTION_ABSOLUTE attribute to this value.
 Setting this parameter to lower values increases the measurement accuracy. Setting this parameter to higher values increases the measurement speed.
 The value of the measurement resolution must be in units appropriate for the measurement function, as shown in the following table.
 DC Volts           - volts
 AC Volts           - volts
 DC Current         - amperes
 AC Current         - amperes
 2-Wire Resistance  - ohms
 4-Wire Resistance  - ohms
 Frequency          - hertz
 Period             - seconds
 Capacitance        - farads
 Temperature        - celsius
 Diode              - volts
 Continuity         - ohms
 Valid Range:
 RGDM3K_VAL_MAX     2
 RGDM3K_VAL_DEF     1
 RGDM3K_VAL_MIN     0
 Default Value: 1 Volts

 *****************************************************************************/
ViStatus _VI_FUNC RGDM3K_ConfigureMeasurement ( ViSession vi, ViInt32 measFunction,
        ViReal64 range, ViReal64 resolution )
{
    ViStatus    error = VI_SUCCESS;

    checkErr ( Ivi_LockSession ( vi, VI_NULL ) );

    /* Set attributes: */
    viCheckParm ( Ivi_SetAttributeViInt32 ( vi, VI_NULL, RGDM3K_ATTR_FUNCTION,
                                            0, measFunction ), 2, "Measurement Function" );
    viCheckParm ( Ivi_SetAttributeViReal64 ( vi, VI_NULL, RGDM3K_ATTR_RANGE,
                  0, range ), 3, "Range" );
    //if (range != RGDM3K_VAL_AUTO_RANGE_ON)
    viCheckParm ( Ivi_SetAttributeViReal64 ( vi, VI_NULL, RGDM3K_ATTR_RESOLUTION_ABSOLUTE,
                  0, resolution ), 4, "Resolution" );

    checkErr ( RGDM3K_CheckStatus ( vi ) );

Error:
    Ivi_UnlockSession ( vi, VI_NULL );
    return error;
}



/*****************************************************************************
 * Function: RGDM3K_ConfigureACBandwidth
 * Purpose:  Configures the AC measurement parameters of the DMM.  These
 *           attributes are RGDM3K_ATTR_AC_MAX_FREQ and RGDM3K_ATTR_MIN_FREQ
 * Note: This funciton is not be implemented.
 *****************************************************************************/
ViStatus _VI_FUNC RGDM3K_ConfigureACBandwidth ( ViSession vi, ViReal64 minFreq,
        ViReal64 maxFreq )
{
    ViStatus    error = VI_SUCCESS;

    checkErr ( Ivi_LockSession ( vi, VI_NULL ) );

    viCheckParm ( Ivi_SetAttributeViReal64 ( vi, VI_NULL, RGDM3K_ATTR_AC_MIN_FREQ,
                  0, minFreq ), 2, "AC Min Frequency" );
    viCheckParm ( Ivi_SetAttributeViReal64 ( vi, VI_NULL, RGDM3K_ATTR_AC_MAX_FREQ,
                  0, maxFreq ), 3, "AC Max Frequency" );
    checkErr ( RGDM3K_CheckStatus ( vi ) );

Error:
    Ivi_UnlockSession ( vi, VI_NULL );
    return error;
}




/*****************************************************************************
 * Function: RGDM3K_ConfigureTrigger
 * Purpose:  Configures the common DMM trigger attributes.  These attributes
 *           are RGDM3K_ATTR_TRIGGER_SOURCE and
 *               RGDM3K_ATTR_TRIGGER_DELAY.
 *****************************************************************************/
ViStatus _VI_FUNC RGDM3K_ConfigureTrigger ( ViSession vi, ViInt32 triggerSource,
        ViReal64 triggerDelay )
{
    ViStatus    error = VI_SUCCESS;

    checkErr ( Ivi_LockSession ( vi, VI_NULL ) );

    /* Set attributes: */
    viCheckParm ( Ivi_SetAttributeViInt32 ( vi, VI_NULL, RGDM3K_ATTR_TRIGGER_SOURCE,
                                            0, triggerSource ), 2, "Trigger Source" );
    viCheckParm ( Ivi_SetAttributeViReal64 ( vi, VI_NULL, RGDM3K_ATTR_TRIGGER_DELAY,
                  0, triggerDelay ), 3, "Trigger Delay" );

    checkErr ( RGDM3K_CheckStatus ( vi ) );

Error:
    Ivi_UnlockSession ( vi, VI_NULL );
    return error;
}

/*****************************************************************************
 * Function: RGDM3K_ConfigureTriggerSlope
 * Purpose:  Configures the common DMM trigger attributes.  These attributes
 *           are RGDM3K_ATTR_TRIGGER_SLOPE
 *
 *****************************************************************************/


ViStatus _VI_FUNC RGDM3K_ConfigureTriggerSlope ( ViSession vi,
        ViInt32 Polarity )
{
    ViStatus    error = VI_SUCCESS;

    checkErr ( Ivi_LockSession ( vi, VI_NULL ) );

    viCheckParm ( Ivi_SetAttributeViInt32 ( vi, VI_NULL, RGDM3K_ATTR_TRIGGER_SLOPE,
                                            0, Polarity ), 2, "Polarity" );

    checkErr ( RGDM3K_CheckStatus ( vi ) );

Error:
    Ivi_UnlockSession ( vi, VI_NULL );
    return error;
}


/*****************************************************************************
 * Function: RGDM3K_ConfigureDB
 * Purpose:  Configures the common DMM DB attributes.  These attributes
 *           are RGDM3K_ATTR_MATH_OPERATION and RGDM3K_ATTR_MATH_DB_REFERENCE
 *
 *****************************************************************************/
ViStatus _VI_FUNC RGDM3K_ConfigureDB ( ViSession vi,
                                       ViBoolean dBEnable,
                                       ViReal64 dBReferenceImpedance )
{
    ViStatus	error = VI_SUCCESS;
    checkErr ( Ivi_LockSession ( vi, VI_NULL ) );

    /*
        If the dBEnable variable equals to VI_TRUE, then set the math
        operation to dB.
        Else get the current math operation. If the operation is equal
        to dB, then disable math operation. Else do nothing.  Don't
        want to disable the math operation when it is not set to dB.
    */

    if ( dBEnable ) {
        viCheckParm ( Ivi_SetAttributeViInt32 ( vi, VI_NULL, RGDM3K_ATTR_MATH_OPERATION, 0,
                                                RGDM3K_VAL_MATH_DB ), 2, "dB Enable" );
        viCheckParm ( Ivi_SetAttributeViReal64 ( vi, VI_NULL,
                      RGDM3K_ATTR_MATH_DB_REFERENCE, 0,
                      dBReferenceImpedance ), 3, "dB Reference" );

    } else {
        ViInt32 mathOperation;

        checkErr ( Ivi_GetAttributeViInt32 ( vi, VI_NULL, RGDM3K_ATTR_MATH_OPERATION, 0,
                                             &mathOperation ) );

        if ( mathOperation == RGDM3K_VAL_MATH_DB )
            viCheckParm ( Ivi_SetAttributeViInt32 ( vi, VI_NULL, RGDM3K_ATTR_MATH_OPERATION, 0,
                                                    RGDM3K_VAL_MATH_OFF ), 2, "dB Enable" );
    }

    checkErr ( RGDM3K_CheckStatus ( vi ) );

Error:
    Ivi_UnlockSession ( vi, VI_NULL );
    return error;
}

/*****************************************************************************
 * Function: RGDM3K_ConfigureDBM
 * Purpose:  Configures the common DMM DBm attributes.  These attributes
 *           are RGDM3K_ATTR_MATH_DBM_REFERENCE
 *
 *****************************************************************************/
ViStatus _VI_FUNC RGDM3K_ConfigureDBM ( ViSession vi,
                                        ViBoolean dbmEnable,
                                        ViInt32 dBmReferenceImpedance )
{
    ViStatus    error = VI_SUCCESS;

    checkErr ( Ivi_LockSession ( vi, VI_NULL ) );

    /*
        If the dBmEnable variable equals to VI_TRUE, then set the math
        operation to dBm.
        Else get the current math operation. If the operation is equal
        to dBm, then disable math operation. Else do nothing.  Don't
        want to disable the math operation when it is not set to dBm.
    */

    if ( dbmEnable ) {
        viCheckParm ( Ivi_SetAttributeViInt32 ( vi, VI_NULL, RGDM3K_ATTR_MATH_OPERATION, 0,
                                                RGDM3K_VAL_MATH_DBM ), 2, "dBm Enable" );
        viCheckParm ( Ivi_SetAttributeViInt32 ( vi, VI_NULL,
                                                RGDM3K_ATTR_MATH_DBM_REFERENCE, 0,
                                                dBmReferenceImpedance ), 3, "dBm Reference" );

    } else {
        ViInt32 mathOperation;

        checkErr ( Ivi_GetAttributeViInt32 ( vi, VI_NULL, RGDM3K_ATTR_MATH_OPERATION, 0,
                                             &mathOperation ) );

        if ( mathOperation == RGDM3K_VAL_MATH_DBM ) {
            viCheckParm ( Ivi_SetAttributeViInt32 ( vi, VI_NULL, RGDM3K_ATTR_MATH_OPERATION, 0,
                                                    RGDM3K_VAL_MATH_OFF ), 2, "dBm Enable" );
        }
    }

    checkErr ( RGDM3K_CheckStatus ( vi ) );

Error:
    Ivi_UnlockSession ( vi, VI_NULL );
    return error;
}


/*****************************************************************************
 * Function: RGDM3K_ConfigureLimit
 * Purpose:  Configures the RGDM3K_ATTR_MATH_OPERATION attributes.  These attributes
 *           are RGDM3K_ATTR_MATH_LIMIT_LOWER and RGDM3K_ATTR_MATH_LIMIT_UPPER
 *
 *****************************************************************************/
ViStatus _VI_FUNC RGDM3K_ConfigureLimit ( ViSession vi,
        ViBoolean limitEnable,
        ViReal64 lowerLimit,
        ViReal64 upperLimit )
{
    ViStatus    error = VI_SUCCESS;

    checkErr ( Ivi_LockSession ( vi, VI_NULL ) );

    /*
        If the limitEnable variable equals to VI_TRUE, then set the math
        operation to limit.
        Else get the current math operation. If the operation is equal
        to limit, then disable math operation. Else do nothing.  Don't
        want to disable the math operation when it is not set to limit.
    */

    if ( limitEnable ) {
        viCheckParm ( Ivi_SetAttributeViInt32 ( vi, VI_NULL, RGDM3K_ATTR_MATH_OPERATION, 0,
                                                RGDM3K_VAL_MATH_LIMIT ), 2, "Limit Enable" );
        viCheckParm ( Ivi_SetAttributeViReal64 ( vi, VI_NULL, RGDM3K_ATTR_MATH_LIMIT_LOWER,
                      0, lowerLimit ), 3, "Limit Lower" );
        viCheckParm ( Ivi_SetAttributeViReal64 ( vi, VI_NULL, RGDM3K_ATTR_MATH_LIMIT_UPPER,
                      0, upperLimit ), 4, "Limit Upper" );

    } else {
        ViInt32 mathOperation;

        checkErr ( Ivi_GetAttributeViInt32 ( vi, VI_NULL, RGDM3K_ATTR_MATH_OPERATION, 0,
                                             &mathOperation ) );

        if ( mathOperation == RGDM3K_VAL_MATH_LIMIT )
            viCheckParm ( Ivi_SetAttributeViInt32 ( vi, VI_NULL, RGDM3K_ATTR_MATH_OPERATION, 0,
                                                    RGDM3K_VAL_MATH_OFF ), 2, "Limit Enable" );
    }

    checkErr ( RGDM3K_CheckStatus ( vi ) );

Error:
    Ivi_UnlockSession ( vi, VI_NULL );
    return error;
}

/*****************************************************************************
 * Function: RGDM3K_ConfigureLimit
 * Purpose:  Configures the RGDM3K_ATTR_MATH_OPERATION attributes.  These attributes
 *           are RGDM3K_ATTR_MATH_NULL_OFFSET
 *
 *****************************************************************************/
ViStatus _VI_FUNC RGDM3K_ConfigureNull ( ViSession vi,
        ViBoolean nullEnable,
        ViReal64 nullOffset )
{
    ViStatus    error = VI_SUCCESS;

    checkErr ( Ivi_LockSession ( vi, VI_NULL ) );

    /*
        If the nullEnable variable equals to VI_TRUE, then set the math
        operation to null.
        Else get the current math operation. If the operation is equal
        to null, then disable math operation. Else do nothing.  Don't
        want to disable the math operation when it is not set to null.
    */

    if ( nullEnable ) {
        viCheckParm ( Ivi_SetAttributeViInt32 ( vi, VI_NULL, RGDM3K_ATTR_MATH_OPERATION, 0,
                                                RGDM3K_VAL_MATH_NULL ), 2, "Null Enable" );
        viCheckParm ( Ivi_SetAttributeViReal64 ( vi, VI_NULL, RGDM3K_ATTR_MATH_NULL_OFFSET,
                      0, nullOffset ), 3, "Null Offset" );

    } else {
        ViInt32 mathOperation;

        checkErr ( Ivi_GetAttributeViInt32 ( vi, VI_NULL, RGDM3K_ATTR_MATH_OPERATION, 0,
                                             &mathOperation ) );

        if ( mathOperation == RGDM3K_VAL_MATH_NULL )
            viCheckParm ( Ivi_SetAttributeViInt32 ( vi, VI_NULL, RGDM3K_ATTR_MATH_OPERATION, 0,
                                                    RGDM3K_VAL_MATH_OFF ), 2, "Null Enable" );
    }

    checkErr ( RGDM3K_CheckStatus ( vi ) );

Error:
    Ivi_UnlockSession ( vi, VI_NULL );
    return error;
}


/*****************************************************************************
 * Function: RGDM3K_ConfigureLimit
 * Purpose:  Configures the RGDM3K_ConfigureMinMax attributes.
 *
 *****************************************************************************/
ViStatus _VI_FUNC RGDM3K_ConfigureMinMax ( ViSession vi,
        ViBoolean minMaxEnable )
{
    ViStatus    error = VI_SUCCESS;

    checkErr ( Ivi_LockSession ( vi, VI_NULL ) );

    /*
        If the minMaxEnable variable equals to VI_TRUE, then set the math
        operation to minMax.
        Else get the current math operation. If the operation is equal
        to minMax, then disable math operation. Else do nothing.  Don't
        want to disable the math operation when it is not set to minMax.
    */

    if ( minMaxEnable ) {
        viCheckParm ( Ivi_SetAttributeViInt32 ( vi, VI_NULL, RGDM3K_ATTR_MATH_OPERATION, 0,
                                                RGDM3K_VAL_MATH_MIN_MAX ), 2, "Min-Max Enable" );

    } else {
        ViInt32 mathOperation;

        checkErr ( Ivi_GetAttributeViInt32 ( vi, VI_NULL, RGDM3K_ATTR_MATH_OPERATION, 0,
                                             &mathOperation ) );

        if ( mathOperation == RGDM3K_VAL_MATH_MIN_MAX )
            viCheckParm ( Ivi_SetAttributeViInt32 ( vi, VI_NULL, RGDM3K_ATTR_MATH_OPERATION, 0,
                                                    RGDM3K_VAL_MATH_OFF ), 2, "Min-Max Enable" );
    }

    checkErr ( RGDM3K_CheckStatus ( vi ) );

Error:
    Ivi_UnlockSession ( vi, VI_NULL );
    return error;
}



/*****************************************************************************
 * Function: RGDM3K_Read
 * Purpose:  Initiates a measurement and fetches the result.
 *****************************************************************************/
ViStatus _VI_FUNC RGDM3K_Read ( ViSession vi, ViInt32 maxTime, ViReal64 *reading )
{
    ViStatus    error = VI_SUCCESS;
    ViStatus    overRange = VI_SUCCESS;

    checkErr ( Ivi_LockSession ( vi, VI_NULL ) );

    IviDmm_InterchangeCheck ( vi, "RGDM3K_Read" );

    if ( reading == VI_NULL )
    { viCheckParm ( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Reading" ); }

    checkErr ( RGDM3K_Initiate ( vi ) );

    checkWarn ( overRange = RGDM3K_Fetch ( vi, maxTime, reading ) );

    checkErr ( RGDM3K_CheckStatus ( vi ) );

Error:
    Ivi_UnlockSession ( vi, VI_NULL );

    if ( error < VI_SUCCESS )
    { return error; }

    else
    { return overRange; }
}



/*****************************************************************************
 * Function: RGDM3K_Initiate
 * Purpose:  Initates a measurement.
 * Notes:    INIT command is not be implemented.
 *****************************************************************************/
ViStatus _VI_FUNC RGDM3K_Initiate ( ViSession vi )
{
    ViStatus    error = VI_SUCCESS;
    ViBoolean   invalidate;

    checkErr ( Ivi_LockSession ( vi, VI_NULL ) );

    IviDmm_InterchangeCheck ( vi, "RGDM3K_Initiate" );
    checkErr ( Ivi_GetAttributeViBoolean ( vi, "", RGDM3K_ATTR_INVALIDATE_NULL_OFFSET, 0, &invalidate ) );

    if ( invalidate ) {
        checkErr ( Ivi_InvalidateAttribute ( vi, "", RGDM3K_ATTR_MATH_NULL_OFFSET ) );
        checkErr ( Ivi_SetAttributeViBoolean ( vi, "", RGDM3K_ATTR_INVALIDATE_NULL_OFFSET, 0, VI_FALSE ) );
    }

    checkErr ( Ivi_GetAttributeViBoolean ( vi, "", RGDM3K_ATTR_INVALIDATE_DB_REFERENCE, 0, &invalidate ) );

    if ( invalidate ) {
        checkErr ( Ivi_InvalidateAttribute ( vi, "", RGDM3K_ATTR_MATH_DB_REFERENCE ) );
        checkErr ( Ivi_SetAttributeViBoolean ( vi, "", RGDM3K_ATTR_INVALIDATE_DB_REFERENCE, 0, VI_FALSE ) );
    }

    if ( !Ivi_Simulating ( vi ) ) {         /* call only when locked */

        checkErr ( Ivi_SetNeedToCheckStatus ( vi, VI_TRUE ) );

        //viCheckErr( viWrite (io, "INIT;", 5, VI_NULL ));
    }

    /*
        Do not invoke the RGDM3K_CheckStatus function here.  It
        is invoked by the high-level driver functions when this function is
        used internally.  After the user calls this function, the end-
        user can check for errors by calling the RGDM3K_error_query
        function.
    */

Error:
    Ivi_UnlockSession ( vi, VI_NULL );
    return error;
}


/*****************************************************************************
 * Function: RGDM3K_Abort
 * Purpose:  Aborts a measurement.
 *****************************************************************************/
ViStatus _VI_FUNC RGDM3K_Abort ( ViSession vi )
{
    ViStatus    error = VI_SUCCESS;

    checkErr ( Ivi_LockSession ( vi, VI_NULL ) );

    if ( !Ivi_Simulating ( vi ) ) {         /* call only when locked */
        ViSession   io = Ivi_IOSession ( vi ); /* call only when locked */

        checkErr ( Ivi_SetNeedToCheckStatus ( vi, VI_TRUE ) );

        viCheckErr ( viClear ( io ) );
    }

    /*
        Do not invoke the RGDM3K_CheckStatus function here.
        After the user calls this function, the user can check
        for errors by calling the RGDM3K_error_query function.
    */

Error:
    Ivi_UnlockSession ( vi, VI_NULL );
    return error;
}


/*****************************************************************************
 * Function: RGDM3K_SendSoftwareTrigger
 * Purpose:  Sends a command to trigger the instrument.
 * Note: This function is not be implemented.
 *****************************************************************************/
ViStatus _VI_FUNC RGDM3K_SendSoftwareTrigger ( ViSession vi )
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     trigSource;

    checkErr ( Ivi_LockSession ( vi, VI_NULL ) );

    checkErr ( Ivi_GetAttributeViInt32 ( vi, "", RGDM3K_ATTR_TRIGGER_SOURCE,
                                         0, &trigSource ) );

    if ( trigSource != RGDM3K_VAL_SOFTWARE_TRIG )
    { viCheckErr ( RGDM3K_ERROR_TRIGGER_NOT_SOFTWARE ); }

    if ( !Ivi_Simulating ( vi ) ) {         /* call only when locked */

        checkErr ( Ivi_SetNeedToCheckStatus ( vi, VI_TRUE ) );

        //viCheckErr( viWrite (io, "*TRG;", 5, VI_NULL));
    }

    /*
        Do not invoke the RGDM3K_CheckStatus function here.
        After the user calls this function, the user can check
        for errors by calling the RGDM3K_error_query function.
    */

Error:
    Ivi_UnlockSession ( vi, VI_NULL );
    return error;
}


/*****************************************************************************
 * Function: RGDM3K_IsOverRange
 * Purpose:  This function takes a measurement value that you obtain from one
 *           of the Measure, Read, or Fetch functions and determines if the
 *           value is a valid measurement value or a value indicating an
 *           over-range condition occurred.
 *****************************************************************************/
ViStatus _VI_FUNC RGDM3K_IsOverRange ( ViSession vi, ViReal64 measurementValue,
                                       ViBoolean *isOverRange )
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     type;

    checkErr ( Ivi_LockSession ( vi, VI_NULL ) );

    if ( isOverRange == VI_NULL )
    { viCheckParm ( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Is Over-Range." ); }

    viCheckErr ( Ivi_GetViReal64Type ( measurementValue, &type ) );

    if ( type == IVI_VAL_TYPE_NAN )
    { *isOverRange = VI_TRUE; }

    else
    { *isOverRange = VI_FALSE; }

Error:
    Ivi_UnlockSession ( vi, VI_NULL );
    return error;
}


/*****************************************************************************
 * Function: RGDM3K_Fetch
 * Purpose:  This function returns the measured value from a previously
 *           initiated measurement.  This function does not trigger the
 *           instrument.
 *
 * Notes:    After this function executes, the value in *readingRef
 *           is an actual reading or a value indicating that an over-range
 *           condition occurred.  If an over-range condition occurs, the
 *           function sets *readingRef to IVI_VAL_NAN and returns
 *           RGDM3K_WARN_OVER_RANGE.
 *****************************************************************************/
ViStatus _VI_FUNC RGDM3K_Fetch ( ViSession vi, ViInt32 maxTime, ViReal64 *readingRef )
{
    ViStatus    error = VI_SUCCESS;
    ViBoolean   overRange = VI_FALSE;
    ViSession   io = VI_NULL;
    ViUInt32    oldTimeout = 0;

    ViBoolean   needToRestoreTimeout = VI_FALSE;

    checkErr ( Ivi_LockSession ( vi, VI_NULL ) );

    if ( readingRef == VI_NULL )
    { viCheckParm ( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Reading" ); }

    if ( !Ivi_Simulating ( vi ) ) { /* call only when locked */
        ViInt32   size;
        size = 10;
        io = Ivi_IOSession ( vi ); /* call only when locked */

        checkErr ( Ivi_SetNeedToCheckStatus ( vi, VI_TRUE ) );

        /* Store the old timeout so that it can be restored later */
        viCheckErr ( viGetAttribute ( io, VI_ATTR_TMO_VALUE, &oldTimeout ) );
        viCheckErr ( viSetAttribute ( io, VI_ATTR_TMO_VALUE, maxTime ) );
        needToRestoreTimeout = VI_TRUE;
        viCheckErr ( viPrintf ( io, ":function2:value1?\n" ) );

        //RGDM3K_delaytime();
        viCheckErr ( viScanf ( io, "%lf", readingRef ) );

        if ( error == VI_ERROR_TMO )
        { error = RGDM3K_ERROR_MAX_TIME_EXCEEDED; }

        viCheckErr ( error );

        /* Check for overrange */
        if ( ( -9.9E37 >= *readingRef ) || ( *readingRef >= 9.9E37 ) ) {
            *readingRef = IVI_VAL_NAN;
            overRange = VI_TRUE;
        }

    } else {
        /* Simulate measurement data */
        ViReal64    range;
        ViInt32     measFunc;

        checkErr ( Ivi_GetAttributeViReal64 ( vi, VI_NULL, RGDM3K_ATTR_RANGE,
                                              0, &range ) );

        if ( range <= 0.0 ) {   /* If auto-ranging or auto-range off, use the max value. */
            checkErr ( Ivi_GetAttributeViInt32 ( vi, VI_NULL, RGDM3K_ATTR_FUNCTION, 0,
                                                 &measFunc ) );

            switch ( measFunc ) {
                case RGDM3K_VAL_DC_VOLTS:


                    range = 1000.0;				/* 1000.0 volts */
                    break;

                case RGDM3K_VAL_AC_VOLTS:
                    range = 750.0;
                    break;

                case RGDM3K_VAL_DC_CURRENT:
                case RGDM3K_VAL_AC_CURRENT:
                    range = 10.0;				/*10.0amps */
                    break;

                case RGDM3K_VAL_2_WIRE_RES:
                case RGDM3K_VAL_4_WIRE_RES:
                    range = 1.0E8;				/* 1.0E8 ohms */
                    break;

                case RGDM3K_VAL_CAPACITANCE:
                    range = 2.0E-4;				/* 1.0E-5 farads */
                    break;

                case RGDM3K_VAL_FREQ:
                    range = 300.0E3;            /* 300 kHz */
                    break;

                case RGDM3K_VAL_PERIOD:
                    range = 333.333E-3;         /* 333.333 ms */
                    break;

                case RGDM3K_VAL_CONTINUITY:
                    range = 2000.0;          	/* 2000.0 ohms */
                    break;

                case RGDM3K_VAL_DIODE:
                    range = 1.0;               	/* 1.0 volts */
                    break;
            }
        }

        *readingRef = range * ( ( ViReal64 ) rand() / ( ViReal64 ) RAND_MAX );
    }

    /*
        Do not invoke rgdm3k_CheckStatus here.
        rgdm3k_Read invokes rgdm3k_CheckStatus after it calls
        this function.  After the user calls this function, the
        user can check for errors by calling rgdm3k_error_query.
    */

Error:

    if ( needToRestoreTimeout ) {
        /* Restore the original timeout */
        viSetAttribute ( io, VI_ATTR_TMO_VALUE, oldTimeout );
    }

    Ivi_UnlockSession ( vi, VI_NULL );

    if ( overRange && ( error >= VI_SUCCESS ) )
    { return RGDM3K_WARN_OVER_RANGE; }

    else
    { return error; }
}



/*****************************************************************************
 * Function: RGDM3K_FetchMinMax
 * Purpose:  This function returns data related to the min-max operation.
 *           The data includes the minimum reading, maximum reading, average
 *           of all readings, and count of total readings since min-max
 *           operation been enabled.  This function does not trigger the
 *           instrument.
 *
 *           After this function executes, the value in *maximum, *minimum,
 *           and *average are actual readings or a value indicating that an
 *           over-range condition occurred.  If an over-range condition
 *           occurs, the function sets *maximum, *minimum, or *average to
 *           IVI_VAL_NAN
 *           and returns RGDM3K_WARN_OVER_RANGE.
 *****************************************************************************/
ViStatus _VI_FUNC RGDM3K_FetchMinMax ( ViSession vi, ViInt32 maxTime,
                                       ViReal64 *minimum, ViReal64 *maximum,
                                       ViReal64 *average, ViInt32 *count )
{
    ViStatus    error = VI_SUCCESS;
    ViBoolean   overRange = VI_FALSE;
    ViSession   io = VI_NULL;
    ViUInt32    oldTimeout = 0;
    ViBoolean   needToRestoreTimeout = VI_FALSE;

    checkErr ( Ivi_LockSession ( vi, VI_NULL ) );

    if ( minimum == VI_NULL )
    { viCheckParm ( IVI_ERROR_INVALID_PARAMETER, 2, "Null address for Minimum Reading" ); }

    if ( maximum == VI_NULL )
    { viCheckParm ( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Maximum Reading" ); }

    if ( average == VI_NULL )
    { viCheckParm ( IVI_ERROR_INVALID_PARAMETER, 4, "Null address for Average Reading" ); }

    if ( count == VI_NULL )
    { viCheckParm ( IVI_ERROR_INVALID_PARAMETER, 5, "Null address for Count" ); }

    if ( !Ivi_Simulating ( vi ) ) {         /* call only when locked */

        io = Ivi_IOSession ( vi );

        checkErr ( Ivi_SetNeedToCheckStatus ( vi, VI_TRUE ) );

        /* Store the old timeout so that it can be restored later */
        viCheckErr ( viGetAttribute ( io, VI_ATTR_TMO_VALUE, &oldTimeout ) );
        viCheckErr ( viSetAttribute ( io, VI_ATTR_TMO_VALUE, maxTime ) );
        needToRestoreTimeout = VI_TRUE;
        viCheckErr ( viPrintf ( io, ":calculate:statistic:count?" ) );
        RGDM3K_delaytime();
        //error = viQueryf (io, ":calculate:statistic:count?", "%lf", count);
        viCheckErr ( viScanf ( io, "%d", count ) );

        RGDM3K_delaytime();
        error = viQueryf ( io, ":calculate:statistic:min?", "%lf", minimum );
        RGDM3K_delaytime();
        error = viQueryf ( io, ":calculate:statistic:max?", "%lf", maximum );
        RGDM3K_delaytime();
        error = viQueryf ( io, ":calculate:statistic:average?", "%lf", average );

        if ( error == VI_ERROR_TMO )
        { error = RGDM3K_ERROR_MAX_TIME_EXCEEDED; }

        viCheckErr ( error );


    } else {
        /* Simulate measurement data */
        ViReal64 range;
        ViInt32  measFunc;

        checkErr ( Ivi_GetAttributeViReal64 ( vi, VI_NULL, RGDM3K_ATTR_RANGE, 0, &range ) );

        if ( range <= 0.0 ) {   /* If auto-ranging or auto-range off, use the max value. */
            checkErr ( Ivi_GetAttributeViInt32 ( vi, VI_NULL, RGDM3K_ATTR_FUNCTION, 0,
                                                 &measFunc ) );

            switch ( measFunc ) {
                case RGDM3K_VAL_DC_VOLTS:


                    range = 1000.0;				/* 1000.0 volts */
                    break;

                case RGDM3K_VAL_AC_VOLTS:
                    range = 750.0;
                    break;

                case RGDM3K_VAL_DC_CURRENT:
                case RGDM3K_VAL_AC_CURRENT:
                    range = 10.0;				/*10.0amps */
                    break;

                case RGDM3K_VAL_2_WIRE_RES:
                case RGDM3K_VAL_4_WIRE_RES:
                    range = 1.0E8;				/* 1.0E8 ohms */
                    break;

                case RGDM3K_VAL_CAPACITANCE:
                    range = 2.0E-4;				/* 1.0E-5 farads */
                    break;

                case RGDM3K_VAL_FREQ:
                    range = 300.0E3;            /* 300 kHz */
                    break;

                case RGDM3K_VAL_PERIOD:
                    range = 333.333E-3;         /* 333.333 ms */
                    break;

                case RGDM3K_VAL_CONTINUITY:
                    range = 2000.0;          	/* 1000.0 ohms */
                    break;

                case RGDM3K_VAL_DIODE:
                    range = 1.0;               	/* 1.0 volts */
                    break;
            }
        }

        *minimum = -range * ( rand() / RAND_MAX );
        *maximum = range * ( rand() / RAND_MAX );
        *average = ( *minimum + *maximum ) / 2.0;
        *count = ( ViInt32 ) ( rand() * 100.0 );
    }

    /*
        Do not invoke RGDM3K_CheckStatus here.
        RGDM3K_ReadMultiPoint invokes RGDM3K_CheckStatus after it
        calls this function.  After the user calls this function, the
        user can check for errors by calling RGDM3K_error_query.
    */

Error:

    if ( needToRestoreTimeout ) {
        /* Restore the original timeout */
        viSetAttribute ( io, VI_ATTR_TMO_VALUE, oldTimeout );
    }

    Ivi_UnlockSession ( vi, VI_NULL );

    if ( overRange && ( error >= VI_SUCCESS ) )
    { return RGDM3K_WARN_OVER_RANGE; }

    else
    { return error; }
}


/*****************************************************************************
 * Function: RGDM3K_DisplayClear
 * Purpose:  Clears user-set text from the RG's screen
 *           and returns to the normal display of measurements.
 * Note: The command ":DISP:TEXT:CLE" is not be implemented.
 *****************************************************************************/
ViStatus _VI_FUNC RGDM3K_DisplayClear ( ViSession vi )
{
    ViStatus    error = VI_SUCCESS;

    checkErr ( Ivi_LockSession ( vi, VI_NULL ) );

    if ( !Ivi_Simulating ( vi ) ) {         /* call only when locked */

        checkErr ( Ivi_SetNeedToCheckStatus ( vi, VI_TRUE ) );

        //viCheckErr( viPrintf (io, ":DISP:TEXT:CLE;"));
    }

    checkErr ( RGDM3K_CheckStatus ( vi ) );

Error:
    Ivi_UnlockSession ( vi, VI_NULL );
    return error;
}


/*****************************************************************************
 * Function: RGDM3K_DisplayMessage
 * Purpose:  Display a message on the front panel. The DMM will display up
 *           to 12 characters in a message. Any additional characters are
 *           truncated.
 * Note: This function is not be implemented.
 *****************************************************************************/
ViStatus _VI_FUNC RGDM3K_DisplayMessage ( ViSession vi, ViConstString message )
{
    ViStatus    error = VI_SUCCESS;

    checkErr ( Ivi_LockSession ( vi, VI_NULL ) );

    if ( !Ivi_Simulating ( vi ) ) {         /* call only when locked */

        checkErr ( Ivi_SetNeedToCheckStatus ( vi, VI_TRUE ) );

        /*
            "DISP:TEXT" requires a quote-enclosed string, so we
            provided the quotes.
        */

        //viCheckErr( viPrintf (io, ":DISP:TEXT \"%s\";", message));
    }

    checkErr ( RGDM3K_CheckStatus ( vi ) );

Error:
    Ivi_UnlockSession ( vi, VI_NULL );
    return error;
}



/*****************************************************************************
 * Function: RGDM3K_GetAttribute<type> Functions
 * Purpose:  These functions enable the instrument driver user to get
 *           attribute values directly.  There are typesafe versions for
 *           ViInt32, ViReal64, ViString, ViBoolean, and ViSession attributes.
 *****************************************************************************/
ViStatus _VI_FUNC RGDM3K_GetAttributeViInt32 ( ViSession vi, ViConstString channelName,
        ViAttr attributeId, ViInt32 *value )
{
    return Ivi_GetAttributeViInt32 ( vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL,
                                     value );
}
ViStatus _VI_FUNC RGDM3K_GetAttributeViReal64 ( ViSession vi, ViConstString channelName,
        ViAttr attributeId, ViReal64 *value )
{
    return Ivi_GetAttributeViReal64 ( vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL,
                                      value );
}
ViStatus _VI_FUNC RGDM3K_GetAttributeViString ( ViSession vi, ViConstString channelName,
        ViAttr attributeId, ViInt32 bufSize,
        ViChar value[] )
{
    return Ivi_GetAttributeViString ( vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL,
                                      bufSize, value );
}
ViStatus _VI_FUNC RGDM3K_GetAttributeViBoolean ( ViSession vi, ViConstString channelName,
        ViAttr attributeId, ViBoolean *value )
{
    return Ivi_GetAttributeViBoolean ( vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL,
                                       value );
}
ViStatus _VI_FUNC RGDM3K_GetAttributeViSession ( ViSession vi, ViConstString channelName,
        ViAttr attributeId, ViSession *value )
{
    return Ivi_GetAttributeViSession ( vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL,
                                       value );
}


/*****************************************************************************
 * Function: RGDM3K_SetAttribute<type> Functions
 * Purpose:  These functions enable the instrument driver user to set
 *           attribute values directly.  There are typesafe versions for
 *           ViInt32, ViReal64, ViString, ViBoolean, and ViSession datatypes.
 *****************************************************************************/
ViStatus _VI_FUNC RGDM3K_SetAttributeViInt32 ( ViSession vi, ViConstString channelName,
        ViAttr attributeId, ViInt32 value )
{
    return Ivi_SetAttributeViInt32 ( vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL,
                                     value );
}
ViStatus _VI_FUNC RGDM3K_SetAttributeViReal64 ( ViSession vi, ViConstString channelName,
        ViAttr attributeId, ViReal64 value )
{
    return Ivi_SetAttributeViReal64 ( vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL,
                                      value );
}
ViStatus _VI_FUNC RGDM3K_SetAttributeViString ( ViSession vi, ViConstString channelName,
        ViAttr attributeId, ViConstString value )
{
    return Ivi_SetAttributeViString ( vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL,
                                      value );
}
ViStatus _VI_FUNC RGDM3K_SetAttributeViBoolean ( ViSession vi, ViConstString channelName,
        ViAttr attributeId, ViBoolean value )
{
    return Ivi_SetAttributeViBoolean ( vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL,
                                       value );
}
ViStatus _VI_FUNC RGDM3K_SetAttributeViSession ( ViSession vi, ViConstString channelName,
        ViAttr attributeId, ViSession value )
{
    return Ivi_SetAttributeViSession ( vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL,
                                       value );
}


/*****************************************************************************
 * Function: RGDM3K_CheckAttribute<type> Functions
 * Purpose:  These functions enable the instrument driver user to check
 *           attribute values directly.  These functions check the value you
 *           specify even if you set the RGDM3K_ATTR_RANGE_CHECK
 *           attribute to VI_FALSE.  There are typesafe versions for ViInt32,
 *           ViReal64, ViString, ViBoolean, and ViSession datatypes.
 *****************************************************************************/
ViStatus _VI_FUNC RGDM3K_CheckAttributeViInt32 ( ViSession vi, ViConstString channelName,
        ViAttr attributeId, ViInt32 value )
{
    return Ivi_CheckAttributeViInt32 ( vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL,
                                       value );
}
ViStatus _VI_FUNC RGDM3K_CheckAttributeViReal64 ( ViSession vi, ViConstString channelName,
        ViAttr attributeId, ViReal64 value )
{
    return Ivi_CheckAttributeViReal64 ( vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL,
                                        value );
}
ViStatus _VI_FUNC RGDM3K_CheckAttributeViString ( ViSession vi, ViConstString channelName,
        ViAttr attributeId, ViConstString value )
{
    return Ivi_CheckAttributeViString ( vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL,
                                        value );
}
ViStatus _VI_FUNC RGDM3K_CheckAttributeViBoolean ( ViSession vi, ViConstString channelName,
        ViAttr attributeId, ViBoolean value )
{
    return Ivi_CheckAttributeViBoolean ( vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL,
                                         value );
}
ViStatus _VI_FUNC RGDM3K_CheckAttributeViSession ( ViSession vi, ViConstString channelName,
        ViAttr attributeId, ViSession value )
{
    return Ivi_CheckAttributeViSession ( vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL,
                                         value );
}


/****************************************************************************
 *  Function: RGDM3K_GetNextCoercionRecord
 *  Purpose:  Get the next coercion record from the queue.
 ****************************************************************************/
ViStatus _VI_FUNC RGDM3K_GetNextCoercionRecord ( ViSession vi, ViInt32 bufferSize, ViChar record[] )
{
    return Ivi_GetNextCoercionString ( vi, bufferSize, record );
}


/*****************************************************************************
 * Function: RGDM3K_LockSession and RGDM3K_UnlockSession Functions
 * Purpose:  These functions enable the instrument driver user to lock the
 *           session around a sequence of driver calls during which other
 *           execution threads must not disturb the instrument state.
 *
 *           NOTE:  The callerHasLock parameter must be a local variable
 *           initialized to VI_FALSE and passed by reference, or you can pass
 *           VI_NULL.
 *****************************************************************************/
ViStatus _VI_FUNC RGDM3K_LockSession ( ViSession vi, ViBoolean *callerHasLock )
{
    return Ivi_LockSession ( vi, callerHasLock );
}

ViStatus _VI_FUNC RGDM3K_UnlockSession ( ViSession vi, ViBoolean *callerHasLock )
{
    return Ivi_UnlockSession ( vi, callerHasLock );
}


/*****************************************************************************
 * Function: RGDM3K_GetErrorInfo and RGDM3K_ClearErrorInfo Functions
 * Purpose:  These functions enable the instrument driver user to
 *           get or clear the error information the driver associates with the
 *           IVI session.
 *****************************************************************************/
ViStatus _VI_FUNC RGDM3K_GetErrorInfo ( ViSession vi, ViStatus *primaryError,
                                        ViStatus *secondaryError, ViChar errorElaboration[256] )
{
    return Ivi_GetErrorInfo ( vi, primaryError, secondaryError, errorElaboration );
}

ViStatus _VI_FUNC RGDM3K_ClearErrorInfo ( ViSession vi )
{
    return Ivi_ClearErrorInfo ( vi );
}


/*****************************************************************************
 * Function: WriteInstrData and ReadInstrData Functions
 * Purpose:  These functions enable the instrument driver user to
 *           write and read commands directly to and from the instrument.
 *
 *           Note:  These functions bypass the IVI attribute state caching.
 *                  WriteInstrData invalidates the cached values for all
 *                  attributes.
 *****************************************************************************/
ViStatus _VI_FUNC RGDM3K_WriteInstrData ( ViSession vi, ViConstString writeBuffer )
{
    return Ivi_WriteInstrData ( vi, writeBuffer );
}

ViStatus _VI_FUNC RGDM3K_ReadInstrData ( ViSession vi, ViInt32 numBytes,
        ViChar rdBuf[], ViInt32 *bytesRead )
{
    return Ivi_ReadInstrData ( vi, numBytes, rdBuf, bytesRead );
}








/*****************************************************************************
 * Function: RGDM3K_GetError and RGDM3K_ClearError Functions
 * Purpose:  These functions enable the instrument driver user to
 *           get or clear the error information the driver associates with the
 *           IVI session.
 * Note:the function is not surported.
 *****************************************************************************/
ViStatus _VI_FUNC RGDM3K_GetError ( ViSession vi,
                                    ViStatus *errorCode,
                                    ViInt32 bufferSize,
                                    ViChar description[] )
{
    ViStatus error = VI_SUCCESS;
    ViStatus primary = VI_SUCCESS,
             secondary = VI_SUCCESS;
    ViChar   elaboration[256] = "";
    ViChar   errorMessage[1024] = "";
    ViChar  *appendPoint = errorMessage;
    ViInt32  actualSize = 0;
    ViBoolean locked = VI_FALSE;

    /* Lock Session */
    if ( vi != 0 ) {
        checkErr ( Ivi_LockSession ( vi, &locked ) );
    }

    /* Test for nulls and acquire error data */
    if ( bufferSize != 0 ) {
        if ( errorCode == VI_NULL ) {
            viCheckParm ( IVI_ERROR_INVALID_PARAMETER, 2, "Null address for Error" );
        }

        if ( description == VI_NULL ) {
            viCheckParm ( IVI_ERROR_INVALID_PARAMETER, 4, "Null address for Description" );
        }

        checkErr ( Ivi_GetErrorInfo ( vi, &primary, &secondary, elaboration ) );
    }

    else {
        checkErr ( Ivi_GetAttributeViString ( vi, VI_NULL, IVI_ATTR_ERROR_ELABORATION, 0, 256, elaboration ) );
        checkErr ( Ivi_GetAttributeViInt32 ( vi, VI_NULL, IVI_ATTR_SECONDARY_ERROR, 0, &secondary ) );
        checkErr ( Ivi_GetAttributeViInt32 ( vi, VI_NULL, IVI_ATTR_PRIMARY_ERROR, 0, &primary ) );
    }

    /* Format data */
    if ( primary != VI_SUCCESS ) {
        ViChar msg[256] = "";
        checkErr ( RGDM3K_error_message ( vi, primary, msg ) );
        appendPoint += sprintf ( appendPoint, "Primary Error: (Hex 0x%08X) %s\n", primary, msg );
    }

    if ( secondary != VI_SUCCESS ) {
        ViChar msg[256] = "";
        checkErr ( RGDM3K_error_message ( vi, secondary, msg ) );
        appendPoint += sprintf ( appendPoint, "Secondary Error: (Hex 0x%08X) %s\n", secondary, msg );
    }

    if ( elaboration[0] ) {
        sprintf ( appendPoint, "Elaboration: %s", elaboration );
    }

    actualSize = ( ViInt32 ) ( strlen ( errorMessage ) ) + 1;

    /* Prepare return values */
    if ( bufferSize == 0 ) {
        error = actualSize;

    } else {
        if ( bufferSize > 0 ) {
            if ( actualSize > bufferSize ) {
                error = actualSize;
                actualSize = bufferSize;
            }
        }

        memcpy ( description, errorMessage, actualSize - 1 );
        description[actualSize - 1] = '\0';
    }

    if ( errorCode ) {
        *errorCode = primary;
    }

Error:
    /* Unlock Session */
    Ivi_UnlockSession ( vi, &locked );
    return error;
}

ViStatus _VI_FUNC RGDM3K_ClearError ( ViSession vi )
{
    return Ivi_ClearErrorInfo ( vi );
}


/****************************************************************************
 *  Function: RGDM3K_GetNextInterchangeWarning,
 *            RGDM3K_ResetInterchangeCheck, and
 *            RGDM3K_ClearInterchangeWarnings
 *  Purpose:  These functions allow the user to retrieve interchangeability
 *            warnings, reset the driver's interchangeability checking
 *            state, and clear all previously logged interchangeability warnings.
 ****************************************************************************/
ViStatus _VI_FUNC RGDM3K_GetNextInterchangeWarning ( ViSession vi,
        ViInt32 bufferSize,
        ViChar warnString[] )
{
    return Ivi_GetNextInterchangeCheckString ( vi, bufferSize, warnString );
}

ViStatus _VI_FUNC RGDM3K_ResetInterchangeCheck ( ViSession vi )
{
    return Ivi_ResetInterchangeCheck ( vi );
}

ViStatus _VI_FUNC RGDM3K_ClearInterchangeWarnings ( ViSession vi )
{
    return Ivi_ClearInterchangeWarnings ( vi );
}


/*****************************************************************************
 * Function: RGDM3K_InvalidateAllAttributes
 * Purpose:  This function invalidates the cached value of all attributes.
 *****************************************************************************/
ViStatus _VI_FUNC RGDM3K_InvalidateAllAttributes ( ViSession vi )
{
    return Ivi_InvalidateAllAttributes ( vi );
}


/****************************************************************************
 *  Function: RGDM3K_GetChannelName
 *  Purpose:  This function returns the highest-level channel name that
 *            corresponds to the specific driver channel string that is in
 *            the channel table at an index you specify.
 ****************************************************************************/
ViStatus _VI_FUNC RGDM3K_GetChannelName ( ViSession vi, ViInt32 index,
        ViInt32 bufferSize,
        ViChar name[] )
{
    ViStatus         error = VI_SUCCESS;
    ViConstString    channelName;
    ViInt32          actualSize = 0;

    checkErr ( Ivi_LockSession ( vi, VI_NULL ) );

    if ( bufferSize != 0 ) {
        if ( name == VI_NULL ) {
            viCheckParm ( IVI_ERROR_INVALID_PARAMETER, 4, "Null address for Channel Name" );
        }
    }

    /* Get channel name */
    viCheckParm ( Ivi_GetNthChannelString ( vi, index, &channelName ), 2, "Index" );

    actualSize = ( ViInt32 ) ( strlen ( channelName ) ) + 1;

    /* Prepare return values */
    if ( bufferSize == 0 ) {
        error = actualSize;

    } else {
        if ( bufferSize > 0 ) {
            if ( actualSize > bufferSize ) {
                error = actualSize;
                actualSize = bufferSize;
            }
        }

        memcpy ( name, channelName, actualSize - 1 );
        name[actualSize - 1] = '\0';
    }

Error:
    Ivi_UnlockSession ( vi, VI_NULL );
    return error;
}


/*****************************************************************************
 * Function: RGDM3K_ResetWithDefaults
 * Purpose:  This function resets the instrument and applies default settings
 *           from the IVI Configuration Store based on the logical name
 *           from which the session was created.
 *
 *****************************************************************************/
ViStatus _VI_FUNC RGDM3K_ResetWithDefaults ( ViSession vi )
{
    ViStatus error = VI_SUCCESS;

    checkErr ( Ivi_LockSession ( vi, VI_NULL ) );
    checkErr ( RGDM3K_reset ( vi ) );
    checkErr ( Ivi_ApplyDefaultSetup ( vi ) );

Error:
    Ivi_UnlockSession ( vi, VI_NULL );
    return error;
}


/****************************************************************************
 *  Function: RGDM3K_Disable
 *  Purpose:  This function places the instrument in a quiescent state as
 *            quickly as possible.
 * Note: This function is not be implemented.
 ****************************************************************************/
ViStatus _VI_FUNC RGDM3K_Disable ( ViSession vi )
{
    ViStatus error = VI_SUCCESS;

    checkErr ( Ivi_LockSession ( vi, VI_NULL ) );

    if ( !Ivi_Simulating ( vi ) ) {
        //Place appropriate code here to disable the instrument.
    }

Error:
    Ivi_UnlockSession ( vi, VI_NULL );
    return error;
}


/****************************************************************************
*  Function: RGDM3KAttrFreqThresholdVoltage_ReadCallback
*  Note: This function is not be implemented.
****************************************************************************/

static ViStatus _VI_FUNC RGDM3KAttrFreqThresholdVoltage_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value )
{
    return VI_SUCCESS;

}
/****************************************************************************
*  Function:RGDM3KAttrFreqThresholdVoltage_WriteCallback
*  Note: This function is not be implemented.
****************************************************************************/
static ViStatus _VI_FUNC RGDM3KAttrFreqThresholdVoltage_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value )
{
    ViStatus    error = VI_SUCCESS;

    if ( value == RGDM3K_VAL_AUTO_THRESHOLD_ON ) {
        //viCheckErr( viPrintf (io, "CMDSET AGILENT"));
        //viCheckErr( viPrintf (io, "FREQ:VOLT:RANG:AUTO ON;"));
        //viCheckErr( viPrintf (io, "CMDSET RIGOL"));
    } else
        if ( value == RGDM3K_VAL_AUTO_THRESHOLD_OFF ) {
            //viCheckErr( viPrintf (io, "CMDSET AGILENT"));
            //viCheckErr( viPrintf (io, "FREQ:VOLT:RANG:AUTO OFF;"));
            //viCheckErr( viPrintf (io, "CMDSET RIGOL"));
        } else
            if ( value == RGDM3K_VAL_AUTO_THRESHOLD_ONCE ) {
                //viCheckErr( viPrintf (io, "CMDSET AGILENT"));
                //viCheckErr( viPrintf (io, "FREQ:VOLT:RANG:AUTO ONCE;"));
                //viCheckErr( viPrintf (io, "CMDSET RIGOL"));
            } else
                //viCheckErr( viPrintf (io, "CMDSET AGILENT"));
            { viCheckErr ( viPrintf ( io, "FREQ:VOLT:RANG %Lf;", value ) ); }

    //viCheckErr( viPrintf (io, "CMDSET RIGOL"));

Error:
    return error;
}

/****************************************************************************
*  Function:RGDM3KAttrPeriodThresholdVoltage_ReadCallback
*  Note: This function is not be implemented.
****************************************************************************/

static ViStatus _VI_FUNC RGDM3KAttrPeriodThresholdVoltage_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value )
{
    return VI_SUCCESS;

}
/****************************************************************************
*  Function:RGDM3KAttrPeriodThresholdVoltage_WriteCallback
*  Note: This function is not be implemented.
****************************************************************************/
static ViStatus _VI_FUNC RGDM3KAttrPeriodThresholdVoltage_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value )
{
    return VI_SUCCESS;

}

/*****************************************************************************
 * Function: RGDM3K_CheckStatus
 * Purpose:  This function checks the status of the instrument to detect
 *           whether the instrument has encountered an error.  This function
 *           is called at the end of most exported driver functions.  If the
 *           instrument reports an error, this function returns
 *           IVI_ERROR_INSTR_SPECIFIC.  The user can set the
 *           IVI_ATTR_QUERY_INSTR_STATUS attribute to VI_FALSE to disable this
 *           check and increase execution speed.
 *
 *           Note:  Call this function only when the session is locked.
 *****************************************************************************/
static ViStatus RGDM3K_CheckStatus ( ViSession vi )
{
    ViStatus    error = VI_SUCCESS;

    if ( Ivi_QueryInstrStatus ( vi ) && Ivi_NeedToCheckStatus ( vi ) && !Ivi_Simulating ( vi ) ) {
        checkErr ( Ivi_SetNeedToCheckStatus ( vi, VI_FALSE ) );
    }

Error:
    return error;
}


/*****************************************************************************
 * Function: RGDM3K_DefaultInstrSetup
 * Purpose:  This function sends a default setup to the instrument.  The
 *           RGDM3K_reset function calls this function.  The
 *           RGDM3K_IviInit function calls this function when the
 *           user passes VI_FALSE for the reset parameter.  This function is
 *           useful for configuring settings that other instrument driver
 *           functions require.
 *
 *           Note:  Call this function only when the session is locked.
 *                  the command is not be implemented.
 *****************************************************************************/
static ViStatus RGDM3K_DefaultInstrSetup ( ViSession vi )
{
    ViStatus    error = VI_SUCCESS;

    /* Invalidate all attributes */
    checkErr ( Ivi_InvalidateAllAttributes ( vi ) );


    if ( !Ivi_Simulating ( vi ) ) {
        ViChar      buffer[BUFFER_SIZE];
        ViInt32     rev;
        ViSession   io = Ivi_IOSession ( vi ); /* call only when locked */

        checkErr ( Ivi_SetNeedToCheckStatus ( vi, VI_TRUE ) );

        /*
           Change the following command string so that it executes the default
           setup for your instrument.  The example does the following:

           *CLS     clears the event/status registers
           *ESE 1   sets the standard event status enable register to recognize
                    operation complete.
           *SRE 32  sets the service request register to enable a service
                    request on operation complete

           These settings are required for the default implementation of the
           WaitForOPCCallback to work correctly.
         */

        /*
           A new command is available starting with firmware Revision 2 which
           allows the user to take readings using INITiate without storing them in
           internal memory.  There is not reason to give users this capability while
           using this driver so this driver will check the revision of the DMM.
           If it is 2 or later, then send the command to make sure all readings
           are sent to the internal memory.
        */

        checkErr ( Ivi_GetAttributeViString ( vi, "", RGDM3K_ATTR_ID_QUERY_RESPONSE, 0,
                                              BUFFER_SIZE, buffer ) );

        sscanf ( buffer, "%*[^,],%*[^,],%*[^,],%*[^-]-%*[^-]-%ld", &rev );

        if ( rev > 1 )
        { viCheckErr ( viPrintf ( io, "*CLS;*ESE 1;*SRE 32;:DATA:FEED RDG_STORE, \"CALC\";" ) ); }

        else
        { viCheckErr ( viPrintf ( io, "*CLS;*ESE 1;*SRE 32;" ) ); }

    }




Error:
    return error;
}


/*****************************************************************************
 *----------------- Attribute Range Tables and Callbacks --------------------*
 *****************************************************************************/

/*- RGDM3K_ATTR_ID_QUERY_RESPONSE -*/

static ViStatus _VI_FUNC RGDM3KAttrIdQueryResponse_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        const ViConstString cacheValue )
{
    ViStatus    error = VI_SUCCESS;
    ViChar      rdBuffer[BUFFER_SIZE];
    ViUInt32    retCnt;

    viCheckErr ( viPrintf ( io, "*IDN?\n" ) );
    RGDM3K_delaytime();
    viCheckErr ( viRead ( io, rdBuffer, BUFFER_SIZE - 1, &retCnt ) );
    rdBuffer[retCnt] = 0;

    checkErr ( Ivi_SetValInStringCallback ( vi, attributeId, rdBuffer ) );

Error:
    return error;
}

/*- RGDM3K_ATTR_SPECIFIC_DRIVER_REVISION -*/

static ViStatus _VI_FUNC RGDM3KAttrSpecificDriverRevision_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        const ViConstString cacheValue )
{
    ViStatus    error = VI_NULL;
    ViChar      driverRevision[256];


    sprintf ( driverRevision,
              "Driver: RGDM3K %d.%d, Compiler: %s %3.2f, "
              "Components: IVIEngine %.2f, VISA-Spec %.2f",
              RGDM3K_MAJOR_VERSION, RGDM3K_MINOR_VERSION,
              IVI_COMPILER_NAME, IVI_COMPILER_VER_NUM,
              IVI_ENGINE_MAJOR_VERSION + IVI_ENGINE_MINOR_VERSION / 1000.0,
              Ivi_ConvertVISAVer ( VI_SPEC_VERSION ) );

    checkErr ( Ivi_SetValInStringCallback ( vi, attributeId, driverRevision ) );
Error:
    return error;
}

/*- RGDM3K_ATTR_INSTRUMENT_FIRMWARE_REVISION -*/

static ViStatus _VI_FUNC RGDM3KAttrInstrumentFirmwareRevision_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        const ViConstString cacheValue )
{
    ViStatus    error = VI_SUCCESS;
    ViChar      idQ[BUFFER_SIZE], rdBuffer[BUFFER_SIZE];

    if ( !Ivi_Simulating ( vi ) ) {         /* call only when locked */
        checkErr ( Ivi_GetAttributeViString ( vi, "", RGDM3K_ATTR_ID_QUERY_RESPONSE, 0,
                                              BUFFER_SIZE, idQ ) );
        sscanf ( idQ, "%*[^,],%*[^,],%*[^,],%256[^\n]", rdBuffer );

    } else {
        /* Simulate Instrument Revision Query */
        strcpy ( rdBuffer, "No revision information available while simulating." );
    }

    checkErr ( Ivi_SetValInStringCallback ( vi, attributeId, rdBuffer ) );

Error:
    return error;
}

/*- RGDM3K_ATTR_INSTRUMENT_MANUFACTURER -*/

static ViStatus _VI_FUNC RGDM3KAttrInstrumentManufacturer_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        const ViConstString cacheValue )
{
    ViStatus    error = VI_SUCCESS;
    ViChar      idQ[BUFFER_SIZE], rdBuffer[BUFFER_SIZE];

    checkErr ( Ivi_GetAttributeViString ( vi, "", RGDM3K_ATTR_ID_QUERY_RESPONSE,
                                          0, BUFFER_SIZE, idQ ) );

    sscanf ( idQ, "%256[^,]", rdBuffer );

    checkErr ( Ivi_SetValInStringCallback ( vi, attributeId, rdBuffer ) );

Error:
    return error;
}

/*- RGDM3K_ATTR_INSTRUMENT_MODEL -*/

static ViStatus _VI_FUNC RGDM3KAttrInstrumentModel_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        const ViConstString cacheValue )
{
    ViStatus    error = VI_SUCCESS;
    ViChar      idQ[BUFFER_SIZE], rdBuffer[BUFFER_SIZE];

    checkErr ( Ivi_GetAttributeViString ( vi, "", RGDM3K_ATTR_ID_QUERY_RESPONSE,
                                          0, BUFFER_SIZE, idQ ) );

    sscanf ( idQ, "%*[^,],%256[^,]", rdBuffer );

    checkErr ( Ivi_SetValInStringCallback ( vi, attributeId, rdBuffer ) );

Error:
    return error;
}

/*- RGDM3K_ATTR_FUNCTION -*/

static IviRangeTableEntry attrFunctionRangeTableForRGDM3KEntries[] = {
    {RGDM3K_VAL_DC_VOLTS, 0, 0, "DCV", 0},
    {RGDM3K_VAL_AC_VOLTS, 0, 0, "ACV", 0},
    {RGDM3K_VAL_AC_CURRENT, 0, 0, "ACI", 0},
    {RGDM3K_VAL_DC_CURRENT, 0, 0, "DCI", 0},
    {RGDM3K_VAL_2_WIRE_RES, 0, 0, "RESISTANCE", 0},
    {RGDM3K_VAL_4_WIRE_RES, 0, 0, "FRESISTANCE", 0},
    {RGDM3K_VAL_FREQ, 0, 0, "FREQUENCY", 0},
    {RGDM3K_VAL_PERIOD, 0, 0, "PERIOD", 0},
    {RGDM3K_VAL_DIODE, 0, 0, "DIODE", 0},
    {RGDM3K_VAL_CONTINUITY, 0, 0, "CONTINUITY", 0},
    {RGDM3K_VAL_CAPACITANCE, 0, 0, "CAPACITANCE", 0},
    {IVI_RANGE_TABLE_LAST_ENTRY}
};

static IviRangeTable attrFunctionRangeTableForRGDM3K = {
    IVI_VAL_DISCRETE,
    VI_FALSE,
    VI_FALSE,
    VI_NULL,
    attrFunctionRangeTableForRGDM3KEntries,
};





static ViStatus _VI_FUNC RGDM3KAttrFunction_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViInt32 value )
{
    ViStatus      error = VI_SUCCESS;
    ViInt32 measFunc;

    //checkErr( Ivi_GetAttrRangeTable (vi, "", RGDM3K_ATTR_FUNCTION,
    //                                 &RngTblPtr));
    //viCheckErr( Ivi_GetViInt32EntryFromValue (value, RngTblPtr, VI_NULL,
    //                                          VI_NULL, VI_NULL, VI_NULL, &cmd,
    //                                          VI_NULL));
    checkErr ( Ivi_GetAttributeViInt32 ( vi, VI_NULL, RGDM3K_ATTR_FUNCTION, 0,
                                         &measFunc ) );

    //viCheckErr( viPrintf (io, "CMDSET RIGOL"));
    switch ( measFunc ) {
        case RGDM3K_VAL_CONTINUITY:
            viCheckErr ( viPrintf ( io, ":function:continuity\n" ) );
            RGDM3K_delaytime();
            RGDM3K_delaytime();
            break;

        case RGDM3K_VAL_DIODE:
            viCheckErr ( viPrintf ( io, ":function:diode\n" ) );
            RGDM3K_delaytime();
            RGDM3K_delaytime();
            break;

        case RGDM3K_VAL_DC_VOLTS:
            viCheckErr ( viPrintf ( io, ":function:voltage:DC\n" ) );
            RGDM3K_delaytime();
            RGDM3K_delaytime();
            break;

        case RGDM3K_VAL_PERIOD:
            viCheckErr ( viPrintf ( io, ":function:period\n" ) );
            RGDM3K_delaytime();
            RGDM3K_delaytime();
            break;

        case RGDM3K_VAL_FREQ:
            viCheckErr ( viPrintf ( io, ":function:frequency\n" ) );
            RGDM3K_delaytime();
            RGDM3K_delaytime();
            break;

        case RGDM3K_VAL_AC_VOLTS:
            viCheckErr ( viPrintf ( io, ":function:voltage:AC\n" ) );
            RGDM3K_delaytime();
            RGDM3K_delaytime();
            break;

        case RGDM3K_VAL_DC_CURRENT:
            viCheckErr ( viPrintf ( io, ":function:current:DC\n" ) );
            RGDM3K_delaytime();
            RGDM3K_delaytime();
            break;

        case RGDM3K_VAL_AC_CURRENT:
            viCheckErr ( viPrintf ( io, ":function:current:AC\n" ) );
            RGDM3K_delaytime();
            RGDM3K_delaytime();
            break;

        case RGDM3K_VAL_2_WIRE_RES:
            viCheckErr ( viPrintf ( io, ":function:resistance\n" ) );
            RGDM3K_delaytime();
            RGDM3K_delaytime();
            break;

        case RGDM3K_VAL_4_WIRE_RES:
            viCheckErr ( viPrintf ( io, ":function:fresistance\n" ) );
            RGDM3K_delaytime();
            RGDM3K_delaytime();
            break;

        case RGDM3K_VAL_CAPACITANCE:
            viCheckErr ( viPrintf ( io, ":function:capacitance\n" ) );
            RGDM3K_delaytime();
            RGDM3K_delaytime();
            break;

        default:
            break;
    }




Error:
    return error;
}

static ViStatus _VI_FUNC RGDM3KAttrFunction_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViInt32 *value )
{
    ViStatus error = VI_SUCCESS;
    ViChar   rdBuffer[BUFFER_SIZE];
    IviRangeTablePtr RngTblPtr;

    viCheckErr ( viPrintf ( io, ":function?\n" ) ); //viCheckErr( viPrintf (io, "FUNC?;"));	  2008.5.4
    RGDM3K_delaytime();
    viCheckErr ( viScanf ( io, "%s", rdBuffer ) );	// viCheckErr( viScanf (io, "\"%[^\"]", rdBuffer));

    checkErr ( Ivi_GetAttrRangeTable ( vi, "", RGDM3K_ATTR_FUNCTION,
                                       &RngTblPtr ) );
    viCheckErr ( Ivi_GetViInt32EntryFromString ( rdBuffer, RngTblPtr,
                 value, VI_NULL, VI_NULL, VI_NULL,
                 VI_NULL ) );


Error:
    return error;
}

/*- RGDM3K_ATTR_RANGE -*/





static IviRangeTableEntry ADC_RangeTableEntries[] = {
    {RGDM3K_VAL_AUTO_RANGE_ON, RGDM3K_VAL_AUTO_RANGE_ON, RGDM3K_VAL_AUTO_RANGE_ON, "", 0},
    {RGDM3K_VAL_AUTO_RANGE_OFF, RGDM3K_VAL_AUTO_RANGE_OFF, RGDM3K_VAL_AUTO_RANGE_OFF, "", 0},
    {RGDM3K_VAL_AUTO_RANGE_ONCE, RGDM3K_VAL_AUTO_RANGE_ONCE, RGDM3K_VAL_AUTO_RANGE_ONCE, "", 0},
    {0, 0.001, 0.0, "", 0},
    {1, 0.01, 0.001, "", 0},
    {2, 0.1, 0.01, "", 0},
    {3, 1.0, 0.1, "", 0},
    {4, 10.0, 1.0, "", 0},
    {5, 0, 0, "", 0},
    {RGDM3K_VAL_MAXADC, 0, 0, "", 0},
    {RGDM3K_VAL_DEFADC, 0, 0, "", 0},
    {RGDM3K_VAL_MINADC, 0, 0, "", 0},
    {IVI_RANGE_TABLE_LAST_ENTRY}
};

static IviRangeTable ADC_RangeTable = {
    IVI_VAL_DISCRETE,
    VI_FALSE,
    VI_TRUE,
    VI_NULL,
    ADC_RangeTableEntries,
};



static IviRangeTableEntry Cap_RangeTableEntries[] = {
    {RGDM3K_VAL_AUTO_RANGE_ON, RGDM3K_VAL_AUTO_RANGE_ON, RGDM3K_VAL_AUTO_RANGE_ON, "", 0},
    {RGDM3K_VAL_AUTO_RANGE_OFF, RGDM3K_VAL_AUTO_RANGE_OFF, RGDM3K_VAL_AUTO_RANGE_OFF, "", 0},
    {RGDM3K_VAL_AUTO_RANGE_ONCE, RGDM3K_VAL_AUTO_RANGE_ONCE, RGDM3K_VAL_AUTO_RANGE_ONCE, "", 0},
    {0, 0.000000001, 0.0, "", 0},
    {1, 0.00000001, 0.000000001, "", 0},
    {2, 0.0000001, 0.00000001, "", 0},
    {3, 0.000001, 0.0000001, "", 0},
    {4, 0.00001, 0.000001, "", 0},
    {5, 0, 0, "", 0},
    {RGDM3K_VAL_MAXCAP, 0, 0, "", 0},
    {RGDM3K_VAL_DEFCAP, 0, 0, "", 0},
    {RGDM3K_VAL_MINCAP, 0, 0, "", 0},
    {IVI_RANGE_TABLE_LAST_ENTRY}
};

static IviRangeTable Cap_RangeTable = {
    IVI_VAL_DISCRETE,
    VI_FALSE,
    VI_TRUE,
    VI_NULL,
    Cap_RangeTableEntries,
};





static IviRangeTableEntry Ohms_RangeTableEntries[] = {
    {RGDM3K_VAL_AUTO_RANGE_ON, RGDM3K_VAL_AUTO_RANGE_ON, RGDM3K_VAL_AUTO_RANGE_ON, "", 0},
    {RGDM3K_VAL_AUTO_RANGE_OFF, RGDM3K_VAL_AUTO_RANGE_OFF, RGDM3K_VAL_AUTO_RANGE_OFF, "", 0},
    {RGDM3K_VAL_AUTO_RANGE_ONCE, RGDM3K_VAL_AUTO_RANGE_ONCE, RGDM3K_VAL_AUTO_RANGE_ONCE, "", 0},
    {0, 100.0, 0.0, "", 0},
    {1, 1000.0, 100.0, "", 0},
    {2, 10000.0, 1000.0, "", 0},
    {3, 100000.0, 10000.0, "", 0},
    {4, 1000000.0, 100000.0, "", 0},
    {5, 10000000.0, 1000000.0, "", 0},
    {6, 100000000.0, 10000000.0, "", 0},
    {RGDM3K_VAL_MINRES, 0, 0, "", 0},
    {RGDM3K_VAL_DEFRES, 0, 0, "", 0},
    {RGDM3K_VAL_MAXRES, 0, 0, "", 0},
    {IVI_RANGE_TABLE_LAST_ENTRY}
};

static IviRangeTable Ohms_RangeTable = {
    IVI_VAL_DISCRETE,
    VI_FALSE,
    VI_TRUE,
    VI_NULL,
    Ohms_RangeTableEntries,
};



static ViStatus _VI_FUNC RGDM3KAttrRange_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value )
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     measFunc;


    checkErr ( Ivi_GetAttributeViInt32 ( vi, VI_NULL, RGDM3K_ATTR_FUNCTION, 0,
                                         &measFunc ) );

    switch ( measFunc ) {
        case RGDM3K_VAL_CONTINUITY:
        case RGDM3K_VAL_DIODE:
        case RGDM3K_VAL_TEMPERATURE:

            break;

        case RGDM3K_VAL_DC_VOLTS:

            checkErr ( Ivi_SetAttributeViReal64 ( vi, "", RGDM3K_ATTR_RANGE_VDC, 0, value ) );
            break;

        case RGDM3K_VAL_PERIOD:
            checkErr ( Ivi_SetAttributeViReal64 ( vi, "", RGDM3K_ATTR_RANGE_PERIOD, 0, value ) );
            break;

        case RGDM3K_VAL_FREQ:

            checkErr ( Ivi_SetAttributeViReal64 ( vi, "", RGDM3K_ATTR_RANGE_FREQ, 0, value ) );
            break;

        case RGDM3K_VAL_AC_VOLTS:
            checkErr ( Ivi_SetAttributeViReal64 ( vi, "", RGDM3K_ATTR_RANGE_VAC, 0, value ) );
            break;

        case RGDM3K_VAL_DC_CURRENT:
            checkErr ( Ivi_SetAttributeViReal64 ( vi, "", RGDM3K_ATTR_RANGE_ADC, 0, value ) );
            break;

        case RGDM3K_VAL_AC_CURRENT:
            checkErr ( Ivi_SetAttributeViReal64 ( vi, "", RGDM3K_ATTR_RANGE_AAC, 0, value ) );
            break;

        case RGDM3K_VAL_2_WIRE_RES:
            checkErr ( Ivi_SetAttributeViReal64 ( vi, "", RGDM3K_ATTR_RANGE_RES, 0, value ) );
            break;

        case RGDM3K_VAL_4_WIRE_RES:
            checkErr ( Ivi_SetAttributeViReal64 ( vi, "", RGDM3K_ATTR_RANGE_FRES, 0, value ) );
            break;

        case RGDM3K_VAL_CAPACITANCE:
            checkErr ( Ivi_SetAttributeViReal64 ( vi, "", RGDM3K_ATTR_RANGE_CAP, 0, value ) );
            break;

        default:
            viCheckErr ( IVI_ERROR_INVALID_CONFIGURATION );
            break;
    }

    if ( ( value == RGDM3K_VAL_AUTO_RANGE_OFF ) || ( value == RGDM3K_VAL_AUTO_RANGE_ONCE ) ) {
        checkErr ( Ivi_InvalidateAttribute ( vi, "", RGDM3K_ATTR_RANGE_VDC ) );
        checkErr ( Ivi_InvalidateAttribute ( vi, "", RGDM3K_ATTR_RANGE_VAC ) );
        checkErr ( Ivi_InvalidateAttribute ( vi, "", RGDM3K_ATTR_RANGE_ADC ) );
        checkErr ( Ivi_InvalidateAttribute ( vi, "", RGDM3K_ATTR_RANGE_AAC ) );
        checkErr ( Ivi_InvalidateAttribute ( vi, "", RGDM3K_ATTR_RANGE_RES ) );
        checkErr ( Ivi_InvalidateAttribute ( vi, "", RGDM3K_ATTR_RANGE_FRES ) );
        checkErr ( Ivi_InvalidateAttribute ( vi, "", RGDM3K_ATTR_RANGE_CAP ) );
    }

Error:
    return error;
}

static ViStatus _VI_FUNC RGDM3KAttrRange_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value )
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     measFunc;

    checkErr ( Ivi_GetAttributeViInt32 ( vi, VI_NULL, RGDM3K_ATTR_FUNCTION, 0,
                                         &measFunc ) );

    switch ( measFunc ) {
        case RGDM3K_VAL_CONTINUITY:
        case RGDM3K_VAL_DIODE:
            //case RGDM3K_VAL_TEMPERATURE:

            *value = RGDM3K_VAL_AUTO_RANGE_ON;
            break;

        case RGDM3K_VAL_DC_VOLTS:

            checkErr ( Ivi_GetAttributeViReal64 ( vi, "", RGDM3K_ATTR_RANGE_VDC, 0, value ) );
            break;

        case RGDM3K_VAL_AC_VOLTS:
        case RGDM3K_VAL_FREQ:
        case RGDM3K_VAL_PERIOD:
            checkErr ( Ivi_GetAttributeViReal64 ( vi, "", RGDM3K_ATTR_RANGE_VAC, 0, value ) );
            break;

        case RGDM3K_VAL_DC_CURRENT:
            checkErr ( Ivi_GetAttributeViReal64 ( vi, "", RGDM3K_ATTR_RANGE_ADC, 0, value ) );
            break;

        case RGDM3K_VAL_AC_CURRENT:
            checkErr ( Ivi_GetAttributeViReal64 ( vi, "", RGDM3K_ATTR_RANGE_AAC, 0, value ) );
            break;

        case RGDM3K_VAL_2_WIRE_RES:
            checkErr ( Ivi_GetAttributeViReal64 ( vi, "", RGDM3K_ATTR_RANGE_RES, 0, value ) );
            break;

        case RGDM3K_VAL_4_WIRE_RES:
            checkErr ( Ivi_GetAttributeViReal64 ( vi, "", RGDM3K_ATTR_RANGE_FRES, 0, value ) );
            break;

        case RGDM3K_VAL_CAPACITANCE:
            checkErr ( Ivi_GetAttributeViReal64 ( vi, "", RGDM3K_ATTR_RANGE_CAP, 0, value ) );
            break;

        default:
            viCheckErr ( IVI_ERROR_INVALID_CONFIGURATION );
            break;
    }

Error:
    return error;
}




static IviRangeTableEntry attrTriggerSourceRangeTableForRGDM3KEntries[] = {
    {RGDM3K_VAL_IMMEDIATE, 0, 0, "AUTO", 0},
    {RGDM3K_VAL_EXTERNAL, 0, 0, "EXT", 0},
    {RGDM3K_VAL_SOFTWARE_TRIG, 0, 0, "SINGLE", 0},
    {IVI_RANGE_TABLE_LAST_ENTRY}
};

static IviRangeTable attrTriggerSourceRangeTableForRGDM3K = {
    IVI_VAL_DISCRETE,
    VI_TRUE,
    VI_TRUE,
    VI_NULL,
    attrTriggerSourceRangeTableForRGDM3KEntries,
};




static ViStatus _VI_FUNC RGDM3KAttrTriggerSource_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViInt32 value )
{
    ViStatus      error = VI_SUCCESS;
    ViString cmd;
    IviRangeTablePtr RngTblPtr;

    checkErr ( Ivi_GetAttrRangeTable ( vi, "", RGDM3K_ATTR_TRIGGER_SOURCE,
                                       &RngTblPtr ) );
    viCheckErr ( Ivi_GetViInt32EntryFromValue ( value, RngTblPtr,
                 VI_NULL, VI_NULL, VI_NULL, VI_NULL,
                 &cmd, VI_NULL ) );
    viCheckErr ( viPrintf ( io, ":trigger:source %s\n", cmd ) );
    RGDM3K_delaytime();




Error:
    return error;
}

static ViStatus _VI_FUNC RGDM3KAttrTriggerSource_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViInt32 *value )
{
    ViStatus error = VI_SUCCESS;
    ViChar   rdBuffer[BUFFER_SIZE];
    ViInt32  rdBufferSize = ( ViInt32 ) ( sizeof ( rdBuffer ) );
    IviRangeTablePtr RngTblPtr;

    viCheckErr ( viPrintf ( io, ":trigger:source?" ) );        //viCheckErr( viPrintf (io, ":TRIG:SOUR?;"));	 2008.5.4 Àî½¨Ê¡
    RGDM3K_delaytime();
    viCheckErr ( viScanf ( io, "%#s", &rdBufferSize, rdBuffer ) );

    checkErr ( Ivi_GetAttrRangeTable ( vi, "", RGDM3K_ATTR_TRIGGER_SOURCE,
                                       &RngTblPtr ) );
    viCheckErr ( Ivi_GetViInt32EntryFromString ( rdBuffer, RngTblPtr,
                 value, VI_NULL, VI_NULL, VI_NULL,
                 VI_NULL ) );



Error:
    return error;
}



static IviRangeTableEntry attrAcMaxFreqTable1Entries[] = {
    {3.0, 300000.0, 300000.0, "", 0},
    {IVI_RANGE_TABLE_LAST_ENTRY}
};
static IviRangeTable attrAcMaxFreqTable1 = {
    IVI_VAL_COERCED,
    VI_TRUE,
    VI_TRUE,
    VI_NULL,
    attrAcMaxFreqTable1Entries,
};



static ViStatus _VI_FUNC RGDM3KAttrAcMaxFreq_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value )
{
    ViStatus   error = VI_SUCCESS;
    ViInt32    measFunc, model;

    checkErr ( Ivi_GetAttributeViInt32 ( vi, VI_NULL, RGDM3K_ATTR_MODEL, 0, &model ) );

    if ( model == RGDM3K_VAL_MODEL_3058 ) {
        /*If measurment function is not AC Current, then
          return 300000.0 as the maximum frequency. */

        checkErr ( Ivi_GetAttributeViInt32 ( vi, VI_NULL, RGDM3K_ATTR_FUNCTION, 0, &measFunc ) );
        *value = ( measFunc == RGDM3K_VAL_AC_CURRENT ) ? 5000.0 : 300000.0;
    }



Error:
    return error;
}





static ViStatus _VI_FUNC RGDM3KAttrTriggerDelay_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value )
{
    ViStatus          error = VI_SUCCESS;

    if ( value == RGDM3K_VAL_AUTO_DELAY_ON ) {
        viCheckErr ( viPrintf ( io, "CMDSET AGILENT" ) );
        viCheckErr ( viPrintf ( io, "TRIG:DEL:AUTO ON;" ) );
        viCheckErr ( viPrintf ( io, "CMDSET RIGOL" ) );

    } else
        if ( value == RGDM3K_VAL_AUTO_DELAY_OFF ) {
            viCheckErr ( viPrintf ( io, "CMDSET AGILENT" ) );
            viCheckErr ( viPrintf ( io, "TRIG:DEL:AUTO OFF;" ) );
            viCheckErr ( viPrintf ( io, "CMDSET RIGOL" ) );
            checkErr ( Ivi_InvalidateAttribute ( vi, "", RGDM3K_ATTR_TRIGGER_DELAY ) );

        } else {

            //viCheckErr( viPrintf (io, "TRIG:DEL %Lf;", value));    /* Specifying a value automatically disables auto-delay. */
            viCheckErr ( viPrintf ( io, ":trigger:auto:interval %0.Lf", value ) );
        }

    RGDM3K_delaytime();

Error:
    return error;
}

static ViStatus _VI_FUNC RGDM3KAttrTriggerDelay_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value )
{
    ViStatus error = VI_SUCCESS;
    ViInt32  autodel;
    viCheckErr ( viPrintf ( io, "CMDSET AGILENT" ) );
    viCheckErr ( viPrintf ( io, "TRIG:DEL:AUTO?;" ) );
    viCheckErr ( viScanf ( io, "%ld", &autodel ) );
    viCheckErr ( viPrintf ( io, "CMDSET RIGOL" ) );

    if ( autodel )
    { *value = RGDM3K_VAL_AUTO_DELAY_ON; }

    else {
        viCheckErr ( viPrintf ( io, "CMDSET AGILENT" ) );
        viCheckErr ( viPrintf ( io, "TRIG:DEL?" ) );
        viCheckErr ( viScanf  ( io, "%Lf", value ) );
        viCheckErr ( viPrintf ( io, "CMDSET RIGOL" ) );
    }

Error:
    return error;
}



/*****************************************************************************
 * HIDDEN ATTRIBUTES
 *****************************************************************************/

/*- RGDM3K_ATTR_RANGE_VDC -*/

static ViStatus _VI_FUNC RGDM3KAttrRangeVdc_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value )
{
    ViStatus    error = VI_SUCCESS;

    viCheckErr ( viPrintf ( io, ":measure:voltage:DC:range?" ) );
    viCheckErr ( viScanf ( io, "%Lf", value ) );


Error:
    return error;
}



static ViStatus _VI_FUNC RGDM3KAttrRangeVdc_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value )
{
    ViStatus    error = VI_SUCCESS;

    if ( value == RGDM3K_VAL_AUTO_RANGE_ON ) {

        viCheckErr ( viPrintf ( io, ":measure AUTO\n" ) );

    } else
        if ( value == RGDM3K_VAL_AUTO_RANGE_OFF ) {

            viCheckErr ( viPrintf ( io, ":measure MANU\n" ) );

        } else
            if ( value == RGDM3K_VAL_AUTO_RANGE_ONCE ) {
                //viCheckErr( viPrintf (io, "CMDSET AGILENT"));
                //viCheckErr( viPrintf (io, "VOLT:DC:RANG:AUTO ONCE;"));
                //viCheckErr( viPrintf (io, "CMDSET RIGOL"));
            } else  /* Writing an explicit range turns off auto ranging. */
            { viCheckErr ( viPrintf ( io, ":measure:voltage:DC %0.Lf\n", value ) ); }

    RGDM3K_delaytime();

Error:
    return error;
}

/*- RGDM3K_ATTR_RANGE_ADC -*/

static ViStatus _VI_FUNC RGDM3KAttrRangeAdc_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value )
{
    ViStatus    error = VI_SUCCESS;

    viCheckErr ( viPrintf ( io, ":measure:current:DC:range?\n" ) );
    viCheckErr ( viScanf ( io, "%Lf", value ) );



Error:
    return error;
}

static ViStatus _VI_FUNC RGDM3KAttrRangeAdc_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value )
{
    ViStatus    error = VI_SUCCESS;

    if ( value == RGDM3K_VAL_AUTO_RANGE_ON ) {
        viCheckErr ( viPrintf ( io, ":measure AUTO\n" ) );

    } else
        if ( value == RGDM3K_VAL_AUTO_RANGE_OFF ) {

            viCheckErr ( viPrintf ( io, ":measure MANU\n" ) );

        } else
            if ( value == RGDM3K_VAL_AUTO_RANGE_ONCE ) {
                //viCheckErr( viPrintf (io, "CMDSET AGILENT"));
                //viCheckErr( viPrintf (io, "CURR:DC:RANG:AUTO ONCE;"));
                //viCheckErr( viPrintf (io, "CMDSET RIGOL"));
            } else
            { viCheckErr ( viPrintf ( io, ":measure:current:DC %0.Lf\n", value ) ); }

    RGDM3K_delaytime();


Error:
    return error;
}

/*- RGDM3K_ATTR_RANGE_AAC -*/

static ViStatus _VI_FUNC RGDM3KAttrRangeAac_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value )
{
    ViStatus    error = VI_SUCCESS;

    viCheckErr ( viPrintf ( io, ":measure:current:AC:range?\n" ) );
    viCheckErr ( viScanf ( io, "%Lf", value ) );

Error:
    return error;
}

static ViStatus _VI_FUNC RGDM3KAttrRangeAac_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value )
{
    ViStatus    error = VI_SUCCESS;

    if ( value == RGDM3K_VAL_AUTO_RANGE_ON ) {
        viCheckErr ( viPrintf ( io, ":measure AUTO\n" ) );

    } else
        if ( value == RGDM3K_VAL_AUTO_RANGE_OFF ) {
            viCheckErr ( viPrintf ( io, ":measure MANU\n" ) );

        } else
            if ( value == RGDM3K_VAL_AUTO_RANGE_ONCE ) {

                //viCheckErr( viPrintf (io, "CMDSET AGILENT"));
                //viCheckErr( viPrintf (io, "CURR:AC:RANG:AUTO ONCE;"));
                //viCheckErr( viPrintf (io, "CMDSET RIGOL"));
            } else
            { viCheckErr ( viPrintf ( io, ":measure:current:AC %0.lf\n", value ) ); }

    RGDM3K_delaytime();


Error:
    return error;
}

/*- RGDM3K_ATTR_RANGE_VAC -*/

static ViStatus _VI_FUNC RGDM3KAttrRangeVac_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value )
{
    ViStatus    error = VI_SUCCESS;

    viCheckErr ( viPrintf ( io, ":measure:voltage:AC:range?\n" ) );
    viCheckErr ( viScanf ( io, "%Lf", value ) );


Error:
    return error;
}

static ViStatus _VI_FUNC RGDM3KAttrRangeVac_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value )
{
    ViStatus    error = VI_SUCCESS;

    if ( value == RGDM3K_VAL_AUTO_RANGE_ON ) {
        viCheckErr ( viPrintf ( io, ":measure AUTO\n" ) );

    } else
        if ( value == RGDM3K_VAL_AUTO_RANGE_OFF ) {
            viCheckErr ( viPrintf ( io, ":measure MANU\n" ) );

        } else
            if ( value == RGDM3K_VAL_AUTO_RANGE_ONCE ) {
                //viCheckErr( viPrintf (io, "CMDSET AGILENT"));
                //viCheckErr( viPrintf (io, "VOLT:AC:RANG:AUTO ONCE;"));
                //viCheckErr( viPrintf (io, "CMDSET RIGOL"));
            } else
            { viCheckErr ( viPrintf ( io, ":measure:voltage:AC %Lf\n", value ) ); }

    RGDM3K_delaytime();


Error:
    return error;
}

/*- RGDM3K_ATTR_RANGE_FRES -*/

static ViStatus _VI_FUNC RGDM3KAttrRangeFres_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value )
{
    ViStatus    error = VI_SUCCESS;

    viCheckErr ( viPrintf ( io, ":measure:fresistance:range?\n" ) );
    viCheckErr ( viScanf ( io, "%Lf", value ) );


Error:
    return error;
}

static ViStatus _VI_FUNC RGDM3KAttrRangeFres_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value )
{
    ViStatus    error = VI_SUCCESS;

    if ( value == RGDM3K_VAL_AUTO_RANGE_ON ) {
        viCheckErr ( viPrintf ( io, ":measure AUTO\n" ) );

    } else
        if ( value == RGDM3K_VAL_AUTO_RANGE_OFF ) {
            viCheckErr ( viPrintf ( io, ":measure MANU\n" ) );

        } else
            if ( value == RGDM3K_VAL_AUTO_RANGE_ONCE ) {
                //viCheckErr( viPrintf (io, "CMDSET AGILENT"));
                //viCheckErr( viPrintf (io, "FRES:RANG:AUTO ONCE;"));
                //viCheckErr( viPrintf (io, "CMDSET RIGOL"));
            } else
            { viCheckErr ( viPrintf ( io, ":measure:fresistance %0.Lf", value ) ); }

    RGDM3K_delaytime();


Error:
    return error;
}

/*- RGDM3K_ATTR_RANGE_RES -*/

static ViStatus _VI_FUNC RGDM3KAttrRangeRes_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value )
{
    ViStatus    error = VI_SUCCESS;

    viCheckErr ( viPrintf ( io, ":measure:resistance:range?\n" ) );
    viCheckErr ( viScanf ( io, "%Lf", value ) );

Error:
    return error;
}

static ViStatus _VI_FUNC RGDM3KAttrRangeRes_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value )
{
    ViStatus    error = VI_SUCCESS;

    if ( value == RGDM3K_VAL_AUTO_RANGE_ON ) {

        viCheckErr ( viPrintf ( io, ":measure AUTO\n" ) );

    } else
        if ( value == RGDM3K_VAL_AUTO_RANGE_OFF ) {

            viCheckErr ( viPrintf ( io, ":measure MANU\n" ) );

        } else
            if ( value == RGDM3K_VAL_AUTO_RANGE_ONCE ) {
                //viCheckErr( viPrintf (io, "CMDSET AGILENT"));
                //viCheckErr( viPrintf (io, "RES:RANG:AUTO ONCE;"));
                //viCheckErr( viPrintf (io, "CMDSET RIGOL"));
            } else
            { viCheckErr ( viPrintf ( io, ":measure:resistance %0.Lf\n", value ) ); }

    RGDM3K_delaytime();


Error:
    return error;
}

/*- RGDM3K_ATTR_RANGE_CAP -*/

static ViStatus _VI_FUNC RGDM3KAttrRangeCap_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value )
{
    ViStatus    error = VI_SUCCESS;

    viCheckErr ( viPrintf ( io, ":measure:capacitance:range?\n" ) );
    viCheckErr ( viScanf ( io, "%Lf", value ) );


Error:
    return error;
}

static ViStatus _VI_FUNC RGDM3KAttrRangeCap_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value )
{
    ViStatus    error = VI_SUCCESS;

    if ( value == RGDM3K_VAL_AUTO_RANGE_ON ) {
        viCheckErr ( viPrintf ( io, ":measure AUTO\n" ) );

    } else
        if ( value == RGDM3K_VAL_AUTO_RANGE_OFF ) {
            viCheckErr ( viPrintf ( io, ":measure MANU\n" ) );

        } else
            if ( value == RGDM3K_VAL_AUTO_RANGE_ONCE ) {
                //viCheckErr( viPrintf (io, "CMDSET AGILENT"));
                //viCheckErr( viPrintf (io, "CAP:RANG:AUTO ONCE;"));
                //viCheckErr( viPrintf (io, "CMDSET RIGOL"));
            } else  /* Writing an explicit range turns off auto ranging. */
            { viCheckErr ( viPrintf ( io, ":measure:capacitance %0.Lf\n", value ) ); }

    RGDM3K_delaytime();


Error:
    return error;
}



/* RGDM3K_ATTR_RESOLUTION_DC */
static IviRangeTableEntry attrResolutionDCRangeTableForRGDM3KEntries[] = {
    {RGDM3K_VAL_MAX, 1.0, 0.0001, "F", 0},
    {RGDM3K_VAL_DEF, 100, 1.0, "M", 0},
    {RGDM3K_VAL_MIN, 1000, 100, "S", 0},
    {IVI_RANGE_TABLE_LAST_ENTRY}
};

static IviRangeTable attrResolutionDCRangeTableForRGDM3K = {
    IVI_VAL_DISCRETE,
    VI_TRUE,
    VI_FALSE,
    VI_NULL,
    attrResolutionDCRangeTableForRGDM3KEntries,
};




static ViStatus _VI_FUNC RGDM3KAttrResolutionAbsolute_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value )
{
    ViStatus error = VI_SUCCESS;
    ViChar      rdBuffer[BUFFER_SIZE];
    ViUInt32    retCnt;
    ViInt32     measFunc;
    checkErr ( Ivi_GetAttributeViInt32 ( vi, VI_NULL, RGDM3K_ATTR_FUNCTION, 0,
                                         &measFunc ) );

    switch ( measFunc ) {


        case RGDM3K_VAL_CAPACITANCE:
            viCheckErr ( viPrintf ( io, ":resolution:capacitance?\n" ) );
            viCheckErr ( viRead ( io, rdBuffer, BUFFER_SIZE - 1, &retCnt ) );
            rdBuffer[retCnt] = 0;
            checkErr ( Ivi_SetValInStringCallback ( vi, attributeId, rdBuffer ) );
            break;

        case RGDM3K_VAL_AC_VOLTS:
            viCheckErr ( viPrintf ( io, ":resolution:voltage:ac?\n" ) );
            viCheckErr ( viRead ( io, rdBuffer, BUFFER_SIZE - 1, &retCnt ) );
            rdBuffer[retCnt] = 0;
            checkErr ( Ivi_SetValInStringCallback ( vi, attributeId, rdBuffer ) );
            break;

        case RGDM3K_VAL_AC_CURRENT:
            viCheckErr ( viPrintf ( io, ":resolution:current:ac?\n" ) );
            viCheckErr ( viRead ( io, rdBuffer, BUFFER_SIZE - 1, &retCnt ) );
            rdBuffer[retCnt] = 0;
            checkErr ( Ivi_SetValInStringCallback ( vi, attributeId, rdBuffer ) );
            break;

        case RGDM3K_VAL_DC_VOLTS:
            viCheckErr ( viPrintf ( io, ":resolution:voltage:dc?\n" ) );
            viCheckErr ( viRead ( io, rdBuffer, BUFFER_SIZE - 1, &retCnt ) );
            rdBuffer[retCnt] = 0;
            checkErr ( Ivi_SetValInStringCallback ( vi, attributeId, rdBuffer ) );
            break;

        case RGDM3K_VAL_DC_CURRENT:
            viCheckErr ( viPrintf ( io, ":resolution:current:dc?\n" ) );
            viCheckErr ( viRead ( io, rdBuffer, BUFFER_SIZE - 1, &retCnt ) );
            rdBuffer[retCnt] = 0;
            checkErr ( Ivi_SetValInStringCallback ( vi, attributeId, rdBuffer ) );
            break;

        case RGDM3K_VAL_2_WIRE_RES:
            viCheckErr ( viPrintf ( io, ":resolution:resistance?\n" ) );
            viCheckErr ( viRead ( io, rdBuffer, BUFFER_SIZE - 1, &retCnt ) );
            rdBuffer[retCnt] = 0;
            checkErr ( Ivi_SetValInStringCallback ( vi, attributeId, rdBuffer ) );
            break;

        case RGDM3K_VAL_4_WIRE_RES:
            viCheckErr ( viPrintf ( io, ":resolution:fresistance?\n" ) );
            viCheckErr ( viRead ( io, rdBuffer, BUFFER_SIZE - 1, &retCnt ) );
            rdBuffer[retCnt] = 0;
            checkErr ( Ivi_SetValInStringCallback ( vi, attributeId, rdBuffer ) );
            break;

        default:
            viCheckErr ( IVI_ERROR_INVALID_CONFIGURATION );
            break;

    }

Error:
    return error;
}

static ViStatus _VI_FUNC RGDM3KAttrResolutionAbsolute_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value )
{

    ViStatus error = VI_SUCCESS;
    ViInt32     measFunc;
    ViString cmd;

    IviRangeTablePtr RngTblPtr;
    checkErr ( Ivi_GetAttributeViInt32 ( vi, VI_NULL, RGDM3K_ATTR_FUNCTION, 0,
                                         &measFunc ) );

    switch ( measFunc ) {
        case RGDM3K_VAL_CONTINUITY:
        case RGDM3K_VAL_DIODE:
            break;

        case RGDM3K_VAL_CAPACITANCE:

            // checkErr( Ivi_GetAttrRangeTable (vi, "", RGDM3K_ATTR_RESOLUTION_VDC,
            //                         &RngTblPtr));
            // viCheckErr( Ivi_GetViInt32EntryFromValue (value, RngTblPtr, VI_NULL,
            //                                  VI_NULL, VI_NULL, VI_NULL, &cmd,
            //                                  VI_NULL));
            // viCheckErr(viPrintf(io,":resolution:capacitance %s\n",cmd));
            break;

        case RGDM3K_VAL_AC_VOLTS:
            checkErr ( Ivi_GetAttrRangeTable ( vi, "", RGDM3K_ATTR_RESOLUTION_VDC,
                                               &RngTblPtr ) );
            viCheckErr ( Ivi_GetViInt32EntryFromValue ( value, RngTblPtr, VI_NULL,
                         VI_NULL, VI_NULL, VI_NULL, &cmd,
                         VI_NULL ) );
            viCheckErr ( viPrintf ( io, ":rate:voltage:ac %s\n", cmd ) );
            break;

        case RGDM3K_VAL_AC_CURRENT:
            checkErr ( Ivi_GetAttrRangeTable ( vi, "", RGDM3K_ATTR_RESOLUTION_VDC,
                                               &RngTblPtr ) );
            viCheckErr ( Ivi_GetViInt32EntryFromValue ( value, RngTblPtr, VI_NULL,
                         VI_NULL, VI_NULL, VI_NULL, &cmd,
                         VI_NULL ) );
            viCheckErr ( viPrintf ( io, ":rate:current:ac %s\n", cmd ) );

            break;

        case RGDM3K_VAL_FREQ:


            break;

        case RGDM3K_VAL_PERIOD:

            break;

        case RGDM3K_VAL_DC_VOLTS:
            checkErr ( Ivi_GetAttrRangeTable ( vi, "", RGDM3K_ATTR_RESOLUTION_VDC,
                                               &RngTblPtr ) );
            viCheckErr ( Ivi_GetViInt32EntryFromValue ( value, RngTblPtr, VI_NULL,
                         VI_NULL, VI_NULL, VI_NULL, &cmd,
                         VI_NULL ) );
            viCheckErr ( viPrintf ( io, ":rate:voltage:dc %s\n", cmd ) );

            break;


        case RGDM3K_VAL_DC_CURRENT:
            checkErr ( Ivi_GetAttrRangeTable ( vi, "", RGDM3K_ATTR_RESOLUTION_VDC,
                                               &RngTblPtr ) );
            viCheckErr ( Ivi_GetViInt32EntryFromValue ( value, RngTblPtr, VI_NULL,
                         VI_NULL, VI_NULL, VI_NULL, &cmd,
                         VI_NULL ) );
            viCheckErr ( viPrintf ( io, ":rate:current:dc %s\n", cmd ) );

            break;

        case RGDM3K_VAL_2_WIRE_RES:
            checkErr ( Ivi_GetAttrRangeTable ( vi, "", RGDM3K_ATTR_RESOLUTION_RES,
                                               &RngTblPtr ) );
            viCheckErr ( Ivi_GetViInt32EntryFromValue ( value, RngTblPtr, VI_NULL,
                         VI_NULL, VI_NULL, VI_NULL, &cmd,
                         VI_NULL ) );
            viCheckErr ( viPrintf ( io, ":rate:resistance %s\n", cmd ) );

            break;

        case RGDM3K_VAL_4_WIRE_RES:
            checkErr ( Ivi_GetAttrRangeTable ( vi, "", RGDM3K_ATTR_RESOLUTION_FRES,
                                               &RngTblPtr ) );
            viCheckErr ( Ivi_GetViInt32EntryFromValue ( value, RngTblPtr, VI_NULL,
                         VI_NULL, VI_NULL, VI_NULL, &cmd,
                         VI_NULL ) );
            viCheckErr ( viPrintf ( io, ":rate:fresistance %s\n", cmd ) );

            break;

        default:
            viCheckErr ( IVI_ERROR_INVALID_CONFIGURATION );
            break;
    }

    /*=CHANGE:===============================================================*
        Modify this example code to set the resolution.

    viCheckErr (viPrintf (io, ":RES %le;", value));
     *============================================================END=CHANGE=*/

Error:
    return error;
}




/* RGDM3K_ATTR_FREQ_VOLTAGE_RANGE */

/*****************************************************************************
 * Function: RGDM3K_ConfigureFrequencyVoltageRange
 * Purpose:  Configures the Frequency voltage range of the DMM.  This
 *           attribute is RGDM3K_ATTR_FREQ_VOLTAGE_RANGE
 *****************************************************************************/
ViStatus _VI_FUNC  RGDM3K_ConfigureFrequencyVoltageRange ( ViSession vi, ViReal64 frequencyVoltageRange )

{
    ViStatus    error = VI_SUCCESS;

    checkErr ( Ivi_LockSession ( vi, VI_NULL ) );

    viCheckParm ( Ivi_SetAttributeViReal64 ( vi, VI_NULL, RGDM3K_ATTR_FREQ_VOLTAGE_RANGE,
                  0, frequencyVoltageRange ), 2, "Freq Voltage Range" );

    checkErr ( RGDM3K_CheckStatus ( vi ) );

Error:
    Ivi_UnlockSession ( vi, VI_NULL );
    return error;
}



/*- RGDM3K_ATTR_MODEL -*/

static IviRangeTableEntry attrModelEntries[] = {
    {RGDM3K_VAL_MODEL_3058, 0, 0, "", 0},
    {IVI_RANGE_TABLE_LAST_ENTRY}
};

static IviRangeTable attrModel = {
    IVI_VAL_DISCRETE,
    VI_TRUE,
    VI_TRUE,
    VI_NULL,
    attrModelEntries,
};






static ViStatus _VI_FUNC RGDM3KAttrFreqVoltageRange_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value )
{
    ViStatus	error = VI_SUCCESS;
    ViReal64    range1, range2;

    checkErr ( Ivi_GetAttributeViReal64 ( vi, VI_NULL, RGDM3K_ATTR_FREQ_THRESHOLD_VOLTAGE, 0, &range1 ) );
    checkErr ( Ivi_GetAttributeViReal64 ( vi, VI_NULL, RGDM3K_ATTR_PERIOD_THRESHOLD_VOLTAGE, 0, &range2 ) );

    if ( range1 == range2 ) {
        if ( range1 ==  RGDM3K_VAL_AUTO_THRESHOLD_ON )
        { *value = RGDM3K_VAL_AUTO_RANGE_ON; }

        else
            if ( range1 == RGDM3K_VAL_AUTO_THRESHOLD_OFF )
            { *value = RGDM3K_VAL_AUTO_RANGE_OFF; }

            else { *value = range1; }

    } else { viCheckErrElab ( error, "Inconsistent Frequency Voltage Range between frequency and period measurement." ); }

Error:
    return error;
}

static ViStatus _VI_FUNC RGDM3KAttrFreqVoltageRange_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value )
{
    ViStatus	error = VI_SUCCESS;

    if ( value == RGDM3K_VAL_AUTO_RANGE_ON ) {
        checkErr ( Ivi_SetAttributeViReal64 ( vi, VI_NULL, RGDM3K_ATTR_FREQ_THRESHOLD_VOLTAGE, 0, RGDM3K_VAL_AUTO_THRESHOLD_ON ) );
        checkErr ( Ivi_SetAttributeViReal64 ( vi, VI_NULL, RGDM3K_ATTR_PERIOD_THRESHOLD_VOLTAGE, 0, RGDM3K_VAL_AUTO_THRESHOLD_ON ) );

    } else
        if ( value == RGDM3K_VAL_AUTO_RANGE_OFF ) {
            checkErr ( Ivi_SetAttributeViReal64 ( vi, VI_NULL, RGDM3K_ATTR_FREQ_THRESHOLD_VOLTAGE, 0, RGDM3K_VAL_AUTO_THRESHOLD_OFF ) );
            checkErr ( Ivi_SetAttributeViReal64 ( vi, VI_NULL, RGDM3K_ATTR_PERIOD_THRESHOLD_VOLTAGE, 0, RGDM3K_VAL_AUTO_THRESHOLD_OFF ) );
            checkErr ( Ivi_InvalidateAttribute ( vi, "", RGDM3K_ATTR_FREQ_THRESHOLD_VOLTAGE ) );
            checkErr ( Ivi_InvalidateAttribute ( vi, "", RGDM3K_ATTR_PERIOD_THRESHOLD_VOLTAGE ) );

        } else
            if ( value == RGDM3K_VAL_AUTO_RANGE_ONCE ) {
                checkErr ( Ivi_SetAttributeViReal64 ( vi, VI_NULL, RGDM3K_ATTR_FREQ_THRESHOLD_VOLTAGE, 0, RGDM3K_VAL_AUTO_THRESHOLD_ONCE ) );
                checkErr ( Ivi_SetAttributeViReal64 ( vi, VI_NULL, RGDM3K_ATTR_PERIOD_THRESHOLD_VOLTAGE, 0, RGDM3K_VAL_AUTO_THRESHOLD_ONCE ) );
                checkErr ( Ivi_InvalidateAttribute ( vi, "", RGDM3K_ATTR_FREQ_THRESHOLD_VOLTAGE ) );
                checkErr ( Ivi_InvalidateAttribute ( vi, "", RGDM3K_ATTR_PERIOD_THRESHOLD_VOLTAGE ) );

            } else {
                checkErr ( Ivi_SetAttributeViReal64 ( vi, VI_NULL, RGDM3K_ATTR_FREQ_THRESHOLD_VOLTAGE, 0, value ) );
                checkErr ( Ivi_SetAttributeViReal64 ( vi, VI_NULL, RGDM3K_ATTR_PERIOD_THRESHOLD_VOLTAGE, 0, value ) );
            }

Error:
    return error;
}

static ViStatus _VI_FUNC RGDM3KAttrAcMinFreq_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value )
{
    return VI_SUCCESS;
}

static ViStatus _VI_FUNC RGDM3KAttrAcMinFreq_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value )
{
    return VI_SUCCESS;
}


static ViStatus _VI_FUNC RGDM3KAttrResolutionVdc_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value )
{
    return VI_SUCCESS;
}

static ViStatus _VI_FUNC RGDM3KAttrResolutionVdc_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value )
{
    return VI_SUCCESS;
}

static ViStatus _VI_FUNC RGDM3KAttrResolutionFres_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value )
{
    return VI_SUCCESS;
}

static ViStatus _VI_FUNC RGDM3KAttrResolutionFres_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value )
{
    return VI_SUCCESS;
}

static ViStatus _VI_FUNC RGDM3KAttrResolutionAdc_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value )
{
    return VI_SUCCESS;
}

static ViStatus _VI_FUNC RGDM3KAttrResolutionAdc_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value )
{
    return VI_SUCCESS;
}

static ViStatus _VI_FUNC RGDM3KAttrResolutionCap_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value )
{
    return VI_SUCCESS;
}

static ViStatus _VI_FUNC RGDM3KAttrResolutionCap_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value )
{
    return VI_SUCCESS;
}

static ViStatus _VI_FUNC RGDM3KAttrResolutionRes_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value )
{
    return VI_SUCCESS;
}

static ViStatus _VI_FUNC RGDM3KAttrResolutionRes_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value )
{
    return VI_SUCCESS;
}

static ViStatus _VI_FUNC RGDM3KAttrRangeFreq_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value )
{
    ViStatus	error = VI_SUCCESS;


    viCheckErr ( viPrintf ( io, ":measure:frequency:range?\n" ) );
    viCheckErr ( viScanf ( io, "%Lf", value ) );


Error:
    return error;
}

static ViStatus _VI_FUNC RGDM3KAttrRangeFreq_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value )
{
    ViStatus	error = VI_SUCCESS;

    if ( value == RGDM3K_VAL_AUTO_RANGE_ON ) {
        viCheckErr ( viPrintf ( io, ":measure AUTO\n" ) );

    } else
        if ( value == RGDM3K_VAL_AUTO_RANGE_OFF ) {
            viCheckErr ( viPrintf ( io, ":measure MANU\n" ) );

        } else
            if ( value == RGDM3K_VAL_AUTO_RANGE_ONCE ) {
                //viCheckErr( viPrintf (io, "CMDSET AGILENT"));
                //viCheckErr( viPrintf (io, "FREQ:VOLT:RANG:AUTO ONCE;"));
                //viCheckErr( viPrintf (io, "CMDSET RIGOL"));
            } else  /* Writing an explicit range turns off auto ranging. */
            { viCheckErr ( viPrintf ( io, ":measure:frequency %0.Lf\n", value ) ); }

    RGDM3K_delaytime();


Error:
    return error;
}

static ViStatus _VI_FUNC RGDM3KAttrRangePeriod_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value )
{
    ViStatus	error = VI_SUCCESS;

    viCheckErr ( viPrintf ( io, ":measure:period:range?\n" ) );
    viCheckErr ( viScanf ( io, "%Lf", value ) );

Error:
    return error;
}

static ViStatus _VI_FUNC RGDM3KAttrRangePeriod_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value )
{
    ViStatus	error = VI_SUCCESS;

    if ( value == RGDM3K_VAL_AUTO_RANGE_ON ) {
        viCheckErr ( viPrintf ( io, ":measure AUTO\n" ) );

    } else
        if ( value == RGDM3K_VAL_AUTO_RANGE_OFF ) {
            viCheckErr ( viPrintf ( io, ":measure MANU\n" ) );

        } else
            if ( value == RGDM3K_VAL_AUTO_RANGE_ONCE ) {
                //viCheckErr( viPrintf (io, "CMDSET AGILENT"));
                //viCheckErr( viPrintf (io, "PER:VOLT:RANG:AUTO ONCE;"));
                //viCheckErr( viPrintf (io, "CMDSET RIGOL"));
            } else  /* Writing an explicit range turns off auto ranging. */
            { viCheckErr ( viPrintf ( io, ":measure:period %0.Lf\n", value ) ); }

    RGDM3K_delaytime();


Error:
    return error;
}

static ViStatus _VI_FUNC RGDM3KAttrAcMaxFreq_RangeTableCallback ( ViSession vi,
        ViConstString channelName,
        ViAttr attributeId,
        IviRangeTablePtr *rangeTablePtr )
{
    IviRangeTablePtr	tblPtr = VI_NULL;

    tblPtr = &attrAcMaxFreqTable1;
    /*
    	NOTE:  Insert code here to select the correct range table
    	or to modify a dynamic range table.  Set the tblPtr
    	local variable to the address of the range table.
    */


    *rangeTablePtr = tblPtr;
    return VI_SUCCESS;
}

static ViStatus _VI_FUNC RGDM3KAttrAcMinFreq_RangeTableCallback ( ViSession vi,
        ViConstString channelName,
        ViAttr attributeId,
        IviRangeTablePtr *rangeTablePtr )
{
    IviRangeTablePtr	tblPtr = VI_NULL;

    tblPtr = &attrAcMinFreqTable1;
    /*
    	NOTE:  Insert code here to select the correct range table
    	or to modify a dynamic range table.  Set the tblPtr
    	local variable to the address of the range table.
    */

    *rangeTablePtr = tblPtr;
    return VI_SUCCESS;
}



static ViStatus _VI_FUNC RGDM3KAttrMathOperation_CheckCallback ( ViSession vi,
        ViConstString channelName,
        ViAttr attributeId,
        ViInt32 value )
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     measFunc;

    checkErr ( Ivi_DefaultCheckCallbackViInt32 ( vi, VI_NULL,
               RGDM3K_ATTR_MATH_OPERATION, value ) );

    if ( value != RGDM3K_VAL_MATH_OFF ) {
        checkErr ( Ivi_GetAttributeViInt32 ( vi, VI_NULL, RGDM3K_ATTR_FUNCTION, 0,
                                             &measFunc ) );

        switch ( measFunc ) {
            case RGDM3K_VAL_DC_CURRENT:
            case RGDM3K_VAL_AC_CURRENT:
            case RGDM3K_VAL_2_WIRE_RES:
            case RGDM3K_VAL_4_WIRE_RES:
            case RGDM3K_VAL_FREQ:
            case RGDM3K_VAL_PERIOD:
            case RGDM3K_VAL_CAPACITANCE:
                if ( ( value == RGDM3K_VAL_MATH_DB ) || ( value == RGDM3K_VAL_MATH_DBM ) ) {
                    viCheckErrElab ( IVI_ERROR_INVALID_VALUE, "Math operation cannot be "
                                     "set to dB or dBm in current measurement function." );
                }

                break;

            case RGDM3K_VAL_CONTINUITY:
            case RGDM3K_VAL_DIODE:
                viCheckErrElab ( IVI_ERROR_INVALID_VALUE, "Math operation can only be disabled "
                                 "while in the Continuity or Diode measurement function." );
                break;
        }
    }

Error:
    return error;
}

static ViStatus _VI_FUNC RGDM3KAttrMathOperation_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViInt32 *value )
{
    ViStatus    error = VI_SUCCESS;
    ViChar      rdBuffer[BUFFER_SIZE];
    ViInt32     state;

    viCheckErr ( viPrintf ( io, ":calculate:function?\n" ) );
    viCheckErr ( viScanf ( io, "%ld", &state ) );

    if ( state == 0 )
    { *value = RGDM3K_VAL_MATH_OFF; }

    else {
        viCheckErr ( viPrintf ( io, ":calculate:function?\n" ) );
        viCheckErr ( viScanf ( io, "%256[^\n]", rdBuffer ) );
        viCheckErr ( Ivi_GetViInt32EntryFromString ( rdBuffer, &attrMathOperationRangeTable,
                     value, VI_NULL, VI_NULL, VI_NULL,
                     VI_NULL ) );
    }

Error:
    return error;
}

static ViStatus _VI_FUNC RGDM3KAttrMathOperation_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViInt32 value )
{
    ViStatus    error = VI_SUCCESS;
    ViString    cmd;

    viCheckErr ( Ivi_GetViInt32EntryFromValue ( value, &attrMathOperationRangeTable,
                 VI_NULL, VI_NULL, VI_NULL, VI_NULL,
                 &cmd, VI_NULL ) );

    if ( value == RGDM3K_VAL_MATH_OFF )
    { viCheckErr ( viPrintf ( io, ":calculate:function NONE\n" ) ); }

    else
    { viCheckErr ( viPrintf ( io, ":calculate:function %s\n", cmd ) ); }

    /*
        If the user fails to set the null offset or dB reference after enabling the null
        or dB operation, the DMM automatically sets the next reading as the null offset
        or the dB reference respectively.
    */
    if ( value == RGDM3K_VAL_MATH_NULL )
    { checkErr ( Ivi_SetAttributeViBoolean ( vi, "", RGDM3K_ATTR_INVALIDATE_NULL_OFFSET, 0, VI_TRUE ) ); }

    else
        if ( value == RGDM3K_VAL_MATH_DB )
        { checkErr ( Ivi_SetAttributeViBoolean ( vi, "", RGDM3K_ATTR_INVALIDATE_DB_REFERENCE, 0, VI_TRUE ) ); }

Error:
    return error;
}



/*- RGDM3K_ATTR_MATH_NULL_OFFSET -*/

static ViStatus _VI_FUNC RGDM3KAttrMathNullOffset_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value )
{
    ViStatus    error = VI_SUCCESS;

    viCheckErr ( viPrintf ( io, ":calculate:rel:offset %0.Lf\n", value ) );
    /* No longer need to invalidated since the user has set the null offset. */
    checkErr ( Ivi_SetAttributeViBoolean ( vi, "", RGDM3K_ATTR_INVALIDATE_NULL_OFFSET, 0, VI_FALSE ) );

Error:
    return error;
}

static ViStatus _VI_FUNC RGDM3KAttrMathNullOffset_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value )
{
    ViStatus    error = VI_SUCCESS;

    viCheckErr ( viPrintf ( io, ":calculate:rel:offset?\n" ) );
    viCheckErr ( viScanf ( io, "%Lf", value ) );

Error:
    return error;
}

static ViStatus _VI_FUNC RGDM3KAttrMathNullOffset_CheckCallback ( ViSession vi,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value )
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     mathOperation;

    checkErr ( Ivi_DefaultCheckCallbackViReal64 ( vi, VI_NULL,
               RGDM3K_ATTR_MATH_NULL_OFFSET,
               value ) );

    checkErr ( Ivi_GetAttributeViInt32 ( vi, VI_NULL, RGDM3K_ATTR_MATH_OPERATION, 0,
                                         &mathOperation ) );

    if ( mathOperation != RGDM3K_VAL_MATH_NULL )
        viCheckErrElab ( IVI_ERROR_INVALID_CONFIGURATION, "Math operation must "
                         "be set to null operation before accessing the null "
                         "offset." );

Error:
    return error;
}

static ViStatus _VI_FUNC RGDM3KAttrMathNullOffset_RangeTableCallback ( ViSession vi,
        ViConstString channelName,
        ViAttr attributeId,
        IviRangeTablePtr *rangeTablePtr )
{
    ViStatus            error = VI_SUCCESS;
    ViInt32             measFunc, model;
    IviRangeTablePtr    tblPtr = VI_NULL;

    checkErr ( Ivi_GetAttributeViInt32 ( vi, VI_NULL, RGDM3K_ATTR_FUNCTION, 0,
                                         &measFunc ) );

    switch ( measFunc ) {
        case RGDM3K_VAL_DC_VOLTS:
        case RGDM3K_VAL_AC_VOLTS:
            tblPtr = &attrMathLimit_Volts_RangeTable;
            break;

        case RGDM3K_VAL_DC_CURRENT:
        case RGDM3K_VAL_AC_CURRENT:
            tblPtr = &attrMathLimit_Current_RangeTable;
            break;

        case RGDM3K_VAL_2_WIRE_RES:
        case RGDM3K_VAL_4_WIRE_RES:
            checkErr ( Ivi_GetAttributeViInt32 ( vi, VI_NULL, RGDM3K_ATTR_MODEL, 0, &model ) );


            tblPtr = &attrMathLimit_Resist_RangeTable;



            break;

        case RGDM3K_VAL_FREQ:
            tblPtr = &attrMathLimit_Freq_RangeTable;
            break;

        case RGDM3K_VAL_PERIOD:
            tblPtr = &attrMathLimit_Period_RangeTable;
            break;

        case RGDM3K_VAL_CAPACITANCE:
            tblPtr = &attrMathLimit_CAP_RangeTable;
            break;

        default:
            viCheckErrElab ( IVI_ERROR_INVALID_CONFIGURATION, "You cannot access "
                             "the null operation's offset in the "
                             "selected measurement function." );
            break;
    }

Error:
    *rangeTablePtr = tblPtr;
    return error;
}




static ViStatus _VI_FUNC RGDM3KAttrMathDbReference_CheckCallback ( ViSession vi,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value )
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     mathOperation;

    checkErr ( Ivi_DefaultCheckCallbackViReal64 ( vi, VI_NULL,
               RGDM3K_ATTR_MATH_DB_REFERENCE,
               value ) );

    checkErr ( Ivi_GetAttributeViInt32 ( vi, VI_NULL, RGDM3K_ATTR_MATH_OPERATION, 0,
                                         &mathOperation ) );

    if ( mathOperation != RGDM3K_VAL_MATH_DB )
        viCheckErrElab ( IVI_ERROR_INVALID_CONFIGURATION, "Math operation must "
                         "be set to dB operation before accessing the dB "
                         "relative reference." );

Error:
    return error;
}

static ViStatus _VI_FUNC RGDM3KAttrMathDbReference_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value )
{
    ViStatus    error = VI_SUCCESS;

    viCheckErr ( viPrintf ( io, ":calculate:DB:reference?\n" ) );
    viCheckErr ( viScanf ( io, "%Lf", value ) );

Error:
    return error;
}

static ViStatus _VI_FUNC RGDM3KAttrMathDbReference_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value )
{
    ViStatus    error = VI_SUCCESS;

    viCheckErr ( viPrintf ( io, ":calculate:DB:reference %0.lf\n", value ) );
    /* No longer need to invalidated since the user has set the dB reference. */
    checkErr ( Ivi_SetAttributeViBoolean ( vi, "", RGDM3K_ATTR_INVALIDATE_DB_REFERENCE, 0, VI_FALSE ) );

Error:
    return error;
}

static ViStatus _VI_FUNC RGDM3KAttrMathDbmReference_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViInt32 *value )
{
    ViStatus    error = VI_SUCCESS;
    ViReal64    result;

    viCheckErr ( viPrintf ( io, ":calculate:DBM:reference?\n" ) );
    viCheckErr ( viScanf ( io, "%Lf", &result ) );
    *value = ( ViInt32 ) result;

Error:
    return error;
}

static ViStatus _VI_FUNC RGDM3KAttrMathDbmReference_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViInt32 value )
{
    ViStatus    error = VI_SUCCESS;

    viCheckErr ( viPrintf ( io, ":calculate:DBM:reference %ld\n", value ) );

Error:
    return error;
}

static ViStatus _VI_FUNC RGDM3KAttrMathLimitUpper_RangeTableCallback ( ViSession vi,
        ViConstString channelName,
        ViAttr attributeId,
        IviRangeTablePtr *rangeTablePtr )
{
    ViStatus            error = VI_SUCCESS;
    ViInt32             measFunc;
    IviRangeTablePtr    tblPtr = VI_NULL;

    checkErr ( Ivi_GetAttributeViInt32 ( vi, VI_NULL, RGDM3K_ATTR_FUNCTION, 0,
                                         &measFunc ) );

    switch ( measFunc ) {
        case RGDM3K_VAL_DC_VOLTS:
        case RGDM3K_VAL_AC_VOLTS:
            tblPtr = &attrMathLimit_Volts_RangeTable;
            break;

        case RGDM3K_VAL_DC_CURRENT:
        case RGDM3K_VAL_AC_CURRENT:
            tblPtr = &attrMathLimit_Current_RangeTable;
            break;

        case RGDM3K_VAL_2_WIRE_RES:
        case RGDM3K_VAL_4_WIRE_RES:


            tblPtr = &attrMathLimit_Resist_RangeTable;

            break;

        case RGDM3K_VAL_FREQ:
            tblPtr = &attrMathLimit_Freq_RangeTable;
            break;

        case RGDM3K_VAL_PERIOD:
            tblPtr = &attrMathLimit_Period_RangeTable;
            break;

        case RGDM3K_VAL_CAPACITANCE:
            tblPtr = &attrMathLimit_CAP_RangeTable;
            break;

        default:
            viCheckErrElab ( IVI_ERROR_INVALID_CONFIGURATION, "You cannot access "
                             "the limit operation's upper limit in the "
                             "selected measurement function." );
            break;
    }

Error:
    *rangeTablePtr = tblPtr;
    return error;
}

static ViStatus _VI_FUNC RGDM3KAttrMathLimitUpper_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value )
{
    ViStatus    error = VI_SUCCESS;

    viCheckErr ( viPrintf ( io, ":calculate:pf:upper?\n" ) );
    RGDM3K_delaytime();
    viCheckErr ( viScanf ( io, "%Lf", value ) );

Error:
    return error;
}

static ViStatus _VI_FUNC RGDM3KAttrMathLimitUpper_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value )
{
    ViStatus    error = VI_SUCCESS;

    viCheckErr ( viPrintf ( io, ":calculate:pf:upper %.10Lf\n", value ) );
    RGDM3K_delaytime();

Error:
    return error;
}

static ViStatus _VI_FUNC RGDM3KAttrMathLimitLower_RangeTableCallback ( ViSession vi,
        ViConstString channelName,
        ViAttr attributeId,
        IviRangeTablePtr *rangeTablePtr )
{
    ViStatus            error = VI_SUCCESS;
    ViInt32             measFunc;//, model;
    IviRangeTablePtr    tblPtr = VI_NULL;

//	checkErr( Ivi_GetAttributeViInt32 (vi, VI_NULL, RGDM3K_ATTR_MODEL, 0, &model));
    checkErr ( Ivi_GetAttributeViInt32 ( vi, VI_NULL, RGDM3K_ATTR_FUNCTION, 0,
                                         &measFunc ) );

    switch ( measFunc ) {
        case RGDM3K_VAL_DC_VOLTS:
        case RGDM3K_VAL_AC_VOLTS:
            tblPtr = &attrMathLimit_Volts_RangeTable;
            break;

        case RGDM3K_VAL_DC_CURRENT:
        case RGDM3K_VAL_AC_CURRENT:
            tblPtr = &attrMathLimit_Current_RangeTable;
            break;

        case RGDM3K_VAL_2_WIRE_RES:
        case RGDM3K_VAL_4_WIRE_RES:

            tblPtr = &attrMathLimit_Resist_RangeTable;

            break;

        case RGDM3K_VAL_FREQ:
            tblPtr = &attrMathLimit_Freq_RangeTable;
            break;

        case RGDM3K_VAL_PERIOD:
            tblPtr = &attrMathLimit_Period_RangeTable;
            break;

        case RGDM3K_VAL_CAPACITANCE:
            tblPtr = &attrMathLimit_CAP_RangeTable;
            break;

        default:
            viCheckErrElab ( IVI_ERROR_INVALID_CONFIGURATION, "You cannot access "
                             "the limit operation's upper limit in the "
                             "selected measurement function." );
            break;
    }

Error:
    *rangeTablePtr = tblPtr;
    return error;
}

static ViStatus _VI_FUNC RGDM3KAttrMathLimitLower_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 *value )
{
    ViStatus    error = VI_SUCCESS;

    viCheckErr ( viPrintf ( io, ":calculate:pf:lower?\n" ) );
    RGDM3K_delaytime();
    viCheckErr ( viScanf ( io, "%Lf", value ) );

Error:
    return error;
}

static ViStatus _VI_FUNC RGDM3KAttrMathLimitLower_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViReal64 value )
{
    ViStatus    error = VI_SUCCESS;

    viCheckErr ( viPrintf ( io, ":calculate:pf:lower %.10Lf\n", value ) );
    RGDM3K_delaytime();

Error:
    return error;
}


static ViStatus _VI_FUNC RGDM3KAttrTriggerSlope_ReadCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViInt32 *value )
{
    ViStatus	error = VI_SUCCESS;
    ViChar   rdBuffer[BUFFER_SIZE];

    viCheckErr ( viPrintf ( io, ":trigger:vmcomplete:polar?\n" ) );
    RGDM3K_delaytime();
    viCheckErr ( viScanf ( io, "%s", rdBuffer ) );

    viCheckErr ( Ivi_GetViInt32EntryFromString ( rdBuffer, &attrTriggerSlopeRangeTable,
                 value, VI_NULL, VI_NULL, VI_NULL,
                 VI_NULL ) );

Error:
    return error;
}

static ViStatus _VI_FUNC RGDM3KAttrTriggerSlope_WriteCallback ( ViSession vi,
        ViSession io,
        ViConstString channelName,
        ViAttr attributeId,
        ViInt32 value )
{
    ViStatus	error = VI_SUCCESS;
    ViString cmd;

    viCheckErr ( Ivi_GetViInt32EntryFromValue ( value, &attrTriggerSlopeRangeTable, VI_NULL,
                 VI_NULL, VI_NULL, VI_NULL, &cmd, VI_NULL ) );
    viCheckErr ( viPrintf ( io, ":trigger:vmcomplete:polar %s\n", cmd ) );
    RGDM3K_delaytime();

Error:
    return error;
}


/*****************************************************************************
 * Function: RGDM3K_InitAttributes
 * Purpose:  This function adds attributes to the IVI session, initializes
 *           instrument attributes, and sets attribute invalidation
 *           dependencies.
 *****************************************************************************/
static ViStatus RGDM3K_InitAttributes ( ViSession vi, ViInt32 modelNum )
{
    ViStatus    error = 0;

    /*- Initialize instrument attributes --------------------------------*/

    checkErr ( Ivi_SetAttributeViInt32 ( vi, VI_NULL,
                                         RGDM3K_ATTR_SPECIFIC_DRIVER_MAJOR_VERSION, 0,
                                         RGDM3K_MAJOR_VERSION ) );
    checkErr ( Ivi_SetAttributeViInt32 ( vi, VI_NULL,
                                         RGDM3K_ATTR_SPECIFIC_DRIVER_MINOR_VERSION, 0,
                                         RGDM3K_MINOR_VERSION ) );
    checkErr ( Ivi_SetAttrReadCallbackViString ( vi, RGDM3K_ATTR_SPECIFIC_DRIVER_REVISION,
               RGDM3KAttrSpecificDriverRevision_ReadCallback ) );
    checkErr ( Ivi_SetAttributeViAddr ( vi, VI_NULL, IVI_ATTR_OPC_CALLBACK, 0,
                                        VI_NULL ) );
    checkErr ( Ivi_SetAttributeViBoolean ( vi, VI_NULL, IVI_ATTR_SUPPORTS_WR_BUF_OPER_MODE,
                                           0, VI_TRUE ) );
    checkErr ( Ivi_SetAttributeViString ( vi, VI_NULL,
                                          RGDM3K_ATTR_GROUP_CAPABILITIES, 0,
                                          "IviDmmBase,"
                                          "IviDmmACMeasurement,"
                                          "IviDmmFrequencyMeasurement,"
                                          //"IviDmmMultiPoint,"
                                          "IviDmmSoftwareTrigger,"
                                          "IviDmmDeviceInfo,"
                                          "IviDmmAutoRangeValue,"
                                          "IviDmmAutoZero" ) );
    checkErr ( Ivi_SetAttributeViInt32 ( vi, "", RGDM3K_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MAJOR_VERSION,
                                         0, RGDM3K_CLASS_SPEC_MAJOR_VERSION ) );
    checkErr ( Ivi_SetAttributeViInt32 ( vi, "", RGDM3K_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MINOR_VERSION,
                                         0, RGDM3K_CLASS_SPEC_MINOR_VERSION ) );
    checkErr ( Ivi_SetAttributeViString ( vi, "", RGDM3K_ATTR_IO_SESSION_TYPE,
                                          0, RGDM3K_IO_SESSION_TYPE ) );
    checkErr ( Ivi_SetAttributeViString ( vi, "", RGDM3K_ATTR_SUPPORTED_INSTRUMENT_MODELS,
                                          0, RGDM3K_SUPPORTED_INSTRUMENT_MODELS ) );
    checkErr ( Ivi_SetAttrReadCallbackViString ( vi, RGDM3K_ATTR_INSTRUMENT_FIRMWARE_REVISION,
               RGDM3KAttrInstrumentFirmwareRevision_ReadCallback ) );
    checkErr ( Ivi_SetAttrReadCallbackViString ( vi, RGDM3K_ATTR_INSTRUMENT_MANUFACTURER,
               RGDM3KAttrInstrumentManufacturer_ReadCallback ) );
    checkErr ( Ivi_SetAttrReadCallbackViString ( vi, RGDM3K_ATTR_INSTRUMENT_MODEL,
               RGDM3KAttrInstrumentModel_ReadCallback ) );
    checkErr ( Ivi_SetAttributeViString ( vi, "", RGDM3K_ATTR_SPECIFIC_DRIVER_VENDOR,
                                          0, RGDM3K_DRIVER_VENDOR ) );
    checkErr ( Ivi_SetAttributeViString ( vi, "", RGDM3K_ATTR_SPECIFIC_DRIVER_DESCRIPTION,
                                          0, RGDM3K_DRIVER_DESCRIPTION ) );
    checkErr ( Ivi_SetAttributeFlags ( vi, RGDM3K_ATTR_INSTRUMENT_FIRMWARE_REVISION,
                                       IVI_VAL_USE_CALLBACKS_FOR_SIMULATION | IVI_VAL_NOT_USER_WRITABLE ) );
    checkErr ( Ivi_SetAttributeFlags ( vi, RGDM3K_ATTR_INSTRUMENT_MANUFACTURER,
                                       IVI_VAL_USE_CALLBACKS_FOR_SIMULATION | IVI_VAL_NOT_USER_WRITABLE ) );
    checkErr ( Ivi_SetAttributeFlags ( vi, RGDM3K_ATTR_INSTRUMENT_MODEL,
                                       IVI_VAL_USE_CALLBACKS_FOR_SIMULATION | IVI_VAL_NOT_USER_WRITABLE ) );

    /*- Add instrument-specific attributes ------------------------------*/
    checkErr ( Ivi_AddAttributeViInt32 ( vi, RGDM3K_ATTR_FUNCTION,
                                         "RGDM3K_ATTR_FUNCTION",
                                         RGDM3K_VAL_DC_VOLTS, 0,
                                         RGDM3KAttrFunction_ReadCallback,
                                         RGDM3KAttrFunction_WriteCallback,
                                         &attrFunctionRangeTableForRGDM3K ) );

    checkErr ( Ivi_AddAttributeViReal64 ( vi, RGDM3K_ATTR_RANGE, "RGDM3K_ATTR_RANGE",
                                          RGDM3K_VAL_AUTO_RANGE_ON, 0,
                                          RGDM3KAttrRange_ReadCallback,
                                          RGDM3KAttrRange_WriteCallback, VI_NULL, 0 ) );


    checkErr ( Ivi_AddAttributeViReal64 ( vi, RGDM3K_ATTR_RESOLUTION_ABSOLUTE,
                                          "RGDM3K_ATTR_RESOLUTION_ABSOLUTE", 0.0001, 0,
                                          RGDM3KAttrResolutionAbsolute_ReadCallback,
                                          RGDM3KAttrResolutionAbsolute_WriteCallback,
                                          &attrResolutionDCRangeTableForRGDM3K, 0 ) );


    checkErr ( Ivi_AddAttributeViInt32 ( vi, RGDM3K_ATTR_TRIGGER_SOURCE,
                                         "RGDM3K_ATTR_TRIGGER_SOURCE",
                                         RGDM3K_VAL_IMMEDIATE, 0,
                                         RGDM3KAttrTriggerSource_ReadCallback,
                                         RGDM3KAttrTriggerSource_WriteCallback,
                                         &attrTriggerSourceRangeTableForRGDM3K ) );


    checkErr ( Ivi_AddAttributeViReal64 ( vi, RGDM3K_ATTR_AC_MAX_FREQ,
                                          "RGDM3K_ATTR_AC_MAX_FREQ", 30000.0, 0,
                                          RGDM3KAttrAcMaxFreq_ReadCallback, VI_NULL,
                                          &attrAcMaxFreqTable1, 0 ) );
    checkErr ( Ivi_SetAttrRangeTableCallback ( vi, RGDM3K_ATTR_AC_MAX_FREQ,
               RGDM3KAttrAcMaxFreq_RangeTableCallback ) );


    checkErr ( Ivi_AddAttributeViReal64 ( vi, RGDM3K_ATTR_TRIGGER_DELAY,
                                          "RGDM3K_ATTR_TRIGGER_DELAY",
                                          RGDM3K_VAL_AUTO_DELAY_OFF, 0,
                                          RGDM3KAttrTriggerDelay_ReadCallback,
                                          RGDM3KAttrTriggerDelay_WriteCallback,
                                          &attrTriggerDelayRangeTable, 0 ) );


    checkErr ( Ivi_AddAttributeViString ( vi, RGDM3K_ATTR_ID_QUERY_RESPONSE,
                                          "RGDM3K_ATTR_ID_QUERY_RESPONSE",
                                          "HEWLETT-PACKARD,3058,0",
                                          IVI_VAL_NOT_USER_WRITABLE,
                                          RGDM3KAttrIdQueryResponse_ReadCallback,
                                          VI_NULL ) );



    checkErr ( Ivi_AddAttributeViReal64 ( vi, RGDM3K_ATTR_RANGE_VDC,
                                          "RGDM3K_ATTR_RANGE_VDC",
                                          RGDM3K_VAL_AUTO_RANGE_ON, IVI_VAL_HIDDEN,
                                          RGDM3KAttrRangeVdc_ReadCallback,
                                          RGDM3KAttrRangeVdc_WriteCallback,
                                          &VDC_RangeTable, 0 ) );

    checkErr ( Ivi_AddAttributeViReal64 ( vi, RGDM3K_ATTR_RANGE_VAC,
                                          "RGDM3K_ATTR_RANGE_VAC",
                                          RGDM3K_VAL_AUTO_RANGE_ON, IVI_VAL_HIDDEN,
                                          RGDM3KAttrRangeVac_ReadCallback,
                                          RGDM3KAttrRangeVac_WriteCallback,
                                          &VAC_RangeTable, 0 ) );

    checkErr ( Ivi_AddAttributeViReal64 ( vi, RGDM3K_ATTR_RANGE_ADC,
                                          "RGDM3K_ATTR_RANGE_ADC",
                                          RGDM3K_VAL_AUTO_RANGE_ON, IVI_VAL_HIDDEN,
                                          RGDM3KAttrRangeAdc_ReadCallback,
                                          RGDM3KAttrRangeAdc_WriteCallback,
                                          &ADC_RangeTable, 0 ) );

    checkErr ( Ivi_AddAttributeViReal64 ( vi, RGDM3K_ATTR_RANGE_AAC,
                                          "RGDM3K_ATTR_RANGE_AAC",
                                          RGDM3K_VAL_AUTO_RANGE_ON, IVI_VAL_HIDDEN,
                                          RGDM3KAttrRangeAac_ReadCallback,
                                          RGDM3KAttrRangeAac_WriteCallback,
                                          &AAC_RangeTable, 0 ) );

    checkErr ( Ivi_AddAttributeViReal64 ( vi, RGDM3K_ATTR_RANGE_RES,
                                          "RGDM3K_ATTR_RANGE_RES",
                                          RGDM3K_VAL_AUTO_RANGE_ON, IVI_VAL_HIDDEN,
                                          RGDM3KAttrRangeRes_ReadCallback,
                                          RGDM3KAttrRangeRes_WriteCallback,
                                          &Ohms_RangeTable, 0 ) );



    checkErr ( Ivi_AddAttributeViReal64 ( vi, RGDM3K_ATTR_RANGE_CAP,
                                          "RGDM3K_ATTR_RANGE_CAP",
                                          RGDM3K_VAL_AUTO_RANGE_ON, IVI_VAL_HIDDEN,
                                          RGDM3KAttrRangeCap_ReadCallback,
                                          RGDM3KAttrRangeCap_WriteCallback,
                                          &Cap_RangeTable, 0 ) );
    checkErr ( Ivi_AddAttributeViInt32 ( vi, RGDM3K_ATTR_MODEL,
                                         "RGDM3K_ATTR_MODEL",
                                         RGDM3K_VAL_MODEL_3058, 0, VI_NULL,
                                         VI_NULL, &attrModel ) );
    checkErr ( Ivi_AddAttributeViReal64 ( vi, RGDM3K_ATTR_FREQ_VOLTAGE_RANGE,
                                          "RGDM3K_ATTR_FREQ_VOLTAGE_RANGE", 10.0, 0,
                                          RGDM3KAttrFreqVoltageRange_ReadCallback,
                                          RGDM3KAttrFreqVoltageRange_WriteCallback,
                                          &attrThresholdVoltageRangeTable, 0 ) );

    checkErr ( Ivi_AddAttributeViReal64 ( vi, RGDM3K_ATTR_FREQ_THRESHOLD_VOLTAGE,
                                          "RGDM3K_ATTR_FREQ_THRESHOLD_VOLTAGE", 10,
                                          0,
                                          RGDM3KAttrFreqThresholdVoltage_ReadCallback,
                                          RGDM3KAttrFreqThresholdVoltage_WriteCallback,
                                          &attrThresholdVoltageRangeTable, 0 ) );

    checkErr ( Ivi_AddAttributeViReal64 ( vi, RGDM3K_ATTR_PERIOD_THRESHOLD_VOLTAGE,
                                          "RGDM3K_ATTR_PERIOD_THRESHOLD_VOLTAGE", 10,
                                          IVI_VAL_USE_CALLBACKS_FOR_SIMULATION,
                                          RGDM3KAttrPeriodThresholdVoltage_ReadCallback,
                                          RGDM3KAttrPeriodThresholdVoltage_WriteCallback,
                                          &attrThresholdVoltageRangeTable, 0 ) );

    checkErr ( Ivi_AddAttributeViReal64 ( vi, RGDM3K_ATTR_AC_MIN_FREQ,
                                          "RGDM3K_ATTR_AC_MIN_FREQ", 20.0, 0,
                                          RGDM3KAttrAcMinFreq_ReadCallback,
                                          RGDM3KAttrAcMinFreq_WriteCallback,
                                          &attrAcMinFreqTable1, 0 ) );
    checkErr ( Ivi_SetAttrRangeTableCallback ( vi, RGDM3K_ATTR_AC_MIN_FREQ,
               RGDM3KAttrAcMinFreq_RangeTableCallback ) );
    checkErr ( Ivi_AddAttributeViReal64 ( vi, RGDM3K_ATTR_RESOLUTION_VDC,
                                          "RGDM3K_ATTR_RESOLUTION_VDC", RGDM3K_VAL_DEF,
                                          IVI_VAL_NEVER_CACHE | IVI_VAL_HIDDEN,
                                          RGDM3KAttrResolutionVdc_ReadCallback,
                                          RGDM3KAttrResolutionVdc_WriteCallback,
                                          &attrResolutionDCRangeTableForRGDM3K, 0 ) );
    checkErr ( Ivi_AddAttributeViReal64 ( vi, RGDM3K_ATTR_RESOLUTION_FRES,
                                          "RGDM3K_ATTR_RESOLUTION_FRES", 0,
                                          IVI_VAL_NEVER_CACHE | IVI_VAL_HIDDEN,
                                          RGDM3KAttrResolutionFres_ReadCallback,
                                          RGDM3KAttrResolutionFres_WriteCallback,
                                          &attrResolutionDCRangeTableForRGDM3K, 0 ) );
    checkErr ( Ivi_AddAttributeViReal64 ( vi, RGDM3K_ATTR_RESOLUTION_ADC,
                                          "RGDM3K_ATTR_RESOLUTION_ADC", RGDM3K_VAL_DEF,
                                          IVI_VAL_NEVER_CACHE | IVI_VAL_HIDDEN,
                                          RGDM3KAttrResolutionAdc_ReadCallback,
                                          RGDM3KAttrResolutionAdc_WriteCallback,
                                          &attrResolutionDCRangeTableForRGDM3K, 0 ) );
    checkErr ( Ivi_AddAttributeViReal64 ( vi, RGDM3K_ATTR_RESOLUTION_CAP,
                                          "RGDM3K_ATTR_RESOLUTION_CAP", RGDM3K_VAL_DEF,
                                          IVI_VAL_NEVER_CACHE | IVI_VAL_HIDDEN,
                                          RGDM3KAttrResolutionCap_ReadCallback,
                                          RGDM3KAttrResolutionCap_WriteCallback,
                                          &attrResolutionDCRangeTableForRGDM3K, 0 ) );
    checkErr ( Ivi_AddAttributeViReal64 ( vi, RGDM3K_ATTR_RESOLUTION_RES,
                                          "RGDM3K_ATTR_RESOLUTION_RES", RGDM3K_VAL_DEF,
                                          IVI_VAL_NEVER_CACHE | IVI_VAL_HIDDEN,
                                          RGDM3KAttrResolutionRes_ReadCallback,
                                          RGDM3KAttrResolutionRes_WriteCallback,
                                          &attrResolutionDCRangeTableForRGDM3K, 0 ) );
    checkErr ( Ivi_AddAttributeViReal64 ( vi, RGDM3K_ATTR_RANGE_FREQ,
                                          "RGDM3K_ATTR_RANGE_FREQ",
                                          RGDM3K_VAL_AUTO_RANGE_ON, IVI_VAL_HIDDEN,
                                          RGDM3KAttrRangeFreq_ReadCallback,
                                          RGDM3KAttrRangeFreq_WriteCallback,
                                          &VAC_RangeTable, 0 ) );
    checkErr ( Ivi_AddAttributeViReal64 ( vi, RGDM3K_ATTR_RANGE_PERIOD,
                                          "RGDM3K_ATTR_RANGE_PERIOD",
                                          RGDM3K_VAL_AUTO_RANGE_ON, IVI_VAL_HIDDEN,
                                          RGDM3KAttrRangePeriod_ReadCallback,
                                          RGDM3KAttrRangePeriod_WriteCallback,
                                          &VAC_RangeTable, 0 ) );

    checkErr ( Ivi_AddAttributeViInt32 ( vi, RGDM3K_ATTR_MATH_OPERATION,
                                         "RGDM3K_ATTR_MATH_OPERATION", 0, 0,
                                         RGDM3KAttrMathOperation_ReadCallback,
                                         RGDM3KAttrMathOperation_WriteCallback,
                                         &attrMathOperationRangeTable ) );
    checkErr ( Ivi_AddAttributeViReal64 ( vi, RGDM3K_ATTR_MATH_DB_REFERENCE,
                                          "RGDM3K_ATTR_MATH_DB_REFERENCE", 0, 0,
                                          RGDM3KAttrMathDbReference_ReadCallback,
                                          RGDM3KAttrMathDbReference_WriteCallback,
                                          &attrMathDbReferenceRangeTable, 0 ) );
    checkErr ( Ivi_AddAttributeViInt32 ( vi, RGDM3K_ATTR_MATH_DBM_REFERENCE,
                                         "RGDM3K_ATTR_MATH_DBM_REFERENCE", 0, 0,
                                         RGDM3KAttrMathDbmReference_ReadCallback,
                                         RGDM3KAttrMathDbmReference_WriteCallback,
                                         &attrMathDbmReferenceRangeTable ) );
    checkErr ( Ivi_AddAttributeViReal64 ( vi, RGDM3K_ATTR_MATH_LIMIT_UPPER,
                                          "RGDM3K_ATTR_MATH_LIMIT_UPPER", 0.0,
                                          IVI_VAL_NEVER_CACHE,
                                          RGDM3KAttrMathLimitUpper_ReadCallback,
                                          RGDM3KAttrMathLimitUpper_WriteCallback,
                                          VI_NULL, 0 ) );
    checkErr ( Ivi_AddAttributeViReal64 ( vi, RGDM3K_ATTR_MATH_LIMIT_LOWER,
                                          "RGDM3K_ATTR_MATH_LIMIT_LOWER", 0.0,
                                          IVI_VAL_NEVER_CACHE,
                                          RGDM3KAttrMathLimitLower_ReadCallback,
                                          RGDM3KAttrMathLimitLower_WriteCallback,
                                          VI_NULL, 0 ) );
    checkErr ( Ivi_AddAttributeViReal64 ( vi, RGDM3K_ATTR_MATH_NULL_OFFSET,
                                          "RGDM3K_ATTR_MATH_NULL_OFFSET", 0.0, 0,
                                          RGDM3KAttrMathNullOffset_ReadCallback,
                                          RGDM3KAttrMathNullOffset_WriteCallback,
                                          VI_NULL, 0 ) );

    checkErr ( Ivi_AddAttributeViBoolean ( vi, RGDM3K_ATTR_INVALIDATE_NULL_OFFSET,
                                           "RGDM3K_ATTR_INVALIDATE_NULL_OFFSET",
                                           VI_FALSE,
                                           IVI_VAL_DONT_CHECK_STATUS | IVI_VAL_HIDDEN,
                                           VI_NULL, VI_NULL ) );

    checkErr ( Ivi_AddAttributeViBoolean ( vi, RGDM3K_ATTR_INVALIDATE_DB_REFERENCE,
                                           "RGDM3K_ATTR_INVALIDATE_DB_REFERENCE",
                                           VI_FALSE,
                                           IVI_VAL_DONT_CHECK_STATUS | IVI_VAL_HIDDEN,
                                           VI_NULL, VI_NULL ) );



    checkErr ( Ivi_AddAttributeViInt32 ( vi, RGDM3K_ATTR_TRIGGER_SLOPE,
                                         "RGDM3K_ATTR_TRIGGER_SLOPE",
                                         RGDM3K_VAL_NEGATIVE, 0,
                                         RGDM3KAttrTriggerSlope_ReadCallback,
                                         RGDM3KAttrTriggerSlope_WriteCallback,
                                         &attrTriggerSlopeRangeTable ) );
    //checkErr (Ivi_AddAttributeViInt32 (vi, RGDM3K_ATTR_OUTPUT_TRIGGER_SLOPE,
    //                                   "RGDM3K_ATTR_OUTPUT_TRIGGER_SLOPE",
    //                                   RGDM3K_VAL_NEGATIVE, 0,
    //                                   RGDM3KAttrOutputTriggerSlope_ReadCallback,
    //                                   RGDM3KAttrOutputTriggerSlope_WriteCallback,
    //                                   &attrTriggerSlopeRangeTable));

    checkErr ( Ivi_AddAttributeViReal64 ( vi, RGDM3K_ATTR_RANGE_FRES,
                                          "RGDM3K_ATTR_RANGE_FRES",
                                          RGDM3K_VAL_AUTO_RANGE_ON, IVI_VAL_HIDDEN,
                                          RGDM3KAttrRangeFres_ReadCallback,
                                          RGDM3KAttrRangeFres_WriteCallback,
                                          &Ohms_RangeTable, 0 ) );
    checkErr ( Ivi_SetAttrRangeTableCallback ( vi, RGDM3K_ATTR_MATH_NULL_OFFSET,
               RGDM3KAttrMathNullOffset_RangeTableCallback ) );
    checkErr ( Ivi_SetAttrCheckCallbackViReal64 ( vi, RGDM3K_ATTR_MATH_NULL_OFFSET,
               RGDM3KAttrMathNullOffset_CheckCallback ) );
    checkErr ( Ivi_SetAttrRangeTableCallback ( vi, RGDM3K_ATTR_MATH_LIMIT_LOWER,
               RGDM3KAttrMathLimitLower_RangeTableCallback ) );
    checkErr ( Ivi_SetAttrRangeTableCallback ( vi, RGDM3K_ATTR_MATH_LIMIT_UPPER,
               RGDM3KAttrMathLimitUpper_RangeTableCallback ) );
    checkErr ( Ivi_SetAttrCheckCallbackViReal64 ( vi, RGDM3K_ATTR_MATH_DB_REFERENCE,
               RGDM3KAttrMathDbReference_CheckCallback ) );
    checkErr ( Ivi_SetAttrCheckCallbackViInt32 ( vi, RGDM3K_ATTR_MATH_OPERATION,
               RGDM3KAttrMathOperation_CheckCallback ) );

    if ( modelNum == RGDM3K_VAL_MODEL_3058 ) {

        checkErr ( Ivi_SetAttributeViInt32 ( vi, VI_NULL, RGDM3K_ATTR_MODEL, VI_NULL, RGDM3K_VAL_MODEL_3058 ) );
    }

    /*- Setup attribute invalidations -----------------------------------*/



    /*- Attribute adjustment to handle appropriate availability of ------*/

    checkErr ( RGDM3K_AdjustAttrsTriggerSourceRngTbl ( vi ) );

Error:
    return error;
}

static ViStatus RGDM3K_AdjustAttrsTriggerSourceRngTbl ( ViSession vi )
{
    ViStatus error = VI_SUCCESS;
    ViSession io;
    ViInt32 model;

    /*Range Table for Function and Trigger Source*/
    IviRangeTablePtr FunctionRngTblPtr, TrigSourRngTblPtr;

    /*Range Table for Range*/
    IviRangeTablePtr RangeResRngTblPtr, RangeAACRngTblPtr, RangeADCRngTblPtr, RangeVOLTRngTblPtr, RangeCapRngTblPtr;

    /*Range Table for Resolution*/
    IviRangeTablePtr ResolutionDCRngTblPtr;

    /*Range Table for Trigger Count, Sample Count and Sample Trigger*/
    //IviRangeTablePtr TriggerCountRngTblPtr, SampleCountRngTblPtr, SampleTriggerRngTblPtr;

    /*Range Table for Aperture Time*/
    //IviRangeTablePtr ApertureTimeRngTblPtr, ApertureTimeFreqRngTblPtr;

    /*Range Table for Voltage Threshold*/
    IviRangeTablePtr ThresholdVoltageRngTblPtr;

    /*Initialize Range Tables*/
    TrigSourRngTblPtr = &attrTriggerSourceRangeTableForRGDM3K;
    FunctionRngTblPtr = &attrFunctionRangeTableForRGDM3K;

    RangeResRngTblPtr =  &Ohms_RangeTable;
    RangeAACRngTblPtr = &AAC_RangeTable;
    RangeADCRngTblPtr = &ADC_RangeTable;
    RangeVOLTRngTblPtr = &VDC_RangeTable;
    RangeCapRngTblPtr = &Cap_RangeTable;

    ResolutionDCRngTblPtr = &attrResolutionDCRangeTableForRGDM3K;

    ThresholdVoltageRngTblPtr = &attrThresholdVoltageRangeTable;

    checkErr ( Ivi_LockSession ( vi, VI_NULL ) );

    io = Ivi_IOSession ( vi );

    checkErr ( Ivi_GetAttributeViInt32 ( vi, VI_NULL, RGDM3K_ATTR_MODEL,
                                         0, &model ) );

    /*Model: 3058*/
    if ( model == RGDM3K_VAL_MODEL_3058 ) {
        /* Delete Attributes not applicable in the model */

        //checkErr(Ivi_DeleteAttribute (vi, RGDM3K_ATTR_TEMP_TRANSDUCER_TYPE));
        //checkErr(Ivi_DeleteAttribute (vi, RGDM3K_ATTR_TEMP_THERMISTOR_RES));
        //checkErr(Ivi_DeleteAttribute (vi, RGDM3K_ATTR_TEMP_RTD_ALPHA));
        //checkErr(Ivi_DeleteAttribute (vi, RGDM3K_ATTR_TEMP_RTD_RES));
        //checkErr(Ivi_DeleteAttribute (vi, RGDM3K_ATTR_TEMP_RTD_COMPENSATION_ENABLED));
        //checkErr(Ivi_DeleteAttribute (vi, RGDM3K_ATTR_RES_OFFSET_COMPENSATION_ENABLED));

        //checkErr(Ivi_DeleteAttribute (vi, RGDM3K_ATTR_TRIGGER_SLOPE));
        //checkErr(Ivi_DeleteAttribute (vi, RGDM3K_ATTR_OUTPUT_TRIGGER_SLOPE));
        //checkErr(Ivi_DeleteAttribute (vi, RGDM3K_ATTR_RESOLUTION_CAP));
    }



    else {
        viCheckErrElab ( RGDM3K_ERROR_INVALID_MODEL_TYPE, "The instrument is not RGDM3K" );
    }

    /*Set the Range table for attribute*/
    checkErr ( Ivi_SetStoredRangeTablePtr ( vi, RGDM3K_ATTR_FUNCTION, FunctionRngTblPtr ) );
    checkErr ( Ivi_SetStoredRangeTablePtr ( vi, RGDM3K_ATTR_TRIGGER_SOURCE, TrigSourRngTblPtr ) );

    checkErr ( Ivi_SetStoredRangeTablePtr ( vi, RGDM3K_ATTR_RANGE_FRES, RangeResRngTblPtr ) );
    checkErr ( Ivi_SetStoredRangeTablePtr ( vi, RGDM3K_ATTR_RANGE_RES, RangeResRngTblPtr ) );
    checkErr ( Ivi_SetStoredRangeTablePtr ( vi, RGDM3K_ATTR_RANGE_AAC, RangeAACRngTblPtr ) );
    checkErr ( Ivi_SetStoredRangeTablePtr ( vi, RGDM3K_ATTR_RANGE_ADC, RangeADCRngTblPtr ) );
    checkErr ( Ivi_SetStoredRangeTablePtr ( vi, RGDM3K_ATTR_RANGE_VAC, RangeVOLTRngTblPtr ) );
    checkErr ( Ivi_SetStoredRangeTablePtr ( vi, RGDM3K_ATTR_RANGE_VDC, RangeVOLTRngTblPtr ) );
    checkErr ( Ivi_SetStoredRangeTablePtr ( vi, RGDM3K_ATTR_RANGE_CAP, RangeCapRngTblPtr ) );

    checkErr ( Ivi_SetStoredRangeTablePtr ( vi, RGDM3K_ATTR_RESOLUTION_VDC, ResolutionDCRngTblPtr ) );
    checkErr ( Ivi_SetStoredRangeTablePtr ( vi, RGDM3K_ATTR_RESOLUTION_ADC, ResolutionDCRngTblPtr ) );
    checkErr ( Ivi_SetStoredRangeTablePtr ( vi, RGDM3K_ATTR_RESOLUTION_FRES, ResolutionDCRngTblPtr ) );
    checkErr ( Ivi_SetStoredRangeTablePtr ( vi, RGDM3K_ATTR_RESOLUTION_RES, ResolutionDCRngTblPtr ) );

    checkErr ( Ivi_SetStoredRangeTablePtr ( vi, RGDM3K_ATTR_FREQ_VOLTAGE_RANGE, ThresholdVoltageRngTblPtr ) );
    checkErr ( Ivi_SetStoredRangeTablePtr ( vi, RGDM3K_ATTR_FREQ_THRESHOLD_VOLTAGE, ThresholdVoltageRngTblPtr ) );
    checkErr ( Ivi_SetStoredRangeTablePtr ( vi, RGDM3K_ATTR_PERIOD_THRESHOLD_VOLTAGE, ThresholdVoltageRngTblPtr ) );





Error:
    Ivi_UnlockSession ( vi, VI_NULL );
    return error;
}


/*****************************************************************************
 *  Function: RGDM3K_GetDriverSetupOption
 *  Purpose:  Get the value of a DriverSetup option.  The driver setup string
 *            is part of the option string the user passes to the
 *            xx_InitWithOptions function.  Specifically it is the part
 *            that follows "DriverSetup=".  This function assumes the
 *            DriverSetup string has the following format:
 *              "OptionTag1 : OptionValue1; OptionTag2 : OptionValue2"
 *            Leading and trailing white space is removed from the option
 *            value and both the option tag and option value may contain
 *            white space.  The option tag is case sensitive.
 *
 *            Example:
 *
 *              "DriverSetup= Model : InstrX ; Special Option:Feature Y "
 *
 *              optionTag "Model"          returns optionValue "InstrX"
 *
 *              optionTag "Special Option" returns optionValue "Feature Y"
 *****************************************************************************/
static ViStatus RGDM3K_GetDriverSetupOption ( ViSession vi,
        ViConstString optionTag,
        ViInt32 optionValueSize,
        ViChar optionValue[] )
{
    ViStatus error = VI_SUCCESS;
    ViChar   setup[BUFFER_SIZE];
    ViChar*  searchOrigin;
    ViChar*  start;
    ViChar*  end;
    ViInt32  n;

    checkErr ( Ivi_LockSession ( vi, VI_NULL ) );

    if ( ( optionValue != VI_NULL ) && ( optionValueSize > 0 ) )
    { optionValue[0] = 0; }

    if ( ( optionTag != VI_NULL ) && ( optionValue != VI_NULL ) && ( optionValueSize > 1 ) ) {
        checkErr ( Ivi_GetAttributeViString ( vi, "", IVI_ATTR_DRIVER_SETUP,
                                              0, BUFFER_SIZE, setup ) );
        searchOrigin = setup;

        while ( searchOrigin != VI_NULL ) {
            searchOrigin = strstr ( searchOrigin, optionTag );

            if ( searchOrigin ) {
                searchOrigin += strlen ( optionTag );
                start = searchOrigin;

                while ( isspace ( *start ) ) { start++; }

                if ( *start == ':' ) {
                    start++;

                    while ( isspace ( *start ) ) { start++; }

                    if ( *start ) {
                        end = strchr ( start, ';' );

                        if ( start != end ) {
                            end = ( end ) ? --end : &setup[strlen ( setup ) - 1];

                            while ( isspace ( *end ) ) { end--; }

                            n = ( ViInt32 ) ( end - start + 1 );

                            if ( n > optionValueSize - 1 )
                            { n = optionValueSize - 1; }

                            strncpy ( optionValue, start, n );
                            optionValue[n] = 0;
                            break;
                        }
                    }
                }
            }
        }
    }

Error:
    Ivi_UnlockSession ( vi, VI_NULL );
    return error;
}


/*****************************************************************************
 *------------------- End Instrument Driver Source Code ---------------------*
 *****************************************************************************/



int listResources ( char *instrDescr )
{
    ViStatus    res;
    ViSession    rm;

    // Initialize and get a handle on the Resource Manager
    res = viOpenDefaultRM ( &rm );

    if ( res == VI_SUCCESS ) {
        {

            // Initialize and get a handle on the Resource Manager
            res = viOpenDefaultRM ( &rm );

            if ( res == VI_SUCCESS ) {

                // Initialize and get a handle on the Resource Manager
                res = viOpenDefaultRM ( &rm );

                if ( res == VI_SUCCESS ) {
                    // See what instruments we have connected
                    ViFindList    findList;
                    ViUInt32    retcnt;
                    //char        instrDescr[VI_FIND_BUFLEN];

                    // Find the first resource
                    res = viFindRsrc ( rm, ( ViString ) "?*:0x1AB1::0x09C4:?*", &findList, &retcnt, instrDescr );

                    if ( res == VI_SUCCESS ) {
                        _RPT1 ( _CRT_WARN, "%s\n", instrDescr );

                        while ( --retcnt ) {
                            // Get the next resource
                            res = viFindNext ( findList, instrDescr );

                            if ( res != VI_SUCCESS ) {
                                break;
                            }

                            _RPT1 ( _CRT_WARN, "%s\n", instrDescr );
                        }

                        viClose ( rm );
                        return 0;
                    }

                }

                viClose ( rm );
                return -1;
            }
        }
    }

    return -1;
}