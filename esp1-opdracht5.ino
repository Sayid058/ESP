//Namen: Sayid Abd-Elaziz & Davy van Weijen
//Opdracht 5

//Led
#define LED1 25
#define LED2 33
#define LED3 32

bool Pause = false; //Pause
int Count_Three, Count_Ten = 0;


struct s_led
{
    byte gpio; // LED GPIO number
    byte state; // LED state
    unsigned napms; // Delay
    TaskHandle_t taskh; // Task handle
};


static s_led leds[3] = {
    {LED1, 0, 500, 0},
    {LED2, 0, 100, 0},
    {LED3, 0, 750, 0}};

static void led_task_func(void *argp)
{
    s_led *ledp = (s_led *)argp;
    unsigned stack_hwm = 0, temp;

    delay(1000);

    for (;;)
    {
        digitalWrite(ledp->gpio, ledp->state ^= 1);
        temp = uxTaskGetStackHighWaterMark(nullptr);
        if (!stack_hwm || temp < stack_hwm)
        {
            stack_hwm = temp;
            printf("Task for gpio %d has stack hwm %u\n",
                   ledp->gpio, stack_hwm);
        }
        delay(ledp->napms);
    }
}

void setup()

{

    int app_cpu = 0; // CPU nummer
    delay(500);      // Pause
    app_cpu = xPortGetCoreID();
    printf("app_cpu is %d (%s core)\n",
           app_cpu,
           app_cpu > 0 ? "Dual" : "Single");

    printf("LEDs on gpios: ");
    for (auto &led : leds)
    {
        pinMode(led.gpio, OUTPUT);
        digitalWrite(led.gpio, LOW);
        xTaskCreatePinnedToCore(
            led_task_func,
            "led_task",
            2048,
            &led,
            1,
            &led.taskh,
            app_cpu);
        printf("%d ", led.gpio);
    }
    putchar('\n');
}


void loop(){
    if (Pause) {
        Count_Three++;
        if (Count_Three >= 3) {
            Count_Three = 0;
            Pause = false;
            for (auto &led : leds) {
                vTaskResume(led.taskh);}
        }
  } 
    else {
        Count_Ten++;
        if (Count_Ten >= 10) {
            Count_Ten = 0;
            Pause = true;
            for (auto &led : leds) {
                vTaskSuspend(led.taskh);}
        }
    }
    delay(1000);
}