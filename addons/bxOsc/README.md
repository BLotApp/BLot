# bxOsc

Open Sound Control (OSC) communication addon for Blot. Provides simple send/receive wrappers for OSC messaging.

## Features

- **OSC Message Handling**: Send and receive OSC messages with type-safe arguments
- **Event Callbacks**: Register callbacks for specific OSC addresses or general message handling
- **Multiple Data Types**: Support for strings, integers, floats, and booleans
- **Sender/Receiver Classes**: Separate classes for dedicated sending and receiving
- **Verbose Logging**: Optional detailed logging for debugging

## Usage

### Basic Setup

```cpp
#include "addons/bxOsc/bxOsc.h"

// In your app setup
auto addonManager = getEngine()->getAddonManager();
if (addonManager) {
    blot::AddonLoader::registerAvailableAddons(addonManager, {"bxOsc"});
}

// Get the OSC addon
auto oscAddon = addonManager->getAddon("bxOsc");
if (auto osc = std::dynamic_pointer_cast<blot::bxOsc>(oscAddon)) {
    // Setup OSC connection
    osc->setupSender("localhost", 8000);
    osc->setupReceiver(8001);
    
    // Register message handlers
    osc->onMessageReceived("/test/message", [](const blot::OscMessage& msg) {
        spdlog::info("Received message: {} with {} args", 
                     msg.address, msg.getNumArgs());
    });
}
```

### Sending Messages

```cpp
// Send different types of messages
osc->sendMessage("/test/string", "Hello OSC");
osc->sendMessage("/test/int", 42);
osc->sendMessage("/test/float", 3.14159f);
osc->sendMessage("/test/bool", true);

// Or create messages manually
blot::OscMessage msg("/custom/address");
msg.addStringArg("Hello");
msg.addIntArg(123);
msg.addFloatArg(45.67f);
osc->sendMessage(msg);
```

### Receiving Messages

```cpp
// Check for waiting messages
if (osc->hasWaitingMessages()) {
    auto msg = osc->getNextMessage();
    spdlog::info("Received: {} with {} args", 
                 msg.address, msg.getNumArgs());
    
    // Extract arguments by type
    std::string str = msg.getStringArg(0);
    int num = msg.getIntArg(1);
    float val = msg.getFloatArg(2);
}
```

### Using Separate Sender/Receiver

```cpp
// Dedicated sender
blot::OscSender sender;
sender.setup("localhost", 8000);
sender.sendMessage(msg);

// Dedicated receiver
blot::OscReceiver receiver;
receiver.setup(8001);
if (receiver.hasWaitingMessages()) {
    auto msg = receiver.getNextMessage();
    // Process message
}
```

## Examples

- *(no dedicated example yet)* 
