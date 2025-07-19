#pragma once

namespace blot {
class IManager {
  public:
	virtual ~IManager() = default;
	virtual void init() {}
	virtual void shutdown() {}
};
} // namespace blot
