/****************************************************************************
 *                       RIGOL rgdm3k Digital Multimeter
 *---------------------------------------------------------------------------
 *   2008-2013, RIGOL Technologies,Inc.  All Rights Reserved.
 *---------------------------------------------------------------------------
 *
 * Title:    rgdm3k.h
 * Purpose: RIGOL rgdm3k Digital Multimeter
 *           instrument driver declarations.
 *
 ****************************************************************************/





#ifndef __RGDM3K_HEADER
#define __RGDM3K_HEADER

#include <ivi.h>
#include <ividmm.h>

#if defined(__cplusplus) || defined(__cplusplus__)
extern "C" {
#endif

/****************************************************************************
 *----------------- Instrument Driver Revision Information -----------------*
 ****************************************************************************/
#define RGDM3K_MAJOR_VERSION         	  1     /* Instrument driver major version */
#define RGDM3K_MINOR_VERSION        	  2	  /* Instrument driver minor version */

#define RGDM3K_CLASS_SPEC_MAJOR_VERSION   3     /* Class specification major version */
#define RGDM3K_CLASS_SPEC_MINOR_VERSION   0     /* Class specification minor version */

#define RGDM3K_SUPPORTED_INSTRUMENT_MODELS  "3058"

#define RGDM3K_DRIVER_VENDOR                "RIGOL"
#ifdef _IVI_mswin64_
#define RGDM3K_DRIVER_DESCRIPTION           "RIGOL DM3058 Digital Multimeters [Compiled for 64-bit.]"
#else
#define RGDM3K_DRIVER_DESCRIPTION           "RIGOL DM3058 Digital Multimeters"
#endif
#define RGDM3K_IO_SESSION_TYPE              "NI-VISA"

/****************************************************************************
 *---------------------------- Attribute Defines ---------------------------*
 ****************************************************************************/

/*- IVI Inherent Instrument Attributes ---------------------------------*/

/* User Options */
#define RGDM3K_ATTR_RANGE_CHECK                   IVI_ATTR_RANGE_CHECK                    /* ViBoolean */
#define RGDM3K_ATTR_QUERY_INSTRUMENT_STATUS       IVI_ATTR_QUERY_INSTR_STATUS             /* ViBoolean */
#define RGDM3K_ATTR_CACHE                         IVI_ATTR_CACHE                          /* ViBoolean */
#define RGDM3K_ATTR_SIMULATE                      IVI_ATTR_SIMULATE                       /* ViBoolean */
#define RGDM3K_ATTR_RECORD_COERCIONS              IVI_ATTR_RECORD_COERCIONS               /* ViBoolean */

/* Instrument Capabilities */
#define RGDM3K_ATTR_CHANNEL_COUNT                 IVI_ATTR_NUM_CHANNELS                   /* ViInt32,  read-only  */
#define RGDM3K_ATTR_GROUP_CAPABILITIES            IVI_ATTR_GROUP_CAPABILITIES             /* ViString, read-only */

/* Driver Information  */
#define RGDM3K_ATTR_SPECIFIC_DRIVER_PREFIX        IVI_ATTR_SPECIFIC_DRIVER_PREFIX         /* ViString, read-only  */
#define RGDM3K_ATTR_SUPPORTED_INSTRUMENT_MODELS   IVI_ATTR_SUPPORTED_INSTRUMENT_MODELS    /* ViString, read-only  */
#define RGDM3K_ATTR_INSTRUMENT_MANUFACTURER       IVI_ATTR_INSTRUMENT_MANUFACTURER        /* ViString, read-only  */
#define RGDM3K_ATTR_INSTRUMENT_MODEL              IVI_ATTR_INSTRUMENT_MODEL               /* ViString, read-only  */
#define RGDM3K_ATTR_INSTRUMENT_FIRMWARE_REVISION  IVI_ATTR_INSTRUMENT_FIRMWARE_REVISION   /* ViString, read-only  */
#define RGDM3K_ATTR_SPECIFIC_DRIVER_REVISION      IVI_ATTR_SPECIFIC_DRIVER_REVISION       /* ViString, read-only  */
#define RGDM3K_ATTR_SPECIFIC_DRIVER_VENDOR        IVI_ATTR_SPECIFIC_DRIVER_VENDOR         /* ViString, read-only  */
#define RGDM3K_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MAJOR_VERSION	IVI_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MAJOR_VERSION /* ViInt32, read-only */
#define RGDM3K_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MINOR_VERSION	IVI_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MINOR_VERSION /* ViInt32, read-only */
#define RGDM3K_ATTR_SPECIFIC_DRIVER_DESCRIPTION   IVI_ATTR_SPECIFIC_DRIVER_DESCRIPTION    /* ViString, read-only  */
#define RGDM3K_ATTR_DRIVER_SETUP                  IVI_ATTR_DRIVER_SETUP
#define RGDM3K_ATTR_INTERCHANGE_CHECK             IVI_ATTR_INTERCHANGE_CHECK



/* Advanced Session Information */
#define RGDM3K_ATTR_LOGICAL_NAME                  IVI_ATTR_LOGICAL_NAME                   /* ViString, read-only  */
#define RGDM3K_ATTR_IO_RESOURCE_DESCRIPTOR        IVI_ATTR_RESOURCE_DESCRIPTOR            /* ViString, read-only  */

#define RGDM3K_ATTR_ID_QUERY_RESPONSE         	(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 01L)   /* ViString, read-only */
#define RGDM3K_ATTR_MODEL                 		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 02L)	/* ViInt32   */

/*- Basic Instrument Operation -----------------------------------------*/

#define RGDM3K_ATTR_FUNCTION                  	IVIDMM_ATTR_FUNCTION                    /* ViInt32   */
#define RGDM3K_ATTR_RANGE                     	IVIDMM_ATTR_RANGE                       /* ViReal64  */
#define RGDM3K_ATTR_RESOLUTION_ABSOLUTE       	IVIDMM_ATTR_RESOLUTION_ABSOLUTE         /* ViReal64  */
#define RGDM3K_ATTR_TRIGGER_SOURCE            	IVIDMM_ATTR_TRIGGER_SOURCE              /* ViInt32   */
#define RGDM3K_ATTR_TRIGGER_DELAY             	IVIDMM_ATTR_TRIGGER_DELAY               /* ViReal64  */
#define RGDM3K_ATTR_TRIGGER_SLOPE               IVIDMM_ATTR_TRIGGER_SLOPE
/*- AC Measurement Extension Group --------------------------------------*/
#define RGDM3K_ATTR_AC_MAX_FREQ               	IVIDMM_ATTR_AC_MAX_FREQ                 /* ViReal64  */
#define RGDM3K_ATTR_AC_MIN_FREQ                 IVIDMM_ATTR_AC_MIN_FREQ

/*- Trigger Slope Extension Group---------------------------------------*/
#define RGDM3K_ATTR_TRIGGER_SLOPE               IVIDMM_ATTR_TRIGGER_SLOPE




/*- Frequency Extension Group ------------------------------------------*/
#define RGDM3K_ATTR_FREQ_VOLTAGE_RANGE      IVIDMM_ATTR_FREQ_VOLTAGE_RANGE
#define RGDM3K_ATTR_FREQ_THRESHOLD_VOLTAGE    	(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 24L)   /* ViReal64  */
#define RGDM3K_ATTR_PERIOD_THRESHOLD_VOLTAGE  	(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 25L)   /* ViReal64  */
/*- Math Extension Group -----------------------------------------------*/
#define RGDM3K_ATTR_MATH_OPERATION          (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 26L)
#define RGDM3K_ATTR_MATH_DB_REFERENCE       (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 27L)
#define RGDM3K_ATTR_MATH_DBM_REFERENCE      (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 28L)
#define RGDM3K_ATTR_MATH_LIMIT_UPPER        (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 29L)
#define RGDM3K_ATTR_MATH_LIMIT_LOWER        (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 30L)
#define RGDM3K_ATTR_MATH_NULL_OFFSET        (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 31L)


/****************************************************************************
 *------------------------ Attribute Value Defines -------------------------*
 ****************************************************************************/

/*- Defined values for attribute RGDM3K_ATTR_FUNCTION -*/

#define RGDM3K_VAL_DC_VOLTS                   	IVIDMM_VAL_DC_VOLTS
#define RGDM3K_VAL_AC_VOLTS                   	IVIDMM_VAL_AC_VOLTS
#define RGDM3K_VAL_DC_CURRENT                 	IVIDMM_VAL_DC_CURRENT
#define RGDM3K_VAL_AC_CURRENT                 	IVIDMM_VAL_AC_CURRENT
#define RGDM3K_VAL_2_WIRE_RES                 	IVIDMM_VAL_2_WIRE_RES
#define RGDM3K_VAL_4_WIRE_RES                 	IVIDMM_VAL_4_WIRE_RES
#define RGDM3K_VAL_FREQ                       	IVIDMM_VAL_FREQ
#define RGDM3K_VAL_PERIOD                     	IVIDMM_VAL_PERIOD
#define RGDM3K_VAL_TEMPERATURE					IVIDMM_VAL_TEMPERATURE
#define RGDM3K_VAL_DIODE                      	(IVIDMM_VAL_FUNC_SPECIFIC_EXT_BASE + 1L)
#define RGDM3K_VAL_CONTINUITY                 	(IVIDMM_VAL_FUNC_SPECIFIC_EXT_BASE + 2L)
#define RGDM3K_VAL_CAPACITANCE					(IVIDMM_VAL_FUNC_SPECIFIC_EXT_BASE + 4L)

/*- Defined values for attribute RGDM3K_ATTR_RANGE -*/

#define RGDM3K_VAL_AUTO_RANGE_ON              	IVIDMM_VAL_AUTO_RANGE_ON
#define RGDM3K_VAL_AUTO_RANGE_OFF             	IVIDMM_VAL_AUTO_RANGE_OFF
#define RGDM3K_VAL_AUTO_RANGE_ONCE				IVIDMM_VAL_AUTO_RANGE_ONCE


/*- Defined values for attribute RGDM3K_ATTR_THRESHOLD_VOLTAGE -*/

#define RGDM3K_VAL_AUTO_THRESHOLD_ON          	IVIDMM_VAL_AUTO_RANGE_ON
#define RGDM3K_VAL_AUTO_THRESHOLD_OFF         	IVIDMM_VAL_AUTO_RANGE_OFF
#define RGDM3K_VAL_AUTO_THRESHOLD_ONCE			IVIDMM_VAL_AUTO_RANGE_ONCE

/*- Defined values for attribute RGDM3K_ATTR_TRIGGER_SOURCE -*/

#define RGDM3K_VAL_IMMEDIATE                  	IVIDMM_VAL_IMMEDIATE
#define RGDM3K_VAL_EXTERNAL                   	IVIDMM_VAL_EXTERNAL
#define RGDM3K_VAL_SOFTWARE_TRIG              	IVIDMM_VAL_SOFTWARE_TRIG



/*- Defined values for attribute RGDM3K_ATTR_TRIGGER_DELAY -*/

#define RGDM3K_VAL_AUTO_DELAY_ON              	IVIDMM_VAL_AUTO_DELAY_ON
#define RGDM3K_VAL_AUTO_DELAY_OFF             	IVIDMM_VAL_AUTO_DELAY_OFF


/*- Defined values for attribute RGDM3K_ATTR_MATH_OPERATION -*/

#define RGDM3K_VAL_MATH_OFF                   	0
#define RGDM3K_VAL_MATH_NULL                  	1
#define RGDM3K_VAL_MATH_DB                    	2
#define RGDM3K_VAL_MATH_DBM                   	3
#define RGDM3K_VAL_MATH_MIN_MAX               	4
#define RGDM3K_VAL_MATH_LIMIT                 	5



/****************************************************************************
 *------------------------------ Useful Macros -----------------------------*
 ****************************************************************************/

/* Define Models:  */
#define RGDM3K_VAL_MODEL_3058              	1


/* Define Trigger Slope */
#define RGDM3K_VAL_POSITIVE                                         IVIDMM_VAL_POSITIVE
#define RGDM3K_VAL_NEGATIVE                                         IVIDMM_VAL_NEGATIVE


#define RGDM3K_VAL_MIN                                              0
#define RGDM3K_VAL_DEF                                              1.000000
#define RGDM3K_VAL_MAX                                              2.000000

#define RGDM3K_VAL_MATH_MATH_NULL                                   1

#define RGDM3K_VAL_MAXVDC                                           4.000000
#define RGDM3K_VAL_DEFVDC                                           2.000000
#define RGDM3K_VAL_MINVDC                                           0

#define RGDM3K_VAL_MAXVAC                                           4.000000
#define RGDM3K_VAL_DEFVAC                                           2.000000
#define RGDM3K_VAL_MINVAC                                           0

#define RGDM3K_VAL_MAXADC                                           5.000000
#define RGDM3K_VAL_DEFADC                                           2.000000
#define RGDM3K_VAL_MINADC                                           0

#define RGDM3K_VAL_MINAAC                                           0
#define RGDM3K_VAL_DEFAAC                                           2.000000
#define RGDM3K_VAL_MAXAAC                                           4.000000

#define RGDM3K_VAL_MINRES                                           0
#define RGDM3K_VAL_DEFRES                                           3.000000
#define RGDM3K_VAL_MAXRES                                           6.000000

#define RGDM3K_VAL_MAXCAP                                           5.000000
#define RGDM3K_VAL_DEFCAP                                           2.000000
#define RGDM3K_VAL_MINCAP                                           0







/****************************************************************************
 *---------------- Instrument Driver Function Declarations -----------------*
 ****************************************************************************/

/*- Init and Close Functions -------------------------------------------*/
ViStatus _VI_FUNC  RGDM3K_init ( ViRsrc resourceName, ViBoolean IDQuery,
                                 ViBoolean resetDevice, ViSession *vi );
ViStatus _VI_FUNC  RGDM3K_InitWithOptions ( ViRsrc resourceName, ViBoolean IDQuery,
        ViBoolean resetDevice, ViConstString optionString,
        ViSession *newVi );
ViStatus _VI_FUNC  RGDM3K_close ( ViSession vi );

/*- Coercion Functions --------------------------------------------------*/
ViStatus _VI_FUNC  RGDM3K_GetNextCoercionRecord ( ViSession vi, ViInt32 bufferSize, ViChar record[] );

/*- Interchangeability Checking Functions ------------------------------*/
ViStatus _VI_FUNC RGDM3K_GetNextInterchangeWarning ( ViSession vi,
        ViInt32 bufferSize,
        ViChar warnString[] );
ViStatus _VI_FUNC RGDM3K_ResetInterchangeCheck ( ViSession vi );
ViStatus _VI_FUNC RGDM3K_ClearInterchangeWarnings ( ViSession vi );

/*- Locking Functions --------------------------------------------------*/
ViStatus _VI_FUNC  RGDM3K_LockSession ( ViSession vi, ViBoolean *callerHasLock );
ViStatus _VI_FUNC  RGDM3K_UnlockSession ( ViSession vi, ViBoolean *callerHasLock );


/*- Basic Instrument Operation -----------------------------------------*/

ViStatus _VI_FUNC  RGDM3K_ConfigureMeasurement ( ViSession vi, ViInt32 measFunction,
        ViReal64 range, ViReal64 resolution );
ViStatus _VI_FUNC  RGDM3K_ConfigureTrigger ( ViSession vi, ViInt32 triggerSource,
        ViReal64 triggerDelay );
ViStatus _VI_FUNC  RGDM3K_Read ( ViSession vi, ViInt32 maxTime, ViReal64 *reading );
ViStatus _VI_FUNC  RGDM3K_Fetch ( ViSession vi, ViInt32 maxTime, ViReal64 *reading );
ViStatus _VI_FUNC  RGDM3K_Abort ( ViSession vi );
ViStatus _VI_FUNC  RGDM3K_Initiate ( ViSession vi );
ViStatus _VI_FUNC  RGDM3K_IsOverRange ( ViSession vi,
                                        ViReal64 measurementValue,
                                        ViBoolean *isOverRange );

/*- AC Measurement Extension Group ----------------------------------------*/

ViStatus _VI_FUNC  RGDM3K_ConfigureACBandwidth ( ViSession vi, ViReal64 minFreq,
        ViReal64 maxFreq );

/*- Frequency Extension Group --------------------------------------------*/
ViStatus _VI_FUNC  RGDM3K_ConfigureFrequencyVoltageRange ( ViSession vi,
        ViReal64 frequencyVoltageRange );



/*- Trigger Slope Extension Group -------------------------------------------*/
ViStatus _VI_FUNC  RGDM3K_ConfigureTriggerSlope ( ViSession vi, ViInt32 Polarity );



ViStatus _VI_FUNC  RGDM3K_ConfigureDB ( ViSession vi, ViBoolean dBEnable,
                                        ViReal64 dBReferenceImpedance );
ViStatus _VI_FUNC  RGDM3K_ConfigureDBM ( ViSession vi, ViBoolean dBmEnable,
        ViInt32 dBmReferenceImpedance );
ViStatus _VI_FUNC  RGDM3K_ConfigureLimit ( ViSession vi, ViBoolean limitEnable,
        ViReal64 lowerLimit, ViReal64 upperLimit );
ViStatus _VI_FUNC  RGDM3K_ConfigureNull ( ViSession vi, ViBoolean nullEnable,
        ViReal64 nullOffset );
ViStatus _VI_FUNC RGDM3K_ConfigureMinMax ( ViSession vi,
        ViBoolean minMaxEnable );
ViStatus _VI_FUNC RGDM3K_FetchMinMax ( ViSession vi, ViInt32 maxTime,
                                       ViReal64 *minimum, ViReal64 *maximum,
                                       ViReal64 *average, ViInt32 *count );


/*- Software Trigger Extension Group -----------------------------------*/
ViStatus _VI_FUNC  RGDM3K_SendSoftwareTrigger ( ViSession vi );


/*- Error Functions ----------------------------------------------------*/
ViStatus _VI_FUNC  RGDM3K_error_query ( ViSession vi, ViInt32 *errorCode,
                                        ViChar errorMessage[] );
ViStatus _VI_FUNC  RGDM3K_error_message ( ViSession vi, ViStatus errorCode,
        ViChar errorMessage[256] );
ViStatus _VI_FUNC RGDM3K_GetError ( ViSession vi, ViStatus *errorCode,
                                    ViInt32 bufferSize, ViChar description[] );
ViStatus _VI_FUNC  RGDM3K_ClearError ( ViSession vi );

/*- Utility Functions --------------------------------------------------*/
ViStatus _VI_FUNC  RGDM3K_DisplayMessage ( ViSession vi, ViConstString message );
ViStatus _VI_FUNC  RGDM3K_DisplayClear ( ViSession vi );
ViStatus _VI_FUNC  RGDM3K_reset ( ViSession vi );
ViStatus _VI_FUNC  RGDM3K_self_test ( ViSession vi, ViInt16 *testResult,
                                      ViChar testMessage[] );
ViStatus _VI_FUNC  RGDM3K_revision_query ( ViSession vi,
        ViChar instrumentDriverRevision[],
        ViChar firmwareRevision[] );
ViStatus _VI_FUNC  RGDM3K_WriteInstrData ( ViSession vi, ViConstString writeBuffer );
ViStatus _VI_FUNC  RGDM3K_ReadInstrData  ( ViSession vi, ViInt32 numBytes,
        ViChar rdBuf[], ViInt32 *bytesRead );
ViStatus _VI_FUNC RGDM3K_InvalidateAllAttributes ( ViSession vi );
ViStatus _VI_FUNC RGDM3K_ResetWithDefaults ( ViSession vi );
ViStatus _VI_FUNC RGDM3K_Disable ( ViSession vi );

/*- Set, Get, and Check Attribute Functions ----------------------------*/
ViStatus _VI_FUNC  RGDM3K_GetAttributeViInt32 ( ViSession vi, ViConstString channelName, ViAttr attribute, ViInt32 *value );
ViStatus _VI_FUNC  RGDM3K_GetAttributeViReal64 ( ViSession vi, ViConstString channelName, ViAttr attribute, ViReal64 *value );
ViStatus _VI_FUNC  RGDM3K_GetAttributeViString ( ViSession vi, ViConstString channelName, ViAttr attribute, ViInt32 bufSize, ViChar value[] );
ViStatus _VI_FUNC  RGDM3K_GetAttributeViSession ( ViSession vi, ViConstString channelName, ViAttr attribute, ViSession *value );
ViStatus _VI_FUNC  RGDM3K_GetAttributeViBoolean ( ViSession vi, ViConstString channelName, ViAttr attribute, ViBoolean *value );

ViStatus _VI_FUNC  RGDM3K_SetAttributeViInt32 ( ViSession vi, ViConstString channelName, ViAttr attribute, ViInt32 value );
ViStatus _VI_FUNC  RGDM3K_SetAttributeViReal64 ( ViSession vi, ViConstString channelName, ViAttr attribute, ViReal64 value );
ViStatus _VI_FUNC  RGDM3K_SetAttributeViString ( ViSession vi, ViConstString channelName, ViAttr attribute, ViConstString value );
ViStatus _VI_FUNC  RGDM3K_SetAttributeViSession ( ViSession vi, ViConstString channelName, ViAttr attribute, ViSession value );
ViStatus _VI_FUNC  RGDM3K_SetAttributeViBoolean ( ViSession vi, ViConstString channelName, ViAttr attribute, ViBoolean value );

ViStatus _VI_FUNC  RGDM3K_CheckAttributeViInt32 ( ViSession vi, ViConstString channelName, ViAttr attribute, ViInt32 value );
ViStatus _VI_FUNC  RGDM3K_CheckAttributeViReal64 ( ViSession vi, ViConstString channelName, ViAttr attribute, ViReal64 value );
ViStatus _VI_FUNC  RGDM3K_CheckAttributeViString ( ViSession vi, ViConstString channelName, ViAttr attribute, ViConstString value );
ViStatus _VI_FUNC  RGDM3K_CheckAttributeViSession ( ViSession vi, ViConstString channelName, ViAttr attribute, ViSession value );
ViStatus _VI_FUNC  RGDM3K_CheckAttributeViBoolean ( ViSession vi, ViConstString channelName, ViAttr attribute, ViBoolean value );

/*- Obsolete function definitions only for backwards compatibility -----*/
/*- Do not use these functions in your programs ------------------------*/

/*- Channel Info Functions ---------------------------------------------*/

/*********************************************************
    Functions reserved for class driver use only.
    End-users should not call these functions.
 *********************************************************/
ViStatus _VI_FUNC  RGDM3K_IviInit ( ViRsrc resourceName, ViBoolean IDQuery,
                                    ViBoolean reset, ViSession vi );
ViStatus _VI_FUNC  RGDM3K_IviClose ( ViSession vi );

/****************************************************************************
 *------------------------ Error And Completion Codes ----------------------*
 ****************************************************************************/

#define RGDM3K_WARN_OVER_RANGE                           IVIDMM_WARN_OVER_RANGE
#define RGDM3K_ERROR_MAX_TIME_EXCEEDED                   IVIDMM_ERROR_MAX_TIME_EXCEEDED
#define RGDM3K_ERROR_TRIGGER_NOT_SOFTWARE                IVIDMM_ERROR_TRIGGER_NOT_SOFTWARE
#define RGDM3K_ERROR_INVALID_MODEL_TYPE                  (IVI_SPECIFIC_ERROR_BASE + 1L)

/*- Obsolete Inherent Instrument Attributes and functions -*/
/*- These attributes and functions have been deprecated and may not
    be supported in future versions of this driver.
-*/
/*- Driver Information -*/
#define RGDM3K_ATTR_SPECIFIC_DRIVER_MAJOR_VERSION IVI_ATTR_SPECIFIC_DRIVER_MAJOR_VERSION  // ViInt32,  read-only  
#define RGDM3K_ATTR_SPECIFIC_DRIVER_MINOR_VERSION IVI_ATTR_SPECIFIC_DRIVER_MINOR_VERSION  // ViInt32,  read-only  

/*- Error Info -*/
#define RGDM3K_ATTR_PRIMARY_ERROR                 IVI_ATTR_PRIMARY_ERROR                  // ViInt32 
#define RGDM3K_ATTR_SECONDARY_ERROR               IVI_ATTR_SECONDARY_ERROR                // ViInt32
#define RGDM3K_ATTR_ERROR_ELABORATION             IVI_ATTR_ERROR_ELABORATION              // ViString

/*- Advanced Session Information -*/
#define RGDM3K_ATTR_IO_SESSION_TYPE               IVI_ATTR_IO_SESSION_TYPE                // ViString, read-only  
#define RGDM3K_ATTR_IO_SESSION                    IVI_ATTR_IO_SESSION                     // ViSession, read-only

/*- Deprecated Attribute Identifiers for Renamed Attributes -*/
#define RGDM3K_ATTR_NUM_CHANNELS                  RGDM3K_ATTR_CHANNEL_COUNT
#define RGDM3K_ATTR_QUERY_INSTR_STATUS            RGDM3K_ATTR_QUERY_INSTRUMENT_STATUS
#define RGDM3K_ATTR_RESOURCE_DESCRIPTOR           RGDM3K_ATTR_IO_RESOURCE_DESCRIPTOR

/*- Deprecated Error Information functions -*/
/*ViStatus _VI_FUNC  RGDM3K_GetErrorInfo (ViSession vi, ViStatus *primaryError,
                                          ViStatus *secondaryError,
                                          ViChar errorElaboration[256]);
ViStatus _VI_FUNC  RGDM3K_ClearErrorInfo (ViSession vi); */

/****************************************************************************
 *---------------------------- End Include File ----------------------------*
 ****************************************************************************/
#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif
#endif /* __RGDM3K_HEADER */




