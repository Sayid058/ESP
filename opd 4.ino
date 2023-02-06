//Sayid Abd-Elaziz & Davy van Weijen, ITV2G
//Opdracht 4: Interrupt voor efficientie
//

#include "time.h"
#include "ESP32TimerInterrupt.h"

// LED is active high
#define GPIO_LED_SEM 14 
#define BUTTON1_PIN  27
#define BUTTON2_PIN 26

#define DEBOUNCE_TIME  30 //in ms

//Semaphorehandle

static SemaphoreHandle_t xSemaphore = NULL;

volatile unsigned long  lastDebounceTime_button1  = 0;
volatile unsigned long  lastDebounceTime_button2  = 0;
volatile bool           SwitchReset_button1  = true;
volatile bool           SwitchReset_button2  = true;

volatile bool           buttonPressed_button1     = false;
volatile bool           alreadyTriggered_button1  = false;
volatile bool           buttonPressed_button2     = false;
volatile bool           alreadyTriggered_button2  = false;

void IRAM_ATTR Falling_button1();
void IRAM_ATTR Rising_button1();
void IRAM_ATTR Falling_button2();
void IRAM_ATTR Rising_button2();
void IRAM_ATTR ButtonCheck();

void IRAM_ATTR Rising_button1() {
  unsigned long currentTime  = millis();
  unsigned long TimeDiff;

  TimeDiff = currentTime - lastDebounceTime_button1;

  if (digitalRead(BUTTON1_PIN) && (TimeDiff > DEBOUNCE_TIME) ) {
    buttonPressed_button1 = false;
    lastDebounceTime_button1 = currentTime;
  }
}


void IRAM_ATTR Falling_button1() {
  unsigned long currentTime  = millis();

  if ( !digitalRead(BUTTON1_PIN) && (currentTime > lastDebounceTime_button1 + DEBOUNCE_TIME)) {
    lastDebounceTime_button1 = currentTime;
    buttonPressed_button1 = true;
  }
}

void IRAM_ATTR Rising_button2() {
  unsigned long currentTime  = millis();
  unsigned long TimeDiff;

  TimeDiff = currentTime - lastDebounceTime_button2;

  if (digitalRead(BUTTON2_PIN) && (TimeDiff > DEBOUNCE_TIME) ) {
    buttonPressed_button2 = false;
    lastDebounceTime_button2 = currentTime;
  }

}


void IRAM_ATTR Falling_button2() {
  unsigned long currentTime  = millis();

  if ( !digitalRead(BUTTON2_PIN) && (currentTime > lastDebounceTime_button2 + DEBOUNCE_TIME)) {
    lastDebounceTime_button2 = currentTime;
    buttonPressed_button2 = true;
  }
}

void IRAM_ATTR HWCheckButton(void)
{
    if ((!alreadyTriggered_button1 && buttonPressed_button1)|| (!alreadyTriggered_button2 && buttonPressed_button2))
    {
        if(buttonPressed_button1)
            alreadyTriggered_button1 = true;
        else
            alreadyTriggered_button2 = true;
        Buttoncheck();
    }
    else if (!buttonPressed_button1 ||/* button2*/ !buttonPressed_button2)
    {
      // Reset flag when button released to avoid triggered repeatedly
        if(buttonPressed_button1)
          alreadyTriggered_button1 = false;
        else
          alreadyTriggered_button2 = false;
        Buttoncheck();
    }
}
void IRAM_ATTR ButtonCheck()
{
  boolean SwitchState_button1 = (digitalRead(BUTTON1_PIN));
  boolean SwitchState_button2 = (digitalRead(BUTTON2_PIN));

  if (!SwitchState_button1 && SwitchReset_button1)
  {
    blink_sem_led(100);
    SwitchReset_button1 = false;
  }
  else if (SwitchState_button1)
    SwitchReset_button1 = true;

  if (!SwitchState_button2 && SwitchReset_button2)
  {
    blink_sem_led(200);
    SwitchReset_button2 = false;
  }
  else if (SwitchState_button1)
    SwitchReset_button2 = true;
}
 
void blink_sem_led(int rate){

 
  if (xSemaphore != NULL){
    if (xSemaphoreTake(xSemaphore, (TickType_t) 10) == pdTRUE)
    
    {    
      int count = 0;
      printf("BLINK_SEM_LED, rate: %d\r\n",rate );

      while (count<10){
        digitalWrite(GPIO_LED_SEM,HIGH);
        delay(rate);
        digitalWrite(GPIO_LED_SEM,LOW);
        delay(rate);
        count++;
      }
    
    xSemaphoreGive(xSemaphore);
    }
  }    
}

// 
//

void setup() {
  int app_cpu = xPortGetCoreID();
  static int left = BUTTON1_PIN;
  static int right = BUTTON2_PIN;
  TaskHandle_t h;
  BaseType_t rc;
  
  delay(2000);          //tijd voor USB

  
  xSemaphore = xSemaphoreCreateBinary();
  xSemaphoreGive(xSemaphore);
 
  pinMode(GPIO_LED_SEM,OUTPUT);
  pinMode(BUTTON1_PIN,INPUT_PULLUP);
  pinMode(BUTTON2_PIN,INPUT_PULLUP);


  attachInterrupt(digitalPinToInterrupt(BUTTON1_PIN), Falling_button1, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON1_PIN), Rising_button1, RISING);
  attachInterrupt(digitalPinToInterrupt(BUTTON2_PIN), Falling_button2, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON2_PIN), Rising_button2, RISING);
}

void loop() {
  vTaskDelete(nullptr);
}
