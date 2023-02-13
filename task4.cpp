#include "mbed.h"
#include "C12832.h" //Imports the library for the LCD screen
#include "math.h"
C12832 lcd(D11, D13, D12, D7, D10); //Creates an LCD Object from the LCD library 


volatile int output_value = 0; // Integer to hold the current state of our LED. Is declared as volatile
InterruptIn fire(D4);
typedef enum {state1, state2, state3, state4, state5, state6} ProgramState;
//Definition of the enum that refers to the states of the program
ProgramState state;

class LED                                           //Begin LED class definition
{

protected:                                          //Protected (Private) data member declaration
    DigitalOut outputSignal;                        //Declaration of DigitalOut object
    bool status;                                    //Variable to recall the state of the LED

public:                                             //Public declarations
    LED(PinName pin) : outputSignal(pin)
    {
        off();   //Constructor - user provides the pin name, which is assigned to the DigitalOut
    }

    void on(void)                                   //Public member function for turning the LED on
    {
        outputSignal = 0;                           //Set output to 0 (LED is active low)
        status = true;                              //Set the status variable to show the LED is on
    }

    void off(void)                                  //Public member function for turning the LED off
    {
        outputSignal = 1;                           //Set output to 1 (LED is active low)
        status = false;                             //Set the status variable to show the LED is off
    }

    void toggle(void)                               //Public member function for toggling the LED
    {
        if (status)                                 //Check if the LED is currently on
            off();                                  //Turn off if so
        else                                        //Otherwise...
            on();                                   //Turn the LED on
    }

    bool getStatus(void)                            //Public member function for returning the status of the LED
    {
        return status;                              //Returns whether the LED is currently on or off
    }
};


class Potentiometer                                 //Begin Potentiometer class definition
{
private:                                            //Private data member declaration
    AnalogIn inputSignal;                           //Declaration of AnalogIn object
    float VDD, currentSampleNorm, currentSampleVolts; //Float variables to speficy the value of VDD and most recent samples

public:                                             // Public declarations
    Potentiometer(PinName pin, float v) : inputSignal(pin), VDD(v) {}   //Constructor - user provided pin name assigned to AnalogIn...
                                                                        //VDD is also provided to determine maximum measurable voltage
    float amplitudeVolts(void)                      //Public member function to measure the amplitude in volts
    {
        return (inputSignal.read()*VDD);            //Scales the 0.0-1.0 value by VDD to read the input in volts
    }
    
    float amplitudeNorm(void)                       //Public member function to measure the normalised amplitude
    {
        return inputSignal.read();                  //Returns the ADC value normalised to range 0.0 - 1.0
    }
    
    void sample(void)                               //Public member function to sample an analogue voltage
    {
        currentSampleNorm = inputSignal.read();       //Stores the current ADC value to the class's data member for normalised values (0.0 - 1.0)
        currentSampleVolts = currentSampleNorm * VDD; //Converts the normalised value to the equivalent voltage (0.0 - 3.3 V) and stores this information
    }
    
    float getCurrentSampleVolts(void)               //Public member function to return the most recent sample from the potentiometer (in volts)
    {
        return currentSampleVolts;                  //Return the contents of the data member currentSampleVolts
    }
    
    float getCurrentSampleNorm(void)                //Public member function to return the most recent sample from the potentiometer (normalised)
    {
        return currentSampleNorm;                   //Return the contents of the data member currentSampleNorm  
    }

};

class SamplingPotentiometer : public Potentiometer {

private:

 float samplingFrequency;
 Ticker sampler; 
 float samplingPeriod(){return(1/samplingFrequency);}


public:
  SamplingPotentiometer(PinName pin, float v,float fs):Potentiometer(pin, v), samplingFrequency(fs){sampler.attach(callback(this,&Potentiometer::sample), samplingPeriod());};
  void turnoff (void)
  {sampler.detach();}
};



class Speaker {
private:
    DigitalOut outputSignal;
    bool state;
    Ticker my_ticker; 


public:
    Speaker(PinName pin) : outputSignal(pin){off();}
    void on(void){
        outputSignal = 1;                           //Set output to 1
        state = 1;                              //Set the status variable to show the speaker is on
        }
    
    void off(void){
        outputSignal = 0;                       
        state = 0;                          
        }
        
    void toggle(void)                               //Public member function for toggling 
{
    if (state)           
        off(); 
        else
        on();
        }          

bool getStatus(void){
    return state;}
};

class SpeakerSample : public Speaker {
    private :
 float samplingFrequency;
 Ticker my_ticker; 
 float samplingPeriod(){return(1/samplingFrequency);}
 
 public : 
SpeakerSample(PinName pin, float fs) : Speaker(pin), samplingFrequency(fs){off();}

void ticker(void)
{my_ticker.attach(callback(this,&Speaker::toggle), samplingPeriod());}

void turnoff(void)
{my_ticker.detach();}

};
void pressedfire1(void){state = state2;lcd.cls(); wait (1.0);} // reaction for state1
void pause(void){state = state4;} // reaction for state2
void resume(void){state = state2;} // reaction for state3
void reset(void){
state = state6;}


int main()
{
    int minutes, seconds;
    state = state1;
    lcd.cls(); //Clear the screen
    LED redled(D5);
    LED blueled(D8);
    LED greenled(D9);
    SpeakerSample mainspeaker(D6,1800);



    while(1) {

        
    switch (state) { // inputs which state we want to be e.g. state 1, state 2, state 3
         case (state1) :
         {
        SamplingPotentiometer leftHand(A0, 9.2, 60);                //Create an object from the potentiometer class. Uses left potentiometer and 3.3V
         SamplingPotentiometer rightHand(A1, 59.2, 60);                //Create an object from the potentiometer class. Uses left potentiometer and 3.3V
         minutes = leftHand.getCurrentSampleVolts();
         seconds = rightHand.getCurrentSampleVolts();
         fire.rise(&pressedfire1);
         lcd.locate(10,0);
        lcd.printf("Set Time Duration %d : %d       ",minutes,seconds);
        lcd.locate(10,10);
        lcd.printf("                                ");
        lcd.locate(10,20);
        lcd.printf("                                ");
        redled.on();
        blueled.off();
        greenled.off();
         break;}
    
    case (state2) :
    {
    redled.off();
    blueled.off();
    greenled.on();
    fire.rise(&pause);
    static int sec = seconds;
    static int min = minutes;
    seconds = seconds - 1;
    lcd.locate(10,0);
    lcd.printf("Time Duration  %d : %d    ",min,sec);
    lcd.locate(10,10);
    lcd.printf("Time Remaining  %d : %d    ",minutes,seconds);
    if (minutes == 0 && seconds == 0)
    {state = state5;}

    if (seconds == 0 && minutes != 0)
    {wait (1.0);
    seconds = 59;
    minutes= minutes - 1;
    lcd.locate(10,10);
    lcd.printf("Time Remaining  %d : %d       ",minutes,seconds);}
    wait (1.0);
    break;}
    
    case (state3) :
    {
        lcd.locate(10,0);
        lcd.printf("Time Duration Reached");
        lcd.locate(10,10);
        lcd.printf("Press the centre button   to continue");
        redled.off();
        greenled.off();
        blueled.on();
        fire.rise(&reset);

        break;
    }
    
    case (state4) :
    {
    lcd.locate(10,0);
    lcd.printf("**TIMER PAUSED**       ");
    lcd.locate(10,10);
    lcd.printf("                            ");
    redled.on();
    greenled.on();
    blueled.off();
    fire.rise(&resume);
    break;}
    
    case (state5) :
    {
    mainspeaker.ticker();
    state = state3;
    break;}
    
    case (state6) :
    {
    mainspeaker.turnoff();
    wait(1);
    state = state1;
    break;}

    
    default:
    {minutes = 0;}
        }
    }
}
