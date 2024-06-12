/*
** Copyright 2022 bitValence, Inc.
** All Rights Reserved.
**
** This program is free software; you can modify and/or redistribute it
** under the terms of the GNU Affero General Public License
** as published by the Free Software Foundation; version 3 with
** attribution addendums as found in the LICENSE.txt.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Affero General Public License for more details.
**
** Purpose:
**   Define platform configurations for the JMSG_LIB application
**
** Notes:
**   1. These macros can only be build with the application and can't
**      have a platform scope because the same app_cfg.h file name is used for
**      all applications following the object-based application design.
**
*/

#ifndef _app_cfg_
#define _app_cfg_

/*
** Includes
*/

#include "jmsg_app_eds_typedefs.h"
#include "jmsg_app_platform_cfg.h"
#include "app_c_fw.h"


/******************************************************************************
** Application Macros
*/

/*
** Versions:
**
** 0.9 - Prototype
*/

#define  JMSG_APP_MAJOR_VER      0
#define  JMSG_APP_MINOR_VER      9


/******************************************************************************
** Init File declarations create:
**
**  typedef enum {
**     CMD_PIPE_DEPTH,
**     CMD_PIPE_NAME
**  } INITBL_ConfigEnum;
**    
**  typedef struct {
**     CMD_PIPE_DEPTH,
**     CMD_PIPE_NAME
**  } INITBL_ConfigStruct;
**
**   const char *GetConfigStr(value);
**   ConfigEnum GetConfigVal(const char *str);
**
** XX(name,type)
*/

#define CFG_APP_CFE_NAME       APP_CFE_NAME
#define CFG_APP_MAIN_PERF_ID   APP_MAIN_PERF_ID

#define CFG_JMSG_APP_CMD_TOPICID            JMSG_APP_CMD_TOPICID
#define CFG_JMSG_APP_STATUS_TLM_TOPICID     JMSG_APP_STATUS_TLM_TOPICID
#define CFG_JMSG_APP_TOPIC_TBL_TLM_TOPICID  JMSG_APP_TOPIC_TBL_TLM_TOPICID
#define CFG_SEND_STATUS_TLM_TOPICID         BC_SCH_2_SEC_TOPICID
      
#define CFG_CMD_PIPE_NAME   CMD_PIPE_NAME
#define CFG_CMD_PIPE_DEPTH  CMD_PIPE_DEPTH

#define CFG_JMSG_TOPIC_TBL_FILE  JMSG_TOPIC_TBL_FILE

#define APP_CONFIG(XX) \
   XX(APP_CFE_NAME,char*) \
   XX(APP_MAIN_PERF_ID,uint32) \
   XX(JMSG_APP_CMD_TOPICID,uint32) \
   XX(JMSG_APP_STATUS_TLM_TOPICID,uint32) \
   XX(JMSG_APP_TOPIC_TBL_TLM_TOPICID,uint32) \
   XX(BC_SCH_2_SEC_TOPICID,uint32) \
   XX(CMD_PIPE_NAME,char*) \
   XX(CMD_PIPE_DEPTH,uint32) \
   XX(JMSG_TOPIC_TBL_FILE,char*)
   

DECLARE_ENUM(Config,APP_CONFIG)


/******************************************************************************
** Event Macros
**
** Define the base event message IDs used by each object/component used by the
** application. There are no automated checks to ensure an ID range is not
** exceeded so it is the developer's responsibility to verify the ranges. 
*/

#define JMSG_APP_BASE_EID  (APP_C_FW_APP_BASE_EID + 0)


#endif /* _app_cfg_ */
