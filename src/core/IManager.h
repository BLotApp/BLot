#pragma once

namespace blot {
class IManager {
  public:
	virtual ~IManager() = default;
	virtual void init() = 0;
	virtual void shutdown() = 0;
};
} // namespace blot
