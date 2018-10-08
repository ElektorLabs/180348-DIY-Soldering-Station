#ifndef ENUMS_H_
    #define ENUMS_H_
/* Enums */
typedef enum {
  NOFAULT =0,
  TEMPSENS_FAIL,
  UNDERVOLTAGE,
  WAIT,
  RECOVER,
  POWERSAVE,
  POWERSAVE_WAIT,
  SLEEP,
  SLEEP_WAIT,
  WELCOME_LOGO,
  WELCOME_LOGO_WAIT,
  WELCOME_TITLE,
  WELCOME_TITLE_WAIT
} fsmstate_t;

typedef struct{
  bool overcurrent;
  uint16_t limit;
  uint16_t value;
} overcurrent_t;

#endif
