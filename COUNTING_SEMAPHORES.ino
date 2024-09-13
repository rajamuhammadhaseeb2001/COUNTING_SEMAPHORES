#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

static SemaphoreHandle_t bin_sem;
static SemaphoreHandle_t counting;
static const int led_pin = 2;

void blink(void *parameters)
{
  int num = *(int *)parameters;
  xSemaphoreGive(bin_sem);

  Serial.print("RECEIVED: ");
  Serial.println(num);

  pinMode(led_pin, OUTPUT);

  //Serial.println("ENTERING LED");
  while (1)
  {
    if (xSemaphoreTake(counting , portMAX_DELAY)==pdTRUE)
    {
    //Serial.println("ENTERED LED");
    digitalWrite(led_pin, HIGH);
    vTaskDelay(num/portTICK_PERIOD_MS);
    //Serial.println("HELLO");
    digitalWrite(led_pin, LOW);
    vTaskDelay(num/portTICK_PERIOD_MS);
    }
    vTaskDelay(1000/portTICK_PERIOD_MS);
  }
}

void setup()
{
  long int delay_arg;

  Serial.begin(115200);
  vTaskDelay(1000/portTICK_PERIOD_MS);
  Serial.println();

  while(Serial.available()<=0);

  delay_arg = Serial.parseInt();
  Serial.print("SENDING....");
  Serial.println(delay_arg);

  bin_sem = xSemaphoreCreateBinary();
  counting = xSemaphoreCreateCounting(5, 0);

  xTaskCreatePinnedToCore(blink,
                          "BLINLED LED",
                          1024,
                          (void *)&delay_arg,
                          1,
                          NULL,
                          app_cpu);
  
 xSemaphoreTake(bin_sem,portMAX_DELAY);
 Serial.println("DONE");
}

void loop ()
{
  for (int i = 0; i < 5; i++)  // This loop controls the number of gives
{
  xSemaphoreGive(counting);
  vTaskDelay(10 / portTICK_PERIOD_MS);
}
  vTaskDelay(portMAX_DELAY);
}