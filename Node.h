#ifndef node_h
#define node_h

#include <RH_RF69.h>
#include <RHReliableDatagram.h>
#include <NodeProtocol.h>
#include <Data.h>

#define NUMBER_OF_FUNCTIONS 16

#define CONFIG_GATEWAY      0
#define CONFIG_TXPOWER      15
#define CONFIG_FREQUENCY    433
#define CONFIG_TIMEOUT      12
#define CONFIG_RETRIES      3
#define CONFIG_RADIOCONFIG  _radio.GFSK_Rb125Fd125
#define CONFIG_KEY          {0x02, 0x4f, 0x82, 0xa9, 0x0b, 0x49, 0xcd, 0x36, 0xe7, 0x23, 0x4d, 0x60, 0xfa, 0xb7, 0x19, 0x88}

#define LIPO_3800           3800
#define LIPO_3750           3750
#define LIPO_3700           3700

#define MAXVOLTAGE          13180

#define PACKET_MESSAGE      0x00
#define ARRAY_MESSAGE       0x01


class Node : public RHReliableDatagram
{
  public:
    Node(uint8_t address, uint8_t pin = 0, uint16_t maximum = MAXVOLTAGE);

    bool init();
    bool hello(void);

    bool send(uint8_t type, uint8_t num = 0, int32_t data = 0, uint8_t to = CONFIG_GATEWAY);
    bool sendArray(uint8_t* buf, uint8_t len, uint8_t to = CONFIG_GATEWAY);

    bool packetAvailable(void);
    bool receive(uint8_t* type = NULL, uint8_t* num = NULL, int32_t* data = NULL, uint8_t* from = NULL, uint16_t* volt = NULL, int16_t* temp = NULL, int16_t* rssi = NULL);

    bool arrayAvailable(void);
    bool receiveArray(uint8_t* buf, uint8_t* len, uint8_t* from = NULL, int16_t* rssi = NULL);

    uint8_t from(void);
    uint8_t to(void);
    uint8_t flags(void);
    uint32_t reTx(void);
    int8_t rssi(void);
    int8_t lastRssi(void);
    int8_t temperature(void);

    bool addFunction(uint8_t(*)(uint8_t, int32_t), uint8_t type);
    void check(void);

    uint16_t voltage(void);
    bool lowVoltage(uint16_t minimum = LIPO_3750);

    void sleep(void);

    int32_t twoToOne(int16_t one, int16_t two);
    int32_t threeToOne(uint8_t one, uint8_t two, uint8_t three);
    void oneToTwo(int32_t data, int16_t* one, int16_t* two);
    void oneToThree(int32_t data, uint8_t* one, uint8_t* two, uint8_t* three);

  private:
    RH_RF69 _radio;
    uint8_t _types[NUMBER_OF_FUNCTIONS] = {0};
    uint8_t (*callback[NUMBER_OF_FUNCTIONS])(uint8_t, int32_t);
    uint8_t _functionCounter;

    bool nodeFunction(uint8_t type, int32_t data);

    void setFlags(uint8_t flags);

    void setNewTxPower(int16_t power = CONFIG_TXPOWER);
    int16_t getTxPower(void);

    void setNewRetries(uint8_t retries = CONFIG_RETRIES);
    uint8_t getRetries(void);

    void setNewTimeout(uint8_t timeout = CONFIG_TIMEOUT);
    uint8_t getTimeout(void);

    uint8_t _thisAddress;
    uint8_t _voltagePin;
    uint16_t _maximum = MAXVOLTAGE;
    bool _lowVoltWarning = false;

    int16_t _power;
    uint8_t _retries;
    uint8_t _timeout;

    typedef struct
    {
      uint8_t    type;
      uint8_t    num;
      int32_t    data;
      uint16_t   volt;
      int16_t    temp;
    } _packet;
};

#endif