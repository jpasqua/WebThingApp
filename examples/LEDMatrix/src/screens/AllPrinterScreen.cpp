
/*
 * AllPrinterScreen:
 *    Display the print status of the next printer that will complete 
 *                    
 */

#include <gui/devices/DeviceSelect.h>
#if DEVICE_TYPE == DEVICE_TYPE_MTX

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
#include <BPA_PrinterGroup.h>
//                                  WebThingApp Includes
#include <gui/Display.h>
//                                  Local Includes
#include "AllPrinterScreen.h"
#include "../../LEDMatrixApp.h"
//--------------- End:    Includes ---------------------------------------------


/*------------------------------------------------------------------------------
 *
 * Constructors and Public methods
 *
 *----------------------------------------------------------------------------*/

AllPrinterScreen::AllPrinterScreen() {
  nLabels = 0;
  labels = NULL;

  init();
  _statusText.clear();
}

void AllPrinterScreen::innerActivation() {
  updateText();
}


// ----- Private Methods

void AllPrinterScreen::updateText() {
  _statusText = "";
  String printerName;
  int pctComplete;
  String completionTime;
  String fileName;
  uint32_t timeLeft;

  if (lmSettings->printMonitorEnabled) {
    for (int i = lmApp->printerGroup->numberOfPrinters()-1; i >= 0; i--) {
      if (lmSettings->printer[i].isActive) {
        PrintClient* p = lmApp->printerGroup->getPrinter(i);
        if (p->getState() >= PrintClient::State::Complete) {
          printerName = lmApp->printerGroup->getDisplayName(i);
          pctComplete = p->getPctComplete();

          timeLeft = p->getPrintTimeLeft();
          fileName = p->getFilename();
          int extIndex = fileName.lastIndexOf(".gcode");
          if (extIndex != -1) fileName.remove(extIndex);

          if (!_statusText.isEmpty()) _statusText += ", ";

          if (lmSettings->allPrinters.printerName) {
            _statusText += printerName;
            _statusText += ": ";
          }
          if (lmSettings->allPrinters.pct) {
            _statusText += pctComplete;
            _statusText += "% ";
          }
          if (lmSettings->allPrinters.fileName) {
            _statusText += fileName;
            _statusText += " ";
          }
          if (lmSettings->allPrinters.completeAt) {
            if (timeLeft) {
              lmApp->printerGroup->completionTime(completionTime, timeLeft);
              _statusText += completionTime;
            }
          }
          int len = _statusText.length();
          if (len && _statusText[len-1] == ' ') _statusText.remove(len-1, 1);
        }
      }
    }
  }
  setText(_statusText, Display.BuiltInFont_ID);
}
#endif






