# Notification & Popup System in UIManager

This document describes how to use the notification (toast) and modal popup system in the Blot UIManager. This system provides a consistent, user-friendly way to display alerts, errors, and information to users.

---

## Features
- **Toasts:** Non-blocking notifications that appear in the top-right corner and fade out after a few seconds.
- **Modals:** Blocking popups that require user acknowledgment (OK button), centered on the screen.
- **Notification Types:** Info, Success, Warning, Error — each with a FontAwesome icon and color.
- **Consistent API:** All notifications and popups are managed by `UIManager`.

---

## API Usage

### Show a Toast Notification
```cpp
uiManager->showNotification("File saved successfully!", NotificationType::Success);
uiManager->showNotification("Could not connect to server.", NotificationType::Error, 5.0f); // 5 seconds
```

### Show a Modal Popup
```cpp
uiManager->showModal(
    "Error", 
    "Could not open file.", 
    NotificationType::Error, 
    [](){ /* on OK callback */ }
);
```

- The `onOk` callback is optional. If provided, it will be called when the user clicks OK.

---

## Notification Types & Icons
| Type    | Icon                        | Color           |
|---------|-----------------------------|-----------------|
| Info    | ICON_FA_INFO_CIRCLE         | Blue            |
| Success | ICON_FA_CHECK_CIRCLE        | Green           |
| Warning | ICON_FA_EXCLAMATION_TRIANGLE| Orange/Yellow   |
| Error   | ICON_FA_TIMES_CIRCLE        | Red             |

---

## Best Practices
- Use **toasts** for non-blocking, informational messages (e.g., "File saved").
- Use **modals** for errors or confirmations that require user action.
- Keep messages concise and clear.
- Use the appropriate notification type for clarity.

---

## Example: Error Handling
```cpp
if (!fileLoaded) {
    uiManager->showModal("File Error", "Could not load the file.", NotificationType::Error);
}
```

---

## Implementation Notes
- Toasts stack vertically in the top-right and auto-fade after their duration.
- Modals are blocking and must be dismissed by the user.
- FontAwesome icons are used for visual clarity.

---

For further customization or to add new notification types, see `UIManager.h` and `UIManager.cpp`. 