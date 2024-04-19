#ifndef COOS_H_
#define COOS_H_

#include "stdint.h"

void 	coos_init(void);
int32_t coos_task_add(void (* task_name)(), uint32_t phase, uint32_t period);
int32_t coos_task_delete(const uint8_t task_index);
void 	coos_dispatch(void);
void	coos_update(void);

#endif
