# CYD-Reader

E-reader portatile su ESP32 con display touch: legge un file di testo dalla scheda SD, lo impagina e ricorda la pagina a cui sei arrivato.

## Funzionalità

- Lettura di `libro.txt` da SD card
- Impaginazione automatica con a capo per parola e paragrafi
- Cambio pagina toccando lo schermo (metà superiore: avanti, metà inferiore: indietro)
- Barra di avanzamento con percentuale
- Salvataggio di pagina e offset in memoria flash (`Preferences`): alla riaccensione riprendi da dove avevi lasciato

## Hardware

Scheda **ESP32-2432S028** ("Cheap Yellow Display"):

- ESP32 (`esp32dev`)
- Display TFT ILI9341 240x320
- Touch resistivo XPT2046
- Slot microSD

I pin sono già configurati in `platformio.ini` (display) e in `src/main.cpp` (touch e SD).

## Compilazione

Serve [PlatformIO](https://platformio.org/).

```bash
pio run -t upload
pio device monitor
```

Le librerie (`TFT_eSPI`, `XPT2046_Touchscreen`) vengono scaricate automaticamente.

## Uso

1. Formatta la microSD in FAT32.
2. Copia nella root un file di testo chiamato `libro.txt`.
3. Inserisci la SD e accendi la scheda.

## Limiti noti

- Massimo 500 pagine (`MAX_PAGES`).
- Solo testo semplice, senza formattazione.
- Font di base: i caratteri accentati dipendono dal font TFT_eSPI caricato.

## Licenza

[MIT](LICENSE)