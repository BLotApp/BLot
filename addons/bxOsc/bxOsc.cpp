#include "bxOsc.h"
#include <spdlog/spdlog.h>

namespace blot {

// OscMessage implementation
void OscMessage::addStringArg(const std::string &arg) {
	arguments.push_back("s:" + arg);
}

void OscMessage::addIntArg(int arg) {
	arguments.push_back("i:" + std::to_string(arg));
}

void OscMessage::addFloatArg(float arg) {
	arguments.push_back("f:" + std::to_string(arg));
}

void OscMessage::addBoolArg(bool arg) {
	arguments.push_back("b:" + std::to_string(arg));
}

std::string OscMessage::getStringArg(int index) const {
	if (index >= 0 && index < static_cast<int>(arguments.size())) {
		const std::string &arg = arguments[index];
		if (arg.length() > 2 && arg[1] == ':') {
			return arg.substr(2);
		}
	}
	return "";
}

int OscMessage::getIntArg(int index) const {
	if (index >= 0 && index < static_cast<int>(arguments.size())) {
		const std::string &arg = arguments[index];
		if (arg.length() > 2 && arg[0] == 'i' && arg[1] == ':') {
			return std::stoi(arg.substr(2));
		}
	}
	return 0;
}

float OscMessage::getFloatArg(int index) const {
	if (index >= 0 && index < static_cast<int>(arguments.size())) {
		const std::string &arg = arguments[index];
		if (arg.length() > 2 && arg[0] == 'f' && arg[1] == ':') {
			return std::stof(arg.substr(2));
		}
	}
	return 0.0f;
}

bool OscMessage::getBoolArg(int index) const {
	if (index >= 0 && index < static_cast<int>(arguments.size())) {
		const std::string &arg = arguments[index];
		if (arg.length() > 2 && arg[0] == 'b' && arg[1] == ':') {
			return arg.substr(2) == "1";
		}
	}
	return false;
}

// bxOsc implementation
bxOsc::bxOsc() : blot::IAddon("bxOsc", "1.0.0") {
	m_host = "localhost";
	m_port = 8000;
	m_connected = false;
	m_verbose = false;
}

bxOsc::~bxOsc() { cleanup(); }

bool bxOsc::init() {
	spdlog::info("[bxOsc] Initializing OSC addon");
	m_initialized = true;
	return true;
}

void bxOsc::setup() { spdlog::info("[bxOsc] Setting up OSC addon"); }

void bxOsc::update(float deltaTime) {
	// Process incoming messages
	simulateNetworkReceive();

	// Process outgoing messages
	simulateNetworkSend();
}

void bxOsc::draw() {
	// OSC addon doesn't need to draw anything
}

void bxOsc::cleanup() {
	if (m_connected) {
		spdlog::info("[bxOsc] Disconnecting from {}:{}", m_host, m_port);
		m_connected = false;
	}
}

void bxOsc::setupOsc(const std::string &host, int port) {
	m_host = host;
	m_port = port;
	spdlog::info("[bxOsc] Setting up OSC connection to {}:{}", host, port);
}

void bxOsc::setupSender(const std::string &host, int port) {
	m_host = host;
	m_port = port;
	m_connected = true;
	spdlog::info("[bxOsc] Setting up OSC sender to {}:{}", host, port);
}

void bxOsc::setupReceiver(int port) {
	m_port = port;
	m_connected = true;
	spdlog::info("[bxOsc] Setting up OSC receiver on port {}", port);
}

void bxOsc::sendMessage(const OscMessage &message) {
	if (!m_connected) {
		spdlog::warn("[bxOsc] Not connected, cannot send message");
		return;
	}

	m_outgoingMessages.push_back(message);
	if (m_verbose) {
		spdlog::info("[bxOsc] Queued message: {} with {} args", message.address,
					 message.getNumArgs());
	}
}

void bxOsc::sendMessage(const std::string &address, const std::string &arg) {
	OscMessage msg(address);
	msg.addStringArg(arg);
	sendMessage(msg);
}

void bxOsc::sendMessage(const std::string &address, int arg) {
	OscMessage msg(address);
	msg.addIntArg(arg);
	sendMessage(msg);
}

void bxOsc::sendMessage(const std::string &address, float arg) {
	OscMessage msg(address);
	msg.addFloatArg(arg);
	sendMessage(msg);
}

void bxOsc::sendMessage(const std::string &address, bool arg) {
	OscMessage msg(address);
	msg.addBoolArg(arg);
	sendMessage(msg);
}

bool bxOsc::hasWaitingMessages() const { return !m_incomingMessages.empty(); }

OscMessage bxOsc::getNextMessage() {
	if (m_incomingMessages.empty()) {
		return OscMessage();
	}

	OscMessage msg = m_incomingMessages.front();
	m_incomingMessages.erase(m_incomingMessages.begin());
	return msg;
}

std::vector<OscMessage> bxOsc::getWaitingMessages() {
	std::vector<OscMessage> messages = m_incomingMessages;
	m_incomingMessages.clear();
	return messages;
}

void bxOsc::onMessageReceived(
	const std::string &address,
	std::function<void(const OscMessage &)> callback) {
	m_addressCallbacks[address] = callback;
}

void bxOsc::onMessageReceived(
	std::function<void(const OscMessage &)> callback) {
	m_generalCallbacks.push_back(callback);
}

void bxOsc::setVerbose(bool verbose) { m_verbose = verbose; }

bool bxOsc::isConnected() const { return m_connected; }

void bxOsc::simulateNetworkReceive() {
	// In a real implementation, this would receive actual OSC messages
	// For now, we'll simulate some incoming messages for testing
	static int frameCount = 0;
	frameCount++;

	if (frameCount % 60 == 0) { // Every 60 frames (1 second at 60fps)
		OscMessage msg("/test/message");
		msg.addStringArg("Hello from OSC");
		msg.addIntArg(frameCount);
		msg.addFloatArg(3.14159f);
		m_incomingMessages.push_back(msg);

		// Trigger callbacks
		for (auto &callback : m_generalCallbacks) {
			callback(msg);
		}

		auto it = m_addressCallbacks.find("/test/message");
		if (it != m_addressCallbacks.end()) {
			it->second(msg);
		}
	}
}

void bxOsc::simulateNetworkSend() {
	// In a real implementation, this would send actual OSC messages
	// For now, we'll just clear the outgoing queue
	if (!m_outgoingMessages.empty()) {
		if (m_verbose) {
			spdlog::info("[bxOsc] Sending {} messages",
						 m_outgoingMessages.size());
		}
		m_outgoingMessages.clear();
	}
}

// OscSender implementation
OscSender::OscSender() : m_connected(false) {}

OscSender::~OscSender() {
	if (m_connected) {
		spdlog::info("[OscSender] Disconnecting from {}:{}", m_host, m_port);
	}
}

void OscSender::setup(const std::string &host, int port) {
	m_host = host;
	m_port = port;
	m_connected = true;
	spdlog::info("[OscSender] Connected to {}:{}", host, port);
}

void OscSender::sendMessage(const OscMessage &message) {
	if (!m_connected) {
		spdlog::warn("[OscSender] Not connected, cannot send message");
		return;
	}
	spdlog::info("[OscSender] Sending message: {} with {} args",
				 message.address, message.getNumArgs());
}

bool OscSender::isConnected() const { return m_connected; }

// OscReceiver implementation
OscReceiver::OscReceiver() : m_listening(false) {}

OscReceiver::~OscReceiver() {
	if (m_listening) {
		spdlog::info("[OscReceiver] Stopping listener on port {}", m_port);
	}
}

void OscReceiver::setup(int port) {
	m_port = port;
	m_listening = true;
	spdlog::info("[OscReceiver] Listening on port {}", port);
}

bool OscReceiver::hasWaitingMessages() const { return !m_messages.empty(); }

OscMessage OscReceiver::getNextMessage() {
	if (m_messages.empty()) {
		return OscMessage();
	}

	OscMessage msg = m_messages.front();
	m_messages.erase(m_messages.begin());
	return msg;
}

std::vector<OscMessage> OscReceiver::getWaitingMessages() {
	std::vector<OscMessage> messages = m_messages;
	m_messages.clear();
	return messages;
}

} // namespace blot
