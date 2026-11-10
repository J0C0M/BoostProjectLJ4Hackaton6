# Planning – Niet-zo-single-feestje (3 projectdagen, Scrum)

Mijlpalen volgens de casus: **ontwerp → bouw → test**. Elke dag is één sprint en eindigt met een product
review en een retrospective. Die leveren het bewijs voor W1, W2 en W3.

## Sprint 1: Ontwerp (dag 1)
**Doel:** een ontwerpdocument dat het team en de docent goedkeuren, en een opgezet Wokwi-project.

- [ ] Dagstart: rollen verdelen (bijv. scrummaster, bouwer hardware/sketch, documentatie/presentatie) en notulen maken (W1)
- [x] Ontwerpbeslissingen genomen (concept, OLED, MQTT, Word-versie)
- [x] Broker en teamnaam voor de topic-prefix kiezen (`broker.hivemq.com`, `smerrig013/BoostProject/`)
- [ ] Ontwerpdocument afmaken (v1.0) en omzetten naar .docx voor de inlevering (nu v0.3, .docx is er)
- [x] Wokwi-account, nieuw ESP32-project, schakeling uit het pinplan opbouwen
- [x] Spike: werken de MFRC522-kaarten? UID's van de kaarten noteren (zie ontwerpdocument §6.5)
- [ ] Spike: werkt MQTT vanuit Wokwi naar de gekozen broker? (testproject `MQTT-test` staat klaar, resultaat nog noteren)
- [ ] **Product review 1:** ontwerp + lege schakeling aan de docent laten zien, feedback noteren
- [ ] **Retrospective 1** (bijv. Start/Stop/Continue)

## Sprint 2: Bouw (dag 2)
**Doel:** één hart werkt volledig en twee harten matchen met elkaar.

> Stand 10 november: alle onderdelen hieronder zijn in de code gebouwd (`wokwi/hart/sketch.ino`) en de code compileert. In Wokwi is alleen het uitlezen van de kaarten getest. De rest testen we met het testplan hieronder.

- [x] Status-knop met kleuren op de LED-ring (single/bezet/ingewikkeld)
- [x] Kaart tikken: UID uitlezen, omzetten naar hart-ID en tonen op de OLED
- [x] Wifi + MQTT: status publiceren (retained) en status van een getikt hart ophalen
- [x] LIKE: like versturen, ontvangen like laten knipperen
- [x] Matchlogica, melodie en regenboog-animatie
- [x] Tweede Wokwi-project (hart B) met dezelfde sketch en `MIJN_ID = 'B'`
- [x] Personeelsmonitor als webpagina gebouwd (`monitor/index.html`, eis E1–E6, zie ontwerpdocument §2.1 en §6.8)
- [ ] ZONE-knop (GPIO 14) toevoegen aan `sketch.ino` en `diagram.json`, en elke 10 s en bij een wissel de zone publiceren op `…/hart/<id>/zone`
- [ ] Hart A in Wokwi bijwerken met de juiste `diagram.json`
- [ ] Match-demo met hart A en B testen in Wokwi (T1–T8)
- [ ] **Product review 2:** demo van de flow, feedback noteren
- [ ] **Retrospective 2**

## Sprint 3: Test & presentatie (dag 3)
**Doel:** een stabiele demo, een presentatie en alle bewijsstukken compleet.

- [ ] Testplan uitvoeren (zie hieronder) en bugs oplossen
- [ ] Code opschonen en commentaar toevoegen, alles in Git zetten (`wokwi/` map)
- [ ] Presentatie maken: aanleiding → idee → doelgroep/stakeholders → techniekkeuze → **live demo** → productie/kosten → privacy → conclusie
- [ ] Achtervangvideo opnemen van de demo (voor als wifi of de broker het tijdens de presentatie niet doet)
- [ ] **Eindpresentatie** (W2)
- [ ] **Eindretrospective** + individuele reflectie (W3)

## Testplan (concept)
| # | Test | Verwacht resultaat |
|---|---|---|
| T1 | STATUS 3× indrukken | groen → rood → geel → groen |
| T2 | Groene kaart tikken op hart B | OLED B: "Hart A – <status van A>" |
| T3 | LIKE zonder voorafgaande tik | Er gebeurt niets (of de melding "eerst tikken") |
| T4 | A liket B | B knippert roze, geen match |
| T5 | Daarna liket B A | Beide harten: MATCH, melodie en regenboog |
| T6 | Onbekende kaart tikken | OLED: "Onbekend hart" |
| T7 | 5 s niets doen na een tik | Terug naar IDLE |
| T8 | Broker niet bereikbaar | Status blijft lokaal werken, OLED: "offline" |
| T9 | Monitor openen, hart A op de dansvloer zetten (demopaneel of ZONE-knop) | Dansvloer toont 1 hart "A" (E2) |
| T10 | Hart A van dansvloer naar bar laten gaan | Binnen 15 s staat A bij de bar en niet meer op de dansvloer (E3) |
| T11 | Hart 60 s niets laten melden | Hart staat onder "Geen signaal" (E4) |
| T12 | Zo veel harten in één zone zetten dat 70% en daarna 100% van de capaciteit wordt bereikt | Zone kleurt oranje ("druk") en daarna rood ("vol") (E5) |
| T13 | Status wisselen, liken en matchen terwijl de monitor openstaat | De monitor toont hier niets van, alleen hart-ID en zone (E6) |

## Inleverlijst (bewijsmateriaal)
- [ ] Ontwerpdocument (v1.0)
- [ ] Wokwi-links (hart A en hart B) + code in Git
- [ ] Notulen van de dagstarts/overleggen (W1)
- [ ] Presentatie + eventueel de demovideo (W2)
- [ ] Retrospectives + individuele reflecties (W3)
- [ ] Verslagen van de product reviews (feedback + wat we ermee gedaan hebben)
