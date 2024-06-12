/*
** Copyright 2022 bitValence, Inc.
** All Rights Reserved.
**
** This program is free software; you can modify and/or redistribute it
** under the terms of the GNU Affero General Public License
** as published by the Free Software Foundation; version 3 with
** attribution addendums as found in the LICENSE.txt
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Affero General Public License for more details.
**
** Purpose:
**   Provide a command and telemetry ground interface to the JMSG_LIB
**
** Notes:
**   None
**
*/

/*
** Includes
*/

#include <string.h>
#include "jmsg_lib.h"
#include "jmsg_app.h"
#include "jmsg_app_eds_cc.h"
#include "jmsg_topic_tbl.h"

/***********************/
/** Macro Definitions **/
/***********************/

/* Convenience macros */
#define  INITBL_OBJ      (&(JMsgApp.IniTbl))
#define  CMDMGR_OBJ      (&(JMsgApp.CmdMgr))
#define  TBLMGR_OBJ      (&(JMsgApp.TblMgr))  

/*******************************/
/** Local Function Prototypes **/
/*******************************/

static int32 InitApp(void);
static int32 ProcessCommands(void);
static void SendStatusPkt(void);


/**********************/
/** File Global Data **/
/**********************/

/* 
** Must match DECLARE ENUM() declaration in app_cfg.h
** Defines "static INILIB_CfgEnum IniCfgEnum"
*/
DEFINE_ENUM(Config,APP_CONFIG)  

static CFE_EVS_BinFilter_t  EventFilters[] =
{  
   /* Event ID          Mask */
   {JMSG_APP_NOOP_EID,  CFE_EVS_NO_FILTER}    // Placeholder 
};

/*****************/
/** Global Data **/
/*****************/

JMSG_APP_Class_t   JMsgApp;


/******************************************************************************
** Function: JMSG_AppMain
**
*/
void JMSG_AppMain(void)
{

   uint32 RunStatus = CFE_ES_RunStatus_APP_ERROR;
   
   CFE_EVS_Register(EventFilters, sizeof(EventFilters)/sizeof(CFE_EVS_BinFilter_t),
                    CFE_EVS_EventFilter_BINARY);

   if (InitApp() == CFE_SUCCESS)      /* Performs initial CFE_ES_PerfLogEntry() call */
   {
      RunStatus = CFE_ES_RunStatus_APP_RUN; 
   }
   
   /*
   ** Main process loop
   */
   while (CFE_ES_RunLoop(&RunStatus))
   {

      RunStatus = ProcessCommands();
      
   } /* End CFE_ES_RunLoop */

   CFE_ES_WriteToSysLog("JMSG_LIB App terminating, run status = 0x%08X\n", RunStatus);   /* Use SysLog, events may not be working */

   CFE_EVS_SendEvent(JMSG_APP_EXIT_EID, CFE_EVS_EventType_CRITICAL, "JMSG_LIB App terminating, run status = 0x%08X", RunStatus);

   CFE_ES_ExitApp(RunStatus);  /* Let cFE kill the task (and any child tasks) */


} /* End of JMSG_AppMain() */



/******************************************************************************
** Function: JMSG_APP_NoOpCmd
**
*/

bool JMSG_APP_NoOpCmd(void *ObjDataPtr, const CFE_MSG_Message_t *MsgPtr)
{

   CFE_EVS_SendEvent (JMSG_APP_NOOP_EID, CFE_EVS_EventType_INFORMATION,
                      "No operation command received for JMSG_LIB App version %d.%d.%d",
                      JMSG_APP_MAJOR_VER, JMSG_APP_MINOR_VER, JMSG_APP_PLATFORM_REV);

   return true;


} /* End JMSG_APP_NoOpCmd() */


/******************************************************************************
** Function: JMSG_APP_ResetAppCmd
**
*/

bool JMSG_APP_ResetAppCmd(void* ObjDataPtr, const CFE_MSG_Message_t *MsgPtr)
{

   CFE_EVS_ResetAllFilters();

   CMDMGR_ResetStatus(CMDMGR_OBJ);
	  
   return true;

} /* End JMSG_APP_ResetAppCmd() */


/******************************************************************************
** Function: InitApp
**
*/
static int32 InitApp(void)
{

   int32 RetStatus = APP_C_FW_CFS_ERROR;
   

   /*
   ** Read JSON INI Table & class variable defaults defined in JSON  
   */

   if (INITBL_Constructor(INITBL_OBJ, JMSG_APP_INI_FILENAME, &IniCfgEnum))
   {
   
      JMsgApp.PerfId = INITBL_GetIntConfig(INITBL_OBJ, CFG_APP_MAIN_PERF_ID);
      CFE_ES_PerfLogEntry(JMsgApp.PerfId);

      /*
      ** JMSG_LIB owns the JMSG_TOPIC_TBL object
      */
      TBLMGR_Constructor(TBLMGR_OBJ, INITBL_GetStrConfig(INITBL_OBJ, CFG_APP_CFE_NAME));
      TBLMGR_RegisterTblWithDef(TBLMGR_OBJ, JMSG_TOPIC_TBL_NAME, 
                                JMSG_TOPIC_TBL_LoadCmd, JMSG_TOPIC_TBL_DumpCmd,  
                                INITBL_GetStrConfig(INITBL_OBJ, CFG_JMSG_TOPIC_TBL_FILE));
                             
      JMsgApp.CmdMid        = CFE_SB_ValueToMsgId(INITBL_GetIntConfig(INITBL_OBJ, CFG_JMSG_APP_CMD_TOPICID));
      JMsgApp.SendStatusMid = CFE_SB_ValueToMsgId(INITBL_GetIntConfig(INITBL_OBJ, CFG_SEND_STATUS_TLM_TOPICID));
         
      /*
      ** Initialize app level interfaces
      */
 
      CFE_SB_CreatePipe(&JMsgApp.CmdPipe, INITBL_GetIntConfig(INITBL_OBJ, CFG_CMD_PIPE_DEPTH), INITBL_GetStrConfig(INITBL_OBJ, CFG_CMD_PIPE_NAME));  
      CFE_SB_Subscribe(JMsgApp.CmdMid, JMsgApp.CmdPipe);
      CFE_SB_Subscribe(JMsgApp.SendStatusMid, JMsgApp.CmdPipe);

      CMDMGR_Constructor(CMDMGR_OBJ);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, JMSG_APP_NOOP_CC,  NULL, JMSG_APP_NoOpCmd,     0);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, JMSG_APP_RESET_CC, NULL, JMSG_APP_ResetAppCmd, 0);

      CMDMGR_RegisterFunc(CMDMGR_OBJ, JMSG_APP_LOAD_TBL_CC, TBLMGR_OBJ, TBLMGR_LoadTblCmd, sizeof(JMSG_LIB_LoadTbl_CmdPayload_t));
      CMDMGR_RegisterFunc(CMDMGR_OBJ, JMSG_APP_DUMP_TBL_CC, TBLMGR_OBJ, TBLMGR_DumpTblCmd, sizeof(JMSG_LIB_DumpTbl_CmdPayload_t));
 
      CMDMGR_RegisterFunc(CMDMGR_OBJ, JMSG_APP_CONFIG_TOPIC_PLUGIN_CC,   NULL, JMSG_TOPIC_TBL_ConfigTopicPluginCmd,   sizeof(JMSG_LIB_ConfigTopicPlugin_CmdPayload_t));
      CMDMGR_RegisterFunc(CMDMGR_OBJ, JMSG_APP_RUN_TOPIC_PLUGIN_TEST_CC, NULL, JMSG_TOPIC_TBL_RunTopicPluginTestCmd,  sizeof(JMSG_LIB_RunTopicPluginTest_CmdPayload_t));
      CMDMGR_RegisterFunc(CMDMGR_OBJ, JMSG_APP_SEND_TOPIC_PLUGIN_TLM_CC, NULL, JMSG_TOPIC_TBL_SendTopicTPluginTlmCmd, 0);

      CFE_MSG_Init(CFE_MSG_PTR(JMsgApp.StatusTlm.TelemetryHeader), CFE_SB_ValueToMsgId(INITBL_GetIntConfig(INITBL_OBJ, CFG_JMSG_APP_STATUS_TLM_TOPICID)), sizeof(JMSG_APP_StatusTlm_t));
      CFE_MSG_Init(CFE_MSG_PTR(JMsgApp.StatusTlm.TelemetryHeader), CFE_SB_ValueToMsgId(INITBL_GetIntConfig(INITBL_OBJ, CFG_JMSG_APP_STATUS_TLM_TOPICID)), sizeof(JMSG_APP_StatusTlm_t));

      /*
      ** Application startup event message
      */
      RetStatus = CFE_SUCCESS;
      CFE_EVS_SendEvent(JMSG_APP_INIT_APP_EID, CFE_EVS_EventType_INFORMATION,
                        "JMSG_LIB App Initialized. Version %d.%d.%d",
                        JMSG_APP_MAJOR_VER, JMSG_APP_MINOR_VER, JMSG_APP_PLATFORM_REV);
                        
   } /* End if INITBL Constructed */
   
   return RetStatus;

} /* End of InitApp() */


/******************************************************************************
** Function: ProcessCommands
**
** 
*/
static int32 ProcessCommands(void)
{
   
   int32  RetStatus = CFE_ES_RunStatus_APP_RUN;
   int32  SysStatus;

   CFE_SB_Buffer_t  *SbBufPtr;
   CFE_SB_MsgId_t   MsgId = CFE_SB_INVALID_MSG_ID;


   CFE_ES_PerfLogExit(JMsgApp.PerfId);
   SysStatus = CFE_SB_ReceiveBuffer(&SbBufPtr, JMsgApp.CmdPipe, CFE_SB_PEND_FOREVER);
   CFE_ES_PerfLogEntry(JMsgApp.PerfId);

   if (SysStatus == CFE_SUCCESS)
   {
      SysStatus = CFE_MSG_GetMsgId(&SbBufPtr->Msg, &MsgId);

      if (SysStatus == CFE_SUCCESS)
      {

         if (CFE_SB_MsgId_Equal(MsgId, JMsgApp.CmdMid))
         {
            CMDMGR_DispatchFunc(CMDMGR_OBJ, &SbBufPtr->Msg);
         } 
         else if (CFE_SB_MsgId_Equal(MsgId, JMsgApp.SendStatusMid))
         {   
            SendStatusPkt();
         }
         else
         {   
            CFE_EVS_SendEvent(JMSG_APP_INVALID_MID_EID, CFE_EVS_EventType_ERROR,
                              "Received invalid command packet, MID = 0x%04X(%d)", 
                              CFE_SB_MsgIdToValue(MsgId), CFE_SB_MsgIdToValue(MsgId));
         }

      } /* End if got message ID */
   } /* End if received buffer */
   else
   {
      if (SysStatus != CFE_SB_NO_MESSAGE)
      {
         RetStatus = CFE_ES_RunStatus_APP_ERROR;
      }
   } 

   return RetStatus;
   
} /* End ProcessCommands() */


/******************************************************************************
** Function: SendStatusPkt
**
*/
void SendStatusPkt(void)
{
   
   /* Good design practice in case app expands to more than one table */
   const TBLMGR_Tbl_t           *LastTbl  = TBLMGR_GetLastTblStatus(TBLMGR_OBJ);
   const JMSG_TOPIC_TBL_Class_t *TopicTbl = JMSG_LIB_GetTopicTbl();
   
   JMSG_APP_StatusTlm_Payload_t *Payload = &JMsgApp.StatusTlm.Payload;

   /*
   ** Framework Data
   */

   Payload->ValidCmdCnt    = JMsgApp.CmdMgr.ValidCmdCnt;
   Payload->InvalidCmdCnt  = JMsgApp.CmdMgr.InvalidCmdCnt;

   /*
   ** Table Data 
   ** - Loaded with status from the last table action 
   */

   Payload->TopicTblLoaded      = TopicTbl->Loaded;
   Payload->LastTblAction       = LastTbl->LastAction;
   Payload->LastTblActionStatus = LastTbl->LastActionStatus;
   
   CFE_SB_TimeStampMsg(CFE_MSG_PTR(JMsgApp.StatusTlm.TelemetryHeader));
   CFE_SB_TransmitMsg(CFE_MSG_PTR(JMsgApp.StatusTlm.TelemetryHeader), true);

} /* End SendStatusPkt() */
