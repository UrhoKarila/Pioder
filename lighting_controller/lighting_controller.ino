#include <assert.h>
//int LED_pin = 3;
//int pins[] = {3,5,6};

const int RED = 0;          //Used to position RED within the array
const int GREEN = 2;        // Because the pins on the DIODER are in RBG ordering. Don't ask me
const int BLUE = 1;         // ---------------"-----------------
const uint8_t port_qty = 2; //Number of 'ports' supported -- I.E., how many independent controls for Lighting are supplied. This is related to the hardware you're running on.
const int tick_rate = 1;    //How often to update lights, in ms
const int array_size = 32;
const bool debug_mode_enabled = true;

typedef struct {
  byte r;
  byte g;
  byte b;
  uint16_t duration;
  uint8_t transition_style;
  bool is_alert;
  bool is_good;
} Lighting;

typedef struct {
  Lighting transitions[array_size];
  Lighting alerts[array_size];
  uint8_t pins[3];
  uint8_t address;
  uint16_t clock;
  Lighting* current_lighting;
  Lighting old_lighting;
  uint8_t current_transition_index;
}  Port;


Port ports[port_qty];       //create array of ports
Lighting DEFAULTLIGHTING;


void setup() {
  Serial.begin(9600); 
  while(!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }


  Lighting DEFAULTLIGHTING;
  DEFAULTLIGHTING.r = 0;
  DEFAULTLIGHTING.g = 0;
  DEFAULTLIGHTING.b = 0;
  DEFAULTLIGHTING.duration = 0;
  DEFAULTLIGHTING.transition_style = 0;
  DEFAULTLIGHTING.is_alert = false;
  DEFAULTLIGHTING.is_good = true;

  runtests();
  
  Port left;
    left.pins[RED] = 3;
    left.pins[GREEN] = 5;
    left.pins[BLUE] = 6;
    left.address = 1;
    left.current_transition_index = 0;

  for(int i = 0; i < 3; i++){
    pinMode(left.pins[i], OUTPUT);
  }

  Port right;
  right.pins[RED] = 9;
  right.pins[GREEN] = 10;
  right.pins[BLUE] = 11;
  right.address = 2;
  right.current_transition_index = 0;
  
  for(int i = 0; i < 3; i++){
    pinMode(right.pins[i], OUTPUT);
  }

  ports[0] = left;
  ports[1] = right;
}

//Update method. Most of the meat happens here.
void tick(Port* port){
  port->clock += tick_rate;

  //If the current transition is complete, get the next one
  if(port->clock >= port->current_lighting->duration){
    //duplicate Lighting in temp variable
    debug("Current transition expired. Finding new one...");
    memcpy(&port->old_lighting, port->current_lighting, sizeof(Lighting));
    //If lighting is an Alert, clear it from the AlertQueue
    if(port->current_lighting->is_alert){
      clearLighting(port->current_lighting);
      debug("Alert complete. Removing alert from queue.");
    }
    //Get next Transition
    //If there is an Alert in the queue, fetch it and load it into current_Lighting
    findNextAlert(port);
    //Else, grab the next Transition, increment index, and begin rendering
    if(!port->current_lighting->is_good){
      getNextTransition(port);
    }
    
  }
}

void clearLighting(Lighting *light){
  light->is_good = false;
}

//returns the next available alert for a port, if there is one. Else, return bad Lighting
Lighting* findNextAlert(Port* p){
  for(int j = 0; j < array_size; j++){
      if(p->alerts[j].is_good){
        debug("Alert found in queue");
        //Serial.println(p->current_lighting);
        return &p->alerts[j];
      }
    }
    debug("No alert found in queue for port");
    Lighting temp;
    temp.is_good = false;
    return &temp;
}

Lighting* getNextTransition(Port* p){
  uint8_t temp_index = p->current_transition_index;
  p->current_transition_index = ((p->current_transition_index++) % array_size);
  while(p->current_transition_index != temp_index){
    if(p->transitions[p->current_transition_index].is_good){
      p->current_lighting = &p->transitions[p->current_transition_index];
      return p->current_lighting;
    }
    else{
      p->current_transition_index = ((p->current_transition_index++) % array_size);
    }

    p->current_lighting = &DEFAULTLIGHTING;
    return;

  }
}

void loop() {
//Read from Serial to see if there's anything to update

  for(int i = 0; i < port_qty; i++){
    tick(&ports[i]);
  }
  delay(tick_rate);
}

void debug(char arr[] ){
  if(debug_mode_enabled){
    Serial.println(arr);
  }
}

float easeLinear(float t) { return float(t) ;}
float easeInQuad(float t) { return float(t*t) ;}
float easeOutQuad(float t) { return float(t)*(2-t) ;}
float easeInOutQuad(float t) { return t<.5 ? float(2*t*(t)) : float(-1+float(4-2*t)*float(t)) ;}
float easeInQuint(float t) { return t*t*t*t*t; }
float easeOutQuint(float t) { return 1+(--t)*t*t*t*t; }
float easeInOutQuint(float t) { return t<.5 ? 16*t*t*t*t*t : 1+16*(--t)*t*t*t*t; }

void runtests(){
  int a = 0;
  assert(a==1);
  Serial.println("Sanity check passed");
  Lighting L1;
  L1.r = 0;
  L1.g = 0;
  L1.b = 0;
  L1.duration = 0;
  L1.transition_style = 0;
  L1.is_alert = false;
  L1.is_good = true;

  assert(L1.r == 0);
  Serial.println("Can create and assign bytes to Lighting Objects");
  assert(L1.is_good == true);
  Serial.println("Seems like the Lighting L1 is good");

  Lighting L2;
  L2.r = 255;
  L2.g = 0;
  L2.b = 255;
  L2.duration = 65535;
  L2.transition_style = 1;
  L2.is_alert = false;
  L2.is_good = true;
  Serial.println("No errors while creating L2");

  Lighting emptyLighting;
  assert(emptyLighting.r == 0);
  assert(emptyLighting.g == 0);
  Serial.println("emptyLighting initialized to 0 for RGB");
  assert(emptyLighting.is_good == false);
  assert(emptyLighting.is_alert == false);  
  Serial.println("emptyLighting defaults to false where applicable");

  assert(L2.is_good == true);
  assert(L2.r == 255);
  Serial.println("L2 is as expected");
  assert(L1.r != L2.r);
  assert(L1.g == L2.g);
  Serial.println("Comparisons between Lightings work fine.");

  Port P1;
  P1.pins[RED] = 3;
  P1.pins[GREEN] = 5;
  P1.pins[BLUE] = 6;
  P1.address = 1;
  P1.current_transition_index = 0;

  assert(P1.address == 1);
  assert(P1.pins[0] == 3);
  assert(P1.current_transition_index == 0);
  Serial.println("P1 seems to have been created as expected.");
  assert(P1.transitions[0].r == 0);
  assert(P1.transitions[1].r == 0);

//  addLighting(P1, L1);

}
