#ifndef node_protocol_simple_h
#define node_protocol_simple_h

enum type
{
  // MQTT --> NODE   0 - 127 (128)
  // NODE --> MQTT 128 - 255 (128) (some errors both way)

  // SET 0 - 59 (60)

  // User functions
  SET_DO              = 0,
  SET_PWM             = 1,
  SET_RGB             = 4,
  SET_SERVO           = 5,
  SEND_IRCODE         = 6,

  // Node functions
  RESET               = 59,


  // GET 60 - 127 (76)

  // User functions
  GET_DO              = 60,
  GET_PWM             = 61,
  GET_DI              = 64,
  GET_AI              = 65,
  GET_TEMPERATURE     = 66,
  GET_HUMIDITY        = 67,
  GET_LIGHT           = 68,
  GET_DISTANCE        = 69,

  // Node functions
  GET_PING            = 119,
  GET_RETRIES         = 120,
  GET_TIMEOUT         = 121,
  GET_TXPOWER         = 122,
  GET_GOOD            = 123,
  GET_BAD             = 124,
  GET_RETX            = 125,
  GET_TOTAL           = 126,
  GET_MILLIS          = 127,


  // DATA 128 - 199 (72)
  DO                  = 128,
  PWM                 = 129,
  DI                  = 130,
  AI                  = 131,
  VIBRATION           = 136,

  NOISE               = 137,
  DISTANCE            = 139,
  IRCODE              = 141,
  LIGHT               = 142,

  TEMPERATURE         = 143,
  HUMIDITY            = 144,
  TEMP_HUM            = 145,
  MOISTURE            = 146,
  GAS                 = 147,
  PRESSURE            = 148,
  AIR_PRESSURE        = 149,
  ALTITUDE            = 150,

  WATTHOUR            = 151,
  WATTHOURTOTAL       = 152,
  WATT                = 153,
  WATT_WATTHOUR       = 154,
  WATT_WATTHOURTOTAL  = 155,

  IMPULSES            = 156,
  PERCENTAGE          = 157,

  ONE_DATA_FIELD      = 197,
  TWO_DATA_FIELD      = 198,
  THREE_DATA_FIELD    = 199,

  // NODE DATA 200 - 229 (30)
  HELLO               = 200,
  PING_ANSWER         = 201,
  HEARTBEAT           = 202,
  LOW_VOLTAGE         = 203,
  GOOD                = 204,
  BAD                 = 205,
  RETX                = 206,
  TOTAL               = 207,
  RETRIES             = 208,
  TIMEOUT             = 209,
  TXPOWER             = 210,
  MILLIS              = 211,

  // ERROR 230 - 255 (26)
  OK                  = 230,  // no error occurred
  NODE_ERROR          = 231,  // node not reached
  TYPE_ERROR          = 232,  // type not available on node (TYPE_ERROR, num, type)
  NUM_ERROR           = 233,  // number not available on node (NUM_ERROR, num, data)
  DATA_ERROR          = 234,  // data invalid for type choosen (DATA_ERROR, num, data)

  MESSAGE_ERROR       = 235,  // MQTT message content wrong
  TYPE_NOT_FOUND      = 236,  // type could not be converted to number
  DATA_NOT_FOUND      = 237   // data could not be converted to state
};


#endif
