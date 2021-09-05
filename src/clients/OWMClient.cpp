

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//#include <ESP8266WiFi.h>
//                                  Third Party Libraries
#include <ArduinoJson.h>
#include <ArduinoLog.h>
#include <JSONService.h>
//                                  WebThing Includes
#include <WTBasics.h>
//                                  Local Includes
#include "OWMClient.h"
//--------------- End:    Includes ---------------------------------------------

static constexpr char OWMServer[] = "api.openweathermap.org";
static constexpr uint16_t OWMPort = 80;

static constexpr char ProxyOWMServer[] = "192.168.1.99";
static constexpr uint16_t ProxyOWMPort = 59641;

//static ServiceDetails owmDetails(ProxyOWMServer, ProxyOWMPort);
static JSONService owmService(ServiceDetails(OWMServer, OWMPort));

OWMClient::OWMClient(String key, int cityID, bool useMetric, String language) :
    _key(key), _cityID(cityID), _useMetric(useMetric), _lang(language)
{
  _endpoint.reserve(100);
}

void OWMClient::update() {
  WTBasics::setStringContent(_endpoint, "/data/2.5/group?id=");
  _endpoint.concat(_cityID);
  _endpoint.concat("&units=");
  _endpoint.concat(_useMetric ? "metric" : "imperial");
  _endpoint.concat("&cnt=1&APPID=");
  _endpoint.concat(_key);
  _endpoint.concat("&lang=");
  _endpoint.concat(_lang);

  DynamicJsonDocument *root = owmService.issueGET(_endpoint, 1024);
  if (!root) {
    Log.warning(F("Failed to update weather info"));
    weather.cached = true;
    weather.error = "Unable to get data from service";
    weather.dt = 0;
    return;
  }
  //serializeJsonPretty(*root, Serial); Serial.println();

  weather.cached = false;
  weather.error = "";

  /* ----- Sample response -----
  {
    "cnt": 1,
    "list": [
      {
        "coord": { "lon": -122.18, "lat": 37.45 },
        "sys": {
          "country": "US",
          "timezone": -25200,
          "sunrise": 1586784938,
          "sunset": 1586832124
        },
        "weather": [
          {
            "id": 800,
            "main": "Clear",
            "description": "clear sky",
            "icon": "01d"
          }
        ],
        "main": {
          "temp": 63.12, "feels_like": 60.17,
          "temp_min": 57, "temp_max": 68,
          "pressure": 1021, "humidity": 59
        },
        "visibility": 16093,
        "wind": { "speed": 4.7 },
        "clouds": { "all": 5 },
        "dt": 1586805383,
        "id": 5372223,
        "name": "Menlo Park"
      }
    ]
  }
  ** -----END: Sample Response */

  JsonObject city = (*root)["list"][0];
  weather.location.lat = city["coord"]["lat"];
  weather.location.lon = city["coord"]["lon"];
  WTBasics::setStringContent(weather.location.country, city["sys"]["country"]);
  weather.location.city = city["name"].as<String>();
  weather.location.cityID = city["id"];

  weather.time.tz = city["sys"]["timezone"];
  weather.time.sunrise = city["sys"]["sunrise"];
  weather.time.sunset = city["sys"]["sunset"];

  WTBasics::setStringContent(weather.description.basic, city["weather"][0]["main"]);
  WTBasics::setStringContent(weather.description.longer, city["weather"][0]["description"]);
  WTBasics::setStringContent(weather.description.icon, city["weather"][0]["icon"]);
  weather.description.code = city["weather"][0]["id"];

  JsonObject cityMain = city["main"];
  weather.readings.temp = cityMain["temp"];
  weather.readings.feelsLike = cityMain["feels_like"];
  weather.readings.minTemp = cityMain["temp_min"];
  weather.readings.maxTemp = cityMain["temp_max"];
  weather.readings.pressure = cityMain["pressure"];
  weather.readings.humidity = cityMain["humidity"];
  weather.readings.windSpeed = city["wind"]["speed"];
  weather.readings.visibility = city["visibility"];
  weather.readings.cloudiness = city["clouds"]["all"];

  weather.dt = city["dt"];
  delete root;
}

void OWMClient::updateForecast(int32_t gmtOffset) {
  StaticJsonDocument<128> filter;
  filter["list"][0]["dt"] = true;
  filter["list"][0]["main"]["temp"] = true;
  filter["list"][0]["weather"][0]["icon"] = true;

  WTBasics::setStringContent(_endpoint, "/data/2.5/forecast?id=");
  _endpoint.concat(_cityID);
  _endpoint.concat("&units=");
  _endpoint.concat(_useMetric ? "metric" : "imperial");
  _endpoint.concat("&APPID=");
  _endpoint.concat(_key);
  _endpoint.concat("&lang=");
  _endpoint.concat(_lang);

  DynamicJsonDocument *root = owmService.issueGET(_endpoint, 8192, &filter);
  if (!root) {
    Log.warning(F("Failed to retreive forecast"));
    return;
  }
  // serializeJsonPretty(*root, Serial); Serial.println();

  // The 5 forecast elements correspond to the 5-day forecast

  JsonArray list = (*root)["list"];

  int operationalDay = -1;

  uint32_t curDT;
  String   curIcon;
  float    curMin = 1000.0f;
  float    curMax = -1000.0f;
  int      dayOfCurDT;
  uint32_t timeOfMaxTemp = 0;
  int forecastIndex = 0;

  // Iterate through the JsonArray this way rather than referencing values by
  // index in a loop. This is much more efficient since the underlying JsonArray
  // is actually a linked list (see ArduinoJson doc for the recommendation)
  for (JsonObject f : list) {
    curDT = f["dt"];
    curDT += gmtOffset;
    dayOfCurDT = day(curDT);

    if (operationalDay == -1) { operationalDay = dayOfCurDT; }

    if (dayOfCurDT != operationalDay) {
      forecast[forecastIndex].dt = timeOfMaxTemp;
      forecast[forecastIndex].loTemp = curMin;
      forecast[forecastIndex].hiTemp = curMax;
      forecast[forecastIndex].icon = curIcon;
      if (++forecastIndex == ForecastElements) break;
      operationalDay = dayOfCurDT;
      curMin = 1000;
      curMax = -1000;
    }
    float curTemp = f["main"]["temp"];
    if (curTemp > curMax) {
      curMax = curTemp;
      timeOfMaxTemp = curDT;
      WTBasics::setStringContent(curIcon, f["weather"][0]["icon"]);
    }
    if (curTemp < curMin) {
      curMin = curTemp;
    }
  }
  if (forecastIndex < ForecastElements) {
    forecast[forecastIndex].dt = timeOfMaxTemp;
    forecast[forecastIndex].loTemp = curMin;
    forecast[forecastIndex].hiTemp = curMax;
    forecast[forecastIndex].icon = curIcon;
  }

  dumpForecast();
  delete root;
}

void OWMClient::dumpForecast() {
  for (int i = 0; i < ForecastElements; i++) {
    forecast[i].dumpToLog();
  }
}











