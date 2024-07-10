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
**   Implement command functions to configure JMSG_LIB
**
** Notes:
**   1. Basecamp file/function naming conventions are violated so 
**      it's clear that the command functions are operating on jmsg_lib's 
**      topic table.
**
*/

/*
** Include Files:
*/

#include <string.h>
#include "jmsg_lib.h"
#include "jmsg_lib_mgr.h"
#include "usr_tplug.h"

/*******************************/
/** Local Function Prototypes **/
/*******************************/

static void SendAllTopicSubscribeTlm(void);
static void SendTopicSubscribeTlm(JMSG_PLATFORM_TopicPlugin_Enum_t TopicPlugin);


/**********************/
/** File Global Data **/
/**********************/

static JMSG_LIB_MGR_Class_t *JMsgLibMgr;


/******************************************************************************
** Function: JMSG_LIB_MGR_Constructor
**
*/
void JMSG_LIB_MGR_Constructor(JMSG_LIB_MGR_Class_t *JMsgLibMgrPtr, const INITBL_Class_t *IniTbl)
{

   JMsgLibMgr = JMsgLibMgrPtr;
   
   memset((void*)JMsgLibMgr, 0, sizeof(JMSG_LIB_MGR_Class_t));

   JMsgLibMgr->TopicSubscribeTlmDelay = INITBL_GetIntConfig(IniTbl, CFG_TOPIC_SUBSCRIBE_TLM_DELAY);
   
   JMsgLibMgr->TopicSubscribeTlmMid = CFE_SB_ValueToMsgId(INITBL_GetIntConfig(IniTbl, CFG_JMSG_LIB_TOPIC_SUBSCRIBE_TLM_TOPICID));
   CFE_MSG_Init(CFE_MSG_PTR(JMsgLibMgr->TopicSubscribeTlm.TelemetryHeader), JMsgLibMgr->TopicSubscribeTlmMid, sizeof(JMSG_LIB_TopicSubscribeTlm_t));

   // User topic plugins must be created prior to topic subscriptions
   USR_TPLUG_Constructor();

   // Allow time for JMSG protocol apps to initialize
   OS_TaskDelay(INITBL_GetIntConfig(IniTbl, CFG_TOPIC_SUBSCRIBE_STARTUP_DELAY));
   SendAllTopicSubscribeTlm();

} /* JMSG_LIB_MGR_Constructor() */


/******************************************************************************
** Function: JMSG_LIB_MGR_ConfigTopicPluginCmd
**
** Enable/disable a plugin topic
**
** Notes:
**   1. Signature must match CMDMGR_CmdFuncPtr_t
**   2. DataObjPtr is not used.
**   3. The functions called send error events so this function only needs to
**      report a successful command. 
**   4. TODO: No protection against enabling a topic plugin that was never constructed. Is this a problem with stubs?
**
*/
bool JMSG_LIB_MGR_ConfigTopicPluginCmd(void *DataObjPtr, const CFE_MSG_Message_t *MsgPtr)
{

   const JMSG_APP_ConfigTopicPlugin_CmdPayload_t *ConfigTopicPlugin = CMDMGR_PAYLOAD_PTR(MsgPtr, JMSG_APP_ConfigTopicPlugin_t);
   bool RetStatus = false;
   JMSG_TOPIC_TBL_SubscriptionOptEnum_t TopicSubscription;
   
   if (ConfigTopicPlugin->Action == APP_C_FW_ConfigEnaAction_ENABLE)
   {
      if (JMSG_TOPIC_TBL_EnablePlugin(ConfigTopicPlugin->Id))
      {
         TopicSubscription = JMSG_TOPIC_TBL_SubscribeToTopicMsg(ConfigTopicPlugin->Id, JMSG_TOPIC_TBL_SUB_TO_ROLE);
         if ((TopicSubscription == JMSG_TOPIC_TBL_SUB_JMSG) || (TopicSubscription == JMSG_TOPIC_TBL_SUB_SB))
         {
            RetStatus = true;
            CFE_EVS_SendEvent(JMSG_LIB_MGR_CONFIG_PLUGIN_EID, CFE_EVS_EventType_INFORMATION, 
                              "Sucessfully enabled plugin topic %d",ConfigTopicPlugin->Id);
         }
         else
         {
            JMSG_TOPIC_TBL_DisablePlugin(ConfigTopicPlugin->Id);            
         }
      }
   }
   else if (ConfigTopicPlugin->Action == APP_C_FW_ConfigEnaAction_DISABLE)
   {
      if (JMSG_TOPIC_TBL_DisablePlugin(ConfigTopicPlugin->Id))
      {
         if (JMSG_TOPIC_TBL_UnsubscribeFromTopicMsg(ConfigTopicPlugin->Id))
         {
            RetStatus = true;
            CFE_EVS_SendEvent(JMSG_LIB_MGR_CONFIG_PLUGIN_EID, CFE_EVS_EventType_INFORMATION, 
                              "Sucessfully disabled plugin topic %d",ConfigTopicPlugin->Id);
         }
      }
   }
   else
   {
      CFE_EVS_SendEvent(JMSG_LIB_MGR_CONFIG_PLUGIN_EID, CFE_EVS_EventType_ERROR, 
                        "Configure plugin topic %d command rejected. Invalid action %d",
                        ConfigTopicPlugin->Id, ConfigTopicPlugin->Action);
   }
  
   return RetStatus;
    
} /* End JMSG_LIB_MGR_ConfigTopicPluginCmd() */


/******************************************************************************
** Function: JMSG_LIB_MGR_ResetStatus
**
*/
void JMSG_LIB_MGR_ResetStatus(void)
{

   return;
   
} /* End JMSG_LIB_MGR_ResetStatus() */


/******************************************************************************
** Function: JMSG_LIB_MGR_RunTopicTest
**
*/
void JMSG_LIB_MGR_RunTopicTest(void)
{
   
   if (JMsgLibMgr->TestActive)
   {
      JMSG_TOPIC_TBL_RunTopicPluginTest(JMsgLibMgr->TestId, false, JMsgLibMgr->TestParam);
      JMsgLibMgr->TestExeCnt++;

   } /* End if TestActive */
   
} /* End JMSG_LIB_MGR_RunTopicTest() */


/******************************************************************************
** Function: JMSG_LIB_MGR_SendAllTopicSubscribeTlmCmd
**
*/
bool JMSG_LIB_MGR_SendAllTopicSubscribeTlmCmd(void *ObjDataPtr, const CFE_MSG_Message_t *MsgPtr)
{

   SendAllTopicSubscribeTlm();

   return true;
   
} /* End JMSG_LIB_MGR_SendAllTopicSubscribeTlmCmd() */


/******************************************************************************
** Function: JMSG_LIB_MGR_SendTopicSubscribeTlmCmd
**
*/
bool JMSG_LIB_MGR_SendTopicSubscribeTlmCmd(void *ObjDataPtr, const CFE_MSG_Message_t *MsgPtr)
{
   const JMSG_APP_SendTopicSubscribeTlm_CmdPayload_t *SendTopicPlugin = CMDMGR_PAYLOAD_PTR(MsgPtr, JMSG_APP_SendTopicSubscribeTlm_t);

   bool RetStatus = false;

   if (SendTopicPlugin->Id >= JMSG_PLATFORM_TopicPlugin_Enum_t_MIN && SendTopicPlugin->Id <= JMSG_PLATFORM_TopicPlugin_Enum_t_MAX)
   {
      SendTopicSubscribeTlm(SendTopicPlugin->Id);
      RetStatus = true;
   }
   else
   {
      CFE_EVS_SendEvent(JMSG_LIB_MGR_SEND_SUBSCRIBE_TLM_EID, CFE_EVS_EventType_ERROR, 
                        "Send topic subscribe telemetry rejected, invalid topic plugin ID %d", SendTopicPlugin->Id);
   }
   return RetStatus;
      
} /* End JMSG_LIB_MGR_SendTopicSubscribeTlmCmd() */



/******************************************************************************
** Function: JMSG_LIB_MGR_StartTopicTestCmd
**
*/
bool JMSG_LIB_MGR_StartTopicTestCmd(void *ObjDataPtr, const CFE_MSG_Message_t *MsgPtr)
{
   bool RetStatus = false;
   const JMSG_APP_StartTopicTest_CmdPayload_t *StartTest = CMDMGR_PAYLOAD_PTR(MsgPtr, JMSG_APP_StartTopicTest_t);   
                                          
   if (StartTest->Id >= JMSG_PLATFORM_TopicPlugin_Enum_t_MIN && StartTest->Id <= JMSG_PLATFORM_TopicPlugin_Enum_t_MAX)
   {
      JMsgLibMgr->TestActive = true;
      JMsgLibMgr->TestExeCnt = 0;
      JMsgLibMgr->TestId     = StartTest->Id;
      JMsgLibMgr->TestParam  = 0;   //TODO: Decide whether need a test parameter   

      CFE_EVS_SendEvent(JMSG_LIB_MGR_START_TOPIC_TEST_EID, CFE_EVS_EventType_INFORMATION, 
                        "Started test %d", JMsgLibMgr->TestId);
      
      JMSG_TOPIC_TBL_RunTopicPluginTest(JMsgLibMgr->TestId, true, JMsgLibMgr->TestParam);
      RetStatus = true;
   }
   else
   {
      CFE_EVS_SendEvent(JMSG_LIB_MGR_START_TOPIC_TEST_EID, CFE_EVS_EventType_ERROR, 
                        "Start test rejected, invalid topic plugin ID %d", JMsgLibMgr->TestId);
   }
   
   return RetStatus;
   
} /* End JMSG_LIB_MGR_StartTopicTestCmd() */


/******************************************************************************
** Function: JMSG_LIB_MGR_StopTopicTestCmd
**
*/
bool JMSG_LIB_MGR_StopTopicTestCmd(void *ObjDataPtr, const CFE_MSG_Message_t *MsgPtr)
{
   
   JMsgLibMgr->TestExeCnt = 0;
   JMsgLibMgr->TestActive = false;

   CFE_EVS_SendEvent(JMSG_LIB_MGR_STOP_TOPIC_TEST_EID, CFE_EVS_EventType_INFORMATION, 
                     "Stopped test %d", JMsgLibMgr->TestId);
   
   return true;
   
} /* End JMSG_LIB_MGR_StopTopicTestCmd() */


/******************************************************************************
** Function: SendAllTopicSubscribeTlm
**
*/
static void SendAllTopicSubscribeTlm(void)
{
   for (JMSG_PLATFORM_TopicPlugin_Enum_t i=JMSG_PLATFORM_TopicPlugin_Enum_t_MIN; i <= JMSG_PLATFORM_TopicPlugin_Enum_t_MAX; i++)
   {
      SendTopicSubscribeTlm(i);
      
   } /* End topic loop */
   
} /* SendAllTopicSubscribeTlm() */

   
/******************************************************************************
** Function: SendTopicSubscribeTlm
**
** Notes:
**   1. Sends a telemetry message that is processed by JMSG protocol apps so
**      they can subscribe to topic plugins that use their protocol
**   2. No check are performed to determine whether a topic A SB duplicate
**      subscription event message will be sent if two subscription requests
**      are made without an unsubscribe requests between them.
*/
static void SendTopicSubscribeTlm(JMSG_PLATFORM_TopicPlugin_Enum_t TopicPlugin)
{

   JMsgLibMgr->TopicSubscribeTlm.Payload.Id       = TopicPlugin;
   JMsgLibMgr->TopicSubscribeTlm.Payload.Protocol = JMSG_TOPIC_TBL_GetTopicProtocol(TopicPlugin);

   CFE_SB_TimeStampMsg(CFE_MSG_PTR(JMsgLibMgr->TopicSubscribeTlm.TelemetryHeader));
   CFE_SB_TransmitMsg(CFE_MSG_PTR(JMsgLibMgr->TopicSubscribeTlm.TelemetryHeader), true);
   OS_TaskDelay(JMsgLibMgr->TopicSubscribeTlmDelay);
      
      
} /* End SendTopicSubscribeTlm() */
