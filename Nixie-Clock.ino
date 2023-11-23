#include "DisplayManager.h"

void setup()
{
	Serial.begin(115200);
	DisplayManager::init();
}

void loop()
{
	DisplayManager::loop();
}