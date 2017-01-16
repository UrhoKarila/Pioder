//int LED_pin = 3;
//int pins[] = {3,5,6};

const int RED = 0;          //Used to position RED within the array
const int GREEN = 2;        // Because the pins on the DIODER are in RBG ordering. Don't ask me
const int BLUE = 1;         // ---------------"-----------------
const uint8_t port_qty = 2; //Number of 'ports' supported -- I.E., how many independent controls for Lighting are supplied. This is related to the hardware you're running on.
Port ports[port_qty];       //create array of ports
const int tick_rate = 1;    //How often to update lights, in ms
const int array_size = 32;
const bool debug_mode_enabled = true;

Lighting DEFAULTLIGHTING;

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


void setup() {
  Serial.begin(9600); 
  while(!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }

  DEFAULTLIGHTING.r = 0;
  DEFAULTLIGHTING.g = 0;
  DEFAULTLIGHTING.b = 0
  DEFAULTLIGHTING.duration = 0;
  DEFAULTLIGHTING.transition_style = 0;
  DEFAULTLIGHTING.is_alert = 0;
  DEFAULTLIGHTING.is_good = true;
  
  Port left;
  left.pins = {3,5,6};
  left.address = 1;
  left.current_transition_index = 0;
  for(int i = 0; i < 3; i++){
    pinMode(left.pins[i], OUTPUT);
  }

  Port right;
  right.pins = {9,10,11};
  right.address = 2;
  right.current_transition_index = 0;
  for(int i = 0; i < 3; i++){
    pinMode(right.pins[i], OUTPUT);
  }

  ports = {left, right};

  runtests();
}

//Update method. Most of the meat happens here.
void tick(Port* port){
  port.clock += tick_rate;

  //If the current transition is complete, get the next one
  if(port.clock >= current_lighting.duration){
    //duplicate Lighting in temp variable
    debug("Current transition expired. Finding new one...");
    memcpy(&old_lighting, current_lighting, sizeof(Lighting));
    //If lighting is an Alert, clear it from the AlertQueue
    if(current_lighting.is_alert){
      clearLighting(current_lighting);
      debug("Alert complete. Removing alert from queue.");
    }
    //Get next Transition
    //If there is an Alert in the queue, fetch it and load it into current_Lighting
    findNextAlert(&port);
    //Else, grab the next Transition, increment index, and begin rendering
    if(!current_lighting.is_good){
      getNextTransition(&port);
    }
    
  }
}

void clearLighting(Lighting *light){
  light.is_good = false;
}

//returns the next available alert for a port, if there is one. Else, return bad Lighting
Lighting* findNextAlert(Port* p){
  for(int j = 0; j < array_size; j++){
      if(p.alerts[j].is_good){
        debug("Alert found in queue");
        Serial.println(current_lighting);
        return = &p.alerts[j];
      }
    }
    debug("No alert found in queue for port");
    Lighting temp;
    temp.is_good = false;
    return &temp;
}

Lighting* getNextTransition(Port* p){
  uint8_t temp_index = p.current_transition_index;
  p.current_transition_index = ((p.current_transition_index++) % array_size);
  while(p.current_transition_index != temp_index){
    if(p.transitions[current_transition_index].is_good){
      p.current_lighting = &p.transitions[current_transition_index];
      return;
    }
    else{
      p.current_transition_index = ((p.current_transition_index++) % array_size);
    }

    p.current_lighting = &DEFAULTLIGHTING;
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
  
}