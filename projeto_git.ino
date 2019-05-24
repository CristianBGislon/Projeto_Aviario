

#include <rgb_lcd.h>
rgb_lcd lcd;

#define tempMax 27
#define lumMin 50
const int pinSensTemp = A3;
const int pinSensLum = A2;
const int pinSensSom = A0;
const int pinBuzzer = 4;
const int pinLed = 7;
const int pinRele = 6;
const int pinBotao = 5;
float tempInst = 0;
float lumInst = 0;
float noise = 0;
bool ventilador = false;
int chave = 1;
const int colorR = 0;
const int colorG = 255;
const int colorB = 0;

void setup()
{
  pinMode(pinSensTemp,INPUT);
  pinMode(pinSensLum,INPUT);
  pinMode(pinRele,OUTPUT);
  pinMode(pinLed,OUTPUT);
  lcd.begin(16,2);  
  lcd.print("Temperatura:"); 
  lcd.setRGB(colorR, colorG, colorB);
  attachInterrupt(pinBotao, state, RISING);
}

void loop()
{
  noise = analogRead(pinSensSom);
  tempInst = temperature(pinSensTemp);  
  lumInst = luminosity();
  

  
  tempInst = temperature(pinSensTemp);  
  lumInst = luminosity();  
  verificaTemp(tempInst,tempMax);
  verificaAlarme(tempInst,tempMax);
  verificaLuminosity(lumInst, lumMin);

  LCD_Control();

  
  delay(1500);

}


float temperature(const int pinTemp){
  const int B = 4275; // Constante B do termistor
  const int R0 = 100000; // Resistência em temperatura ambiente (100kΩ)
  float a = analogRead(pinTemp);
  float R = (1023.0-a)*R0/a;
  float temperatura = 1.0/(log(R/R0)/B+1/298.15)-273.15;
  return (temperatura);  
  }

void verificaTemp(float tempIns,int tempMaxx){
  if (tempIns > tempMaxx){
    digitalWrite(pinRele,HIGH);
    ventilador = true;
    
    }
    else if (tempIns <  tempMaxx - 1) {
      digitalWrite(pinRele,LOW);
      ventilador = false;
      }  
  }

void verificaAlarme(float temp,int tempMaxx){
    if (temp > tempMaxx + 2){
      alarm(true,pinBuzzer); 
      lcd.setRGB(255, 0, 0);
    }
    else {
      alarm(false,pinBuzzer); 
      lcd.setRGB(colorR, colorG, colorB);
      }  
  }

void alarm(boolean activate, const int pinBuzzer){
  if(activate == true){
    tone(pinBuzzer,1500,300);
    noTone(pinBuzzer); 
    delay(300);   
    }
   else{
    noTone(pinBuzzer); 
    }
}

float luminosity(){
  return map(analogRead(pinSensLum),0,800,0,100);
  }

void verificaLuminosity(int lum, int lumMinn){
  if (lum < lumMinn){
    digitalWrite(pinLed,HIGH);    
    }
   else{
    digitalWrite(pinLed,LOW);
    }
  
  }


/*void NoiceControl(){
  
}*/
  void state()
{       
  chave++; 
  lcd.clear();
  if (chave >4) { chave = 1;}
  }



  void LCD_Control(){
        switch(chave)
            {
          case 1:   lcd.home();
                    lcd.print("Temperature:"); 
                    lcd.setCursor(0,1);
                    lcd.print(tempInst);
                    lcd.print("C");
                    break;
          case 2:   lcd.home();
                    lcd.print("Luminosity:"); 
                    lcd.setCursor(0,1);
                    lcd.print(lumInst);
                    break;               
          case 3:   lcd.home();
                    lcd.print("Noise:"); 
                    lcd.setCursor(0,1);
                    lcd.print(noise);
                    break; 

                    
          case 4:   lcd.home();
                    if(ventilador)
                    { lcd.clear();
                      lcd.print("Ventilador: ON");}
                    else
                    {lcd.clear();
                    lcd.home();
                    lcd.print("Ventilador: OFF");
                    }
                    break;    
         }
        }
  
