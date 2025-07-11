#ifndef INC_APP_MAIN_H_
#define INC_APP_MAIN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h" // Incluye main.h, que a su vez incluye los headers de HAL

/**
  * @brief  Main application logic.
  * This function is called from main.c after basic system initialization.
  * @retval None
  */
void app_main(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_APP_MAIN_H_ */
