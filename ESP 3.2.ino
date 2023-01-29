//Sayid Abd-Elaziz & Davy van Weijen, ITV2G
//3.2

#include "time.h"

// LED is active high
#define GPIO_LED_SEM 14 
#define GPIO_BUTTONL  27
#define GPIO_BUTTONR  26

#define DEBOUNCE_TIME  30

//Declare SemaphoreHandle
static SemaphoreHandle_t xSemaphore = NULL;
//
//
static void debounce_task(void *argp) {

  unsigned button_gpio = *(unsigned*)argp;
  int lastSteadyState = LOW;       
  int lastFlickerableState = LOW; 
  int currentState;                
  unsigned long lastDebounceTime = 0;  
  
  for (;;) {

    currentState = digitalRead(button_gpio)

    if (currentState != lastFlickerableState) {
      // reset the timer
      lastDebounceTime = millis();
      // save the last flickerable state
      lastFlickerableState = currentState;
    }
    if ((millis() - lastDebounceTime) > DEBOUNCE_TIME) {
 
      if(lastSteadyState == HIGH && currentState == LOW){
        printf("The button %d is pressed\r\n", button_gpio);
        if (button_gpio==26)
          blink_sem_led(200);
        else
          blink_sem_led(400);
      }
      else if(lastSteadyState == LOW && currentState == HIGH){
        printf("The button %d is released\r\n", button_gpio);
      }
      // save the the last steady state
      lastSteadyState = currentState;
    }
    taskYIELD();
  }
}



void blink_sem_led(int rate){

  //set Semaphore and when done relase the Semaphore
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
    //free Semaphore
    xSemaphoreGive(xSemaphore);
    }
  }    
}

//Installatizon

void setup() {
  int app_cpu = xPortGetCoreID();
  static int left = GPIO_BUTTONL;
  static int right = GPIO_BUTTONR;
  TaskHandle_t h;
  BaseType_t rc;
  
  delay(2000);          

  xSemaphore = xSemaphoreCreateBinary();

  xSemaphoreGive(xSemaphore);
  pinMode(GPIO_LED_SEM,OUTPUT);
  pinMode(GPIO_BUTTONL,INPUT_PULLUP);
  pinMode(GPIO_BUTTONR,INPUT_PULLUP);

  rc = xTaskCreatePinnedToCore(
    debounce_task,
    "debounceL",
    2048,     // Size of the stack
    &left,    // Left button gpio
    1,        // Prioriteit
    &h,       // Task 
    app_cpu   // CPU
  );
  assert(rc == pdPASS);
  assert(h);
  rc = xTaskCreatePinnedToCore(
    debounce_task,
    "debounceR",
    2048,     // Size of the stack
    &right,   // Right button gpio
    1,        // Prioriteit
    &h,       // Task
    app_cpu   // CPU
  );
  assert(rc == pdPASS);
  assert(h);
}

void loop() {
  vTaskDelete(nullptr);
}

