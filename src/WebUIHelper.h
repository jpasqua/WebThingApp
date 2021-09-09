#ifndef WebUIHelper_h
#define WebUIHelper_h

namespace WebUIHelper {
  constexpr const char* checkedOrNot[2] = {"", "checked='checked'"};

  extern ESPTemplateProcessor *templateHandler;

  void init(const __FlashStringHelper* customActions);

  void showUpdatingIcon();
  void hideUpdatingIcon();

  void wrapWebAction(
      const char* actionName, std::function<void(void)> action,
      bool showIcon = true);
  void wrapWebPage(
      const char* pageName, const char* htmlTemplate,
      ESPTemplateProcessor::Mapper mapper,
      bool showIcon = true);


  namespace Default {
    void homePage();
    void devPage();
    void updateDevConfig();
  }
}
#endif	// WebUIHelper_h