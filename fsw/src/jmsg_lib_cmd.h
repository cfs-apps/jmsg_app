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
**   Define command functions to configure JMSG_LIB
**
** Notes:
**   1. The EDS definitions split between JMSG_LIB and JMSG_APP are 
**      unconventional. I tried to define eveything in jmsg_lib.xml
**      and implement the command functions in jmsg_topic_tbl.* but
**      command codes (CC) are not generated unless a command interface
**      is defined in the EDS. I created jmsg_lib_cmds.* and moved
**      just the command packet definitions to jmsg_app. Another
**      dependency goal is to make sure jmsg_app can build wihtout 
**      jmsg_app.
**   2. Basecamp file/function naming conventions are violated so 
**      it's clear that the command functions are operating on jmsg_lib's 
**      topic table.
**
*/

#ifndef _jmsg_lib_cmds_
#define _jmsg_lib_cmds_

/*
** Includes
*/

#include "app_c_fw.h"

/***********************/
/** Macro Definitions **/
/***********************/

/*
** Event Message IDs
*/

#define JMSG_LIB_CMD_CONFIG_PLUGIN_EID (JMSG_LIB_CMD_BASE_EID + 0)
#define JMSG_LIB_CMD_RUN_TEST_EID      (JMSG_LIB_CMD_BASE_EID + 1)


/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function: JMSG_TOPIC_TBL_ConfigTopicPluginCmd
**
** Enable/disable a plugin topic
**
** Notes:
**   1. Signature must match CMDMGR_CmdFuncPtr_t
**   2. DataObjPtr is not used
**
*/
bool JMSG_TOPIC_TBL_ConfigTopicPluginCmd(void* DataObjPtr, const CFE_MSG_Message_t *MsgPtr);


/******************************************************************************
** Function: JMSG_TOPIC_TBL_RunTopicPluginTestCmd
**
** Execute a topic's SB message test.
** 
** Notes:
**   1. Index must be less than JMSG_LIB_TopicPlugin_Enum_t_MAX
**
*/
bool JMSG_TOPIC_TBL_RunTopicPluginTestCmd(void* DataObjPtr, const CFE_MSG_Message_t *MsgPtr);


#endif /* _jmsg_lib_cmds_ */
