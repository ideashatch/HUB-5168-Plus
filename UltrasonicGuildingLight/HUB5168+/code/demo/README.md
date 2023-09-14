
## ameba ws2812b 使用方式
0. WS2812B name(LED_PIN,NUM_LEDS); 實例化
1. begin : name.begin(); 開始使用led燈條，用於setup
2. show	 : name.show(); 將設定好的led燈調顯示出來
3. clear : name.clear() 將所有led清空
4. setLEDCount   :	name.setLEDCount(num_led) 將傳入參數值重新分配LED陣列的大小，用於動態規劃LED陣列
5. setPixelColor :	name.setPixelColor(led_Number,rColor,gColor,bColor) 設置單一個led的顏色
6. fill	         : name.fill(rColor,gColor,bColor,first,count) 設置填滿一個區段(first,first+count)顏色
7. colorHSV      : color = colorHSV(hue,sat,val); 將HSV格式轉為RGB格式，通常拿來使用在rainbow內，不會單獨出現
8. rainbow	     : led.rainbow(firstPixelHue);  顯示彩虹，用明度來決定的

