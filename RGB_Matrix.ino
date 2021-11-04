#include <math.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <Fonts/TomThumb.h>

#define LED_PIN 14
#define BUTTON_PIN 15
#define PI 3.1415926535897932384626433832795
#define BRIGHTNESS 255
#define TEXT_BRIGHTNESS 255

#define WIDTH 15
#define HEIGHT 5

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(WIDTH, HEIGHT, LED_PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
  NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);

int lastState = HIGH;
int currentState;
int mode = 0;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  matrix.setFont(&TomThumb);
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(255);
  matrix.setTextColor(matrix.Color(0,0,0));
}

int x    = matrix.width();
bool textDisplay = true;
float time = 0;
float prevTime = 0;

void loop() {
  time = float(millis())/1000;

  // button input -----------------------------------------------------------------------------------
  currentState = digitalRead(BUTTON_PIN);

  if(lastState == HIGH && currentState == LOW) {
    mode++;
    mode = mode%6;
  }
  lastState = currentState;

  // graphics loop -----------------------------------------------------------------------------------
  for (int u = 0; u < matrix.width(); u++){
    for (int v = 0; v < matrix.height(); v++){
      float s = ((float)u)/matrix.width();
      float t = ((float)v)/matrix.height();
      float* col;

      switch (mode) {
        case 0:
          col = colourFade(time);  // fades between colours of the rainbow
          break;
        case 1:
          col = rainbow(s,t,time); // scrolls the rainbow
          break;
        case 2:
          col = diagonal_rainbow(s,t,time); // scrolls diagonally
          break;
        case 3:
          col = spinning_rainbow(s,t,time);  // spinning rainbow!!!
          break;
        case 4:
          col = weird_interpolation_of_the_three(s,t,time);
          break;
        default:
          col = weird_interpolation_of_the_three_v2(s,t,time);
          break;
      }
             
      col[0] = min(1,col[0]);col[1] = min(1,col[1]);col[2] = min(1,col[2]);
      matrix.drawPixel(u,v,matrix.Color(BRIGHTNESS*col[0],BRIGHTNESS*col[1],BRIGHTNESS*col[2]));
    }
  }

  
  // display some text  -------------------------------------------------------------------------------------------
  if (textDisplay) {
//    matrix.fill(0); // no background - comment out to use fun rainbows made ^^^
    matrix.setCursor(x, 5);
    matrix.print(("HAPPY CHRISTMAS Josh!!"));
    if (prevTime + 0.1 < time){
      if(--x < -80) {
        x = matrix.width();
        textDisplay = false; //if you want it to loop forever - comment out this line
      }
      prevTime = time;
    }
  }
  
  matrix.show();
}

// ------------------------------- Colour fade - fades between different colours of the rainbow ------------
float* colourFade(float time) {
  static float col[3];
  col[0] = 0.5*sin(time)+0.5;
  col[1] = 0.5*sin(time + 2*PI/3)+0.5;
  col[2] = 0.5*sin(time + 4*PI/3)+0.5;

  return col;
}

// -------------------------------- Scrolls the rainbow from right to left ----------------------------------
float* rainbow(float u, float v, float time) {  
  static float rgb[3];
  return hsv2rgb(u+time,1.0,1.0,rgb);
}

// -------------------------------- Scrolls the rainbow diagonally ----------------------------------
float* diagonal_rainbow(float u, float v, float time) {  
  static float rgb[3];
  return hsv2rgb(u+v+time,1.0,1.0,rgb);
}

// -------------------------------- Spins a wheel of colour, except it's a rectangle, so it's not really a wheel ----------------------------------
float* spinning_rainbow(float u, float v, float time) {
//  // Use polar coordinates instead of cartesian
  float centre_u = 0.5-u;
  float centre_v = 0.5-v;

  float angle = atan2(centre_v,centre_u) + time;
  
  static float rgb[3];
  return hsv2rgb(angle/(2*PI)+0.5,1.0,1.0,rgb);
}

// --------------------------------------- ALL OF THE ABOVE!!!!!!!!!!!!! ---------------------------------------------------------------------
float* weird_interpolation_of_the_three(float u, float v, float time){
  float a = sin(time/4); a*=(a>0);
  float b = sin(time/4 + 2*PI/3); b*=(b>0);
  float c = sin(time/4 + 4*PI/3); c*=(c>0);
  
  float* col1 = colourFade(time);  // fades between colours of the rainbow

  float* col2 = rainbow(u,v,time); // scrolls the rainbow

  float* col3 = spinning_rainbow(u,v,time);  // spinning rainbow!!!

  static float rtn[3];
  rtn[0] = a * col1[0] + b * col2[0] + c * col3[0];
  rtn[1] = a * col1[1] + b * col2[1] + c * col3[1];
  rtn[2] = a * col1[2] + b * col2[2] + c * col3[2];
  return rtn;
  
}
// --------------------------------------- ALL OF THE ABOVE V2!!!! ---------------------------------------------------------------------
float* weird_interpolation_of_the_three_v2(float u, float v, float time){
  float a = 1;
  float b = 0.5*sin(time/4)+0.5;
  float c = 0.5*sin(time/4 + PI)+0.5;
  
  float* col1 = colourFade(time);  // fades between colours of the rainbow

  float* col2 = rainbow(u,v,time); // scrolls the rainbow

  float* col3 = spinning_rainbow(u,v,time);  // spinning rainbow!!!

  static float rtn[3];
  rtn[0] = mix(col1[0],col2[0]*b+col3[0]*c,0.4);
  rtn[1] = mix(col1[1],col2[1]*b+col3[1]*c,0.4);
  rtn[2] = mix(col1[2],col2[2]*b+col3[2]*c,0.4);
  return rtn;
  
}

// -------------------- helper functions ------------------------------------------
float fract(float x) { return x - int(x); }

float mix(float a, float b, float t) { return a + (b - a) * t; }

float step(float e, float x) { return x < e ? 0.0 : 1.0; }

float* hsv2rgb(float h, float s, float b, float* rgb) {
  // h = hue, s = saturation, b = brightness
  rgb[0] = s * mix(1.0, constrain(abs(fract(h + 1.0) * 6.0 - 3.0) - 1.0, 0.0, 1.0), b);
  rgb[1] = s * mix(1.0, constrain(abs(fract(h + 0.6666666) * 6.0 - 3.0) - 1.0, 0.0, 1.0), b);
  rgb[2] = s * mix(1.0, constrain(abs(fract(h + 0.3333333) * 6.0 - 3.0) - 1.0, 0.0, 1.0), b);
  return rgb;
}

float* rgb2hsv(float r, float g, float b, float* hsv) {
  float s = step(b, g);
  float px = mix(b, g, s);
  float py = mix(g, b, s);
  float pz = mix(-1.0, 0.0, s);
  float pw = mix(0.6666666, -0.3333333, s);
  s = step(px, r);
  float qx = mix(px, r, s);
  float qz = mix(pw, pz, s);
  float qw = mix(r, px, s);
  float d = qx - min(qw, py);
  hsv[0] = abs(qz + (qw - py) / (6.0 * d + 1e-10));
  hsv[1] = d / (qx + 1e-10);
  hsv[2] = qx;
  return hsv;
}
