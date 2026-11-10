# Planning – Niet-zo-single-feestje (3 projectdagen, Scrum)

Mijlpalen volgens de casus: **ontwerp → bouw → test**. Elke dag is één sprint en eindigt met een product
review en een retrospective. Die leveren het bewijs voor W1, W2 en W3.

## Sprint 1: Ontwerp (dag 1)
**Doel:** een ontwerpdocument dat het team en de docent goedkeuren, en een opgezet Wokwi-project.

- [ ] Dagstart: rollen verdelen (bijv. scrummaster, bouwer hardware/sketch, documentatie/presentatie) en notulen maken (W1)
- [x] Ontwerpbeslissingen genomen (concept, OLED, MQTT, Word-versie)
- [ ] Broker en teamnaam voor de topic-prefix kiezen
- [ ] Ontwerpdocument afmaken (v1.0) en eventueel omzetten naar .docx voor de inlevering
- [ ] Wokwi-account, nieuw ESP32-project, schakeling uit het pinplan opbouwen
- [ ] Spike: werken de MFRC522-kaarten? UID's van de kaarten noteren
- [ ] Spike: werkt MQTT vanuit Wokwi naar de gekozen broker?
- [ ] **Product review 1:** ontwerp + lege schakeling aan de docent laten zien, feedback noteren
- [ ] **Retrospective 1** (bijv. Start/Stop/Continue)

## Sprint 2: Bouw (dag 2)
**Doel:** één hart werkt volledig en twee harten matchen met elkaar.

- [ ] Status-knop met kleuren op de LED-ring (single/bezet/ingewikkeld)
- [ ] Kaart tikken: UID uitlezen, omzetten naar hart-ID en tonen op de OLED
- [ ] Wifi + MQTT: status publiceren (retained) en status van een getikt hart ophalen
- [ ] LIKE: like versturen, ontvangen like laten knipperen
- [ ] Matchlogica, melodie en regenboog-animatie
- [ ] Tweede Wokwi-project (hart B) met dezelfde sketch en `MIJN_ID = "B"`
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

## Inleverlijst (bewijsmateriaal)
- [ ] Ontwerpdocument (v1.0)
- [ ] Wokwi-links (hart A en hart B) + code in Git
- [ ] Notulen van de dagstarts/overleggen (W1)
- [ ] Presentatie + eventueel de demovideo (W2)
- [ ] Retrospectives + individuele reflecties (W3)
- [ ] Verslagen van de product reviews (feedback + wat we ermee gedaan hebben)
