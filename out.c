for(int f = 0; f < 6; f += 2){
	for(int i = 0; i < LED_COUNT; i += 1){
		if(i % 6 == f){
			leds.setPixelColor(i + CENTER_OFFSET, 0, 0, 255);
		}else if(i % 6 == (f + 1) % 6){
			leds.setPixelColor(i + CENTER_OFFSET, 0, 255, 0);
		}else if(i % 6 == (f + 2) % 6){
			leds.setPixelColor(i + CENTER_OFFSET, 255, 0, 0);
		}else{
			leds.setPixelColor(i + CENTER_OFFSET, 0, 0, 0);
		}
	}
	leds.show();
	delay(DELAY);
	var +=     3;
	var += 2;
}
print();
