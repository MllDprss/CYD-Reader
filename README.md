# CYD-Reader

<table>
  <tr>
    <td>
      <img width="200" alt="CYD-Reader" src="https://github.com/user-attachments/assets/ae34c941-93f4-47c6-9b4b-0d2e3ee3f511" />
    </td>
    <td>
      <p>Portable e-reader for ESP32 with a touchscreen display: reads a <code>.txt</code> file from an SD card, formats it into pages, and remembers where you left off.</p>
    </td>
  </tr>
</table>

*[Leggi in Italiano 🇮🇹](#italiano-)*

## Features
- Reads `libro.txt` from an SD card
- Automatic pagination with word-wrapping and paragraph breaks
- Touch page-turning (top half: next page, bottom half: previous page)
- Progress bar with percentage
- Saves current page and byte offset to flash memory (`Preferences`) so you can resume reading after powering off

## Hardware
**ESP32-2432S028 ("Cheap Yellow Display")** board:
- ESP32 (`esp32dev`)
- 240x320 ILI9341 TFT display
- XPT2046 resistive touchscreen
- microSD card slot

All pins are pre-configured in `platformio.ini` (display) and `src/main.cpp` (touch and SD).

## Build & Upload
Requires [PlatformIO](https://platformio.org/).

    pio run -t upload
    pio device monitor

Dependencies (`TFT_eSPI`, `XPT2046_Touchscreen`) are downloaded automatically during compilation.

## Usage
1. Format the microSD card as **FAT32**.
2. Copy a plain text file named `libro.txt` to the root directory of the SD card.
3. Insert the microSD card and power on the board.

## Known Limitations
- Maximum of 500 pages (`MAX_PAGES`).
- Plain text only (no rich formatting).
- Basic font support: accented characters depend on the loaded `TFT_eSPI` font.

---

## Italiano 🇮🇹

E-reader portatile su ESP32 con display touch: legge un file di testo dalla scheda SD, lo impagina e ricorda la pagina a cui sei arrivato.

### Funzionalità
- Lettura di `libro.txt` da scheda SD
- Impaginazione automatica con a capo per parola e paragrafi
- Cambio pagina toccando lo schermo (metà superiore: avanti, metà inferiore: indietro)
- Barra di avanzamento con percentuale
- Salvataggio di pagina e offset in memoria flash (`Preferences`): alla riaccensione riprendi da dove avevi lasciato

### Hardware
Scheda **ESP32-2432S028 ("Cheap Yellow Display")**:
- ESP32 (`esp32dev`)
- Display TFT ILI9341 240x320
- Touch resistivo XPT2046
- Slot microSD

I pin sono già configurati in `platformio.ini` (display) e in `src/main.cpp` (touch e SD).

### Compilazione
Serve [PlatformIO](https://platformio.org/).

    pio run -t upload
    pio device monitor

Le librerie (`TFT_eSPI`, `XPT2046_Touchscreen`) vengono scaricate automaticamente.

### Uso
1. Formatta la microSD in **FAT32**.
2. Copia nella root un file di testo chiamato `libro.txt`.
3. Inserisci la SD e accendi la scheda.

### Limiti noti
- Massimo 500 pagine (`MAX_PAGES`).
- Solo testo semplice, senza formattazione.
- Font di base: i caratteri accentati dipendono dal font `TFT_eSPI` caricato.
