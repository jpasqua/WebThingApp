#ifndef WebUIHelper_h
#define WebUIHelper_h

namespace WebUIHelper {
  extern ESPTemplateProcessor  *templateHandler;

  void init(PGM_P customActions);

  void showUpdatingIcon();
  void hideUpdatingIcon();

  void wrapWebAction(
      const char* actionName, std::function<void(void)> action,
      bool showIcon = true);
  void wrapWebPage(
      const char* pageName, const char* htmlTemplate,
      ESPTemplateProcessor::ProcessorCallback mapper,
      bool showIcon = true);


  namespace Default {
    void homePage();
    void devPage();
    void updateDevConfig();
  }
}
#endif	// WebUIHelper_h