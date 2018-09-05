#include <WioLTEforArduino.h>
#include <stdio.h>
#include "Ultrasonic.h"


#define APN               "soracom.io"
#define USERNAME          "sora"
#define PASSWORD          "sora"

#define WEBHOOK_EVENTNAME "line_trigger"
#define WEBHOOK_KEY       "cRllYNotxhd9jpRHUSlp9T"
#define WEBHOOK_URL       "https://maker.ifttt.com/trigger/"WEBHOOK_EVENTNAME"/with/key/"WEBHOOK_KEY

#define ULTRASONIC_PIN  (WIOLTE_D38)

#define INTERVAL         (1000)
#define WORKING_TIME     (3600)

Ultrasonic ultrasonic(ULTRASONIC_PIN);
char data[256];
int sitting_time;
int event_count;
WioLTE Wio;
  
void setup() {
  delay(200);

  SerialUSB.println("");
  SerialUSB.println("--- START ---------------------------------------------------");
  
  SerialUSB.println("### I/O Initialize.");
  Wio.Init();
  
  SerialUSB.println("### Power supply ON.");
  Wio.PowerSupplyLTE(true);
  delay(500);

  SerialUSB.println("### Turn on or reset.");
  if (!Wio.TurnOnOrReset()) {
    SerialUSB.println("### ERROR! ###");
    return;
  }

  SerialUSB.println("### Connecting to \""APN"\".");
  if (!Wio.Activate(APN, USERNAME, PASSWORD)) {
    SerialUSB.println("### ERROR! ###");
    return;
  }

  SerialUSB.println("### Setup completed.");

  sprintf(data, "{\"value1\":\"体操してね。\",\"value2\":\"3分くらい。\",\"value3\":\"戻ったらマイコンリセットね。\"}");

}


void loop() {
  int status;
  long distance;

  distance = ultrasonic.MeasureInCentimeters(); // two measurements should keep an interval
  //SerialUSB.print(distance);
  //SerialUSB.println(" cm");

  // モニタと人の距離は40cmくらいが適切らしいです。
  if(distance< 100) {
    Wio.LedSetRGB(16, 0, 0);  // red
    sitting_time++;
  } else {
    Wio.LedSetRGB(0, 16, 0);  // green
    
  }
  
  if(sitting_time >= WORKING_TIME)  // 作業は1時間以内
    if(( event_count < 10) && (sitting_time % 60 == 0)) {  // 1分おきに10回まで通知

      SerialUSB.println("### Post.");
      SerialUSB.print("Post:");
      SerialUSB.print(data);
      SerialUSB.println("");
      if (!Wio.HttpPost(WEBHOOK_URL, data, &status)) {
        SerialUSB.println("### ERROR! ###");
        goto err;
      }
      SerialUSB.print("Status:");
      SerialUSB.println(status);
    
      event_count++;

    }

  // 休憩が終わったらマイコンを手動リセットしてください。
  

err:
  //SerialUSB.println("### Wait.");
  delay(INTERVAL);
}

