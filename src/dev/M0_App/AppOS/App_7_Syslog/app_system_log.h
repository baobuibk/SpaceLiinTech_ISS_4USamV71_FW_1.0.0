/* 
 * File:   app_system_log.h
 * Author: HTSANG
 *
 * Created on May 28, 2026, 4:40 PM
 */

#ifndef APP_SYSTEM_LOG_H
#define	APP_SYSTEM_LOG_H

#include "os.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "M4_Middlewares/LWL/lwl.h"

extern const osThreadAttr_t syslog_attr;

void syslog_init(void);
void App_SysLog(void *param);

#endif	/* APP_SYSTEM_LOG_H */

