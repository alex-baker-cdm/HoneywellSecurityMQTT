#include "simple_test.h"
#include "../src/analogDecoder.h"
#include <vector>
#include <cmath>
#include <map>
#include <string>
#include <stdint.h>

class IntegrationTestHelper {
public:
    AnalogDecoder analogDecoder;
    std::vector<char> receivedBits;
    
    IntegrationTestHelper() {
        receivedBits.clear();
        analogDecoder.setCallback([this](char data) {
            receivedBits.push_back(data);
        });
    }
    
    std::vector<bool> manchesterEncode(const std::vector<bool>& bits) {
        std::vector<bool> encoded;
        for (bool bit : bits) {
            if (bit) {
                encoded.push_back(false);
                encoded.push_back(true);
            } else {
                encoded.push_back(true);
                encoded.push_back(false);
            }
        }
        return encoded;
    }
    
    std::vector<bool> payloadToBits(uint64_t payload, int numBits = 64) {
        std::vector<bool> bits;
        for (int i = numBits - 1; i >= 0; i--) {
            bits.push_back((payload >> i) & 1);
        }
        return bits;
    }
    
    void sendBitStream(const std::vector<bool>& bits) {
        for (bool bit : bits) {
            float magnitude = bit ? 1.0f : 0.0f;
            for (int i = 0; i < 17; i++) {
                analogDecoder.handleMagnitude(magnitude);
            }
        }
    }
    
    void sendPacket(uint64_t payload) {
        std::vector<bool> bits = payloadToBits(payload);
        std::vector<bool> manchesterBits = manchesterEncode(bits);
        sendBitStream(manchesterBits);
    }
    
    void sendPreamble() {
        for (int i = 0; i < 32; i++) {
            sendBitStream({true, false});
        }
    }
};

TEST(IntegrationTest, AnalogToDigitalConversion) {
    IntegrationTestHelper helper;
    
    uint64_t testPayload = 0xFFFE812345A00000ul;
    
    helper.sendPreamble();
    helper.sendPacket(testPayload);
    
    EXPECT_GT(helper.receivedBits.size(), 0);
}

TEST(IntegrationTest, ManchesterEncodingDecoding) {
    IntegrationTestHelper helper;
    
    std::vector<bool> testBits = {true, false, true, true, false, false, true, false};
    std::vector<bool> manchesterBits = helper.manchesterEncode(testBits);
    
    helper.sendBitStream(manchesterBits);
    
    EXPECT_EQ(helper.receivedBits.size(), manchesterBits.size());
    
    for (size_t i = 0; i < manchesterBits.size() && i < helper.receivedBits.size(); i++) {
        EXPECT_EQ(helper.receivedBits[i], manchesterBits[i] ? 1 : 0);
    }
}

TEST(IntegrationTest, SyncPatternDetection) {
    IntegrationTestHelper helper;
    
    uint64_t syncPattern = 0xFFFE000000000000ul;
    
    helper.sendPreamble();
    helper.sendPacket(syncPattern);
    
    EXPECT_GT(helper.receivedBits.size(), 64);
}

TEST(IntegrationTest, MultiplePacketProcessing) {
    IntegrationTestHelper helper;
    
    uint64_t packet1 = 0xFFFE812345A00000ul;
    uint64_t packet2 = 0xFFFE822222B00000ul;
    
    helper.sendPreamble();
    helper.sendPacket(packet1);
    
    size_t firstPacketBits = helper.receivedBits.size();
    
    helper.sendPreamble();
    helper.sendPacket(packet2);
    
    EXPECT_GT(helper.receivedBits.size(), firstPacketBits);
}

TEST(IntegrationTest, SignalStrengthVariation) {
    IntegrationTestHelper helper;
    
    std::vector<bool> testBits = {true, false, true, true};
    std::vector<bool> manchesterBits = helper.manchesterEncode(testBits);
    
    for (bool bit : manchesterBits) {
        float magnitude = bit ? 0.8f : 0.0f;
        for (int i = 0; i < 17; i++) {
            helper.analogDecoder.handleMagnitude(magnitude);
        }
    }
    
    EXPECT_EQ(helper.receivedBits.size(), manchesterBits.size());
}

TEST(IntegrationTest, NoiseFiltering) {
    IntegrationTestHelper helper;
    
    for (int i = 0; i < 100; i++) {
        helper.analogDecoder.handleMagnitude(0.1f);
    }
    
    size_t noiseBits = helper.receivedBits.size();
    
    std::vector<bool> validBits = {true, true, true, true};
    std::vector<bool> manchesterBits = helper.manchesterEncode(validBits);
    helper.sendBitStream(manchesterBits);
    
    EXPECT_GT(helper.receivedBits.size(), noiseBits);
}

TEST(IntegrationTest, DecimationRatioIntegration) {
    IntegrationTestHelper helper;
    
    for (int i = 0; i < 16; i++) {
        helper.analogDecoder.handleMagnitude(1.0f);
    }
    EXPECT_EQ(helper.receivedBits.size(), 0);
    
    helper.analogDecoder.handleMagnitude(1.0f);
    EXPECT_EQ(helper.receivedBits.size(), 1);
}

TEST(IntegrationTest, ThresholdAdaptationIntegration) {
    IntegrationTestHelper helper;
    
    for (int i = 0; i < 17; i++) {
        helper.analogDecoder.handleMagnitude(1.0f);
    }
    EXPECT_EQ(helper.receivedBits.back(), 1);
    
    for (int i = 0; i < 17; i++) {
        helper.analogDecoder.handleMagnitude(0.8f);
    }
    EXPECT_EQ(helper.receivedBits.back(), 1);
    
    for (int i = 0; i < 17; i++) {
        helper.analogDecoder.handleMagnitude(0.1f);
    }
    EXPECT_EQ(helper.receivedBits.back(), 0);
}

TEST(IntegrationTest, ContinuousDataStream) {
    IntegrationTestHelper helper;
    
    for (int packet = 0; packet < 3; packet++) {
        uint64_t payload = 0xFFFE000000000000ul | (packet << 24);
        helper.sendPreamble();
        helper.sendPacket(payload);
    }
    
    EXPECT_GT(helper.receivedBits.size(), 192);
}

TEST(IntegrationTest, AlternatingPatterns) {
    IntegrationTestHelper helper;
    
    std::vector<bool> alternating;
    for (int i = 0; i < 32; i++) {
        alternating.push_back(i % 2 == 0);
    }
    
    std::vector<bool> manchesterBits = helper.manchesterEncode(alternating);
    helper.sendBitStream(manchesterBits);
    
    EXPECT_EQ(helper.receivedBits.size(), manchesterBits.size());
}

TEST(IntegrationTest, LongPacketProcessing) {
    IntegrationTestHelper helper;
    
    uint64_t longPayload = 0xFFFEFFFFFFFFFFFFul;
    
    helper.sendPreamble();
    helper.sendPacket(longPayload);
    
    EXPECT_GE(helper.receivedBits.size(), 64);
}

TEST(IntegrationTest, CallbackIntegration) {
    AnalogDecoder decoder;
    int callbackCount = 0;
    
    decoder.setCallback([&callbackCount](char data) {
        callbackCount++;
    });
    
    for (int i = 0; i < 34; i++) {
        decoder.handleMagnitude(1.0f);
    }
    
    EXPECT_EQ(callbackCount, 2);
}
