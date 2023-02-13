#include "mbed.h"
#include "C12832.h" 
#include<string>

InterruptIn up(A2);
InterruptIn down(A3);
InterruptIn fire(D4);
Ticker my_ticker; 
float increment = 150;
float frequency = 1800;

C12832 lcd(D11, D13, D12, D7, D10); 


int speakerstatus = 1;

volatile int output_value = 0; // Integer to hold the current state of our LED. Is declared as volatile
volatile int red_led_value = 0;
volatile int green_led_value = 0;
volatile int blue_led_value = 0;

class Speaker {
private:
    DigitalOut outputSignal;
    char state; // Can be set to either 1 or 0 to record output value

public:
    Speaker(PinName pin) : outputSignal(pin){off();}
    void on(void){
        outputSignal = 1;                           //Set output to 0 (LED is active low)
        state = 1;                              //Set the status variable to show the LED is on
        };
    
    void off(void){
        outputSignal = 0;                           //Set output to 1 (LED is active low)
        state = 0;                             //Set the status variable to show the LED is off
        }
        
    void toggle(void)                               //Public member function for toggling the LED
    {
        if (state == 1)                                 //Check if the LED is currently on
            off();                                  //Turn off if so
        else                                        //Otherwise...
            on();                                   //Turn the LED on
    }

    bool getStatus(void)                            //Public member function for returning the status of the LED
    {
        return state;                              //Returns whether the LED is currently on or off
    }

};

class LED {
private:
    DigitalOut outputSignal;
    char state;
    
public:
    LED(PinName pin) : outputSignal(pin){off();}
    void on(void){
        outputSignal = 0;                           //Set output to 1 (LED is active low)
        state = 1;                             //Set the status variable to show the LED is off
        }
        
    void off(void){
        outputSignal = 1;
        state = 0;
        }
        
    void toggle(void){
        if (state == 1)
        off();
        else
        on();
        }
        
        
    char getStatus(void)
    {
        return state;
    }
};     



void toggleISR() { // Callback function (ISR) for Ticker interrupt
    if (output_value == 1) { // Checks on the value of the status variable...
        output_value = 0;} // ...and toggles the output accordingly
    else {
        output_value = 1;
        }
}

void pressedup()               //The interrupt service routine (ISR)
{
    frequency = frequency + increment ;    //Toggle the green LED when the ISR is called
    my_ticker.attach(&toggleISR,1/(2*frequency));
    lcd.cls(); //Clear the screen
    lcd.locate(20,0); //Locate at (20,0)
    lcd.printf("Frequency : %f",frequency);
}

void presseddown()               //The interrupt service routine (ISR)
{
    frequency = frequency - increment ;    //Toggle the green LED when the ISR is called
    my_ticker.attach(&toggleISR,1/(2*frequency));
    lcd.cls(); //Clear the screen
    lcd.locate(20,0); //Locate at (20,0)
    lcd.printf("Frequency : %f",frequency);
}

void pressedfire()
{
    if (speakerstatus == 1)
    {my_ticker.detach();
    lcd.cls(); //Clear the screen
    lcd.locate(20,0); //Locate at (20,0)
    lcd.printf("Buzzer : OFF");
     red_led_value = 1;
     blue_led_value = 0;
     green_led_value = 0;
     speakerstatus =0;}
    else if(speakerstatus == 0)
    {my_ticker.attach(&toggleISR,1/(2*frequency));
    lcd.cls(); //Clear the screen
    lcd.locate(20,0); //Locate at (20,0)
    lcd.printf("Frequency : %f",frequency);
    green_led_value = 1;
    blue_led_value = 0;
    red_led_value = 0 ;
    speakerstatus =1;}


}


int main()
{
    LED redled(D5);
    LED blueled(D8);
    LED greenled(D9);
    up.rise(&pressedup);
    down.rise(&presseddown);
    fire.rise(&pressedfire);
    Speaker mainspeaker(D6);
    lcd.cls(); //Clear the screen
    lcd.locate(20,0); //Locate at (20,0)
    lcd.printf("Frequency : %f",frequency);
    my_ticker.attach(&toggleISR,1/(2*frequency)); //Specify which function to run, and the time period of hte interrupt (1 second)
    green_led_value = 1;
    red_led_value = 0;
    blue_led_value = 0;
    
    while(1) {
        if (output_value == 1)
        {mainspeaker.on();}
        if (output_value == 0)
        {mainspeaker.off();}
        if (red_led_value == 1)
        {redled.on();}
        if (red_led_value == 0)
        {redled.off();}
        if (green_led_value == 1)
        {greenled.on();}
        if (red_led_value == 0)
        {greenled.off();}
        if (blue_led_value == 1)
        {blueled.on();}
        if (blue_led_value == 0)
        {blueled.off();}
        
        
    }
}
