#pragma once
#include <spdlog/spdlog.h>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <vector>

// Property type enumeration for editor reflection
enum propTypes {
	EPT_BOOL = 1,
	EPT_INT,
	EPT_UINT,
	EPT_FLOAT,
	EPT_DOUBLE,
	EPT_STRING,
	EPT_IMVEC4,
	EPT_IMVEC2,
	EPT_COUNT
};

// Property descriptor struct for editor reflection
struct sProp {
	uint32_t id;
	std::string name;
	propTypes type;
	void *data;
};

// SFINAE: Checks if T has GetProperties() returning std::vector<sProp>
template <typename T> class has_get_properties {
  private:
	template <typename U>
	static auto test(int)
		-> decltype(std::declval<U>().GetProperties(), std::true_type{});
	template <typename> static std::false_type test(...);

  public:
	static constexpr bool value = decltype(test<T>(0))::value;
};

// If T has GetProperties(), call it
template <typename T>
typename std::enable_if<has_get_properties<T>::value, std::vector<sProp>>::type
TryGetProperties(T &component) {
	return component.GetProperties();
}

// If T does not have GetProperties(), return empty and log warning
template <typename T>
typename std::enable_if<!has_get_properties<T>::value, std::vector<sProp>>::type
TryGetProperties(T &) {
	spdlog::warn(
		"[Property Inspector] WARNING: Component '{}' missing GetProperties()",
		typeid(T).name());
	return {};
}
