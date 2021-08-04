# WebThingApp
![](../../doc/images/WebThing_Logo_256.png)

A library for building apps based on the [WebThing library](https://github.com/jpasqua/WebThing).

# Introduction

In this context, the word app is used somewhat loosely. What we're really talking about is a [`WebThing`](https://github.com/jpasqua/WebThing) with a touchscreen display as opposed to a standalone `WebThing` such as an [air quality monitor](https://github.com/jpasqua/PurpleHaze) or a weather station. Here's an example of a `WebThingApp` called [MultiMon](https://github.com/jpasqua/MultiMon). It monitors up to 4 3D printers as well as providing time and weather information.

![](doc/images/MarbleMonitor512.jpg)

The goal of the library is to do most of the work of handling the Web UI, GUI, settings management, and plugins, so the "app" can focus on the core functionality it is providing. In the case of [MultiMon](https://github.com/jpasqua/MultiMon), that means it can focus on 3D Printer functionality and leave (most of) the rest to `WebThingApp`.

# Main Concepts and Components

The library is organized around a few main concepts and components:

* **[WebThing](https://github.com/jpasqua/WebThing)**: WebThing is a generic ESP8266/ESP32 framework for building *things* with a web configuration capability. They are called *things* as in Internet of *Things*. This library provides core functionality such as connecting to WiFi, saving and retreiving settings from the file system, and a Web UI capability that handles core configuration tasks as well as providing a framework for additional Web UI functionality.
* **Settings**: Information that is typically provided by the user to customize the operation of `WebThing`, `WebThingApp`, and any Plugins. These are stored in three separate files, though the developer doesn't really need to think about that.
* **DataBroker**: A rudimentary, but extensible, mechanism that allows data providers to publish information that other parts of the application can consume.
* **Display**: At the lowest layer we have the Display namespace.
	* It is primarily a shim on top of the [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) library which provides all of the graphics functionality.
	* It provides lower level functions such as setting the screen brightness, and calibarating the touch panel.
	* It also provides a wrapper over the TFT_eSPI font mechansim to reduce memory consumption.
* **Screen**: Above that we have the `Screen` class and a number of common subclasses.
	* A `Screen` instance displays information and handles user input. For example, there is a `WeatherScreen` that provides weather information based on data from [OpenWeatherMap.org](https://openweathermap.org).
	* The `Screen` instances provided in this library may be used by an application or not depending on their applicability.
	* Applications may create their own sublclasses of `Screen` to provide application-specific functionality and UI.
* **ScreenMgr**: As the name implies, ScreenMgr manages multiple screens that are used as part of an application.
	* It keeps track of all registered screens.
	* It is used by the application or other `Screen` instances to cause a screen to be displayed; For example, `ScreenMgr::display(weatherScreen)`
	* It provides other utlity functions related to screen management
* **Plugin**: A Plugin is a way of extending a `WebThingApp` in a low-code or no-code manner. Plugins use JSON files to define:
	* Their settings
	* A Web UI to adjust the settings 
	* A Screen to display information in a custom layout. The data to be displayed is given by keys that are looked up via the `DataBroker`.
	* Basic identifying information for the Plugin
* **App**: App objects hold the state of the application and provide the functions used in the main loop. Specific App sublasses, such as MultiMonApp, provide app-specific functionality such as monitoring printers and publishing printer-related data to the Data broker.

# File Structure for the `WebThingApp` library

The library is structured as follows:

````
WebThingApp
├── README.md
├── data
│   └── wta
│       ├── ConfigDev.html
│       ├── ConfigDisplay.html
│       ├── ConfigPlugins.html
│       ├── ConfigWeather.html
│       └── HomePage.html
├── doc
└── src
    ├── WTApp.[h,cpp]
    ├── WTAppImpl.[h,cpp]
    ├── WTAppSettings.[h,cpp]
    ├── WebUIHelper.[h,cpp]
    ├── clients
    │   ├── BlynkClient.[h,cpp]
    │   ├── CoinbaseClient.[h,cpp]
    │   └──  OWMClient.[h,cpp]
    ├── gui
    │   ├── Button.[h,cpp]
    │   ├── ...
    │   ├── UIOptions.[h,cpp]
    │   └── fonts
    │       └── ...
    ├── plugins
    │   ├── Plugin.[h,cpp]
    │   ├── PluginMgr.[h,cpp]
    │   └── common
    │       ├── BlynkPlugin.[h,cpp]
    │       ├── CryptoPlugin.[h,cpp]
    │       └── GenericPlugin.[h,cpp]
    └── screens
        ├── CalibrationScreen.[h,cpp]
        ├── ...
        ├── WiFiScreen.[h,cpp]
        └── images
            └── ...

````

Description:

* **`WebThingApp`**: The root directory houses the implementation of the app infrastructure:
  * **`WTApp`**: The base class for apps. It is really a data class providing the common data used by all apps.
  * **`WTAppImpl`**: A subclass of `WTApp` that provides most of the app logic and structure. It is subclassed by apps like `MultiMon` to provide app-specific functionality.
  * **`WTAppSettings`**: Contains the data and code to manage the common settings used by all apps. It is subclassed by apps like `MultiMon` to provide app-specific settings.
  * **`WebUIHelper`**: A set of functions and data that build on the Web UI provided by `WebThing` to provide Web UI functions that are common to most apps. This is not a class, it is a namespace. Apps like `MultiMon` will use WebUIHelper to provide most of their WebUI and then add endpoints for app-specific functionality.
* **`clients`**: This directory contains code that implements client objects for web services, sensors, or other data providers/actuators that are common to many `WebThingApps`. Of course applications can use existing libraries if they exist or define their own.
* **`data`**: This directory contains one subdirectory: `wta`. It contains the HTML files that correspond to the pages that will be served by `WebUIHelper`. The `wta` directory should be copied into (or linked to) the applications data directory.
* **`gui`**: This directory has the definitions and implementations for core GUI elements such as `Display`, `Screen`, `ScreenMgr` and `Theme`. The common screens included in the library, as well as any app-specific screens, are built on the GUI elements.
* **`plugins`**: This directory contains the both the implementation of the Plugin mechanism and a subdirectory containing a number of standard plugins that may be used by `WebThingApps`.
* **`screens`**: This directory contains the definition and implementation of a number of common Screens that are likely to be used by many `WebThingApps`. The list of common screens is defined [below](#common_screens).
	
# File Structure for an example App

Let's look at the files and directory structure of a typical `WebThingApp`. In this case the app is called `MultiMon	` and files often have the prefix `MM`. Here is an overview of the directory structure.

````
MultiMon
├── MMDataSupplier.[h,cpp]
├── MMSettings.[h,cpp]
├── MMWebUI.[h,cpp]
├── MultiMon.ino
├── MultiMonApp.[h,cpp]
├── README.md
├── data
│   ├── ...
│   ├── plugins
│   │   ├── 1_gnrc
│   │   ├── 2_blynk
│   │   └── 4_ovw
│   └── wt
│   │   └── ...
│   └── wta
│       └── ...
└── src
    ├── clients
    │   ├── DuetClient.[h,cpp]
    │   ├── MockPrintClient.h
    │   ├── OctoClient.[h,cpp]
    │   └── PrintClient.h
    └── screens
        ├── AppTheme.h
        ├── DetailScreen.[h,cpp]
        ├── SplashScreen.[h,cpp]
        ├── TimeScreen.[h,cpp]
        └── images
            └── ...
````

Description:

* The `MultiMon` directory contains the source code for the app itself:

	* `MMDataSupplier`: This module contains a dataSupplier function which gets plugged into the `DataBroker` to publish app specific information. In this case it is information about the configuration and status of 3D printers.
	* `MMSettings`: Defines, internalizes, and externalizes the settings that are specific to this app.
	* `MMWebUI`: Provides app-specific web pages and functions. For example, the home page and a printer configuration page. `WebThingApp` provides many other pages such as general settings, display settings, and weather settings.
	* `MultiMon.ino`: This is a bridge between the Arduino `setup()` and `loop()` functions and the app initialization and operation. It is boilerplate and there is no app-specific code in this module.
	* `MultiMonApp`: This is the core of the application. It is a subclass of `WTAppImpl` and `WTApp` which are part of the `WebThingApp` library.
* The `data` contains contains all the files that will be written to the file system as part of the build process. There are four sets of files in the data directory:

	* At the root are HTML files that are used by any custom pages served up by the app's Web UI. For example, a custom home page. You may also place a settings.json file here if you want to have settings loaded into the app by default. Otherwise the user will need to configure the app settings when the device starts the first time.
	* The `plugins` subdirectory contains subdirectories for each plugin to be loaded. See the [plugin section](#plugins) for details.
	* The `wt` subdirectory contains HTML for the pages displayed by the `WebThing` Web UI pages. These are low level configuration items such as the hostname to use. You may also place a settings.json file here if you want to have settings loaded into the app by default. Otherwise the user will need to configure all the `WebThings` settings when the device starts the first time.
	* The `wta` subdirectory contains HTML for the pages displayed by the `WebUIHelper`. These are pages that are common to most apps like a page to configure display settings.

* The `src/clients` directory contains code that implements client objects for web services, sensors, or other data providers/actuators. Of course applications can use existing libraries if they exist. This directory is for app-specific clients.

* The `src/screens` directory contains code for app-specific screens. Most apps will use the common screens provided by `WebThingApp`, but they will also typically provide at least one custom screen.

# Screens

There are conceptually three types of screens you need to be aware of:

1. Common screens that are provided by `WebThingApp`
2. Custom screens that are implemented by your app
3. `FlexScreens` whose layout and data sources are given in JSON files and do not require custom code.

This sction describes each.

<a name="common_screens"></a>
## Common `Screen` Subclasses

This library ships with the following `Screen` subclasses which can be used without modification.

* `CalibrationScreen`: Provides a process for the user to calibrate the touch screen. The application would typically want to save the calibration data, so it can provide a change-listener callback which will be invoked by `CalibrationScreen` any time the process is completed.
* `ConfigScreen`: When a WebThing comes online for the first time it typically has no WiFi connection information and must be configured by the user. If this is the case, the app may display the `ConfigScreen` to instruct the user to do so.
* `EnterNumberScreen`: Displays a number pad that allows the user to enter numerical data.
* `ForecastScreen`: Provides a 5-day forecast based on data from [OpenWeatherMap.org](https://openweathermap.org). It is typically used in conjunction with `WeatherScreen`.
* `RebootScreen`: Some `WebThing` applications may provide expert or developer interfaces. These may wish to invoke a `RebootScreen` which asks the user to confirm a reboot and upon confirmation, reboots the device.
* `UtilityScreen`: This screen provides information about the "thing" such as the name, version number, the web address, and the wifi signal strength. It also provides buttons that can be used to:
  * Jump directly to a named plugin screen (up to 4)
  * Adjust the screen brightness
  * Ask the application to refresh data from whatever sources it is monitoring
  * Enter the touch screen calibration process (see CalibrationScreen).
  * Return tot he home screen.
* `WeatherScreen`: Provides current weather information based on data from [OpenWeatherMap.org](https://openweathermap.org). It is typically used in conjunction with `ForecastScreen `.
* `WiFiScreen`: During the startup process of a `WebThing`, one of the earliest steps is to connect to WiFi. This screen can be displayed to tell the user that the connection is being established.

All of these screens are loaded by default. If you want to exclude one of them to save space, you'll need to remove it from `WTAppImpl::registerScreens()`. This amounts to commenting out two lines: the declaration and the instantiation.

## Creating new subclasses of `Screen`

The `Screen` class has three functions that you need to write in your subclass:

* The constructor: Do whatever setup is necessary for your object. If your Screen has button areas that can be touched by the user to trigger an activity, this is where you would typically define them.
* The `display()` function: This is the heart of the `Screen`. It is responsible for displaying all data and graphics.
	* It can optimize the redrawing of elements as it sees fit based on whether information has changed, *unless* the `force` parameter is set in which case it must redraw the entire screen. This happens, for example, when the user moves from one screen to another.
	* This function will use the [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) library to actually draw the visual elements. Please refer to the documentation for that library to learn how to use it. You can also look at the common `Screen` subclasses as examples.
* The `processPeriodActivity()` function: A `Screen` may be static or it may need to be updated periodically. When a `Screen` is being displayed, the `processPeriodActivity` function will be called each time through the Arduino loop. The function can decide if it is time to refresh part or all of the `Screen` and act accordingly. For example, if the `Screen` is display a clock with seconds, it could update each second. For a clock with minutes it would update each minute. For a `Screen` that displays stock market information, it would only update if new information has been retrieved.
* The `ButtonCallback`: If you defined buttons in your constructor you will provide a callback which is invoked if the button is touched. It decides what to do. A trivial example is a **Home** button. When the user touches the button, your callback should invoke `ScreenMgr` and request that the home screen be displayed.

## Plugins and FlexScreen

WebThing applications can be extended using the plugin mechanism ([see below](#plugins)). A plugin can define a `Screen` without new code using the `FlexScreen` mechanism, which:

 * Creates a `Screen` based on a definition given in a JSON document
 * That document specifies the layout of the elements on the screen and the data that should be used to fill those elements
 * The data is accessed via the `WebThing DataBroker`. The JSON document provides the keys to be used and `FlexScreen` uses the `DataBroker` to get values associated with the keys at runtime.
 * There is only one user interaction defined for a `FlexScreen`. Touching anywhere on the display invokes the `ButtonDelegate` supplied when the `FlexScreen` is instantiated.

So, a developer need never use `FlexScreen` directly. The `Plugin` system will use it as needed if plugins are loaded.

<a name="plugins"></a>
# Plugins

## Overview

As mentioned above, `WebThingApp` contains a simple Plugin system which supports expansion and customization in a no-code or low-code manner. Without any code a plugin can add a new screen to the GUI which presents existing information (e.g. weather data or grill status) information in a custom layout. With code, plugins may also add new data sources (e.g. readings from a locally attached sensor or data gathered from a REST API). For example, you may want a screen that contains a specific set of information in a specific layout that suits your needs. `WebThingApp` ships with an example plugin that you can use to help create your own.

Plugins are largely declarative, using JSON files to describe the GUI they present on the attached display, their settings, the Web form to display/edit the settings, and references to the data they consume. 

You can have many instances of the same type of plugin or of different types of plugin. At the moment, `WebThingApp` allows up to four plugins but this is a somewhat arbitrary limit. More could be added if memory allows.

***NOTE***: This document is just as nascent as the Plugin system itself. Looking at the examples will be required to understand the mechanism.

### Dependencies
The Web UI page used to display plugin settings is based on [JSON Form](https://github.com/jsonform/jsonform). It has a number of dependencies on various CSS and Javascript files.
You do not need to install these files - they are just used in the browser. None of this is used directly by `WebThingApp`. This list is provided for information only.

* [JSON Form](https://github.com/jsonform/jsonform)
* [JQuery](https://github.com/jquery/jquery)
* [Underscore.js](https://github.com/jashkenas/underscore)
* [Bootstrap](https://github.com/twbs/bootstrap) (CSS file only): `WebThingApp` uses a customized version which allows pages generated by [JSON Form](https://github.com/jsonform/jsonform) to coexist with the [W3.CSS](https://www.w3schools.com/w3css/defaulT.asp) styles used elsewhere in the Web UI. The custom CSS file is hosted [here](https://jpasqua.github.io/static/bootstrap-3.3.7-iso.css) and is version 3.3.7.

## Structure of Plugins

A plugin consists of four of JSON descriptors described below, and optionally C++ code for any deeper customizations. To create a no-code plugin (called a Generic plugin), only the JSON files are needed.

<a name="descriptors"></a>
### JSON Descriptors

Each plugin requires four JSON descriptor files. All descriptors for a plugin are placed in a subdirectory of `/data/plugins` which can have any name you'd like. The directory name is not germane to the plugin. The four JSON files are listed below and must have these names:

* **plugin.json**: A simple file which gives:
  * The display name of the plugin
  * The plugin type. The type must either be "generic" for no-code plugins, or must correspond to a name you've given in your custom code.
  * A namespace. If this plugin makes data available to other plugins, the namespace is used to reference it. See the section on [data values](#datavalues).
* **settings.json**: If the plugin has settings, for example an API key for a REST service it uses, they are given in this file. This is very much like the main `WebThingApp` settings file, but the contents are up to the plugin developer. For generic (no-code) plugins, there are just a few settings:
	* Whether the plugin is enabled
	* How often to refresh the UI
	* A representation of the units of the refresh interval (i.e. is it seconds, minutes, etc.)
* **form.json**: A [jsonform](https://github.com/jsonform/jsonform) descriptor that is used by the Web UI to allow the user to display and edit the settings. Using this mechanism, a plugin can augment a `WebThingApp`'s Web UI without creating a new HTML template or writing custom code. The Web UI for all plugins are displayed by the [Configure Plugins](../Readme.md#configure-plugins) page 
* **screen.json**: A description of the layout and content of the screen. The FlexScreen class is responsible for parsing this description and displaying information - no custom code is required. FlexScreen can display things like floats, ints, and string using a format you specify. It can also display more complex things like a progress/status bar. At the moment, that's the only complex thing.

### Custom Code

You can create a plugin that goes beyond what is possible via a generic plugin. There are two types of code that may be involved in a custom plugin: the plugin itself and one or more data sources:

* **The plugin code**: The plugin code is a subclass of the built-in Plugin class. It is typically fairly simple and is responsible for a few things:
  * Serializing and Deserializing any custom settings from the settings.json file. This is fairly mechanical and can be copied and customized from one of the example plugins.
  * Mapping names to values. When you are describing your screen layout in the screen.json file, each field has a key. That key is a string that names the data value to be displayed. It is up to the plugin code to handle requests to map keys to actual values. For example, a key might be "headline" and the plugin might map that to a value from a news source which provides top headlines.
* **Data sources**: A data source is any code that makes data available for use by one or more plugins. This could be something that reads information from local sensors, a remote device, a news service - whatever. It is responsible for getting the data and providing an API for consuming it.

Plugin code should be placed in the `src/plugins` directory and the data sources should be placed  in the `src/clients` directory.

To make a new type of plugin available, you must add it to the `PluginFactory` which is typically defined in your main app. A `PluginFactory` simply takes a plugin name, as defined in a plugin.json file, and returns an instance of that plugin. See the sample code for details. 

## Describing the GUI Layout

A major part of the plugin is the screen it displays in the GUI. This is defined in the `screen.json` file. The overall structure is:

````
{
  "items": [ { }, ... { } ],
  "bkg": "0x000000",
  "name": "Displayed Name"
}
````
The bulk of the content is in the `items` array, each element of which describes an item that is displayed on the screen. I sometimes refer to this as a field. An item may be static content, like a label, or it may be a reference to some value. Before going into the details of items, there are two other simple elements:

* `bkg`: Provide the background color of the screen as a 24bit hex color code
* `name`: The name that will be used to identify the plugin in the GUI and Web UI. This should be a fairly short name as space is limited in the GUI.

The `items` have a number of elements, some of which are optional:

````
{
  "x": 86,  "y": 35,
  "w": 117,  "h": 26,
  "xOff": 58, "yOff": 13,
  "justify": "MC",
  "font": "SB9",
  "color": "0xFFFFFF",
  "format": "%s",
  "strokeWidth": 2
  "type": "STRING",
  "key": "stress"
}
````

The elements are as follows:

* `x, y, w, h`: *Required*. These provide the size and location of the field on the display. The location (0, 0) represents the upper left corner of the display. Note that displayed values are not clipped to the bounding box defined by `w, h`. 
* `xOff, yOff	`: *Optional*. Gives the offset within the bounding box where the content will be rendered. If not specified, it will default to (0, 0).
* `justify`: *Optional*. Defines the alignment of the displayed content within the field. For example, if you would like the content to be centered horizontally and vertical within the field's bounding box, then set `(xOff, yOff)` to `(width/2, height/2)` and use `MC` as the `justify` value.
  
  Allowed values are:
  * TL: Top Left
  * TC: Top Center
  * TR: Top Right
  * ML: Middle Left
  * MC: Middle Center
  * MR: Middle Left
  * BL: Bottom Left
  * BC: Bottom Center
  * BR: Bottom Right

  If not specified, it will default to `TL`. 
* `font`: *Optional*. The font to be used. The structure of the names are: `Family` `Style` `Size`. The families are Mono (M), SansSerif (S), Digital (D). The styles are Regular (default), Bold (B), Oblique (O), and Bold-Oblique (BO). The allowed values are:
  * M9, MB9, MO9, MBO9: 9 point Mono fonts in all styles
  * S9, SB9, SO9, SBO9: point Sans fonts in all styles
  * S12, SB12, SO12, SBO12: 12 point Sans fonts in all styles
  * S18, SB18, SO18, SBO18: 18 point Sans fonts in all styles
  * S24, SB24, SO24, SBO24: 24 point Sans fonts in all styles
  * D20, D72, D100: 7-Segment Digital Font in large sizes
  
  `font` may also be a number corresponding to a built-in font in the [TFT\_eSPI](https://github.com/Bodmer/TFT_eSPI). If this item is omitted, it defaults to built-in font 2.
* `color`: *Required*. The color of the content of the field. This is a 24-bit (888) hex color specifier (not a name) which may begin with `#` or `0x`.
* `format`: *Required*. The format is used to display the content of the field. It is a `printf` style format. For example if the field is meant to display a temperature, then the format might be: `"Temp: %0.1fF"`. If the field is just a static label, then the format specifies the label. Note: while optional, if this is not supplied, then no content will be displayed other than a border if specified.   The `format` must correspond to the `type`. For example, if the `type` is `FLOAT`, then the `format` must include some variation of `%f`. 
  
  There are special values of the format string that begin with '#'. These format strings indicate that a a custom display element should be used rather than displaying the result textually. The current set of custom display elements are:
  * `#progress`: Display a progress bar.
* `strokeWidth`: *Optional*. If supplied it specifies the size of a border to be drawn around (inside of) the bounding box. If not supplied, then no border will be drawn.
* `type`: *Optional*. Specifies the type of the value to be displayed in the field.
  * Allowed values are: `INT`, `FLOAT`, `STRING`, `BOOL`.
  * All of the previous types refer to single values. There are a few pre-defined types which are composite values:
      * `CLOCK`: This is a composite of three INT values representing an hour, minute, and second. A value of this type should be used with a format such as `%2d:%02d:%02d`, which would display the hour, minute, and seconds. You could also use `%2d:%02d`, which would display only the hour and minute.
      * `STATUS`: This composite type consists of an `INT` and a `STRING`. For example they might be a status code and a message. The associated format must have the `%s` for the message before the `%d` for the code. For example: `"Error %s (%d)"`.
      * A value of this type may also be used in conjunction with the `#progress` format which will display a progress bar.
* `key`: The name of a data value that should be supplied by the Plugin framework when the screen is displayed. Allowed values are defined by the custom Plugin code ***or*** a system value may be used (see [below](#datavalues)). For example, a custom Plugin that reads sensor data might have a value called "stress". This name may be used as a key.

<a name="datavalues"></a>
## Data Values

Any plugin (generic or custom) may access values made available by an app-wide data broker. Values are provided by a number of built-in sources, but can also be provided by plugins and their associated data providers.  For example, the Blynk Weather plugin example makes all of its data available to other plugins.

Values have names that are composed of multiple parts. All keys begin with a '$' followed by a single letter:

* `$S`: Corresponds to the system namespace which contains keys for things like the current time and heap statistics
* `$G`: Corresponds to the Grill namespace which contains keys related to the grill being monitored such as the status of the grill and the requested grill temperature.
* `$W`: Corresponds to the weather namespace which contains keys such as the temperature in the selected city.
* `$E`: Corresponds to the ***extension*** namespace which is where plugins can publish their own data values.

The list of available values for `$S`, `$P`, and `$W` are given in the table below.


| Full name | Type 	  | Description	|
|-----------|:-------:|------------	|
| **System**    |         |	             |
| `$S.author` | STRING  | GrillMon's author                 |
| `$S.heap`   | STRING  | Heap free space and fragmentation |
| `$S.time`   | CLOCK   | The current time |
| **Weather**   |   | |
| `$W.temp`   | FLOAT   | Current temperature |
| `$W.desc`   | STRING  | Short description of weather conditions  |
| `$W.ldesc`  | STRING  | Longer description of weather conditions |
| **Grill**   |   | Note that if *GrillMon* is not connected to a grill, the values below will not be meaningful. |
| `$G.connected`  | BOOL | Indicates whether GrillMon is connected to a grill |
| `$G.id`  | STRING | The ID of the connected grill. |
| `$G.fv`  | STRING | The firmware version of the connected grill. |
| `$G.state`  | STRING | The state of the connected grill: On, Off, or Fan. |
| `$G.fireState`  | STRING | The state of the flame of the connected grill. For example `Starting` or `Running` |
| `$G.warning`  | STRING | A representation of the current warning. If there is no warning (which is normal), it will be "None" |
| `$G.dft`  | INT | The desired food temperature. |
| `$G.cft`  | INT | The current food temperature. |
| `$G.dgt`  | INT | The desired grill temperature. |
| `$G.cgt`  | INT | The current grill temperature. |
| `$G.foodTemps`  | STRING | A formatted version of the food temperatures in the form `Current / Desired` |
| `$G.grillTemps`  | STRING | A formatted version of the grill temperatures in the form `Current / Desired` |

Any plugin that publishes data should provide a similar list of available data. See the [Blynk Weather Plugin](#bwp) for an example.


## Plugin Examples

`WebThingApp` provides several plugins that may be used (or not) by specific apps. All of the JSON descriptor files for these plugins are in the `data/plugins/` directory, but the settings files are named `sample.json` rather than `settings.json`. Simple rename the `sample.json` to `settings.json` if you want to use it (without the proper name, the plugin won't be loaded).

<a name="bwp"></a>
### The Blynk Weather Plugin

The Blynk Weather Plugin is an example of a custom plugin that provides a new data source and custom plugin code. The new data source gets data from the [Blynk](https://blynk.io) service. In this example a weather station such as [JAWS](https://github.com/jpasqua/JAWS) writes readings like temperature and humidity to the Blynk service. The Blynk Weather plugin collects data from up to two weather stations and displays the information in a new screen:

![](doc/images/BlynkScreen.png)

This plugin consists of the following files:

* `BlynkClient.[cpp,h]`: The client code that reads data from the Blynk service that was written there by a weather station. Though BlynkClient is used by the BlynkWeather plugin, it can also be used by any plugin that wants to read arbitrary information from Blynk.
* `BlynkPlugin.[cpp,h]`: The custom plugin code that manages settings and maps names to values. 
* `data/plugins/blynk1/*.json`: The JSON plugin descriptors as [described above](#descriptors).

This plugin has a number of settings that need to be specified by the user such as the Blynk IDs associated with the two weather stations and nicknames for each station that will be displayed in the GUI. The `form.json` file describes the web form that will be used to present the settings to the user. [jsonform](https://github.com/jsonform/jsonform) is a very rich tool that is well described on github. Please refer to its [wiki](https://github.com/jsonform/jsonform/wiki) to learn more.

To use this plugin you would have to have a a weather system that publishes data out to Blynk. However, the plugin can be easily adapted to other sensor data that is available through the Blynk service.

This plugin also acts a data source for other plugins. All of the data it retrieves from Blynk can be accessed through the [DataBroker](#datavalues). The plugin uses the `BW` namespace, so to access Blynk Virtual pin 0 on device 0, the full key would be `$E.BW.0/V0`:

* `$E.` means this is a request for data from an extension data source (i.e. a plugin)
* `BW.` means get it from the plugin whose namespace was declared to be `BW`
* `0/V0` means Blynk device 0, virtual pin 0. For this example, that is the temperature reading.

The full set of available data is:

| Full name | Type 	  | Description	|
|-----------|:-------:|------------	|
| `$E.BW.NN1`  | STRING  | The nickname for the first Blynk device      |
| `$E.BW.NN2`  | STRING  | The nickname for the second Blynk device     |
| `$E.BW.0/V0` | FLOAT   | The temp reading from the first Blynk device |
| `$E.BW.0/V1` | FLOAT   | The humidity reading from the first Blynk device |
| `$E.BW.0/V2` | FLOAT   | The barometer reading from the first Blynk device |
| `$E.BW.0/V7` | FLOAT   | The battery reading from the first Blynk device |
| `$E.BW.0/V8` | STRING  | The formatted time of the most recent readings from the first Blynk device |
|  | **NOTE**  | All of the same readings are available for the second Blynk device. Just replace the leading 0 with a 1. For example, `$E.BW.0/V1` &rarr; `$E.BW.1/V1` |

### The Generic Plugin

The generic plugin shows how to create a custom screen with your own layout that displays existing data from the [system or any other plugin](#datavalues). In this case, there is no code - just the JSON descriptor files.

![](doc/images/PluginSplash.png)

Some things to notice in the descriptor files:

* **plugin.json**:
  * The name specified here is the display name. It gets used by `UtilityScreen` to label a button. It is also used in the [plugin settings page](404) in the Web UI.
  * The type is "generic" which tells the system that this is a generic plugin rather than one that requires custom code.
* **screen.json**: The screen layout has examples of left, right, and center-justifying fields. It uses different fonts, different sizes, and different colors. It pulls data from the system namespace, the grill namespace, the weather namespace, and from the Blynk plugin. If you're not using that plugin you can remove those fields. It also uses the `CLOCK` type.
* **settings.json**: Generic plugins have four elements in their settings file. Only two are visible to the user:
  * `version`: *Not user visible*. This is the version of the settings structure. It is used internally by the system.
  * `enabled`: *User visible*. Is the plugin enabled? That is, will the user be able to select this plugin in the GUI. 
  * `refreshInterval`: *User visible*. How often the GUI should be updated.
  * `riScale`: *Not User Visible*. The units of `refreshInterval`. Internally all refresh times are represented in milliseconds. If you want the user to operate in seconds, then `riScale` should be 1000 (1000 millis in a second). If you want the user to set values in minutes, then `riScale` should be 60*1000.
* **form.json**: This file describes how the settings should be displayed to the user in the Web UI. A couple of points of interest here:
  * The version setting is not listed at all since it is not sent from the browser to the app.
  * The `riScale` field is sent to the server, but the user doesn't see it, so it is declared as hidden.

# Limitations, innefficiencies, oddities, etc.

There are numerous limitations and innefficiencies in this library and [WebThing](https://github.com/jpasqua/WebThing) overall. As they occur to me, I will list them here.

* Bound to one display type. At the moment this library assumes a specific display type which is 320x240 pixels and has a touch screen. The underlying TFT_eSPI library has more flexibility and so could this library.
* Support for OpenWeatherMap and weather screens is built in at a fairly low level. This should be optional and modular, including the settings, Web UI configuration, and inclusion of the associated screens.
* There should be a type of plugin with no screen.
	* It would simply be a data provider (based on one or more clients) with settings and a Web UI for configuration.
	* If that were the case, then the OpenWeatherMap stuff would just be a plugin rather than being built-in
* DataBroker can only provide String results which limits its utility.
	*  For example, it would be nice if all (or at least most) Screens could be implemented solely based on data from the DataBroker rather than having hard linkages to client code.
	*  The same is true for the Web UI. It should be able to get all of it's data from the DataBroker. 
