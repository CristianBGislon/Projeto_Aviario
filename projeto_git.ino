#include <rgb_lcd.h>
#include <SPI.h>
#include <Ethernet.h>

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
bool ArtificialLight = false;
const int colorR = 0, colorG = 255, colorB = 0;

String readString;  
char SensorTensao[100]; //Utilize esta variavel para receber os valores de saida

byte mac[] = {0x98, 0x4F, 0xEE, 0x01, 0xEE, 0x27 };
IPAddress ip(223,254,242,220);

EthernetClient cliente;
EthernetServer server(80);


void setup()
{
  pinMode(pinSensTemp,INPUT);
  pinMode(pinSensLum,INPUT);
  pinMode(pinRele,OUTPUT);
  pinMode(pinLed,OUTPUT);
  lcd.begin(16,2);  
  lcd.setRGB(colorR, colorG, colorB);
  attachInterrupt(pinBotao, state, RISING);
  
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
    // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  
}

void loop()
{
  comunicacao();

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
      chave = 1;
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
  else if (lum > lumMinn + 1){
    digitalWrite(pinLed,LOW);
    }
  
  }

  void state()
  {       
    chave++; 
    lcd.clear();
    if (chave > 5) { chave = 1;}
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
                      lcd.print("Fan: ON");}
                    else
                    {lcd.clear();
                    lcd.home();
                    lcd.print("Fan: OFF");
                    }
                    break;  
          case 5:   lcd.home();
                    if(ArtificialLight)
                    { 
                      lcd.clear();
                      lcd.print("Artificial Light:");
                      lcd.setCursor(0,1);
                      lcd.print("ON");
                      }
                    else
                    {
                      lcd.clear();
                      lcd.home();
                      lcd.print("Artificial Light:");
                      lcd.setCursor(0,1);
                      lcd.print("OFF");
                    }
                    break;    
         }
        }
  
void comunicacao(){
 // Conecte o cliente ao servidor
EthernetClient cliente = server.available();
if (cliente)  {
  boolean currentLineIsBlank = true;
  while (cliente.connected()){
    if (cliente.available()){

        //Dentre da conexao ja estabelecida e disponivel utilize este codigo
        char c = cliente.read();
        if (readString.length() < 100) {readString += c;}
        
        if (c == '\n' && currentLineIsBlank) { 
            //Atualização de valores  (Segundo loop)        
          noise = analogRead(pinSensSom);
          tempInst = temperature(pinSensTemp);  
          lumInst = luminosity();
          verificaTemp(tempInst,tempMax);
          verificaAlarme(tempInst,tempMax);
          verificaLuminosity(lumInst, lumMin);
          LCD_Control();
          
          //Codigo para criar o layoute da pagina
        cliente.println("HTTP/1.1 200 OK"); //send new page
        cliente.println("Content-Type: text/html");
        cliente.println();  
        cliente.println("<html><head><title >Aviario Inteligente</title>");
        cliente.println("<meta http-equiv='refresh' content='1' />");
        cliente.println("<meta name='apple-mobile-web-app-capable' content='yes' />");
        cliente.println("<meta name='apple-mobile-web-app-status-bar-style' content='black-translucent' />");
        cliente.println("<link rel='stylesheet' type='text/css' href='http://randomnerdtutorials.com/ethernetcss.css' />");
        cliente.println("<script>");
        cliente.println("function drawPicture(){");
        cliente.println("var canvas = document.getElementById('example');");
        cliente.println("var context = canvas.getContext('2d');");
        
        cliente.println("context.moveTo(0, 100);");
        cliente.println("context.lineTo(1000, 100);");
        cliente.println("context.moveTo(0, 200);");
        cliente.println("context.lineTo(1000, 200);");
        cliente.println("context.moveTo(0, 300);");
        cliente.println("context.lineTo(1000, 300);");
        cliente.println("context.moveTo(0, 400);");
        cliente.println("context.lineTo(1000, 400);");
        cliente.println("context.font = '30px Arial';");
        sprintf(SensorTensao, "context.fillText('Temperatura Atual: %3.2f C',5,100-10);",tempInst);
        cliente.println(SensorTensao); 
        cliente.println("context.fillText('Temperatura Max:',5,200-10);");
        cliente.println("context.fillText('Temperatura Min:',5,300-10);");
        cliente.println("context.fillText('Luminosidade Atual:',5,400-10);");
        //cliente.println("context.strokeText('1V',5,500);");    
        cliente.println("context.strokeStyle = 'hsla(444, 0%, 0%, 1)';");
       cliente.println(" context.stroke();");
        
        cliente.println("context.stroke();");
        cliente.println("context.lineWidth = 1;");
        cliente.println("context.beginPath();");



//Codigo para atualizar os valores da pagina
        cliente.println("context.strokeStyle = 'hsla(360, 100%, 50%, 1)';");
        cliente.println("context.stroke();");
        cliente.println("}");
        cliente.println("</script>");
        cliente.println("<style type='text/css'>");
         cliente.println(" canvas { border: 2px solid black; }");// archivo css
        cliente.println("</style>");
        cliente.println("</head>");

        cliente.println("<BODY bgcolor= \"  #B8B8B8\"> ");
        cliente.println("<body onload='drawPicture();'>");
        cliente.println("<img src = \"https://upload.wikimedia.org/wikipedia/commons/6/6f/Brasao_UFSC_vertical_extenso.svg\" width  = 150 height = 150> </img>");
        cliente.println("<H1>Pratica Placa Galileo</H1><hr/><br/><H2>Voltagem</H2><br/>"); 
        cliente.println("<canvas id='example' width='1000' height='500'>");
        cliente.println("There is supposed ");
        cliente.println("</canvas>");
        cliente.println("<img src = \"http://www.clker.com/cliparts/Z/I/X/s/l/b/fan-hmi-rnng.svg.hi.png\" width  = 150 height = 150> </img>");  
        //cliente.println("<img src = \"https://thumbs.gfycat.com/BlandCompassionateBoilweevil-size_restricted.gif\" width  = 150 height = 150> </img>");  
 
//        sprintf(SensorTensao, "<H1>%d,%d V </H1>",sensorValue2/100 ,sensorValue2%100);
 //       cliente.println(SensorTensao);
        
        cliente.println("<br/><br/><br/>");   
        cliente.println("<p>Intel</p>");  
        
        cliente.println("</body>");
        cliente.println("</html>");        
          //break;

          delay(10);           // Delay de tempo para o servidor
          cliente.stop();     // encerra conexao


  delay(1500);
            
         readString="";
        }
        if (c == '\n') {
           currentLineIsBlank = true;
        } 
        else if (c != '\r') {
           currentLineIsBlank = false;
        }
      }
    }
     Serial.println("cliente desconectado");

  }
}

