#include <gtest/gtest.h>

#include "protocol.h"

TEST(PROTOCOL, CheckSizeOfPacket) { EXPECT_EQ(sizeof(Packet), 32U); }
