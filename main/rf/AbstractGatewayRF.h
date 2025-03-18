#ifndef IGATEWAYRF_H
#define IGATEWAYRF_H

#include <ArduinoJson.h>

#include "./ZCommonRF.h"

class AbstractGatewayRF {
public:
  AbstractGatewayRF(ZCommonRF& iZCommonRF) : rfHandler(iZCommonRF) {};

  virtual ~AbstractGatewayRF() {};

  virtual bool disableReceive() = 0;

  virtual bool enableReceive() = 0;

  virtual void RFtoX() = 0;

#if simpleReceiving
  virtual void XtoRF(const char* topicOri, const char* datacallback) = 0;
#endif

#if jsonReceiving
  virtual void XtoRF(const char* topicOri, JsonObject& RFdata) = 0;
#endif

  void setRFHandler(ZCommonRF& rfHandler) {
    this->rfHandler = rfHandler;
  }

protected:
  ZCommonRF& rfHandler;
};

#endif // IGATEWAYRF_H