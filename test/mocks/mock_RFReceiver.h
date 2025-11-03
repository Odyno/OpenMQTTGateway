#include <gmock/gmock.h> // Brings in gMock.
#include <rf/RFReceiver.h> // Include the RFReceiver base class

class MockRFReceiver : public RFReceiver {
public:
  MOCK_METHOD(void, enable, (), (override));
  MOCK_METHOD(void, disable, (), (override));
  MOCK_METHOD(int, getReceiverID, (), (const, override));
};