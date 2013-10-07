##Installation##
The Arduino IDE has a built-in function for installing libraries. Go to **Sketch > Import Library... > Add Library...** and select the zip file or folder containing the library.

If you're not using the Arduino Development Environment make sure the files are in a folder named 'CommonSense' and place the folder in your libraries folder.

##Reference Documentation##
Have a look at the [library documentation](http://senseobservationsystems.github.com/commonsense-arduino-lib/) for more information.
The API methods available to developers can be found on the [CommonSense Developers Pages](http://www.sense-os.nl/developers).

##Examples##
Sketch examples are included with this library. You can select one from **File > Examples > CommonSense**.

##Usage##
Let's get started by including the CommonSense library. This will also include arduino core dependencies:

```c
#include <CommonSense.h>
```

We will need some information to connect and logon to CommonSense:

```c
const char *username = "test";
const char *password = "098f6bcd4621d373cade4e832627b4f6";
const char *deviceName = "Arduino Uno";
const byte MAC[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
```

We will also need to store the sensor identifier:

```c
long pressure;
```

Now it's time to create an object so we can talk to CommonSense:

```c
CommonSense sense = CommonSense(username,password,deviceName,MAC);
```

Now that's done, we should use CommonSense::begin() inside the setup() of the sketch.
This function sets up ethernet and time to call CommonSense::login().

```c
sense.begin();
```

With CommonSense::login() having saved the session id within the object,
we can create the sensors in our CommonSense account.

```c
pressure = sense.createSensor("Pressure", "P1 Pressure Sensor", "P1SEN");
```

We got our sensors up, now it's time to acquire data and upload it.

```c
sense.uploadData(pressure, analogRead(A0));
```

Your data should now be visible in the sensor library.


##License##
Licensed under the [Apache License, Version 2.0](http://www.apache.org/licenses/LICENSE-2.0.html)