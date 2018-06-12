// PasoConBarreraV0.4
// Simulación de un paso a nivel con barrera
// 28 de Abril de 2018
// KLM


#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// ***** Variables *****

// Sensores
// Sensor Barrera IR - Sensor1
int sensor1=3;  //  Pin asignado al sensor de barrera IR
int lectura1;   //  Var. almacenamiento valor del sensor 1
// Sensor Ultrasonido - Sensor2
int sensor2=2; //  Pin asigndo al sensor Ult. - s2  señal 'echo'
int pulso=4;    //  Pin asociado al emisor de señal del s2 - trigger
float tiempo;   //  Var. alm. tiempo recepción señal eco
int distancia;  //  Var. alm. valor de la distancia al obstáculo
int lectura2;   //  Var. alm. valor definitivo s2 (high-low)

// Semáforos
//  Variables semáforos de aviso- Leds RGB
int semaforoA_PinRojo=9;
int semaforoA_PinVerde=10; //  Designación Pins para brazos leds RGB
int semaforoA_PinAzul=11;
//  Variables semaforos de barrera - Leds rojos
int semaforos1=7;
int semaforos2=8;

// buzzer - Alarma sonora
int buzzer=5;

//  Variables de los servos barreras
Servo barreras; //  Inicialización del objeto Servo para los servos
int anguloBarreras=90;  // Var. Ángulo de inclinación del servo - barreras

// Display LCD I2C
LiquidCrystal_I2C   lcd (0x3F, 20, 4);  // Direccion, filas y columnas

// Variables generales
 int controlBarrera=0;    //  Variable de control


// ***** Funciones *****

// Escecnario 0
// No hay tren cerca. El paso es libre aunque con precacución
// La barrera permanecerá subida
void escenario0()
{
  unsigned long tiempoRef0=millis();  // Var. para poner contador de tiempo a 0
                                      //  en el escenario 0

  // Subir Barreras (en caso de que esté bajada)
  if (anguloBarreras == 0)
  {
    unsigned long tBarrera0 = millis();   //  Var. para controlar
                                          //  tiempo de subida de barrera
    //  La barrera se subirá hasta los 90º
    //  con un incremento de 1º cada 20 ms
    while (anguloBarreras<90)
    {
      barreras.write(anguloBarreras);
      if (millis()>(tBarrera0+20))
      {
        anguloBarreras++;
        tBarrera0=millis();
        barreras.write(anguloBarreras);
      }  //  end est. if millis
    }  //  end estructura while anguloBarreras
  }  //  end estructura if anguloBarreras

  //  Semáforos de barrera apagados
  digitalWrite(semaforos1, LOW);
  digitalWrite(semaforos2, LOW);

  // Display LCD Indicando situacion
  lcd.clear();      //  Limpiar Display
  lcd.backlight();  //  Activar luz de fondo
  lcd.setCursor(0, 0);
  lcd.print(" Paso Libre  A520");
  lcd.setCursor(0, 1);
  lcd.print("    Circule con");
  lcd.setCursor(0, 2);
  lcd.print("     Precaucion");

  //  Semáforo de aviso en ambar intermitente
  while((millis()>=tiempoRef0) && (millis()-tiempoRef0<500))
  {
  analogWrite(semaforoA_PinRojo, 255);
  analogWrite(semaforoA_PinVerde, 255);
  analogWrite(semaforoA_PinAzul, 0);
  }
  while((millis()-tiempoRef0>=500) && (millis()-tiempoRef0<1000))
  {
    analogWrite(semaforoA_PinRojo, 0);
    analogWrite(semaforoA_PinVerde, 0);
    analogWrite(semaforoA_PinAzul, 0);
  }  //  end while millis

  // Zumbador apagado

}  //  end escenario0



//  Escecnario 1
//  Paso del tren. Se corta el paso
void escenarioUno()
{
  unsigned long tiempoRef1=millis();  // Var. para poner contador de tiempo a 0
                                      // en el escenario 1
  // Bajar Barreras
  // Si la barrera se encuentra subida se bajará
  // deciendo un grado cada 20 ms
  if (anguloBarreras == 90)
  {
    unsigned long tBarrera = millis();
    while (anguloBarreras>0)
    {
      barreras.write(anguloBarreras);
      if (millis()>(tBarrera+20))
      {
        anguloBarreras--;
        tBarrera=millis();
        barreras.write(anguloBarreras);
      }  //  end est. if millis
    }  //  end estructura while anguloBarreras
  }  //  end estructura if anguloBarreras


  //  Semáforo de aviso fijado en rojo
  analogWrite(semaforoA_PinRojo, 255);
  analogWrite(semaforoA_PinVerde, 0);
  analogWrite(semaforoA_PinAzul, 0);

  // Display LCD Indicando situacion
  lcd.clear();      //  Limpiar Display
  lcd.backlight();  //  Activar luz de fondo
  lcd.setCursor(0, 0);
  lcd.print("      Atencion!");
  lcd.setCursor(0, 1);
  lcd.print("    Tren Pasando");
  lcd.setCursor(0, 2);
  lcd.print("   Barrera Bajada");

  // Semáforo de barrera en rojo, alternando los dos Leds
  // Suena la Alarma de manera intermitente
  //
  while((millis()>=tiempoRef1) && (millis()-tiempoRef1<200))
  {
    digitalWrite(semaforos1, HIGH);
    digitalWrite(semaforos2, LOW);
    analogWrite(buzzer, 30);
  }  //  end while <300
  while((millis()-tiempoRef1>=200) && (millis()-tiempoRef1<400))
  {
    digitalWrite(semaforos1, LOW);
    digitalWrite(semaforos2, HIGH);
    analogWrite(buzzer, 0);
  }  //  end while <500

}  //  end escenario1



//  ***** setup *****
void setup()
{
    pinMode(sensor1, INPUT);
    pinMode(sensor2, INPUT);
    pinMode(pulso, OUTPUT);
    pinMode(semaforoA_PinRojo, OUTPUT);
    pinMode(semaforoA_PinVerde, OUTPUT);
    pinMode(semaforoA_PinAzul, OUTPUT);
    pinMode(semaforos1, OUTPUT);
    pinMode(semaforos2, OUTPUT);
    pinMode(buzzer, OUTPUT);
    // Servos - barreras
    barreras.attach(6);   //  Pin  de conexion con los servos
    // lcd
    Wire.begin();
    lcd.begin(20, 4);
}  //  end setup


//  ***** loop *****
void loop()
{
    // Activar Sensores
    // Sensor 1 - Sensor de barrera IR
    lectura1=digitalRead(sensor1);
    // Sensor 2 - Sensor ultrasonido
    lectura2=HIGH;
    digitalWrite(pulso,LOW);  //  Por cuestión de estabilización del sensor
    delayMicroseconds(5);
    digitalWrite(pulso, HIGH);  // Envío del pulso ultrasónico
    delayMicroseconds(10);
    tiempo = pulseIn(sensor2, HIGH);  //  Funcion para medir el tiempo del eco
    distancia = 0.01715*tiempo;  // Fórmula para calcular la distancia
    if(distancia<10){lectura2=LOW;}   //  Sensor detecta Tren


    // Detectar el escenario en curso
    if (controlBarrera==1) {escenarioUno();}
    else {escenario0();}


    // Sensor1 detecta - Llega el tren - activación escenario 1
    if(lectura1==LOW && controlBarrera==0)       //  if 1
    {
      controlBarrera=1;  //  Control - Situación de barrera bajada
    } //  end if lectura1==low

    // Sensor de salida se activa - el tren se va
    if(lectura2==LOW && controlBarrera==1)
    {
       controlBarrera=0;
    }

}  //  end loop
