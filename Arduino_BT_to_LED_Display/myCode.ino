#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

// Define the number of devices we have in the chain and the hardware interface

#define	MAX_DEVICES	4 	//number of matrices
#define	CLK_PIN		13	//clock pin
#define	DATA_PIN	11	//data pin
#define	CS_PIN		10	//cs pin

// set to 1 if we are implementing the user interface pot
#define	USE_UI_CONTROL	1

#if USE_UI_CONTROL
#define	SPEED_IN	A5
uint8_t	frameDelay = 25;	// default frame delay value
#endif // USE_UI_CONTROL

// Hardware SPI connection
MD_Parola P = MD_Parola(CS_PIN, MAX_DEVICES);
// Arbitrary output pins
// MD_Parola P = MD_Parola(DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

#define	SPEED_TIME	25
#define	PAUSE_TIME	1000

// Turn on debug statements to the serial output
#define  DEBUG  0

#if  DEBUG
#define	PRINT(s, x)	{ Serial.print(F(s)); Serial.print(x); }
#define	PRINTS(x)	Serial.print(F(x))
#define	PRINTX(x)	Serial.println(x, HEX)
#else
#define	PRINT(s, x)
#define PRINTS(x)
#define PRINTX(x)
#endif

bool firsttime = true;
char c;
void Write(char *tulisan);
String stringOne;
char charBuf[200];

// Global variables
uint8_t  curText;
char	*pc[] =
{
  ""
};

uint8_t  inFX, outFX;
textEffect_t	effect[] =
{
  PA_SCROLL_LEFT
};

#if USE_UI_CONTROL
void doUI(void)
{
  // set the speed if it has changed
  {
    int16_t	speed = map(analogRead(SPEED_IN), 0, 1023, 0, 250);

    if (speed != (int16_t)P.getSpeed())
    {
//      P.setSpeed(speed);      //UNCOMMENT these lines to use variable resistor connected to pin A5 as the scrolling speed controller (*)  
//      P.setPause(speed);      //(*)
//      frameDelay = speed;     //(*)

      int speed1 = 70; 		//Text Scroll Speed       //COMMENT these lines to use variable resistor as the scrolling speed controller (**) <- the smaller the value, the faster scrolling speed would be   
      P.setSpeed(speed1);     //(**)
      P.setPause(speed1);     //(**)
      frameDelay = speed1;    //(**)
      
      PRINT("\nChanged speed to ", P.getSpeed());
    }
  }
}
#endif // USE_UI_CONTROL





char* string2char(String command){
    if(command.length()!=0){
        char *p = const_cast<char*>(command.c_str());
        return p;
    }
}


void setup(void)
{

#if USE_UI_CONTROL
  pinMode(SPEED_IN, INPUT);
  doUI();
#endif // USE_UI_CONTROL

  P.begin();
  P.setInvert(false);
  P.displayText(pc[curText], PA_CENTER, SPEED_TIME, PAUSE_TIME, effect[inFX], effect[outFX]);

  Serial.begin(9600);
  Serial.println("Check"); //text displayed while serial communication

}

void loop()
{

  while (Serial.available()) 
  {
         
    if (Serial.available() > 0) 
    {
      firsttime = false;
      stringOne = Serial.readString();
      Serial.println(stringOne);
                 
      stringOne.toCharArray(charBuf, 200) ;
    }

 
    Serial.flush();  
  }
  
if (firsttime == false) Write(charBuf);
else Write("Welcome!");
      
}





void Write(char *tulisan)
{
  
#if USE_UI_CONTROL
	doUI();
#endif // USE_UI_CONTROL


char *pc[] = {tulisan};

  if (P.displayAnimate()) // animates and returns true when an animation is completed
  {
    // Set the display for the next string.
    curText = (++curText);
    P.setTextBuffer(pc[curText]);
    
    // When we have gone back to the first string, set a new exit effect
    // and when we have done all those set a new entry effect.
    if (curText == 0)
    {
      outFX = (++outFX);
      if (outFX == 0)
      {
        inFX = (++inFX);
        if (inFX == 0) 
          P.setInvert(!P.getInvert());
      }

      P.setTextEffect(effect[inFX], effect[outFX]);
    }

    // Tell Parola we have a new animation
    P.displayReset();
  }
}
