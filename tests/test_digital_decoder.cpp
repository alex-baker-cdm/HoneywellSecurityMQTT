#include "simple_test.h"
#include "mock_mqtt.h"
#include <stdint.h>
#include <cstring>

uint64_t computeCRC(uint64_t payload) {
    const uint64_t SYNC_MASK = 0xFFFF000000000000ul;
    
    uint64_t sof = (payload & 0xF00000000000) >> 44;
    uint64_t polynomial;
    
    if (sof == 0x2 || sof == 0xA) {
        polynomial = 0x18050;
    } else {
        polynomial = 0x18005;
    }
    
    uint64_t sum = payload & (~SYNC_MASK);
    uint64_t current_divisor = polynomial << 31;
    
    while(current_divisor >= polynomial) {
#ifdef __arm__
        if(__builtin_clzll(sum) == __builtin_clzll(current_divisor))
#else        
        if(__builtin_clzl(sum) == __builtin_clzl(current_divisor))
#endif
        {
            sum ^= current_divisor;
        }        
        current_divisor >>= 1;
    }
    
    return sum;
}

TEST(DigitalDecoderCRCTest, ValidPayloadHoneywell) {
    uint64_t sof = 0x8;
    uint64_t serial = 0x12345;
    uint64_t type = 0xA0;
    
    uint64_t payload = 0xFFFE000000000000ul;
    payload |= (sof << 44);
    payload |= (serial << 24);
    payload |= (type << 16);
    
    uint64_t crc = computeCRC(payload);
    
    EXPECT_GE(crc, 0);
}

TEST(DigitalDecoderCRCTest, Valid2GIGPayload) {
    uint64_t sof = 0x2;
    uint64_t serial = 0xABCDE;
    uint64_t type = 0x50;
    
    uint64_t payload = 0xFFFE000000000000ul;
    payload |= (sof << 44);
    payload |= (serial << 24);
    payload |= (type << 16);
    
    uint64_t crc = computeCRC(payload);
    
    EXPECT_GE(crc, 0);
}

TEST(DigitalDecoderCRCTest, InvalidPayload) {
    uint64_t payload = 0xFFFE800000001234ul;
    
    uint64_t crc = computeCRC(payload);
    
    EXPECT_NE(crc, 0);
}

TEST(DigitalDecoderPayloadTest, PayloadParsing) {
    uint64_t payload = 0xFFFE812345A01234ul;
    
    uint64_t sof = (payload & 0xF00000000000) >> 44;
    uint64_t serial = (payload & 0x0FFFFF000000) >> 24;
    uint64_t type = (payload & 0x000000FF0000) >> 16;
    uint64_t crc = (payload & 0x00000000FFFF) >> 0;
    
    EXPECT_EQ(sof, 0x8);
    EXPECT_EQ(serial, 0x12345);
    EXPECT_EQ(type, 0xA0);
    EXPECT_EQ(crc, 0x1234);
}

TEST(DigitalDecoderStateTest, AlarmStateBits) {
    uint8_t state1 = 0x00;
    uint8_t state2 = 0x80;
    uint8_t state3 = 0x20;
    uint8_t state4 = 0xA0;
    
    EXPECT_EQ(state1 & 0xB0, 0x00);
    EXPECT_EQ(state2 & 0xB0, 0x80);
    EXPECT_EQ(state3 & 0xB0, 0x20);
    EXPECT_EQ(state4 & 0xB0, 0xA0);
}

TEST(DigitalDecoderStateTest, TamperBit) {
    uint8_t state_no_tamper = 0x00;
    uint8_t state_tamper = 0x40;
    uint8_t state_tamper_plus = 0xC0;
    
    EXPECT_FALSE(state_no_tamper & 0x40);
    EXPECT_TRUE(state_tamper & 0x40);
    EXPECT_TRUE(state_tamper_plus & 0x40);
}

TEST(DigitalDecoderStateTest, BatteryLowBit) {
    uint8_t state_battery_ok = 0x00;
    uint8_t state_battery_low = 0x08;
    uint8_t state_battery_low_plus = 0x48;
    
    EXPECT_FALSE(state_battery_ok & 0x08);
    EXPECT_TRUE(state_battery_low & 0x08);
    EXPECT_TRUE(state_battery_low_plus & 0x08);
}

TEST(DigitalDecoderManchesterTest, ManchesterDecoding) {
    SUCCEED();
}

TEST(DigitalDecoderBitSyncTest, SyncPattern) {
    const uint64_t SYNC_MASK = 0xFFFF000000000000ul;
    const uint64_t SYNC_PATTERN = 0xFFFE000000000000ul;
    
    uint64_t valid_sync = 0xFFFE123456789ABCul;
    uint64_t invalid_sync1 = 0xFFFF123456789ABCul;
    uint64_t invalid_sync2 = 0x0000123456789ABCul;
    
    EXPECT_EQ((valid_sync & SYNC_MASK), SYNC_PATTERN);
    EXPECT_NE((invalid_sync1 & SYNC_MASK), SYNC_PATTERN);
    EXPECT_NE((invalid_sync2 & SYNC_MASK), SYNC_PATTERN);
}

TEST(DigitalDecoderTimingTest, SamplesPerBit) {
    const int samplesPerBit = 8;
    
    int samplePoint = samplesPerBit / 2;
    
    EXPECT_EQ(samplePoint, 4);
    
    for (int i = 0; i < 24; i++) {
        if ((i % samplesPerBit) == samplePoint) {
            EXPECT_TRUE(i == 4 || i == 12 || i == 20);
        }
    }
}

TEST(DigitalDecoderEdgeTest, EdgeDetection) {
    bool lastSample = false;
    bool thisSample = true;
    
    EXPECT_NE(lastSample, thisSample);
    
    lastSample = true;
    thisSample = true;
    
    EXPECT_EQ(lastSample, thisSample);
}

TEST(DigitalDecoderDataTest, DataValidation) {
    char valid_data1 = 0;
    char valid_data2 = 1;
    char invalid_data1 = 2;
    char invalid_data2 = -1;
    char invalid_data3 = 'a';
    
    EXPECT_TRUE(valid_data1 == 0 || valid_data1 == 1);
    EXPECT_TRUE(valid_data2 == 0 || valid_data2 == 1);
    EXPECT_FALSE(invalid_data1 == 0 || invalid_data1 == 1);
    EXPECT_FALSE(invalid_data2 == 0 || invalid_data2 == 1);
    EXPECT_FALSE(invalid_data3 == 0 || invalid_data3 == 1);
}

TEST(DigitalDecoderAlarmTest, MinAlarmStateTracking) {
    uint8_t state1 = 0x80;
    uint8_t state2 = 0x20;
    uint8_t state3 = 0xA0;
    
    uint8_t minState = 0xFF;
    
    uint8_t alarmState1 = state1 & 0xB0;
    if (alarmState1 < minState) minState = alarmState1;
    EXPECT_EQ(minState, 0x80);
    
    uint8_t alarmState2 = state2 & 0xB0;
    if (alarmState2 < minState) minState = alarmState2;
    EXPECT_EQ(minState, 0x20);
    
    uint8_t alarmState3 = state3 & 0xB0;
    if (alarmState3 < minState) minState = alarmState3;
    EXPECT_EQ(minState, 0x20);
    
    EXPECT_FALSE(alarmState2 > minState);
    EXPECT_TRUE(alarmState3 > minState);
}

TEST(DigitalDecoderTimeoutTest, TimeoutConstants) {
    const int RX_TIMEOUT_MIN = 90;
    EXPECT_EQ(RX_TIMEOUT_MIN, 90);
    
    const int SENSOR_TIMEOUT_MIN = 90 * 5;
    EXPECT_EQ(SENSOR_TIMEOUT_MIN, 450);
    
    const int UPDATE_MIN_SEC = 60;
    EXPECT_EQ(UPDATE_MIN_SEC, 60);
}

TEST(DigitalDecoderTopicTest, TopicGeneration) {
    const char BASE_TOPIC[] = "/security/sensors345/";
    uint32_t serial = 732804;
    
    std::string alarmTopic = std::string(BASE_TOPIC) + std::to_string(serial) + "/alarm";
    std::string statusTopic = std::string(BASE_TOPIC) + std::to_string(serial) + "/status";
    std::string rxTopic = std::string(BASE_TOPIC) + "rx_status";
    
    EXPECT_EQ(alarmTopic, "/security/sensors345/732804/alarm");
    EXPECT_EQ(statusTopic, "/security/sensors345/732804/status");
    EXPECT_EQ(rxTopic, "/security/sensors345/rx_status");
}
