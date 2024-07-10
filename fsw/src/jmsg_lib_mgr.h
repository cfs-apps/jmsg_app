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
**   Manage the topic table owned by JMSG_LIB
**
** Notes:
**   1. The JMSG lib and app designs are similar to the cFE service
**      designs. The app provides the command and telemtry ground
**      interface to the library. A major difference is that JMSG_LIB 
**      contains a table that configures the topics being managed.
**   2. TODO: Describe coupling and dependencies.
**
*/

#ifndef _jmsg_lib_mgr_
#define _jmsg_lib_mgr_

/*
** Includes
*/

#include "app_cfg.h"

/***********************/
/** Macro Definitions **/
/***********************/

/*
** Event Message IDs
*/

#define JMSG_LIB_MGR_CONFIG_PLUGIN_EID      (JMSG_LIB_MGR_BASE_EID + 0)
#define JMSG_LIB_MGR_SEND_SUBSCRIBE_TLM_EID (JMSG_LIB_MGR_BASE_EID + 1)
#define JMSG_LIB_MGR_START_TOPIC_TEST_EID   (JMSG_LIB_MGR_BASE_EID + 2)
#define JMSG_LIB_MGR_STOP_TOPIC_TEST_EID    (JMSG_LIB_MGR_BASE_EID + 3)

/**********************/
/** Type Definitions **/
/**********************/


typedef struct
{
   uint32                        TopicSubscribeTlmDelay;
   CFE_SB_MsgId_t                TopicSubscribeTlmMid;
   JMSG_LIB_TopicSubscribeTlm_t  TopicSubscribeTlm;
     
   bool    TestActive;
   uint32  TestExeCnt;
   int16   TestParam;
   JMSG_PLATFORM_TopicPlugin_Enum_t  TestId;
   
} JMSG_LIB_MGR_Class_t;


/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function: JMSG_LIB_MGR_Constructor
**
** Construct and register topic plugins for the JMSG_LIB_MGR app
**
*/
void JMSG_LIB_MGR_Constructor(JMSG_LIB_MGR_Class_t *JMsgLibMgrPtr, const INITBL_Class_t *IniTbl);


/******************************************************************************
** Function: JMSG_LIB_MGR_ConfigTopicPluginCmd
**
** Enable/disable a plugin topic
**
** Notes:
**   1. Signature must match CMDMGR_CmdFuncPtr_t
**   2. DataObjPtr is not used
**
*/
bool JMSG_LIB_MGR_ConfigTopicPluginCmd(void* DataObjPtr, const CFE_MSG_Message_t *MsgPtr);


/******************************************************************************
** Function: JMSG_LIB_MGR_ResetStatus
**
*/
void JMSG_LIB_MGR_ResetStatus(void);


/******************************************************************************
** Function: JMSG_LIB_MGR_RunTopicTest
**
*/
void JMSG_LIB_MGR_RunTopicTest(void);


/******************************************************************************
** Function: JMSG_LIB_MGR_SendAllTopicSubscribeTlmCmd
**
*/
bool JMSG_LIB_MGR_SendAllTopicSubscribeTlmCmd(void *ObjDataPtr, const CFE_MSG_Message_t *MsgPtr);


/******************************************************************************
** Function: JMSG_LIB_MGR_SendTopicSubscribeTlmCmd
**
*/
bool JMSG_LIB_MGR_SendTopicSubscribeTlmCmd(void *ObjDataPtr, const CFE_MSG_Message_t *MsgPtr);


/******************************************************************************
** Function: JMSG_LIB_MGR_StartTopicTestCmd
**
*/
bool JMSG_LIB_MGR_StartTopicTestCmd(void *ObjDataPtr, const CFE_MSG_Message_t *MsgPtr);


/******************************************************************************
** Function: JMSG_LIB_MGR_StopTopicTestCmd
**
*/
bool JMSG_LIB_MGR_StopTopicTestCmd(void *ObjDataPtr, const CFE_MSG_Message_t *MsgPtr);


/******************************************************************************
** Function: JMSG_LIB_MGR_SubscribeToTopicPluginsCmd
**
*/
bool JMSG_LIB_MGR_SubscribeToTopicPluginsCmd(void *ObjDataPtr, const CFE_MSG_Message_t *MsgPtr);


#endif /* _jmsg_lib_mgr_ */
