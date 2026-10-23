#include <LiquidCrystal.h>

#define LM35_SENSOR_CHANNEL 0
#define ADC_REF_VOLTAGE 5.0

const int rs=12, en=11, d4=5, d5=4, d6=3, d7=2;

LiquidCrystal lcd(rs,en,d4,d5,d6,d7);

volatile int ore= 17;
volatile int minute= 4;
volatile int secunde= 44;

volatile bool actualizare_afisaj= false;

void init_adc()
{
  ADMUX|=(1<<REFS0);
  ADCSRA|=(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
  ADCSRA|=(1<<ADEN);
}

float citire_temp()
{
  ADMUX&=0xF0;
  ADMUX|=LM35_SENSOR_CHANNEL;
  ADCSRA|=(1<<ADSC);
  
  while(ADCSRA&(1<<ADSC)){}
  
  uint16_t adc_value=ADC;
  float tensiune= (float)adc_value*ADC_REF_VOLTAGE/1024.0;
  float temperatura= tensiune*100.0;
  
  return temperatura;
}

void init_timer1()
{
  noInterrupts();
  TCCR1A= 0;
  TCCR1B= 0;
  TCNT1= 0;
  
  OCR1A= 15624;
  TCCR1B|=(1<<WGM12);
  TCCR1B|=(1<<CS12)|(1<<CS10);
  TIMSK1|=(1<<OCIE1A);
  interrupts();
}

ISR(TIMER1_COMPA_vect)
{
  secunde++;
  if(secunde >= 60)
  {
    secunde= 0;
    minute++;
    
    if(minute >= 60)
    {
      minute= 0;
      ore++;
    
      if(ore >= 24)
      {
        ore= 0;
      }
    }
  }
  
  actualizare_afisaj= true;
    
}

void setup()
{
  lcd.begin(16,2);
  init_adc();
  init_timer1();
  
  Serial.begin(9600);
  
}

void loop()
{
  if(actualizare_afisaj)
  {
    actualizare_afisaj= false;
    
    int tmp= (int)citire_temp();
    
    lcd.setCursor(0,0);
    lcd.print("Temp = ");
    lcd.print(tmp);
    lcd.print((char)176);
    lcd.print("C");
    
    lcd.setCursor(0,1);
    lcd.print("Ora ");
    if(ore < 10)
    {
      lcd.print("0");
    }
    lcd.print(ore);
    lcd.print(":");
    if(minute<10)
    {
      lcd.print("0");
    }
    lcd.print(minute);
    lcd.print(":");
    if(secunde < 10)
    {
      lcd.print("0");
    }
    lcd.print(secunde);
    lcd.print(" ");
    
    Serial.print("Temp: ");
    Serial.print(tmp);
    Serial.print("C|Ora: ");
    if(ore<10)
    {
      Serial.print("0");
    }
    Serial.print(ore);
    Serial.print(":");
    if(minute<10)
    {
      Serial.print("0");
    }
    Serial.print(minute);
    Serial.print(":");
    if(secunde<10)
    {
      Serial.print("0");
    }
    Serial.print(secunde);
    
  }
}
