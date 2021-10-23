#include "stm8s.h"

void stepper_next();
void stepper_resetAll();
void stepper_init();
void stepper_tim5_tick();

void applyCurrentValue(uint16_t value);
void stepper_setThreshold(uint8_t value);
void stepper_setEnabled(bool value);