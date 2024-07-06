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
**   1. JMSG_APP and JMSG_LIB provide a JSON message "topic plugin"
**      service for networking apps that route JSON messages between
**      an external JSON network node and the Software Bus. 
**
*/
#ifndef _jmsg_app_
#define _jmsg_app_

/*
** Includes
*/

#include "app_cfg.h"
#include "jmsg_lib_mgr.h"

/***********************/
/** Macro Definitions **/
/***********************/

/*
** Events
*/

#define JMSG_APP_INIT_APP_EID      (JMSG_APP_BASE_EID + 0)
#define JMSG_APP_NOOP_EID          (JMSG_APP_BASE_EID + 1)
#define JMSG_APP_EXIT_EID          (JMSG_APP_BASE_EID + 2)
#define JMSG_APP_INVALID_MID_EID   (JMSG_APP_BASE_EID + 3)


/**********************/
/** Type Definitions **/
/**********************/


/******************************************************************************
** Command Packets
*/
/* See EDS */

/******************************************************************************
** Telemetry Packets
*/
/* See EDS */

/******************************************************************************
** App Class
*/
typedef struct 
{

   /* 
   ** App Framework
   */ 

   INITBL_Class_t    IniTbl; 
   CFE_SB_PipeId_t   CmdPipe;
   CMDMGR_Class_t    CmdMgr;
   TBLMGR_Class_t    TblMgr;
      
   /*
   ** Telemetry Packets
   */
   
   JMSG_APP_StatusTlm_t   StatusTlm;

   
   /*
   ** JMSG_APP State & Contained Objects
   */ 
   
   uint32 PerfId;
   
   CFE_SB_MsgId_t  CmdMid;
   CFE_SB_MsgId_t  ExecuteMid;
     
   JMSG_LIB_MGR_Class_t  JMsgLibMgr;
   
} JMSG_APP_Class_t;


/*******************/
/** Exported Data **/
/*******************/

extern JMSG_APP_Class_t  JMsgApp;


/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function: JMSG_AppMain
**
*/
void JMSG_AppMain(void);


#endif /* _jmsg_app_ */
