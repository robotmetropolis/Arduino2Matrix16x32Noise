#include <FastLED.h>

const uint8_t kMatrixWidth  = 16;
const uint8_t kMatrixHeight = 32;
const bool    kMatrixSerpentineLayout = true;

#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
#define MAX_DIMENSION ((kMatrixWidth > kMatrixHeight) ? kMatrixWidth : kMatrixHeight)

CRGB leds[NUM_LEDS];

// The 16 bit version of our coordinates
static uint16_t x;
static uint16_t y;
static uint16_t z;
static uint8_t speed = 4; // Declare 'speed' here
static uint8_t scale = 30; // Declare 'scale' here

CRGBPalette16 currentPalette;

int potPin = A0; // Declarar potPin como variable global

  
class Noise {
  public:
  // We're using the x/y dimensions to map to the x/y pixels on the matrix.  We'll
  // use the z-axis for "time".  speed determines how fast time moves forward.  Try
  // 1 for a very slow moving effect, or 60 for something that ends up looking like
  // water.
  uint16_t speed = 10; // speed is set dynamically once we've started up
  
  // Scale determines how far apart the pixels in our noise matrix are.  Try
  // changing these values around to see how it affects the motion of the display.  The
  // higher the value of scale, the more "zoomed out" the noise will be.  A value
  // of 1 will be so zoomed in, you'll mostly see solid colors.
  uint16_t scale = 30; // scale is set dynamically once we've started up
  
  // This is the array that we keep our computed noise values in
  uint8_t noise[MAX_DIMENSION][MAX_DIMENSION];
  
  CRGBPalette16 currentPalette;
  // Paletas de colores
  //CRGBPalette16 currentPalette = RainbowColors_p;
  CRGBPalette16 blackAndWhitePalette = CRGBPalette16(CRGB::Black, CRGB::Black, CRGB::White, CRGB::White);
  CRGBPalette16 RedPalette = CRGBPalette16(CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red);
  CRGBPalette16 OceanPalette = OceanColors_p;
  CRGBPalette16 ForestPalette = ForestColors_p;
  CRGBPalette16 CloudPalette = CloudColors_p;
  CRGBPalette16 LavaPalette = LavaColors_p;
  CRGBPalette16 PartyPalette = PartyColors_p;
  CRGBPalette16 RainbowStripePalette = RainbowStripeColors_p;
  
  uint8_t       colorLoop = 1;
  bool done = false;
  
  void setup() {
    // Initialize our coordinates to some random values
    currentPalette = PartyColors_p;
    x = random16();
    y = random16();
    z = random16();
  }
  
  bool loop() {
  
    // generate noise data
    fillnoise8();
    
    // convert the noise data to colors in the LED array
    // using the current palette
    mapNoiseToLEDsUsingPalette();
    // delay(10);
    
//    EVERY_N_MILLISECONDS(30000) {
//      done = true;
//    }

    return !done;
  }
  
  // Fill the x/y array of 8-bit noise values using the inoise8 function.
  void fillnoise8() {
    // If we're running at a low "speed", some 8-bit artifacts become visible
    // from frame-to-frame.  In order to reduce this, we can do some fast data-smoothing.
    // The amount of data smoothing we're doing depends on "speed".
    uint8_t dataSmoothing = 0;
    if (speed < 50) {
      dataSmoothing = 200 - (speed * 4);
    }
    
    for (int i = 0; i < MAX_DIMENSION; i++) {
      int ioffset = scale * i;
      for (int j = 0; j < MAX_DIMENSION; j++) {
        int joffset = scale * j;
        
        uint8_t data = inoise8(x + ioffset, y + joffset, z);
  
        // The range of the inoise8 function is roughly 16-238.
        // These two operations expand those values out to roughly 0..255
        // You can comment them out if you want the raw noise data.
        data = qsub8(data, 16);
        data = qadd8(data, scale8(data, 39));
  
        if (dataSmoothing) {
          uint8_t olddata = noise[i][j];
          uint8_t newdata = scale8(olddata, dataSmoothing) + scale8(data, 256 - dataSmoothing);
          data = newdata;
        }
        
        noise[i][j] = data;
      }
    }
    
    z += speed;
    
    // apply slow drift to X and Y, just for visual variation.
    x += speed / 8;
    y -= speed / 16;
  }
  
  void mapNoiseToLEDsUsingPalette()
  {
    static uint8_t ihue = 0;
    
    for (int i = 0; i < kMatrixWidth; i++) {
      for (int j = 0; j < kMatrixHeight; j++) {
        // We use the value at the (i,j) coordinate in the noise
        // array for our brightness, and the flipped value from (j,i)
        // for our pixel's index into the color palette.
  
        uint8_t index = noise[j][i];
        uint8_t bri = noise[i][j];
  
        // if this palette is a 'loop', add a slowly-changing base value
        if (colorLoop) { 
          index += ihue;
        }
  
        // brighten up, as the color palette itself often contains the 
        // light/dark dynamic range desired
        if (bri > 127) {
          bri = 255;
        } else {
          bri = dim8_raw(bri * 2);
        }
  
        CRGB color = ColorFromPalette(currentPalette, index, bri);
        leds[XY(i, j)] = color;
      }
    }
    
    ihue += 1;
  }
  
  // There are several different palettes of colors demonstrated here.
  //
  // FastLED provides several 'preset' palettes: RainbowColors_p, RainbowStripeColors_p,
  // OceanColors_p, CloudColors_p, LavaColors_p, ForestColors_p, and PartyColors_p.
  //
  // Additionally, you can manually define your own color palettes, or you can write
  // code that creates color palettes on the fly.
  
  // 1 = 5 sec per palette
  // 2 = 10 sec per palette
  // etc
  #define HOLD_PALETTES_X_TIMES_AS_LONG 1

  void changePalette(int index) {
  switch (index) {
    case 0:
      Serial.println("case 0");
      currentPalette = RainbowColors_p; speed = 4; //scale = 25;
      break;
    case 1:
    Serial.println("case 1");
      currentPalette = blackAndWhitePalette; speed = 4; //scale = 25;
      break;
    case 2:
      Serial.println("case 2");SetupRandomPalette(); speed = 1; //scale = 25;
      //currentPalette = RedPalette; speed = 4; scale = 25;
      break;
    case 3:
      Serial.println("case 3");SetupRandomPalette(); speed = 1; //scale = 80;
      //currentPalette = OceanPalette; speed = 4; scale = 25;
      break;
    case 4:
      Serial.println("case 4");SetupRandomPalette(); speed = 2; //scale = 60;   
      //currentPalette = ForestPalette; speed = 4; scale = 25;
      break;
    case 5:
      Serial.println("case 5");SetupRandomPalette(); speed = 2; //scale = 5;       
      //currentPalette = CloudPalette; speed = 4; scale = 25;
      break;
    case 6:
      Serial.println("case 6");currentPalette = LavaPalette; speed = 4; //scale = 15;
      break;
    case 7:
      Serial.println("case 7");currentPalette = PartyPalette; speed = 4; //scale = 25;
      break;
    case 8:
      Serial.println("case 8");currentPalette = PartyPalette; speed = 40; //scale = 80;
      break;
    case 9:
      Serial.println("case 9");currentPalette = PartyPalette; speed = 1; //scale = 5;
      break;  
    case 10:
      Serial.println("case 10");currentPalette = RainbowStripePalette; speed = 4; //scale = 25;     
      break;  
    default:
      break;
  }
  
    // Asignamos la escala actualizada del potenciómetro
    scale = analogRead(potPin);
    scale = map(scale, 0, 1023, 5, 100);
    
    // Actualizamos la animación con los nuevos valores de paleta, velocidad y escala
    fillnoise8();
    mapNoiseToLEDsUsingPalette();
    FastLED.show();
    Serial.println("Palette changed. Speed: " + String(speed) + ", Scale: " + String(scale));
    
    // Actualizamos la escala en el objeto noiseRoutine
    //noiseRoutine.scale = scale;
  }

  
  
  // This function generates a random palette that's a gradient
  // between four different colors.  The first is a dim hue, the second is 
  // a bright hue, the third is a bright pastel, and the last is 
  // another bright hue.  This gives some visual bright/dark variation
  // which is more interesting than just a gradient of different hues.
  void SetupRandomPalette()
  {
    currentPalette = CRGBPalette16( 
                        CHSV(random8(), 255, 32), 
                        CHSV(random8(), 255, 255), 
                        CHSV(random8(), 128, 255), 
                        CHSV(random8(), 255, 255)); 
  }
  
  // This function sets up a palette of black and white stripes,
  // using code.  Since the palette is effectively an array of
  // sixteen CRGB colors, the various fill_* functions can be used
  // to set them up.
  void SetupBlackAndWhiteStripedPalette()
  {
    // 'black out' all 16 palette entries...
    fill_solid(currentPalette, 16, CRGB::Black);
    // and set every fourth one to white.
    currentPalette[0] = CRGB::White;
    currentPalette[4] = CRGB::White;
    currentPalette[8] = CRGB::White;
    currentPalette[12] = CRGB::White;
  }
  
  // This function sets up a palette of purple and green stripes.
  void SetupPurpleAndGreenPalette()
  {
    CRGB purple = CHSV(HUE_PURPLE, 255, 255);
    CRGB green  = CHSV(HUE_GREEN, 255, 255);
    CRGB black  = CRGB::Black;
    
    currentPalette = CRGBPalette16( 
      green,  green,  black,  black,
      purple, purple, black,  black,
      green,  green,  black,  black,
      purple, purple, black,  black );
  }
  
  uint16_t XY(uint8_t x, uint8_t y)
  {
    uint16_t i;
    if (kMatrixSerpentineLayout == false) {
      i = (y * kMatrixWidth) + x;
    }
    if (kMatrixSerpentineLayout == true) {
      if (y & 0x01) {
        // Odd rows run backwards
        uint8_t reverseX = (kMatrixWidth - 1) - x;
        i = (y * kMatrixWidth) + reverseX;
      } else {
        // Even rows run forwards
        i = (y * kMatrixWidth) + x;
      }
    }
    return i;
  }
};

Noise noiseRoutine;


// Estado del botón
int buttonState = HIGH;

// Estado anterior del botón
int previousButtonState = HIGH;

// Índice de la paleta actual
int paletteIndex = 0;


void setup() {
  Serial.begin(115200); // Inicializar la comunicación con la consola
  
  FastLED.addLeds<WS2812B, 5, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(50);

  noiseRoutine.setup();

  pinMode(13, INPUT_PULLUP);
  previousButtonState = digitalRead(13);
  Serial.println("Setup complete.");
}



void loop() {
  
  // Leer el estado del botón
  buttonState = digitalRead(13);

  // Leer el valor del potenciómetro y mapearlo al rango de escalas
  int potValue = analogRead(potPin);
  int mappedScale = map(potValue, 0, 1023, 1, 8);

  // Si el valor del potenciómetro ha cambiado, actualizar la escala
  if (mappedScale != scale) {
    scale = mappedScale;
    noiseRoutine.scale = scale; // Actualizar la escala en el objeto noiseRoutine
    noiseRoutine.changePalette(paletteIndex);
    Serial.println("Potentiometer value: " + String(potValue));
    Serial.println("Scale value: " + String(scale));
  }    
  // Si el botón se ha presionado (cambio de alto a bajo)
  if (buttonState == LOW && previousButtonState == HIGH) {
    Serial.println("Button pressed!");
 
  // Incrementar el índice de la paleta
    paletteIndex++;

    // Si hemos llegado al final de las opciones, reiniciamos al principio
    if (paletteIndex >= 11) {
      paletteIndex = 0;
    }
    // Cambiar la paleta según el índice seleccionado
    noiseRoutine.changePalette(paletteIndex);
    
    delay(100);
  }
  // Actualizar el estado anterior del botón
  previousButtonState = buttonState;

  // Llamar a noiseRoutine.loop() después de actualizar los valores
  if (noiseRoutine.loop()) {
    FastLED.show();
  }
}
