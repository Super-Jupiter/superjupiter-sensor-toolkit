#include <Arduino.h>
#include "pipeline.h"
void setup(){Serial.begin(115200);const auto r=superjupiter::portfolio_example::run();Serial.println(r.passed?"PASS synthetic pipeline":"FAIL synthetic pipeline");}
void loop(){}
