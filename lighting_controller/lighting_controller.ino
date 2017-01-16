//int LED_pin = 3;
//int pins[] = {3,5,6};

const int RED = 0;
const int GREEN = 1;
const int BLUE = 2; 

typedef struct {
  byte r;
  byte g;
  byte b;
  uint16_t duration;
  uint8_t transition_style;
} Lighting;

typedef struct {
  Lighting transitions[32];
  Lighting alerts[32];
  uint8_t pins[3];
  uint8_t address;
}  Port;




void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); 
  while(!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }

  Port left;
  left.pins = {3,5,6};
  left.address = 1;
  for(int i = 0; i < 3; i++){
    pinMode(left.pins[i], OUTPUT);
  }
}

void loop() {
  // put your main code here, to run repeatedly:


  for(int i = 0; i < 3; i ++){


    for(int j = 0; j <= 100; j++){
      float val = easeLinear(j/100.0);
      analogWrite(pins[i], 255*val);
      delay(10);
      Serial.println(255*val);
    }
    for(int j = 0; j <= 100; j++){
      float val = easeLinear(j/100.0);
      analogWrite(pins[i], 255*(1-val));
      delay(10);
      Serial.println(255*(1-val));
    }

    for(int j = 0; j <= 100; j++){
      float val = easeInOutQuad(j/100.0);
      analogWrite(pins[i], 255*val);
      delay(10);
      Serial.println(255*val);
    }
    for(int j = 0; j <= 100; j++){
      float val = easeInOutQuad(j/100.0);
      analogWrite(pins[i], 255*(1-val));
      delay(10);
      Serial.println(255*(1-val));
    }

//    for(int j = 0; j <= 100; j++){
//      float val = easeInQuint(j/100.0);
//      analogWrite(pins[i], 255*val);
//      delay(10);
//      Serial.println(255*val);
//    }
//    for(int j = 0; j <= 100; j++){
//      float val = easeOutQuint(j/100.0);
//      analogWrite(pins[i], 255*(1-val));
//      delay(10);
//      Serial.println(255*(1-val));
//    }
//
//    for(int j = 0; j <= 100; j++){
//      float val = easeOutQuint(j/100.0);
//      analogWrite(pins[i], 255*val);
//      delay(10);
//      Serial.println(255*val);
//    }
//    for(int j = 0; j <= 100; j++){
//      float val = easeInQuint(j/100.0);
//      analogWrite(pins[i], 255*(1-val));
//      delay(10);
//      Serial.println(255*(1-val));
//    }
    
    

//    for(int j = 0; j < 255; j ++){
//      analogWrite(pins[i], j);
//
//      delay(10);
//    }
//    for(int j = 255; j >= 0; j --){
//      analogWrite(pins[i], j);
//
//      delay(10);
//    }
  }
}


float easeLinear(float t) { return float(t) ;}
float easeInQuad(float t) { return float(t*t) ;}
float easeOutQuad(float t) { return float(t)*(2-t) ;}
float easeInOutQuad(float t) { return t<.5 ? float(2*t*(t)) : float(-1+float(4-2*t)*float(t)) ;}
float easeInQuint(float t) { return t*t*t*t*t; }
float easeOutQuint(float t) { return 1+(--t)*t*t*t*t; }
float easeInOutQuint(float t) { return t<.5 ? 16*t*t*t*t*t : 1+16*(--t)*t*t*t*t; }

