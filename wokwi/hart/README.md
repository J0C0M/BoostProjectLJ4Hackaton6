# Smèrrig-hart – Wokwi-prototype

**Wokwi-projecten**
- Hart A: https://wokwi.com/projects/480295580175545345
- Hart B: https://wokwi.com/projects/480300556742591489 (`MIJN_ID = 'B'`)

| Bestand | Plakken in Wokwi-tabblad |
|---|---|
| `diagram.json` | **diagram.json**: alle onderdelen + draden |
| `sketch.ino` | **sketch.ino**: de hart-code |
| `libraries.txt` | **libraries.txt**: de benodigde bibliotheken |

## In Wokwi zetten
1. Open het project in Wokwi en stop de simulatie (■).
2. Per bestand: klik op het tabblad, druk op `Ctrl+A`, plak de inhoud van het bestand hier met `Ctrl+V`.
3. Klik op **SAVE** en daarna op ▶.

## Hart B
**SAVE ▾ → Save a copy**, noem het `Smerrig-hart B` en verander in `sketch.ino`:
```cpp
const char MIJN_ID = 'B';
```

## Bediening
| Actie | Hoe |
|---|---|
| Status wisselen | Groene knop STATUS (of toets `S`) |
| Hart-tik | Klik op de MFRC522 → kies een kaart → **Tap** (groen = A, geel = B, rood = C) |
| Like sturen | Rode knop LIKE (of toets `L`) binnen 5 s na een tik |

## Demo (twee tabbladen naast elkaar)
1. Start hart A en hart B. Wacht tot beide OLED's **online** tonen.
2. A: zet status op single. B: zet status op ingewikkeld.
3. A tikt de **gele** kaart (= B) en drukt op LIKE → B knippert roze.
4. B tikt de **groene** kaart (= A) en drukt op LIKE → **MATCH** op beide harten.

MQTT-topics: `smerrig013/BoostProject/hart/<id>/status|like|match` op `broker.hivemq.com`
(meekijken: https://www.hivemq.com/demos/websocket-client/, subscribe op `smerrig013/BoostProject/hart/#`).
