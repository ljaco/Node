#include <Node.h>
#include <avr/wdt.h>

Node::Node(uint8_t thisAddress, uint8_t pin, uint16_t maximum)
  :  RHReliableDatagram(_radio, thisAddress)
{
  _thisAddress = thisAddress;
  _voltagePin = pin;
  _maximum = maximum;
  _functionCounter = 0;
}

bool Node::init()
{
  bool state = RHReliableDatagram::init();
  uint8_t key [16] = CONFIG_KEY;
  _radio.setEncryptionKey(key);
  _radio.setModemConfig(CONFIG_RADIOCONFIG);
  _radio.setFrequency(CONFIG_FREQUENCY);
  setThisAddress(_thisAddress);
  setNewTxPower();
  setNewTimeout();
  setNewRetries();
  return state;
}

bool Node::hello(void)
{
  return send(HELLO);
}

bool Node::send(uint8_t type, uint8_t num, int32_t data, uint8_t to)
{
  _packet packet;
  packet.type = type;
  packet.num = num;
  packet.data = data;
  packet.volt = voltage();
  _radio.setModeIdle();
  packet.temp = temperature();
  uint8_t message [sizeof(packet)];
  memcpy(message, &packet, sizeof(packet));
  setFlags(PACKET_MESSAGE);
  return sendtoWait(message, sizeof(message), to);
}

bool Node::sendArray(uint8_t* buf, uint8_t len, uint8_t to)
{
  setFlags(ARRAY_MESSAGE);
  return sendtoWait(buf, len, to);
}

bool Node::packetAvailable(void)
{
  return (available() && ((flags() & RH_FLAGS_APPLICATION_SPECIFIC) == PACKET_MESSAGE));
}

bool Node::receive(uint8_t* type, uint8_t* num, int32_t* data, uint8_t* from, uint16_t* volt, int16_t* temp, int16_t* rssi)
{
  if (packetAvailable())
  {
    _packet packet;
    uint8_t message [sizeof(packet)];
    uint8_t len = sizeof(message);
    uint8_t fromAddress;
    if (recvfromAck(message, &len, &fromAddress))
    {
      if (len == sizeof(packet))
      {
        memcpy(&packet, message, sizeof(message));
        if (nodeFunction(packet.type, packet.data))
          return false;  // data received but was nodeFunction
        if (type)
        {
          *type = packet.type;
          if (num)  *num  = packet.num;
          if (data) *data = packet.data;
          if (from) *from = fromAddress;
          if (volt) *volt = packet.volt;
          if (temp) *temp = packet.temp;
          if (rssi) *rssi = _radio.lastRssi();
          return true;  // data received and saved
        }
      }
    }
  }
  return false;  // no data received
}

bool Node::arrayAvailable(void)
{
  return (available() && ((flags() & RH_FLAGS_APPLICATION_SPECIFIC) == ARRAY_MESSAGE));
}

bool Node::receiveArray(uint8_t* buf, uint8_t* len, uint8_t* from, int16_t* rssi)
{
  if (arrayAvailable())
  {
    if (recvfromAck(buf, len, from))
    {
      if (rssi) *rssi = _radio.lastRssi();
      return true;  // data received and saved
    }
  }
  return false;  // no data received
}

uint8_t Node::from(void)
{
  return _radio.headerFrom();
}
uint8_t Node::to(void)
{
  return _radio.headerTo();
}
uint8_t Node::flags(void)
{
  return _radio.headerFlags();
}
uint32_t Node::reTx(void)
{
  return RHReliableDatagram::retransmissions();
}
int8_t Node::rssi(void)
{
  return _radio.rssiRead();
}
int8_t Node::lastRssi(void)
{
  return _radio.lastRssi();
}
int8_t Node::temperature(void)
{
  return _radio.temperatureRead();
}

bool Node::addFunction(uint8_t (*function)(uint8_t, int32_t), uint8_t type)
{
  if (_functionCounter < NUMBER_OF_FUNCTIONS)
  {
    callback[_functionCounter] = function;
    _types[_functionCounter] = type;
    _functionCounter++;
    return true;
  }
  else
    return false;
}

void Node::check()
{
  if (packetAvailable())
{
  uint8_t type;
  uint8_t num;
  int32_t data;
  if (receive(&type, &num, &data))
    {
      for (uint8_t i = 0; i < _functionCounter; ++i)
      {
        if (_types[i] == type)
        {
          uint8_t status = callback[i](num, data);
          switch (status)
          {
            case NUM_ERROR:
              send(NUM_ERROR, num, data);
              return;

            case DATA_ERROR:
              send(DATA_ERROR, num, data);
              return;

            case OK:
              return;

            default:
              return;
          }
        }
      }
      send(TYPE_ERROR, num, type);  // no registered function found that matches
      return;
    }
  }
}

uint16_t Node::voltage()
{
  if (_voltagePin != 0)
    return map(analogRead(_voltagePin), 0, 1023, 0, _maximum);
  else
    return 0;
}

bool Node::lowVoltage(uint16_t minimum)
{
  int16_t volt = voltage();
  int16_t delta = minimum - volt;
  if (delta >= 0 && delta <= 2000)
  {
    if (!_lowVoltWarning)
    {
      send(LOW_VOLTAGE, 0, volt);
      _lowVoltWarning = true;
    }
    return true;
  }
  else if (delta <= -100)
  {
    _lowVoltWarning = false;
    return false;
  }
  return _lowVoltWarning;
}

void Node::sleep(void)
{
  _radio.sleep();
}

bool Node::nodeFunction(uint8_t type, int32_t data)
{
  switch (type)
  {
    case RESET:
      wdt_enable(WDTO_15MS);
      delay(1000);
      return true;

    case GET_GOOD:
      send(GOOD, 0, _radio.txGood());
      return true;

    case GET_BAD:
      send(TYPE_ERROR);
      return true;

    case GET_RETX:
      send(RETX, 0, retransmissions());
      return true;

    case GET_TOTAL:
      send(TYPE_ERROR);
      return true;

    case GET_PING:
      send(PING_ANSWER, 0, _radio.lastRssi());
      return true;

    case GET_MILLIS:
      send(MILLIS, 0, millis());
      return true;

    case SET_TXPOWER:
      setNewTxPower(data);
      //send(TXPOWER, 0, getTxPower());
      return true;

    case GET_TXPOWER:
      send(TXPOWER, 0, getTxPower());
      return true;

    case SET_RETRIES:
      setNewRetries(data);
      //send(RETRIES, 0, getRetries());
      return true;

    case GET_RETRIES:
      send(RETRIES, 0, getRetries());
      return true;

    case SET_TIMEOUT:
      setNewTimeout(data);
      //send(TIMEOUT, 0, getTimeout());
      return true;

    case GET_TIMEOUT:
      send(TIMEOUT, 0, getTimeout());
      return true;

    default:
      return false;
  }
}

void Node::setFlags(uint8_t flags)
{
  _radio.setHeaderFlags(flags, RH_FLAGS_APPLICATION_SPECIFIC);
}

void Node::setNewTxPower(int16_t power)
{
  if (power < -18)
    power = -18;
  if (power > 20)
    power = 20;
  _power = power;
  _radio.setTxPower(_power);
}

int16_t Node::getTxPower(void)
{
  return _power;
}

void Node::setNewRetries(uint8_t retries)
{
  _retries = retries;
  setRetries(_retries);
}

uint8_t Node::getRetries(void)
{
  return _retries;
}

void Node::setNewTimeout(uint8_t timeout)
{
  _timeout = timeout;
  setTimeout(_timeout);
}

uint8_t Node::getTimeout(void)
{
  return _timeout;
}

int32_t Node::twoToOne(int16_t one, int16_t two)
{
  union
  {
    int16_t input[2];
    int32_t output;
  } shift;

  shift.input[0] = two;
  shift.input[1] = one;
  return shift.output;
}

void Node::oneToTwo(int32_t data, int16_t* one, int16_t* two)
{
  union
  {
    int16_t output[2];
    int32_t input;
  } shift;

  shift.input = data;
  *two = shift.output[0];
  *one = shift.output[1];
}

int32_t Node::threeToOne(uint8_t one, uint8_t two, uint8_t three)
{
  union
  {
    int8_t input[3];
    int32_t output;
  } shift;

  shift.input[0] = three;
  shift.input[1] = two;
  shift.input[2] = one;
  return shift.output;
}

void Node::oneToThree(int32_t data, uint8_t* one, uint8_t* two, uint8_t* three)
{
  union
  {
    int8_t output[3];
    int32_t input;
  } shift;

  shift.input = data;
  *three = shift.output[0];
  *two   = shift.output[1];
  *one   = shift.output[2];;
}
