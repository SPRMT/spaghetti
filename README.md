## General description.
Drone detector for detecting enemy UAVs of the operational-tactical level such as ORLAN, ZALA ets"
```
enum class GuessedDroneType {
  None = 0,
  Unknown = 1,
  Orlan = 2,
  Zala = 3,
  Eleron = 4,
  ZalaLancet = 5,
  Lancet = 6,
  FPV_CrossFire = 7,
  FPV_ELRS = 8,
  MaybeOrlan = 9,
  MaybeZala = 10,
  MaybeLancet = 11,
  MaybeEleron = 12,
  DJI = 13,
  Supercam = 14,
  MaybeSupercam = 15,
  REB = 16,
  CryptoOrlan = 17,
  DJIe = 18
};
```

## Tactics
Each vehicle can be equipped with a drone detector, networked through StarLink. In the event of potential threats, the detectors notify the line operator through an aggregation system


## Topics
- Configuration (subscribed): `/tsukorok-config/<STATION_ID>`
- Detection data (publishes): `tsukorok/<STATION_ID>`
- OTA update (subscribed): `/tsukorok-update>`
