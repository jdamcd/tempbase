### WiFi temperature sensor

Logs timestamped temperature readings to a Firebase database at a set interval.

#### Hardware

  * Arduino MKR1000
  * DHT11 temperature & humidity sensor

#### Parameters

`SSID` & `PASSWORD` are the WiFi credentials.  `INTERVAL` is the measurement frquency (default is 5 mins).

##### Firebase
  
  * `HOST` e.g. `yourapp.firebaseio.com`
  * `SECRET` is the database secret for auth (Project settings > Database)
  * `PATH` e.g. `/home/bedroom.json`

#### Payload

```
{
	"timestamp":1472420271,
	"temperature":30,
	"humidity":45
}
```

