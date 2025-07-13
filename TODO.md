# TODO

## UI Improvements

### Multi-Column Support
- [ ] Add multi-column support to ThemePanel
- [ ] Extend multi-column support to other window types
- [ ] Implement responsive column layouts
- [ ] Add column configuration options

## Features

### Generalized Properties System
- [ ] Implement generalized properties system with type-safe property handling
- [ ] Add support for dynamic property types:
  ```cpp
  typedef enum blotTypes {
      EPT_BOOL = 1,
      EPT_INT,
      EPT_UINT,
      EPT_FLOAT,
      EPT_DOUBLE,
      EPT_STRING,
      EPT_IMVEC4,
      EPT_COUNT
  } blotTypes;
  
  struct blotProp {
      uint32_t id;
      std::string name;
      propTypes type;
      void* data;
  };
  ```
- [ ] Create generic property editor that can handle any property type
- [ ] Integrate with ECS system for automatic property reflection
- [ ] Add property change callbacks and undo/redo support
- [ ] Implement property serialization/deserialization

## Bug Fixes

## Performance

## Logging System
- [ ] Create custom logging system to replace `std::cout` usage
- [ ] Implement different log levels (DEBUG, INFO, WARNING, ERROR, CRITICAL)
- [ ] Add log formatting with timestamps, file/line information, and log levels
- [ ] Support for log output to file, console, and UI
- [ ] Add log filtering and log level configuration
- [ ] Integrate with existing UI system for log display in dedicated window
- [ ] Add log rotation and log file management
- [ ] Consider using a library like spdlog or implementing a lightweight custom solution

## Documentation 