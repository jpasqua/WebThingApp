#ifndef WebUIHelper_h
#define WebUIHelper_h

namespace WebUIHelper {
  constexpr const char* checkedOrNot[2] = {"", "checked='checked'"};

  void init(const __FlashStringHelper* customActions);

  namespace Default {
    void homePage();
    void devPage();
    void updateDevConfig();
  }
}
#endif	// WebUIHelper_h