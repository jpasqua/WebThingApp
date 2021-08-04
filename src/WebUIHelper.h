#ifndef WebUIHelper_h
#define WebUIHelper_h

namespace WebUIHelper {
  static const String checkedOrNot[2];

  extern ESPTemplateProcessor  *templateHandler;

  void init(String& customActions);
  namespace Default {
    void homePage();
    void devPage();
    void updateDevConfig();
  }
}
#endif	// WebUIHelper_h