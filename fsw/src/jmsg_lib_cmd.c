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
#include "app_cfg.h"
#include "jmsg_lib_cmd.h"


/******************************************************************************
** Function: JMSG_TOPIC_TBL_ConfigTopicPluginCmd
**
** Enable/disable a plugin topic
**
** Notes:
**   1. Signature must match CMDMGR_CmdFuncPtr_t
**   2. DataObjPtr is not used.
**   3. The functions called send error events so this function only needs to
**      report a successful command. 
**   4. TODO: No protection against enabling a topic plugin that was never constructed
**
*/
bool JMSG_TOPIC_TBL_ConfigTopicPluginCmd(void *DataObjPtr, const CFE_MSG_Message_t *MsgPtr)
{

   const JMSG_LIB_ConfigTopicPlugin_CmdPayload_t *ConfigTopicPlugin = CMDMGR_PAYLOAD_PTR(MsgPtr, JMSG_APP_ConfigTopicPlugin_t);
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
            CFE_EVS_SendEvent(JMSG_LIB_CMD_CONFIG_PLUGIN_EID, CFE_EVS_EventType_INFORMATION, 
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
            CFE_EVS_SendEvent(JMSG_LIB_CMD_CONFIG_PLUGIN_EID, CFE_EVS_EventType_INFORMATION, 
                              "Sucessfully disabled plugin topic %d",ConfigTopicPlugin->Id);
         }
      }
   }
   else
   {
      CFE_EVS_SendEvent(JMSG_LIB_CMD_CONFIG_PLUGIN_EID, CFE_EVS_EventType_ERROR, 
                        "Configure plugin topic %d command rejected. Invalid action %d",
                        ConfigTopicPlugin->Id, ConfigTopicPlugin->Action);
   }
  
   return RetStatus;
    
} /* End JMSG_TOPIC_TBL_ConfigTopicPluginCmd() */


/******************************************************************************
** Function: JMSG_TOPIC_TBL_RunTopicPluginTestCmd
**
** Call a plugin's test function.
**
** Notes:
**   1. The function is called once. Teh JMSG network apps (e.g. JMSG_UDP) 
**      supply test that run multiple times.
*/
bool JMSG_TOPIC_TBL_RunTopicPluginTestCmd(void* DataObjPtr, const CFE_MSG_Message_t *MsgPtr)
{

   const JMSG_LIB_RunTopicPluginTest_CmdPayload_t *PluginTest = CMDMGR_PAYLOAD_PTR(MsgPtr, JMSG_APP_RunTopicPluginTest_t);
   bool RetStatus = false;

   if (JMSG_TOPIC_TBL_ValidTopicPlugin(PluginTest->Id))
   {
      CFE_EVS_SendEvent(JMSG_LIB_CMD_RUN_TEST_EID, CFE_EVS_EventType_INFORMATION, 
                        "Running test for plugin topic %d", PluginTest->Id);
      JMSG_TOPIC_TBL_RunTopicPluginTest(PluginTest->Id, true, PluginTest->Param);
      RetStatus = true;
   }
   else
   {
      CFE_EVS_SendEvent(JMSG_LIB_CMD_RUN_TEST_EID, CFE_EVS_EventType_ERROR, 
                        "Run plugin topic test command rejected. Id %d either invalid or not loaded", 
                        PluginTest->Id);

   }
  
   return RetStatus;
   
} /* End JMSG_TOPIC_TBL_RunTopicPluginTestCmd() */
