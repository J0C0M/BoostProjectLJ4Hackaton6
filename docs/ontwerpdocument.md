# Ontwerpdocument – Niet-zo-single-feestje

**Concept:** het Smèrrig-hart, een slimme hartketting voor Smèrrig in Poppodium 013  
**Project:** Hackaton 6 (LJ4), kerntaak 2  
**Prototype-omgeving:** [Wokwi](https://wokwi.com/) (ESP32)  
**Versie:** 0.3 (prototype gebouwd in Wokwi)  
**Code:** https://github.com/J0C0M/BoostProjectLJ4Hackaton6 (branch `development`, map `wokwi/hart/`)

> Dit document bouwt voort op de eerste conceptuitwerking (NFC-hartketting, uitgewerkt voor Tinkercad).
> In de wijzigingslog onderaan staat wat er is veranderd en waarom.

---

## 1. Aanleiding

Poppodium 013 in Tilburg organiseert rond 14 februari het feest **Smèrrig**. Eerdere edities werkten met
gekleurde bekers (single, bezet of "het is ingewikkeld") en later met een briefjessysteem met nummers. Dat
werkte, maar het past niet meer bij deze tijd. 013 wil een upgrade: een **hartketting of broche met een
microcontroller (Arduino of ESP)**, met bijvoorbeeld licht, geluid of connecties.

Dit document beschrijft het idee, de keuzes en de onderbouwing, en hoe we het prototype in Wokwi bouwen.

## 2. Het idee: het Smèrrig-hart

Elke bezoeker krijgt bij de ingang een hartketting. In het hart zitten een kleine ESP32, een ring met
LED's, een zoemer, twee knopjes en een NFC-chip.

| Functie | Wat de bezoeker doet | Wat het hart doet |
|---|---|---|
| **Status** | Druk op de statusknop | Het hart licht op in de statuskleur: groen = single, rood = bezet, geel = het is ingewikkeld. Zo werkten de bekers vroeger, maar nu is het ook in een donkere zaal te zien. |
| **Hart-tik** | Tik je hart tegen het hart van iemand anders | Het hart leest het ID van het andere hart en toont de status ervan. |
| **Like** | Druk binnen een paar seconden na de tik op de like-knop | Er gaat anoniem een like naar het andere hart. Dat hart knippert kort ("iemand vindt je leuk"). Het is de digitale versie van het briefje. |
| **Match** | Jullie hebben elkaar allebei geliket | Beide harten spelen een korte melodie en laten een regenboog-animatie zien. Het is een moment om elkaar te zoeken. |

**Waarom dit concept:**
- We houden wat werkte (de kleurcodes van de bekers en het anonieme briefje) en maken het interactief.
- **Licht werkt in een donkere club.** Een gekleurd plastic hart zie je op de dansvloer niet, een lichtgevend hart wel.
- **De tik is een bewuste actie.** Je moet naar iemand toe. Dat maakt het spannend en sociaal, en het voelt niet als tracking op afstand.
- **Geen app nodig.** Alles werkt met het hart zelf. Wie wil, kan het hart ook met een telefoon scannen (zie 4.3).

## 3. Doelgroep en stakeholders

### Doelgroep
Bezoekers van Smèrrig: jongeren en jongvolwassenen (ongeveer 18–30) die willen dansen en iemand willen
leren kennen. Ze willen geen gedoe, willen niets installeren en willen zelf bepalen wat ze van zichzelf
laten zien.

**Gevolgen voor het ontwerp:** binnen 5 seconden te begrijpen, bediening met één hand, ook
aangeschoten nog bruikbaar, standaard anoniem.

### Stakeholders

| Stakeholder | Belang | Gevolg voor ons ontwerp |
|---|---|---|
| Bezoekers | Leuke avond, makkelijk contact, controle over eigen gegevens | Twee knoppen, duidelijke kleuren, standaard anoniem |
| Poppodium 013 (opdrachtgever) | Onderscheidend concept, beheersbare kosten, weinig organisatielast | Herbruikbare harten met statiegeld, eenvoudige uitgifte |
| Organisatie Smèrrig | Sfeer en veiligheid | Blokkeerfunctie, geen persoonsgegevens op het hart |
| Beveiliging en bar | Geen extra drukte | Uitgifte bij de garderobe/ingang, geen opladen tijdens het feest |
| Projectteam (studenten) | Haalbaar prototype in 3 dagen | Een sketch voor alle harten, bewezen Wokwi-onderdelen |

## 4. Keuze van de techniek

### 4.1 Vergelijking

| Criterium | Passieve NFC-tag (eerste concept) | **ESP32-hart met LED + NFC (keuze)** | Bluetooth-scan op afstand |
|---|---|---|---|
| Past bij de opdracht ("microcontroller in het hart") | Nee | **Ja** | Ja |
| Zichtbaar in een donkere zaal | Nee, alleen een gekleurd hart | **Ja, licht** | Ja |
| Interactie | Telefoon nodig | **Hart-tegen-hart, geen telefoon nodig** | Automatisch, minder bewust |
| Privacy | Goed | **Goed (alleen een willekeurig ID)** | Matig (constante uitwisseling) |
| Kosten per stuk | Zeer laag | Gemiddeld, terug te verdienen met hergebruik | Gemiddeld |
| Batterij | Niet nodig | Nodig (kleine LiPo, één avond) | Nodig |
| Te simuleren in Wokwi | Nauwelijks | **Volledig: ESP32, LED-ring, zoemer, NFC-lezer, wifi** | Gedeeltelijk |

### 4.2 Keuze en argumenten
We kiezen voor het **ESP32-hart met LED's en NFC**:
1. **Het sluit aan op de vraag van 013:** een microcontroller in het hart, met licht, geluid en connecties.
2. **Je ziet het in het donker.** Het grootste nadeel van een passieve tag is dat je de status op de dansvloer niet ziet.
3. **De tik blijft een bewust moment.** Het goede argument uit het eerste concept houden we.
4. **Het is volledig te simuleren in Wokwi.** De ESP32 heeft wifi, zodat we in de demo twee harten echt met elkaar kunnen laten matchen.

### 4.3 Benoemde beperkingen
- **Kosten en batterij:** een actief hart is duurder dan een tag. We lossen dat op met hergebruik (zie §7).
- **Telefoon als terugvaloptie:** op de achterkant van het hart zit ook een passieve NFC-sticker met hetzelfde ID. Wie wil, kan het hart scannen met een telefoon (webpagina, geen app). Dat is een mogelijke uitbreiding en zit niet in het prototype.
- **Netwerk:** de harten praten via de wifi van 013 met een server. In het prototype gebruiken we hiervoor een openbare MQTT-broker.

## 5. Keuze van de prototypetool: Wokwi

| Criterium | Tinkercad | **Wokwi** |
|---|---|---|
| Boards | Arduino Uno e.d. | **Arduino, ESP32, Raspberry Pi Pico** |
| NFC/RFID | Geen lezer, alleen een knop als nabootsing | **MFRC522-lezer met virtuele kaarten en tags om te "tikken"** |
| Wifi/internet | Nee | **Ja (netwerk `Wokwi-GUEST`), dus MQTT en HTTP** |
| Adresseerbare LED-ring (NeoPixel) | Ja | Ja |
| OLED-scherm | Beperkt | Ja (SSD1306) |
| Versiebeheer | Nee | **Ja: `diagram.json` en de sketch kunnen in Git** |
| 3D-behuizing | Ja (Tinkercad 3D) | Nee (niet nodig voor ons prototype) |

**Keuze: Wokwi.** Het eerste concept koos voor Tinkercad, omdat geen van beide tools NFC zou kunnen
simuleren. Dat klopt niet meer: Wokwi heeft een MFRC522-lezer met virtuele kaarten. Bovendien kan een
ESP32 in Wokwi online, zodat we **twee harten in twee browsertabbladen echt met elkaar kunnen laten
matchen**. Voor de presentatie is dat veel sterker dan losse knoppen.

## 6. Ontwerp van het prototype

### 6.1 Componenten

| Wokwi-onderdeel | Part-ID | Staat voor in het echte product |
|---|---|---|
| ESP32 DevKit C v4 | `board-esp32-devkit-c-v4` | ESP32-C3 mini-module in het hart |
| LED-ring, 12 LED's | `wokwi-led-ring` | LED-rand rondom het hart (statuskleur, effecten) |
| MFRC522 RFID-lezer | `board-mfrc522` | NFC-antenne in het hart (hart-tik) |
| Virtuele kaart/tag | (in de MFRC522-bediening) | Het hart van iemand anders |
| Drukknop "STATUS" | `wokwi-pushbutton` | Statusknop |
| Drukknop "LIKE" | `wokwi-pushbutton` | Like-knop (eventueel aanraakvlak) |
| Piëzo-zoemer | `wokwi-buzzer` | Kleine zoemer/trilmotor |
| OLED 128×64 | `board-ssd1306` | Alleen voor de demo: laat zien wat er gebeurt (zit niet in het product) |

### 6.2 Pinplan (ESP32 DevKit C v4)

| Component | Pin(nen) |
|---|---|
| MFRC522 (SPI) | SDA/SS → GPIO 5, SCK → GPIO 18, MOSI → GPIO 23, MISO → GPIO 19, RST → GPIO 4, 3V3, GND |
| OLED (I²C) | SDA → GPIO 21, SCL → GPIO 22, VCC → 3V3, GND |
| LED-ring | DIN → GPIO 13, VCC → 5V, GND |
| Knop STATUS | GPIO 32 → knop → GND (`INPUT_PULLUP`) |
| Knop LIKE | GPIO 33 → knop → GND (`INPUT_PULLUP`) |
| Zoemer | GPIO 25 → zoemer → GND |

> Zo is het ook gebouwd in Wokwi (zie `wokwi/hart/diagram.json`).

### 6.3 Toestanden (state machine)

```
             STATUS-knop: volgende status (melding 1,5 s)
           ┌───────────┐
           ▼           │
      ┌──────────┐ ────┘          like ontvangen (anoniem)
      │   IDLE   │ ──────────────────────────────► [roze knipperen, 2,5 s] ──► IDLE
      │ (eigen   │
      │ status-  │      kaart getikt (MFRC522)
      │ kleur,   │ ──────────────────────────────► ┌──────────────┐ OLED: "Hart B: single"
      │ ademt)   │                                 │    GETIKT    │       "LIKE = like sturen"
      └──────────┘                                 │ (kleur ander │ ring: kleur van het andere
           ▲                                       │ hart + wit   │       hart + rondlopend wit
           │        5 s niets gedaan               │ lichtje)     │       lichtje
           ├────────────────────────────────────── └──────────────┘
           │                                              │ LIKE-knop
           │                                              ▼
           │                                      like versturen (MQTT)
           │   geen wederzijdse like:                     │
           │   "Like verstuurd" (2 s)                     ▼
           ├───────────────────────────────────── heeft B mij al geliket?
           │                                              │ ja
           │                                              ▼
           │        na 6 s                        ┌──────────────┐
           └───────────────────────────────────── │    MATCH!    │ melodie + regenboog
                                                  └──────────────┘ match-bericht naar B
```

Naast deze toestanden toont het hart **korte meldingen** op de OLED. De ring blijft dan in de
eigen statuskleur:

| Situatie | Melding op de OLED | Duur |
|---|---|---|
| Status gewisseld | "Status: single / bezet / ingewikkeld" | 1,5 s |
| LIKE zonder voorafgaande tik | "Tik eerst een hart aan" | 1,5 s |
| Eigen hart getikt | "Dit is je eigen hart :)" | 2 s |
| Onbekende kaart getikt | "Onbekend hart" + UID | 2 s |
| LIKE terwijl het hart offline is | "Offline: like niet verstuurd" | 2 s |
| Like verstuurd, nog geen match | "Like verstuurd naar hart B" | 2 s |

Bovenaan de OLED staan altijd het eigen hart-ID, de eigen status en de verbinding
("WiFi verbinden...", "offline" of "online"). Zonder verbinding werken de status en het tikken
gewoon door, alleen likes kunnen dan niet worden verstuurd.

### 6.4 Communicatie (MQTT)

- **Netwerk:** `Wokwi-GUEST` (geen wachtwoord)
- **Broker:** `broker.hivemq.com`, poort 1883 (openbare testbroker; alternatief `test.mosquitto.org`)
- **Topic-prefix:** `smerrig013/BoostProject/` (uniek, zodat we geen berichten van anderen binnenkrijgen)

| Topic | Payload | Wie publiceert | Wie luistert |
|---|---|---|---|
| `…/hart/<id>/status` (retained) | `single` / `bezet` / `ingewikkeld` | Elk hart bij een statuswissel en bij het verbinden | Alle harten (elk hart onthoudt de laatste status van ieder hart) |
| `…/hart/<id>/like` | `<van-id>` | Het hart dat liket | Hart <id> |
| `…/hart/<id>/match` | `<van-id>` | Het hart dat de match ontdekt | Hart <id> |

**Matchlogica, zonder server:** elk hart onthoudt wie hij heeft geliket en van wie hij likes heeft
ontvangen. Als A op like drukt en A al een like van B had, is er een match: A speelt de melodie en stuurt
`match` naar B. In het echte product doet een server dit, zodat het ook werkt na een herstart en
zodat er blokkeren en melden mogelijk is.

### 6.5 Hart-ID's in de demo
De kaarten in de MFRC522-bediening van Wokwi hebben elk een vast UID. Die koppelen we in de code aan
hart-ID's:

| Wokwi-kaart | UID | Staat voor |
|---|---|---|
| Groene kaart | `11 22 33 44` | Hart A |
| Gele kaart | `55 66 77 88` | Hart B |
| Rode kaart | `AA BB CC DD` | Hart C |
| Overige kaarten (blauw, NFC-tag, sleutelhanger) | – | "Onbekend hart" |

We gebruiken één sketch voor elk hart. Alleen de constante `MIJN_ID` verschilt (A of B). In de demo
draaien we twee Wokwi-projecten naast elkaar.

### 6.6 Demo-scenario (presentatie)
1. Hart A zet de status op **single** (groen), hart B op **het is ingewikkeld** (geel).
2. A tikt de gele kaart (= hart B). De OLED van A toont "Hart B: ingewikkeld".
3. A drukt op LIKE. Hart B knippert roze: "iemand vindt je leuk".
4. B tikt de groene kaart (= hart A) en drukt op LIKE. **MATCH:** beide harten spelen een melodie en laten een regenboog zien.

### 6.7 Gerealiseerd prototype
Het prototype is gebouwd in Wokwi en staat in Git.

| Wat | Waar |
|---|---|
| Hart A (Wokwi) | https://wokwi.com/projects/480295580175545345 |
| Hart B (Wokwi) | https://wokwi.com/projects/480300556742591489 |
| Code en schakeling | `wokwi/hart/` in de repo: `sketch.ino`, `diagram.json`, `libraries.txt` |
| Instructies | `wokwi/hart/README.md` |

**Bediening in de simulatie**

| Actie | Hoe |
|---|---|
| Status wisselen | Groene knop STATUS (of toets `S`) |
| Hart-tik | Klik op de MFRC522, kies een kaart en klik op **Tap** |
| Like sturen | Rode knop LIKE (of toets `L`) binnen 5 s na een tik |

**Opbouw van de code:** de code werkt niet-blokkerend, zodat licht, geluid, scherm en netwerk
tegelijk kunnen lopen. Elke lus leest de knoppen en de NFC-lezer, verwerkt MQTT-berichten en werkt
daarna de LED-ring, de zoemer en het scherm bij volgens de toestand uit §6.3. Hart B is dezelfde code
met `MIJN_ID = 'B'`.

**Stand van het testen (10 november)**
- De code compileert zonder fouten voor de ESP32.
- Het uitlezen van de kaarten met de MFRC522 is in Wokwi getest en werkt. De UID's kloppen met §6.5.
- De volledige match-demo met hart A en hart B (§6.6) moet nog getest worden. Dat doen we met het testplan in `planning.md`.

**Eigen test-ID's voor de demo:** wie meer harten wil laten zien, voegt een kaart en een ID toe aan
`HARTEN` en `KAART_UID` in de code.

## 7. Productie en kosten

| Onderdeel (indicatie, nog bij leveranciers navragen) | Prijs per stuk bij grote aantallen |
|---|---|
| ESP32-C3 mini-module | ± € 2–3 |
| LED-strip/ring (12 LED's) | ± € 1–2 |
| NFC-lezer (kleine module) + passieve sticker | ± € 2–4 |
| LiPo-batterij + laadchip | ± € 2–3 |
| Behuizing (spuitgiet of 3D-print) + koord | ± € 1–2 |
| **Totaal** | **± € 8–14** |

**Hoe het betaalbaar blijft:**
- **Statiegeld en hergebruik:** bezoekers lenen het hart (bijv. € 5 statiegeld) en leveren het bij vertrek weer in. De harten gaan meerdere edities mee, ook bij Smèrrig-feesten buiten 013.
- **Opladen:** de harten liggen vóór het feest in laadbakken. Eén acculading is genoeg voor één avond, want de LED's staan gedimd.
- **Uitgifte:** bij de garderobe. De bezoeker krijgt een hart en kiest zijn status.

## 8. Privacy en veiligheid
- Op het hart staat alleen een **willekeurig ID**, geen naam of telefoonnummer.
- Likes zijn **anoniem**. Pas bij een wederzijdse like (match) weten beide mensen het.
- Na het feest worden alle gegevens van de server gewist en worden de harten gereset.
- In het echte product kun je een hart blokkeren (twee keer tikken en de STATUS-knop vasthouden) en kan de organisatie een hart uitschakelen bij overlast.
- In het prototype gebruiken we een openbare broker. Daar komen **geen persoonsgegevens** op, alleen demo-ID's.

## 9. Werkwijze en beoordeling
We werken met **Scrum** in drie korte sprints (één per projectdag), elk met een planning, een product
review en een retrospective. De planning staat in [`planning.md`](planning.md).

Bewijs voor kerntaak 2:
- **B1-K2-W1 Overleggen:** notulen van de dagstarts en de afspraken met de opdrachtgever/docent.
- **B1-K2-W2 Presenteren:** de presentatie en de live demo van twee matchende harten in Wokwi.
- **B1-K2-W3 Reflecteren:** een retrospective per sprint en een individuele reflectie.

## 10. Conclusie
Het Smèrrig-hart neemt de kracht van de bekers en de briefjes mee naar een lichtgevende, interactieve
ketting. Je ziet de status ook in het donker, de tik is een bewust en spannend moment en likes blijven
anoniem tot er een match is. Met Wokwi kunnen we dit realistisch laten zien: een echte ESP32, een
NFC-lezer, LED's en twee harten die via het internet met elkaar matchen.

---

## Beslissingen
| Onderwerp | Besluit |
|---|---|
| Concept | Actief ESP32-hart (geen passieve NFC-tag) |
| Demo-weergave | OLED-scherm in de schakeling (zit niet in het echte product) |
| Verbinding in het prototype | MQTT met twee harten in twee Wokwi-projecten, plus een achtervangvideo |
| Extra's | Ontwerpdocument ook als Word-versie (.docx). Geen 3D-hart. |
| Broker en topic-prefix | `broker.hivemq.com`, prefix `smerrig013/BoostProject/` |

## Wijzigingslog
| Versie | Wijziging | Reden |
|---|---|---|
| 0.1 | Prototypetool van Tinkercad naar Wokwi | Teamkeuze. Wokwi heeft een NFC-lezer (MFRC522) en wifi/MQTT. |
| 0.1 | Concept van passieve NFC-tag naar ESP32-hart met LED's, zoemer en NFC | De opdracht vraagt om een microcontroller en de status moet in het donker zichtbaar zijn |
| 0.1 | Prototypeontwerp, pinplan, toestanden en MQTT-topics toegevoegd | Voorbereiding op de bouw in Wokwi |
| 0.2 | Teambeslissingen vastgelegd (concept, OLED, MQTT, Word-versie) | Ontwerpbespreking |
| 0.3 | Broker, topic-prefix en kaart-UID's ingevuld, §6.7 Gerealiseerd prototype toegevoegd | Prototype gebouwd in Wokwi |
| 0.3 | Toestanden (§6.3), meldingen, tijden en MQTT-luisteraars (§6.4) gelijkgetrokken met de code, teststand toegevoegd | Document moet beschrijven wat er gebouwd is |
