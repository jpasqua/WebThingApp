#ifndef WebUIHelper_h
#define WebUIHelper_h

namespace WebUIHelper {
  static const String checkedOrNot[2];

  extern ESPTemplateProcessor  *templateHandler;

  void init(String& customActions);

  void showUpdatingIcon();
  void hideUpdatingIcon();

  void wrapWebAction(const char* actionName, std::function<void(void)> action);
  void wrapWebPage(
      const char* pageName, const char* htmlTemplate,
      ESPTemplateProcessor::ProcessorCallback mapper);


  namespace Default {
    void homePage();
    void devPage();
    void updateDevConfig();
  }
}
#endif	// WebUIHelper_h