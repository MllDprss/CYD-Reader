#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <XPT2046_Touchscreen.h>
#include <SD.h>
#include <Preferences.h>

// --- PIN TOUCH ---
#define XPT2046_IRQ 36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK 25
#define XPT2046_CS 33

// --- PIN SD CARD ---
#define SD_CS 5

TFT_eSPI tft = TFT_eSPI(); 
SPIClass mySpi = SPIClass(HSPI);
XPT2046_Touchscreen ts(XPT2046_CS, XPT2046_IRQ);

File libro;
Preferences prefs;

// --- VARIABILI ESTETICHE (Margini e Interlinea) ---
const int MARGIN_LEFT = 15;    
const int MARGIN_RIGHT = 15;   
const int MARGIN_TOP = 15;     // Abbassato da 45 a 15 (Addio barra in alto!)
const int MARGIN_BOTTOM = 35;  // Spazio per la barra in basso
const int LINE_SPACING = 6;    

// --- VARIABILI IMPAGINAZIONE ---
const int MAX_PAGES = 500;       
uint32_t pageOffsets[MAX_PAGES]; 
int currentPage = 0;             
int maxPageReached = 0;          

// --- VARIABILI TOUCH ---
unsigned long lastTouchTime = 0;
const int debounceDelay = 500;   
bool wasTouched = false;         

void stampaIntestazione() {
  tft.fillScreen(TFT_BLACK); // Sfondo della pagina testo (Nero)
  
  // Calcolo percentuale
  uint32_t fileSize = libro.size();
  uint32_t currentPos = pageOffsets[currentPage];
  float percentuale = 0.0;
  if (fileSize > 0) {
    percentuale = (float)currentPos / fileSize;
  }

  // --- PALETTE COLORI CUSTOM (R, G, B) ---
  // Se il verde dovesse sembrare ancora strano, puoi giocare con i numeri qui sotto!
  uint16_t cianoTenue = tft.color565(220, 245, 245);       // Sfondo chiaro
  uint16_t verdeBarra = tft.color565(0, 220, 50);          // Verde brillante forzato
  uint16_t grigioSfondoBarra = tft.color565(170, 200, 200); // Grigio-ciano per la barra vuota
  uint16_t testoPercentuale = tft.color565(30, 30, 30);    // Grigio scurissimo/Nero per il testo

  // 1. Disegniamo lo sfondo dell'intera area in basso (dal pixel Y 290 fino alla fine)
  tft.fillRect(0, 290, 240, 30, cianoTenue);
  
  // Linea di separazione leggermente più scura dello sfondo
  tft.drawLine(0, 290, 240, 290, tft.color565(150, 200, 200));

  int xBarra = 10;                 
  int yBarra = 302;                
  int larghezzaMassimaBarra = 150; 
  
  // 2. Sfondo della barra (parte vuota)
  tft.fillRect(xBarra, yBarra, larghezzaMassimaBarra, 8, grigioSfondoBarra); 
  
  // 3. Riempimento verde della barra
  int pixelRiempiti = larghezzaMassimaBarra * percentuale;
  if (pixelRiempiti > 0) {
    tft.fillRect(xBarra, yBarra, pixelRiempiti, 8, verdeBarra);
  }

  // 4. Testo percentuale (Testo scuro, con lo sfondo ciano in modo che non crei un "buco" nero)
  tft.setTextFont(2);
  tft.setTextSize(1);
  tft.setCursor(170, 298);
  
  int percInt = (int)(percentuale * 100);
  if (percInt < 10) tft.print(" ");  
  if (percInt < 100) tft.print(" "); 
  tft.print(percInt);
  tft.print("%");
}

void leggiPagina() {
  stampaIntestazione();
  
  tft.setTextFont(2);
  tft.setTextSize(1);

  int altezzaTesto = tft.fontHeight(); 
  int interlinea = altezzaTesto + LINE_SPACING;

  int cursorX = MARGIN_LEFT;
  int cursorY = MARGIN_TOP;

  libro.seek(pageOffsets[currentPage]);

  if (!libro.available()) {
    tft.setCursor(MARGIN_LEFT, 150);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("--- FINE DEL LIBRO ---");
    return;
  }

  while (libro.available()) {
    
    uint32_t inizioParola = libro.position();
    
    String parola = "";
    char separatore = 0; 

    while (libro.available()) {
      char c = libro.read();
      if (c == '\r') continue; 
      
      if (c == ' ') {
        separatore = c;
        break; 
      }
      
      if (c == '\n') {
        separatore = c;
        bool rigaVuota = false;
        
        // Se troviamo altri "a capo", li mangiamo ma ci segniamo che vogliamo 1 riga vuota
        while (libro.available() && (libro.peek() == '\n' || libro.peek() == '\r')) {
          char scartato = libro.read(); 
          if (scartato == '\n') {
            rigaVuota = true; 
          }
        }
        
        if (rigaVuota) {
          separatore = '\v'; // Usiamo un carattere speciale per dire "Doppio a capo"
        }
        break; 
      }
      
      parola += c; 
    }

    int larghezzaParola = 0;
    if (parola.length() > 0) {
      larghezzaParola = tft.textWidth(parola);
    }

    if (cursorX + larghezzaParola > (240 - MARGIN_RIGHT)) {
      cursorX = MARGIN_LEFT; 
      cursorY += interlinea; 
    }

    if (cursorY + altezzaTesto > (320 - MARGIN_BOTTOM)) {
      libro.seek(inizioParola); 
      break; 
    }

    if (parola.length() > 0) {
      tft.setCursor(cursorX, cursorY);
      tft.print(parola);
      cursorX += larghezzaParola; 
    }

    // GESTIONE DEI SEPARATORI
    if (separatore == '\n') {
      // Normale a capo
      cursorX = MARGIN_LEFT;
      cursorY += interlinea;
    } else if (separatore == '\v') {
      // Doppio a capo (Fine paragrafo: lascia esattamente 1 riga vuota!)
      cursorX = MARGIN_LEFT;
      cursorY += (interlinea * 2);
    } else if (separatore == ' ') {
      // Spazio normale
      int larghezzaSpazio = tft.textWidth(" ");
      if (cursorX + larghezzaSpazio <= (240 - MARGIN_RIGHT)) {
        tft.print(" ");
        cursorX += larghezzaSpazio;
      }
    }
  }
  
  if (currentPage >= maxPageReached) {
    maxPageReached = currentPage + 1;
    if (maxPageReached < MAX_PAGES) {
      pageOffsets[maxPageReached] = libro.position();
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  tft.setTextWrap(false); 
  
  mySpi.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  ts.begin(mySpi);
  ts.setRotation(0);

  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("Avvio e-reader...");

  if (!SD.begin(SD_CS)) {
    tft.setTextColor(TFT_RED);
    tft.println("Errore SD Card! Inserita?");
    return;
  }

  libro = SD.open("/libro.txt", FILE_READ);
  if (!libro) {
    tft.setTextColor(TFT_RED);
    tft.println("File libro.txt non trovato!");
    return;
  }

  pageOffsets[0] = 0;
  currentPage = 0;
  maxPageReached = 0;

  // --- CARICAMENTO SALVATAGGI ---
  prefs.begin("ebook", false); // Apriamo la cartella "ebook" in memoria
  
  // Leggiamo i dati salvati (se non esistono, il secondo numero è il valore di default, cioè 0)
  currentPage = prefs.getInt("pagina", 0);
  maxPageReached = prefs.getInt("maxPag", 0);
  
  // Se avevamo salvato delle pagine in passato, ricarichiamo tutto l'Array degli offset!
  if (maxPageReached > 0) {
    prefs.getBytes("offsets", pageOffsets, sizeof(pageOffsets));
  } else {
    pageOffsets[0] = 0; // È la prima volta in assoluto che apriamo il libro
  }

  leggiPagina();
}

void loop() {
  bool currentlyTouched = ts.touched();

  if (currentlyTouched && !wasTouched) { 
    TS_Point p = ts.getPoint();
    
    if (p.z > 500 && p.z < 3500) { 
      if (millis() - lastTouchTime > debounceDelay) {
        int mappedY = map(p.y, 300, 3800, 0, 320);

        if (mappedY < 160) {
          if (libro.available()) {
            currentPage++;
            leggiPagina();
            Serial.println("-> Avanti");
          }
        }
        else {
          if (currentPage > 0) {
            currentPage--;
            leggiPagina();
            Serial.println("<- Indietro");
          }
        }
        // --- SALVATAGGIO IN MEMORIA DOPO AVER GIRATO PAGINA ---
        prefs.putInt("pagina", currentPage);
        prefs.putInt("maxPag", maxPageReached);
        prefs.putBytes("offsets", pageOffsets, sizeof(pageOffsets));
        
        lastTouchTime = millis();
      }
    }
  }

  wasTouched = currentlyTouched; 
  delay(50);
}