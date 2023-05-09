#ifndef WebUIHelper_h
#define WebUIHelper_h

namespace WebUIHelper {
  constexpr const char* checkedOrNot[2] = {"", "checked='checked'"};

  void init(const __FlashStringHelper* customActions);

  // Show or Hide a "busy" indicator on the screen. If your app has additional
  // ways to show status, it can use register it's own function to do so using
  // WebUI::registerBusyCallback(). The app's callback may also call this function
  // in order to show/hide the busy indicator on the display.
  void showBusyStatus(bool busy);

  namespace Default {
    void homePage();
  }
}
#endif	// WebUIHelper_h