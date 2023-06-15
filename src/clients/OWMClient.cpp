

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//#include <ESP8266WiFi.h>
//                                  Third Party Libraries
#include <ArduinoJson.h>
#include <ArduinoLog.h>
#include <JSONService.h>
//                                  WebThing Includes
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
  _endpoint.reserve(132);
}

void OWMClient::update() {
  _endpoint ="/data/2.5/group?id=";
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
  // serializeJsonPretty(*root, Serial); Serial.println();

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
  weather.location.country = city["sys"]["country"].as<const char*>();
  weather.location.city = city["name"].as<const char*>();
  weather.location.cityID = city["id"];

  weather.time.tz = city["sys"]["timezone"];
  weather.time.sunrise = city["sys"]["sunrise"];
  weather.time.sunset = city["sys"]["sunset"];

  weather.description.basic = city["weather"][0]["main"].as<const char*>();
  weather.description.basic[0] = toupper(weather.description.basic[0]);
  weather.description.longer = city["weather"][0]["description"].as<const char*>();
  weather.description.longer[0] = toupper(weather.description.longer[0]);
  weather.description.icon = city["weather"][0]["icon"].as<const char*>();
  weather.description.code = city["weather"][0]["id"];


  JsonObject cityMain = city["main"];
  weather.readings.temp = cityMain["temp"];
  weather.readings.feelsLike = cityMain["feels_like"];
  weather.readings.minTemp = cityMain["temp_min"];
  weather.readings.maxTemp = cityMain["temp_max"];
  weather.readings.pressure = cityMain["pressure"];
  weather.readings.humidity = cityMain["humidity"];
  weather.readings.windSpeed = city["wind"]["speed"];
  weather.readings.windDeg = city["wind"]["deg"];
  weather.readings.visibility = city["visibility"];
  weather.readings.cloudiness = city["clouds"]["all"];

  weather.dt = city["dt"];
  delete root;
}

void OWMClient::updateForecast(int32_t gmtOffset) {
  StaticJsonDocument<128> filter;
  filter["dt"] = true;
  filter["main"]["temp"] = true;
  filter["weather"][0]["icon"] = true;

  _endpoint = "/data/2.5/forecast?id=";
  _endpoint.concat(_cityID);
  _endpoint.concat("&units=");
  _endpoint.concat(_useMetric ? "metric" : "imperial");
  _endpoint.concat("&APPID=");
  _endpoint.concat(_key);
  _endpoint.concat("&lang=");
  _endpoint.concat(_lang);

  WiFiClient* owmResponse = owmService.initiateRequest(_endpoint.c_str(), GET, "");
  if (!owmResponse) {
    Log.warning("Unable to update forecast");
    return;
  }

  // The 5 forecast elements correspond to the 5-day forecast
  int operationalDay = -1;

  uint32_t curDT;
  String   curIcon;
  float    curMin = 1000.0f;
  float    curMax = -1000.0f;
  int      dayOfCurDT;
  uint32_t timeOfMaxTemp = 0;
  int forecastIndex = 0;
  StaticJsonDocument<512> doc;

  owmResponse->find("\"list\":[");  // Position the stream at beginning of array
  do {
    auto error = deserializeJson(doc, *owmResponse, DeserializationOption::Filter(filter));
    if (error) {
      Log.warning(F("Error (%s) while parsing forecast"), error.c_str());
      owmResponse->stop();
      delete owmResponse;
      // TO DO: Any other cleanup needed?
      return;
    }
    curDT = doc["dt"];
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
    float curTemp = doc["main"]["temp"];
    if (curTemp > curMax) {
      curMax = curTemp;
      timeOfMaxTemp = curDT;
      curIcon = doc["weather"][0]["icon"].as<const char*>();
    }
    if (curTemp < curMin) {
      curMin = curTemp;
    }
  } while (owmResponse->findUntil(",", "]"));
  owmResponse->stop();
  delete owmResponse;

  if (forecastIndex < ForecastElements) {
    forecast[forecastIndex].dt = timeOfMaxTemp;
    forecast[forecastIndex].loTemp = curMin;
    forecast[forecastIndex].hiTemp = curMax;
    forecast[forecastIndex].icon = curIcon;
  }

  // If the actual current temperature is higher than the predicted
  // high for today, then update the prediction!
  if (day(weather.dt) == day(forecast[0].dt)) {
    if (weather.readings.temp > forecast[0].hiTemp) {
      forecast[0].hiTemp = weather.readings.temp;
    }
  }

  _timeOfLastForecastUpdate = millis();
  dumpForecast();
}

String OWMClient::getTextForIcon(String& icon) {
  if (icon.startsWith("01")) { return "Clear"; }
  if (icon.startsWith("02")) { return "Few Clouds"; }
  if (icon.startsWith("03")) { return "Scattered Clouds"; }
  if (icon.startsWith("04")) { return "Broken Clouds"; }
  if (icon.startsWith("09")) { return "Showers"; }
  if (icon.startsWith("10")) { return "Rain"; }
  if (icon.startsWith("11")) { return "Thunderstorm"; }
  if (icon.startsWith("13")) { return "Snow"; }
  if (icon.startsWith("50")) { return "Haze"; }
  else return "";
}

void OWMClient::dumpForecast() {
  for (int i = 0; i < ForecastElements; i++) {
    forecast[i].dumpToLog();
  }
}

const char* directions[] = {
  "N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE",
  "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW"
};

const char* OWMClient::dirFromDeg(float deg) {
  int dirIndex = ((int)((deg + 11.5)/22.5))%16;
  return directions[dirIndex];
}









