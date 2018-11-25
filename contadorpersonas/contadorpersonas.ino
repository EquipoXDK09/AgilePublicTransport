#include <TimerOne.h>

//GET https://api.thingspeak.com/update?api_key=ZTFZ0WIHLOAOTLIQ&field1=0

bool sensorArriba_f=0, sensorAbajo_f=0;
int personas=0, conteo=0;

/*void sensorAbajo_it();
void sensorArriba_it();
void revisarCambio_it();
*/
void setup() {
  // put your setup code here, to run once:
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(13, OUTPUT);
attachInterrupt(digitalPinToInterrupt(2), sensorAbajo_it, RISING);
attachInterrupt(digitalPinToInterrupt(3), sensorArriba_it, RISING);
Timer1.initialize(100000);
Timer1.attachInterrupt(revisarCambio_it);
Serial.begin(9600);


}

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.println(personas);
  if(personas%2==0){
    digitalWrite(13, HIGH);
  } else {
    digitalWrite(13, LOW);
  }

  Serial.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80");
  delay(500);
  Serial.println("AT+CIPSEND=74");
  delay(100);
  Serial.print("GET https://api.thingspeak.com/update?api_key=ZTFZ0WIHLOAOTLIQ&field8=");
  Serial.println(String(personas));
  delay(10000);
  //Serial.println(personas);
}

void sensorAbajo_it(){
  sensorAbajo_f=1;
  if(sensorArriba_f == 1 && personas>0 && conteo!=0){
    personas--;
    sensorArriba_f=0; 
    sensorAbajo_f=0;
  }
  conteo=10;
}

void sensorArriba_it(){
  sensorArriba_f=1;
  if(sensorAbajo_f == 1 && personas <50 && conteo!=0){
    personas++;
    sensorArriba_f=0; 
    sensorAbajo_f=0;
  }
  conteo=10;
}

void revisarCambio_it(){
  if(conteo>0){
    conteo--;
  } else {
    sensorArriba_f=0; 
    sensorAbajo_f=0;
  }
}
