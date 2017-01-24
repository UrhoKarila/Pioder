#include <assert.h>
//#include "MemoryFree.h"
//#include <stdio.h>
//int LED_pin = 3;
//int pins[] = {3,5,6};

const int RED = 0;          //Used to position RED within the array
const int GREEN = 1;        // Because the pins on the DIODER are in RBG ordering. Don't ask me
const int BLUE = 2;         // ---------------"-----------------
const uint8_t port_qty = 2; //Number of 'ports' supported -- I.E., how many independent controls for Lighting are supplied. This is related to the hardware you're running on.
const int tick_rate = 10;    //How often to update lights, in ms
const int array_size = 16;
const bool debug_mode_enabled = true;

typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint16_t duration;
  uint8_t transition_style;
  bool is_alert;
  bool is_good;
} Lighting;

typedef struct {
  Lighting transitions[array_size];
  Lighting alerts[array_size];
  uint8_t pins[3];
  uint8_t colors[3];
  uint8_t address;
  uint16_t clock;
  Lighting* current_lighting;
  Lighting old_lighting;
  uint8_t current_transition_index;
}  Port;


Port ports[port_qty];       //create array of ports
Lighting DEFAULTLIGHTING;
Lighting NULL_LIGHTING;

void setup() {
  Serial.begin(9600); 
  while(!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }
  // Serial.print(F("freeMemory()="));
  // Serial.println(freeMemory());
  delay(5000);

  Serial.println(F("Setup in progress"));

  delay(500);

  Lighting DEFAULTLIGHTING;
  DEFAULTLIGHTING.r = 0;
  DEFAULTLIGHTING.g = 0;
  DEFAULTLIGHTING.b = 0;
  DEFAULTLIGHTING.duration = 100;
  DEFAULTLIGHTING.transition_style = 0;
  DEFAULTLIGHTING.is_alert = false;
  DEFAULTLIGHTING.is_good = true;

  NULL_LIGHTING.r = 0;
  NULL_LIGHTING.g = 0;
  NULL_LIGHTING.b = 0;
  NULL_LIGHTING.duration = 0;
  NULL_LIGHTING.transition_style = 0;
  NULL_LIGHTING.is_alert = false;
  NULL_LIGHTING.is_good = false;

  delay(500);
  Serial.println(F("Running tests"));
  delay(500);
  
  Port left;
  left.pins[RED] = 3;
  left.pins[GREEN] = 6;
  left.pins[BLUE] = 5;
  left.address = 1;
  left.current_transition_index = 0;
  //memcpy(&left.old_lighting, &DEFAULTLIGHTING, sizeof(Lighting));
  initPort(&left);

  for(int i = 0; i < 3; i++){
    pinMode(left.pins[i], OUTPUT);
    // analogWrite(left.pins[i], (3-i) * 75);
  }
  runtests();


  

  Port right;
  right.pins[RED] = 9;
  right.pins[GREEN] = 11;
  right.pins[BLUE] = 10;
  right.address = 2;
  right.current_transition_index = 0;
  //memcpy(&right.old_lighting, &DEFAULTLIGHTING, sizeof(Lighting));
  initPort(&right);
  
  for(int i = 0; i < 3; i++){
    pinMode(right.pins[i], OUTPUT);
  }


  ports[0] = left;
  ports[1] = right;
}

void initPort(Port* p){
  // Serial.println(F("Initializing Port"));
  // Serial.println(F("Initializing CL"));
  p->current_lighting = &DEFAULTLIGHTING;
  // Serial.println(F("Initializing OL"));
  // delay(500);
  // memcpy(&p->old_lighting, &DEFAULTLIGHTING, sizeof(Lighting));
  p->old_lighting = DEFAULTLIGHTING;
  for(int i = 0; i < array_size; i++){
    // Serial.println((int)&p->transitions[i]);
    // delay(500);
    p->transitions[i].is_good = false;
    p->alerts[i].is_good = false;
  }
}

//Update method. Most of the meat happens here.
void tick(Port* port){
  port->clock += tick_rate;

  //If the current transition is complete, get the next one
  if(port->clock >= port->current_lighting->duration){
                                  debug("Current transition expired. Finding new one...");
    memcpy(&port->old_lighting, port->current_lighting, sizeof(Lighting));//duplicate Lighting in temp variable
    if(port->current_lighting->is_alert){    //If lighting is an Alert, clear it from the AlertQueue
      clearLighting(port->current_lighting);
                                  debug("Alert complete. Removing alert from queue.");
    }

    port->current_lighting = findNextAlert(port);     //If there is an Alert in the queue, fetch it and load it into current_Lighting
    if(port->current_lighting->is_good){
          Serial.println(F("Alert found!"));
    }
    else{     //Else, grab the next Transition, increment index, and begin rendering
                                  Serial.println(F("No alert found, searching for transitions."));
      getNextTransition(port);
    }

    port->clock = 0;
  }

  for(uint8_t i = 0; i < 3; i++){
    //analogWrite
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
  return &NULL_LIGHTING;
}

Lighting* getNextTransition(Port* p){
  uint8_t temp_index = p->current_transition_index;
  p->current_transition_index = ((p->current_transition_index + 1) % array_size);
  while(p->current_transition_index != temp_index){
    if(p->transitions[p->current_transition_index].is_good){
     //p->current_lighting = p->transitions[p->current_transition_index];
      return &p->transitions[p->current_transition_index];
    }
    else{
      p->current_transition_index = ((p->current_transition_index + 1) % array_size);
    }
  }
  //p->current_lighting = &DEFAULTLIGHTING;
  return &DEFAULTLIGHTING;
}

void addLighting(Port* p, Lighting* l){
  //Find the first available slot in the appropriate Queue for p, then stuff l in there
  Lighting* q;
  if(l->is_alert){
    q = p->alerts;
  }
  else{
    q = p->transitions;
  }
  while(q->is_good){
    q++;
  }
  memcpy(q, l, sizeof(Lighting));
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

bool areEqual(Lighting* L1, Lighting* L2){
  return ((L2->r == L1->r) &&
  (L2->g == L1->g) &&
  (L2->b == L1->b) &&
  (L2->duration == L1->duration) &&
  (L2->transition_style == L1->transition_style) &&
  (L2->is_alert == L1->is_alert) &&
  (L2->is_good == L1->is_good));
}

void runtests(){
  int a = 0;
  assert(a==0);
  Serial.println(F("Sanity check passed"));
  Lighting L1;
  L1.r = 1;
  L1.g = 0;
  L1.b = 0;
  L1.duration = 50;
  L1.transition_style = 0;
  L1.is_alert = false;
  L1.is_good = true;

  assert(areEqual(&L1, &L1));

  assert(L1.r == 1);
  Serial.println(F("Can create and assign bytes to Lighting Objects"));
  assert(L1.is_good == true);
  Serial.println(F("Seems like the Lighting L1 is good"));


  Lighting L2;
  L2.r = 255;
  L2.g = 0;
  L2.b = 255;
  L2.duration = 1000;
  L2.transition_style = 1;
  L2.is_alert = false;
  L2.is_good = true;
  Serial.println(F("No errors while creating L2"));
  assert(!areEqual(&L1, &L2));

  Lighting L3;
  L3.r = 3;
  L3.g = 3;
  L3.b = 3;
  L3.duration = 11;
  L3.transition_style = 3;
  L3.is_alert = true;
  L3.is_good = true;
  Serial.println(F("No errors while creating L3"));

  
  Lighting emptyLighting;
  Serial.println(F("Created emptyLighting"));
  emptyLighting.r = 0;
  emptyLighting.g = 0;
  emptyLighting.b = 0;
  emptyLighting.duration = 0;
  emptyLighting.transition_style = 0;
  emptyLighting.is_alert = 0;
  emptyLighting.is_good = 0;

  assert(L2.is_good == true);
  assert(L2.r == 255);
  Serial.println(F("L2 is as expected"));
  assert(L1.r != L2.r);
  assert(L1.g == L2.g);
  Serial.println(F("Comparisons between Lightings work fine."));
  
  // delay(500);

  Port P1;
  P1.pins[RED] = 3;
  P1.pins[GREEN] = 5;
  P1.pins[BLUE] = 6;
  P1.address = 1;
  P1.current_transition_index = 0;
  Serial.println(F("Initializing P1"));
  // delay(500);
  initPort(&P1);
  Serial.println(F("Initialized P1"));
  // delay(500);

  assert(P1.address == 1);
  assert(P1.pins[0] == 3);
  assert(P1.current_transition_index == 0);
  Serial.println(F("P1 seems to have been created as expected."));
  assert(P1.transitions[0].is_good == false);
  assert(P1.transitions[1].is_good == false);

  Serial.println(F("Testing addLighting"));
  assert(P1.transitions[0].is_good == 0);
  addLighting(&P1, &L1);
  assert(P1.transitions[0].is_good == 1);
  //Serial.println(F("Is now good"));

  assert(areEqual(&P1.transitions[0], &L1));
  Serial.println(F("First lighting added ok"));

  assert(P1.transitions[1].is_good == 0);
  addLighting(&P1, &L2);
  assert(P1.transitions[1].is_good == 1);
  //Serial.println(F("Is now good"));

  assert(areEqual(&P1.transitions[1], &L2));
  Serial.println(F("Second lighting added ok"));

  assert(P1.alerts[0].is_good == 0);
  addLighting(&P1, &L3);
  assert(P1.alerts[0].is_good == 1);
  //Serial.println(F("Is now good"));

  assert(areEqual(&P1.alerts[0], &L3));
  Serial.println(F("Alert lighting added ok"));
  delay(50);
  Serial.println(F("addLighting complete"));
  delay(50);




  Serial.println(F("Testing lighting transitions"));
  delay(50);
  // Serial.print(F("freeMemory() = "));
  // Serial.println(freeMemory());
  // delay(500);
  

  Port P2;
  P2.pins[RED] = 9;
  P2.pins[GREEN] = 11;
  P2.pins[BLUE] = 10;
  P2.address = 2;
  P2.current_transition_index = 0;
  initPort(&P2);
  Serial.println(F("Created empty port"));
  delay(50);
//delay(5000);
  //Testing getNextTransition on a Port with an empty queue. Should return the Default
  Lighting* testLightptr = getNextTransition(&P2);
  //Serial.println((long)&L1);
  //Serial.println((int)&DEFAULTLIGHTING);
  // Serial.println((int)&P2.transitions[0]);
  // Serial.println(P2.transitions[0].is_good);
  // Serial.println((long)testLightptr);
  delay(500);
  assert(areEqual(testLightptr, &DEFAULTLIGHTING));
  testLightptr = getNextTransition(&P2);
  assert(areEqual(testLightptr, &DEFAULTLIGHTING));
  Serial.println(F("Getting expected defaultlighting when searching empty list"));



  Serial.println(F("Attempting to iterate through P1's transition list"));
  delay(50);
  assert(P1.transitions[0].is_good);
  testLightptr = getNextTransition(&P1);
  //Serial.println((int)testLightptr);
  //Serial.println((int)&P1.transitions[0]);
  delay(50);
  assert(areEqual(testLightptr, &L2));
  Serial.println(F("Getting expected lighting L2"));
  //Serial.println((int)&P1.transitions[0]);
  //Serial.println((int)&P1.transitions[1]);
  //Serial.println((int)&testLightptr);
  delay(50);
  testLightptr = getNextTransition(&P1);
  Serial.println(F("L1/test pointer locations"));
  Serial.println((int)&L1);
  Serial.println((int)&testLightptr);
  delay(50);
  assert(areEqual(testLightptr, &L1));
  Serial.println(F("Getting expected lighting L1"));
  delay(50);
  // testLightptr = getNextTransition(&P1);
  // assert(areEqual(testLightptr, &L3));
  // Serial.println(F("Getting expected lighting L3"));
  testLightptr = getNextTransition(&P1);
  assert(areEqual(testLightptr, &L2));
  Serial.println(F("Getting expected lighting L2"));
  delay(50);
  testLightptr = getNextTransition(&P1);
  assert(areEqual(testLightptr, &L1));
  Serial.println(F("Getting expected lighting L1"));
  delay(50);
  // testLightptr = getNextTransition(&P1);
  // assert(areEqual(testLightptr, &L3));
  // Serial.println(F("Getting expected lighting L3"));
  testLightptr = getNextTransition(&P1);
  assert(areEqual(testLightptr, &L2));
  Serial.println(F("Transition loops fine on list."));
  Serial.println(F("Completed Transition tests"));

  delay(50);

  Serial.println(F("Testing Tick"));  
  
  tick(&P2);
  assert(areEqual(P2.current_lighting, &DEFAULTLIGHTING));
  Serial.println(F("Tick gives expected DefaultLighting on empty port"));
  tick(&P2);
  assert(areEqual(P2.current_lighting, &DEFAULTLIGHTING));
  assert(areEqual(&P2.old_lighting, &DEFAULTLIGHTING));
  Serial.println(F("Seems to work for empty queues"));

  Serial.println(F("Testing on populated queues - P1"));
  tick(&P1);
  delay(500);
  assert(areEqual(P1.current_lighting, &L3));
  Serial.println(F("Gotten alert as expected"));
  delay(500);
  tick(&P1);    // 0 on L1
  // 
  //   Serial.println(P1.clock);
  Serial.println(P1.current_lighting->is_alert);
  // delay(500);
  //     tick(&P1);    // 0 on L1
  // Serial.println(P1.clock);
  // Serial.println(P1.current_lighting->is_alert);
  // delay(500);
  //     tick(&P1);    // 0 on L1
  // Serial.println(P1.clock);
  // Serial.println(P1.current_lighting->is_alert);
  // delay(500);
  tick(&P1);    // 0 on L1
  Serial.println(P1.clock);
  Serial.println(P1.current_lighting->is_alert);
  Serial.println(P1.current_lighting->r);
  Serial.println(P1.current_lighting->duration);
  delay(500);

  assert(areEqual(&P1.old_lighting, &L3));
  assert(areEqual(P1.current_lighting, &L1));
  Serial.println(F("Ticked to normal transitions ok"));
  tick(&P1);    //10
  tick(&P1);    //20
  tick(&P1);    //30
  tick(&P1);    //40
  delay(50);
  assert(areEqual(&P1.old_lighting, &L3));
  assert(areEqual(P1.current_lighting, &L1));
  Serial.println(F("Transition has hung around for the expected length of time"));
  assert(P1.clock == 40);
  tick(&P1);    //50

  Serial.println(F("Completed tick tests"));  

  Serial.println(F("Testing Alerts"));

  testLightptr = findNextAlert(&P2);
  //This should have returned a pointer to the NULL_LIGHTING
  assert(areEqual(testLightptr, &NULL_LIGHTING));
  Serial.println(F("Received expected NULL_LIGHTING"));

  testLightptr = findNextAlert(&P1);
  //This should return a good alert
  assert(!areEqual(testLightptr, &NULL_LIGHTING));
  assert(areEqual(testLightptr, &P1.alerts[0]));
  assert(areEqual(testLightptr, &L3));
  Serial.println(F("Received expected valid alert"));



  delay(50000);
delay(500);


}
