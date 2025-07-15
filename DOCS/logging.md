# Logging in Blot

## Overview
Blot uses [spdlog](https://github.com/gabime/spdlog) for logging. This provides fast, flexible, and thread-safe logging with support for log levels, sinks, and formatting.

## How to Use

Include the spdlog header in any file:
```cpp
#include <spdlog/spdlog.h>
```

Log messages at various levels:
```cpp
spdlog::info("Hello, {}!", "world");
spdlog::warn("This is a warning");
spdlog::error("Something went wrong: {}", errorMsg);
```

## LogWindow Integration

Blot's in-app LogWindow is wired to receive log messages via a custom spdlog sink. All log messages sent to spdlog will also appear in the LogWindow UI.

- To log to the in-app window, just use `spdlog` as above.
- The LogWindow will display messages in real time.

## Best Practices
- Use appropriate log levels (`info`, `warn`, `error`, `debug`, `trace`).
- Avoid logging sensitive information.
- Prefer structured messages with formatting.
- Use log categories or tags if needed for filtering.

## Advanced
- You can add more sinks (e.g., file, rotating file, etc.) via spdlog's API.
- See the [spdlog documentation](https://github.com/gabime/spdlog) for more advanced usage. 