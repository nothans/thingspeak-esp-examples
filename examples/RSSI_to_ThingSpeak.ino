/*
 ESP8266 --> ThingSpeak Channel via MKR1000 Wi-Fi
 
 The ThingSpeak Client sketch is designed for the Arduino MKR1000 Wi-Fi.
 This sketch writes analog and Wi-Fi signal strength data to a ThingSpeak channel
 using the ThingSpeak API (https://www.mathworks.com/help/thingspeak).
 Arduino Requirements:
 
   * Arduino MKR1000 with ATMEL WINC1500 Wi-Fi module
   * Arduino 1.6.5+ IDE
 
 ThingSpeak Setup:
 
   * Sign Up for New User Account - https://thingspeak.com/users/sign_up
   * Create a new Channel by selecting Channels, My Channels, and then New Channel
   * Enable one field
   * Note the Write API Key and Channel ID
    
 Setup Wi-Fi
  * Enter SSID
  * Enter Password
  
 Tutorial: http://nothans.com/measure-wi-fi-signal-levels-with-the-esp8266-and-thingspeak
   
 Created: Feb 1, 2017 by Hans Scharler (http://nothans.com)
 
*/
