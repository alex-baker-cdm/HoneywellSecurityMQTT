#ifndef __MOCK_MQTT_H__
#define __MOCK_MQTT_H__

#include <string>
#include <vector>
#include <utility>

// Mock MQTT class for testing without actual MQTT broker
class MockMqtt
{
public:
    MockMqtt(const char *id, const char *host, int port, const char *username, 
             const char *password, const char *will_topic, const char *will_message) {}
    
    ~MockMqtt() {}
    
    bool send(const char *topic, const char *message) {
        messages.push_back(std::make_pair(std::string(topic), std::string(message)));
        return true;
    }
    
    bool set_will(const char *topic, const char *message) {
        return true;
    }
    
    // Test helper methods
    void clearMessages() {
        messages.clear();
    }
    
    size_t getMessageCount() const {
        return messages.size();
    }
    
    std::pair<std::string, std::string> getMessage(size_t index) const {
        if (index < messages.size()) {
            return messages[index];
        }
        return std::make_pair("", "");
    }
    
    bool hasMessage(const std::string &topic, const std::string &message) const {
        for (const auto &msg : messages) {
            if (msg.first == topic && msg.second == message) {
                return true;
            }
        }
        return false;
    }
    
private:
    std::vector<std::pair<std::string, std::string>> messages;
};

#endif
