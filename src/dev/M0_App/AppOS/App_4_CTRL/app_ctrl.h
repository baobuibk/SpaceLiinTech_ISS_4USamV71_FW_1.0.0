/* ============================================================
 * app_ctrl.h
 * ============================================================ */
#ifndef APP_CTRL_H
#define APP_CTRL_H

#include "os.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const osThreadAttr_t ctrl_attr;
void App_CtrlTask(void *param);

#ifdef __cplusplus
}
#endif
#endif /* APP_CTRL_H */
