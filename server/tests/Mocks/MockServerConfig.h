#ifndef MOCK_SERVER_CONFIG_H
#define MOCK_SERVER_CONFIG_H

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "IServerConfig.h"

class MockServerConfig : public IServerConfig {
    public:
	
    MOCK_METHOD(std::optional<ServerConnectData*>, GetAuthorizedServer, (const std::string &name), ());

};

#endif // MOCK_CHANNEL_H__