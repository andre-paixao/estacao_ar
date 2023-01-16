// sketch criada por André paixão do canal Ideias e inventos.
// publicada 09/01/23.
// Bibliotecas 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);

// Simbolos personalizados para o lcd
byte graus[8]={B11000,B11000,B00000,B00111,B01000,B01000,B01000,B00111};
byte seta[8]={B00000,B01000,B00100,B11110,B00111,B11110,B00100,B01000};

// Variaveis
byte modo = 0; 
int tempAjust = 0;
int termopar = 0;
int potenciometro = 0;
unsigned long temp1 = 0;
unsigned long temp2 = 0;

// Definição do pinos
#define pinTermopar A7 // Leitura da temperatura
#define pinPotenciometro A6 // Ajuste de temperatura
#define pinChave 10 // Chave magnetica da ponta
#define pinFan 9 // Aciona mosfet que liga o step-up da ventoinha
#define pinTriac 3 // Relé de estado solido
#define pinLED 5 // Led indicador do aquecimento
#define pinTone 12 // buzzer passivo

// Frequencia dos tones
#define G  1568
#define F  1397
#define C  1047
#define A  1760


void setup() {

 pinMode (pinFan ,OUTPUT); 
 pinMode (pinTriac ,OUTPUT); 
 pinMode (pinTone ,OUTPUT); 
 pinMode (pinTermopar, INPUT);
 pinMode (pinPotenciometro, INPUT);
 pinMode (pinChave, INPUT_PULLUP);


// Inicialização do lcd
lcd.init();
lcd.backlight();
lcd.createChar(1, graus);
lcd.createChar(2, seta);

lcd.setCursor(0, 0);
lcd.print("Estacao AR 850");
lcd.setCursor(0, 1);
tone(pinTone, A, 200);
lcd.print("Ideias & invento");
delay (2000);
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("Em Espera");
}

void loop() {

lerPot (); // Faz a leitura da temperatura e do ajuste de temperatura

if (millis() - temp1  > 300 ){ temp1 = millis(); printLCD (); } // Realiza atualização no lcd a cada 300 ms

if (modo == 0 ) { // Desligado , em espera
  digitalWrite (pinFan, LOW);
  digitalWrite (pinTriac , LOW);
  digitalWrite (pinLED , LOW); }

if (modo == 1 ) { // Aquecendo
  digitalWrite (pinTriac , HIGH);
  digitalWrite (pinLED , HIGH);
  delay (400);
  digitalWrite (pinTriac , LOW);
  digitalWrite (pinLED , LOW);
  delay (200);
  lcd.setCursor(7, 1);
  lcd.write (2);
  if (termopar >= tempAjust ) {tone(pinTone, G, 100); modo = 2; }
                }

if (modo == 2 ){ // Estabilizado, mantendo temperatura
  if ((tempAjust - termopar) > 2  ) {lcd.setCursor(7, 1); lcd.write (2); digitalWrite (pinTriac , HIGH); digitalWrite (pinLED , HIGH); }
  if (termopar >= tempAjust) {lcd.setCursor(7, 1); lcd.print (' '); digitalWrite (pinTriac , LOW); digitalWrite (pinLED , LOW); }
  if  ((termopar - tempAjust) > 100 ) {
                                       lcd.setCursor(0, 0);
                                       lcd.print ("                ");
                                       lcd.setCursor(0, 0);
                                       lcd.print ("ERRO TEMPERATURA");
                                       tone(pinTone, A, 200);  }
               }


if (modo == 3 ) { // Esfriando... Desligando
  digitalWrite (pinTriac , LOW);
  digitalWrite (pinLED , LOW);
  lcd.setCursor(7, 1); 
  lcd.print (' ');
  if (termopar < 60) { tone(pinTone, C, 100); modo = 0; lcd.setCursor(0, 0); lcd.print("Em espera       ");}
                }

} // Fim do loop



void printLCD () {
lcd.setCursor(0, 1);
lcd.print(termopar);
lcd.write(1);
lcd.print ("   ");

lcd.setCursor(12, 1);
lcd.print(tempAjust);
lcd.write(1);
lcd.print ("   ");

} // Fim do printLCD

void lerPot () {
 tempAjust = analogRead (pinPotenciometro);
 tempAjust = map(tempAjust, 0, 1023, 0, 450);

 if (millis() - temp2  > 60 ){temp2 = millis();
                              termopar = analogRead (pinTermopar); 
                              termopar = map(termopar, 0, 500, 10, 500); }

if (digitalRead (pinChave) == 1 ) { if (modo != 1 && modo != 2  ) { tone(pinTone, A, 100); 
                                                                    modo = 1 ; 
                                                                    lcd.setCursor(0, 0); 
                                                                    lcd.print("Aquecendo        "); 
                                                                    digitalWrite (pinFan, HIGH);
                                                                    delay (1000);} 
  } else { if (modo != 0 && modo != 3 ) { tone(pinTone, C, 100); 
                                          modo = 3; 
                                          lcd.setCursor(0, 0); 
                                          lcd.print("Esfriando        ");} }
/*
MODO
0 = Em espera / Desligado
1 = Aquecendo até a temperatura selecionada
2 = Mantendo a temperatura selecionada
3 = Esfriando até chegar a 60 graus

*/


} // Fim do lerPot
