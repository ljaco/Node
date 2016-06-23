#ifndef data_h
#define data_h

enum data
{
  // data to states conversion
  OFF                 = 0,
  ON                  = 1,
  TOGGLE              = 2,

  PUSH                = 1,
  RELEASE             = 0,

  PUSHED              = 1,
  RELEASED            = 0,

  // predefined servo positions
  POS0                = 200,
  POS1                = 201,
  POS2                = 202,
  POS3                = 203,
  POS4                = 204,

  // increase/decrease pwm, servo
  TOGGLE_PWM          = 256,
  INCREASE_SLOW       = 257,
  DECREASE_SLOW       = 258,
  INCREASE_FAST       = 259,
  DECREASE_FAST       = 260,

  // access all connected num of one type at the same time
  ALL_NUM             = 255
};

#endif