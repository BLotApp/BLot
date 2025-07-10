#pragma once

#include "../../src/AddonBase.h"
#include <string>
#include <vector>
#include <functional>
#include <memory>

// OSC message structure
struct OscMessage {
    std::string address;
    std::vector<std::string> arguments;
    std::string host;
    int port;
    
    OscMessage(const std::string& addr = "") : address(addr) {}
    
    void addStringArg(const std::string& arg);
    void addIntArg(int arg);
    void addFloatArg(float arg);
    void addBoolArg(bool arg);
    
    std::string getStringArg(int index) const;
    int getIntArg(int index) const;
    float getFloatArg(int index) const;
    bool getBoolArg(int index) const;
    
    int getNumArgs() const { return arguments.size(); }
};

class bxOsc : public AddonBase {
public:
    bxOsc();
    ~bxOsc();
    
    // AddonBase implementation
    bool init() override;
    void setup() override;
    void update(float deltaTime) override;
    void draw() override;
    void cleanup() override;
    
    // OSC setup
    void setup(const std::string& host, int port);
    void setupSender(const std::string& host, int port);
    void setupReceiver(int port);
    
    // Message sending
    void sendMessage(const OscMessage& message);
    void sendMessage(const std::string& address, const std::string& arg);
    void sendMessage(const std::string& address, int arg);
    void sendMessage(const std::string& address, float arg);
    void sendMessage(const std::string& address, bool arg);
    
    // Message receiving
    bool hasWaitingMessages() const;
    OscMessage getNextMessage();
    std::vector<OscMessage> getWaitingMessages();
    
    // Event handling
    void onMessageReceived(const std::string& address, std::function<void(const OscMessage&)> callback);
    void onMessageReceived(std::function<void(const OscMessage&)> callback);
    
    // Utility
    void setVerbose(bool verbose);
    bool isConnected() const;
    std::string getHost() const { return m_host; }
    int getPort() const { return m_port; }
    
private:
    std::string m_host;
    int m_port;
    bool m_connected;
    bool m_verbose;
    
    // Message queues
    std::vector<OscMessage> m_incomingMessages;
    std::vector<OscMessage> m_outgoingMessages;
    
    // Event callbacks
    std::unordered_map<std::string, std::function<void(const OscMessage&)>> m_addressCallbacks;
    std::vector<std::function<void(const OscMessage&)>> m_generalCallbacks;
    
    // Network simulation (in a real implementation, this would use actual OSC libraries)
    void simulateNetworkReceive();
    void simulateNetworkSend();
};

// OSC Sender class for sending messages
class OscSender {
public:
    OscSender();
    ~OscSender();
    
    void setup(const std::string& host, int port);
    void sendMessage(const OscMessage& message);
    bool isConnected() const;
    
private:
    std::string m_host;
    int m_port;
    bool m_connected;
};

// OSC Receiver class for receiving messages
class OscReceiver {
public:
    OscReceiver();
    ~OscReceiver();
    
    void setup(int port);
    bool hasWaitingMessages() const;
    OscMessage getNextMessage();
    std::vector<OscMessage> getWaitingMessages();
    
private:
    int m_port;
    bool m_listening;
    std::vector<OscMessage> m_messages;
}; 