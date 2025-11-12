#include "simple_test.h"
#include "../src/analogDecoder.h"
#include <vector>

class AnalogDecoderTestHelper {
public:
    AnalogDecoder decoder;
    std::vector<char> receivedData;
    
    AnalogDecoderTestHelper() {
        receivedData.clear();
        decoder.setCallback([this](char data) {
            receivedData.push_back(data);
        });
    }
};

TEST(AnalogDecoderTest, DecimationRatio) {
    AnalogDecoderTestHelper helper;
    
    for (int i = 0; i < 16; i++) {
        helper.decoder.handleMagnitude(1.0f);
    }
    EXPECT_EQ(helper.receivedData.size(), 0);
    
    helper.decoder.handleMagnitude(1.0f);
    EXPECT_EQ(helper.receivedData.size(), 1);
    
    for (int i = 0; i < 16; i++) {
        helper.decoder.handleMagnitude(1.0f);
    }
    EXPECT_EQ(helper.receivedData.size(), 1);
    
    helper.decoder.handleMagnitude(1.0f);
    EXPECT_EQ(helper.receivedData.size(), 2);
}

TEST(AnalogDecoderTest, HighMagnitudeProducesOne) {
    AnalogDecoderTestHelper helper;
    for (int i = 0; i < 17; i++) {
        helper.decoder.handleMagnitude(1.0f);
    }
    
    ASSERT_EQ(helper.receivedData.size(), 1);
    EXPECT_EQ(helper.receivedData[0], 1);
}

TEST(AnalogDecoderTest, LowMagnitudeProducesZero) {
    AnalogDecoderTestHelper helper;
    for (int i = 0; i < 17; i++) {
        helper.decoder.handleMagnitude(0.0f);
    }
    
    ASSERT_EQ(helper.receivedData.size(), 1);
    EXPECT_EQ(helper.receivedData[0], 0);
}

TEST(AnalogDecoderTest, ThresholdAdaptation) {
    AnalogDecoderTestHelper helper;
    for (int i = 0; i < 17; i++) {
        helper.decoder.handleMagnitude(1.0f);
    }
    EXPECT_EQ(helper.receivedData.back(), 1);
    
    for (int i = 0; i < 17; i++) {
        helper.decoder.handleMagnitude(0.8f);
    }
    EXPECT_EQ(helper.receivedData.back(), 1);
    
    for (int i = 0; i < 17; i++) {
        helper.decoder.handleMagnitude(0.1f);
    }
    EXPECT_EQ(helper.receivedData.back(), 0);
}

TEST(AnalogDecoderTest, SaturationHandling) {
    AnalogDecoderTestHelper helper;
    for (int i = 0; i < 17; i++) {
        helper.decoder.handleMagnitude(2.0f);
    }
    
    ASSERT_EQ(helper.receivedData.size(), 1);
    EXPECT_EQ(helper.receivedData[0], 1);
}

TEST(AnalogDecoderTest, SmoothingFilter) {
    AnalogDecoderTestHelper helper;
    
    for (int i = 0; i < 34; i++) {
        helper.decoder.handleMagnitude(0.0f);
    }
    size_t zeroCount = helper.receivedData.size();
    
    for (int i = 0; i < 17; i++) {
        helper.decoder.handleMagnitude(1.0f);
    }
    
    for (int i = 0; i < 17; i++) {
        helper.decoder.handleMagnitude(0.0f);
    }
    
    EXPECT_GT(helper.receivedData.size(), zeroCount);
}

TEST(AnalogDecoderTest, OOKDecay) {
    AnalogDecoderTestHelper helper;
    for (int i = 0; i < 34; i++) {
        helper.decoder.handleMagnitude(1.0f);
    }
    
    for (int i = 0; i < 170; i++) {
        helper.decoder.handleMagnitude(0.6f);
    }
    
    EXPECT_GT(helper.receivedData.size(), 2);
}

TEST(AnalogDecoderTest, MinimumThreshold) {
    AnalogDecoderTestHelper helper;
    
    for (int i = 0; i < 17; i++) {
        helper.decoder.handleMagnitude(0.01f);
    }
    EXPECT_EQ(helper.receivedData.back(), 0);
    
    for (int i = 0; i < 17; i++) {
        helper.decoder.handleMagnitude(0.3f);
    }
    EXPECT_EQ(helper.receivedData.back(), 1);
}

TEST(AnalogDecoderNoCallbackTest, NoCallbackDoesNotCrash) {
    AnalogDecoder decoder;
    for (int i = 0; i < 20; i++) {
        decoder.handleMagnitude(1.0f);
    }
    SUCCEED();
}
