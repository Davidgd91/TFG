/*****************************************************************************/
/*!
    \archivo      
    \descripcion  
    \autor   David Garcia Diez
    \fecha     
    \version  1.0
******************************************************************************/

/*****************************************************************************/
/*                          MODULOS IMPORTADOS                               */
/*****************************************************************************/


/*****************************************************************************/
/*                               DEFINES                                     */
/*****************************************************************************/
#define S0        A10
#define S1        A9
#define S2        A8

#define T0        A11
#define T1        A12
#define T2        A13

#define POT_STR   A6
#define LED_EL    53 
#define LED_A     51
#define LED_D     49
#define LED_G     47
#define LED_B     45
#define LED_EH    43

#define GUIT_R    2
#define GUIT_V    3
#define GUIT_A    4

#define SW_LIGADO     37
#define SW_VEL        35
#define SW_TRASTE     33
#define SW_BASS_GUIT  31

#define N_STR     3
#define N_FRET    13
#define N_KEYS    13

#define EAD       0
#define ADG       1
#define DGB       2
#define GBE       3

#define FRET0     0
#define FRET1     830
#define FRET2     700
#define FRET3     595
#define FRET4     510
#define FRET5     447
#define FRET6     380
#define FRET7     330
#define FRET8     280
#define FRET9     232
#define FRET10    184
#define FRET11    136
#define FRET12    85
#define FRET13    10
#define FRETFN    5

#define NOTA_E_LOW  40
#define NOTA_A      45
#define NOTA_D      50
#define NOTA_G      55
#define NOTA_B      60
#define NOTA_E_HIGH 65

#define BASS_E 28
#define BASS_A 33
#define BASS_D 38
#define BASS_G 43


/*****************************************************************************/
/*                         PROTOTIPO DE FUNCIONES                            */
/*****************************************************************************/
void imprimir();
void readControls();
bool comprobarPulsado(int i);
void determinaTraste ();
void ligadoNotas();
void rutinaEnvio();
void noteOn(int pitch, int velocity);
void noteOff(int pitch);
void cortarNota();
unsigned char estadoCuerdas();
unsigned char determinaNota(int i);
unsigned short valorCuerda(int i);

/*****************************************************************************/
/*                          PARAMETROS GLOBALES                              */
/*****************************************************************************/
// Definicion de las notas al aire en la guitarra
unsigned char guit_default[N_STR] = {NOTA_E_LOW, NOTA_A, NOTA_D};
unsigned char guit_1_down[N_STR]  = {NOTA_A,     NOTA_D, NOTA_G};
unsigned char guit_2_down[N_STR]  = {NOTA_D,     NOTA_G, NOTA_B};
unsigned char guit_3_down[N_STR]  = {NOTA_G,     NOTA_B, NOTA_E_HIGH};
// Definicion de las notas al aire en el bajo
unsigned char bass_default[N_STR] = {BASS_E, BASS_A, BASS_D};
unsigned char bass_1_down[N_STR]  = {BASS_A, BASS_D, BASS_G};


unsigned short valorString[N_FRET+2] = {
    FRET0,FRET1,FRET2,FRET3,FRET4,FRET5,FRET6,
    FRET7,FRET8,FRET9,FRET10,FRET11,FRET12,FRET13,FRETFN};  // Valor de las cuerdas leidas  

bool            T_activeOld[] = {false, false, false}; // Variable que indica si esta activo cada sensor
bool            T_activeNew[] = {false, false, false}; // Variable que indica si esta activo cada sensor
short           T_value[]     = {0, 0, 0};             // Variable que indica si esta activo cada sensor
unsigned char   T_pins[]      = {T0, T1, T2};          // Definicion de los pines de cada sensor

unsigned short S_vals[N_STR];             // Valor actual de cada cuerda
unsigned char  S_pins[] = {S0, S1, S2};   // Definicion de los pines de cada cuerda

unsigned char  E_notaNueva[N_STR] = {0,0,0}; // Almacena el valor nuevo de la nota de cada cuerda
unsigned char  E_notaVieja[N_STR] = {0,0,0}; // Almacena el valor antiguo de la nota de cada cuerda

unsigned char fretTouched[N_STR] = {0,0,0};

unsigned char led_GUIT[] = {LED_EL,LED_A,LED_D, // Pines de salida para los leds que indican
                          LED_G,LED_B,LED_EH}; // que cuerdas estan activas
unsigned char guit_onof[] = {GUIT_R,GUIT_V,GUIT_A}; // Activada el modo guitarra
                                                   
unsigned char  cuerdas=EAD;    // cuerda en la que se encuentra, por defecto EAD

bool bass_guit=true;   // Cambia de modo de guitarra a bajo
bool fretless=true;     // Cambia de modo de funcionamiento de fret o fretless
bool ligado=true;    // Cambia de modo ligado de notas a modo no ligado
bool vel_Midi=true; // Cambia de modo de registrar la velocidad del midi
                   // ya sea por la fuerza del sensor o mediante un pot

void setup() 
{

  Serial.begin(115200);

  pinMode(SW_LIGADO, INPUT);
  pinMode(SW_BASS_GUIT, INPUT);
  pinMode(SW_VEL, INPUT);
  pinMode(SW_TRASTE, INPUT);
  pinMode(POT_STR,INPUT);
    
  for(int i=0; i< 6; i++)
    pinMode(led_GUIT[i],OUTPUT);
  for(int i=0; i< 2; i++)
    pinMode(guit_onof[i],OUTPUT);
  

  for(int i=0; i<N_STR; i++)
  {
    pinMode(T_pins[i], INPUT);
    pinMode(S_pins[i], INPUT);
  }
}

void loop() {

  readControls();
  determinaTraste();
 // imprimir();
  if(ligado) 
    ligadoNotas();
  rutinaEnvio();
  cortarNota();
  delay(10);
}

void imprimir()
{
    for (int i=0; i<N_STR; i++)
  {
    Serial.println("TRASTE "+String(i)+" = "+String(fretTouched[i]));
  }
   /* output each array element's value */

     
     // 
     // Serial.println("TRASTE "+String(i)+" = "+String(fretTouched[i]));
  // Serial.println("CUERDA "+String(i)+" = "+String(S_vals[i]));
     // Serial.println("NOTA "+String(i)+" = "+String(S_active[i]));
  //Serial.println("NOTA1 "+String(ligado1));
     // Serial.println("\n"); 
    Serial.println("LIGADO "+String(ligado));
  Serial.println("vel_Midi "+String(vel_Midi));
  Serial.println("fretless "+String(fretless));
  Serial.println("bass_guit "+String(bass_guit));
  Serial.println("\n"); 
}

/*****************************************************************************/
/*!
    \fn             void readControls()
    \param [in]     none
    \return         none
    \descripcion:   funcion que lee el valor de los sensores de entrada del 
                    dispositivo.

    \autor          David Garcia Diez
******************************************************************************/
void readControls(){
  ligado      = digitalRead(SW_LIGADO);
  vel_Midi    = digitalRead(SW_VEL);
  fretless    = digitalRead(SW_TRASTE);
  bass_guit   = digitalRead(SW_BASS_GUIT);
  ligado      = digitalRead(SW_LIGADO);

  cuerdas     = estadoCuerdas();
  for (int i=0; i<N_STR; i++)
  {
    T_activeNew[i] = comprobarPulsado(i,vel_Midi);
    S_vals[i]      = valorCuerda(i);
  }
  
}


/*****************************************************************************/
/*!
    \fn             bool comprobarPulsado(int i)
    \param [in]     int int   : indice del FSR activado
    \return         short actuvo : valor del sensor activado
    \descripcion:   funcion para comprobar que se ha presioniado un FSR

    \autor          David Garcia Diez
******************************************************************************/
bool comprobarPulsado(int i, bool velMidi)
{
  bool  activo=false;
  //short potVel=0;
  short valorFSR = analogRead(T_pins[i]);
  if(velMidi)
    T_value[i] = map (valorFSR, 0, 980, 60, 127);//mapeo los valores leido por el FSR
  else
  {
    // Aqui se simula la lectura del potenciometro
    //short potVel = analogRead(PIN ANALOGICO POT VELOCIDAD); // Por ahora enviamos un 127
    //T_value[i] = map (potVel, 0, 980, 60, 127);//mapeo los valores leido por el FSR
    T_value[i]=127;
  }

  if((valorFSR>15) && (valorFSR<950) || !fretless)
    activo=true;

  return activo;
}

/*****************************************************************************/
/*!
    \fn             unsigned short valorCuerda(int i)
    \param [in]     int int   : indice de la cuerda a leer
    \return         short media : valor leido en el pot despues de 10 lecturas
    \descripcion:   Media de 10 lecturas de una cuerda

    \autor          David Garcia Diez
******************************************************************************/
unsigned short valorCuerda(int i)
{
  unsigned short media=0;
  for(int j=0;j<10;j++)
  {
    media = media+analogRead(S_pins[i]);
  }
    media = media/10;

  return media;
}

/*****************************************************************************/
/*!
    \fn             unsigned char estadoCuerdas()
    \param [in]     none
    \return         char pos : configuracion de las cuerdas deseadas
    \descripcion:   Lee el valor del potenciometro el cual se encarga de cargar
                    una configuracion de cuerdas u otra

    \autor          David Garcia Diez
******************************************************************************/
unsigned char estadoCuerdas()
{
  unsigned short potCuerda=0; 
  unsigned char  pos=0;
  potCuerda = analogRead(POT_STR);
  pos = map (potCuerda, 0, 1023, 0, 3);
  // HAY QUE INCLUIR LA SECUENCIA PARA EL BAJO
  // Y TENER EL RGB PARA BAJO, GUITARRA Y FRETLESS
  if(fretless)
  {
    analogWrite(guit_onof[0], 255);  // Color VERDE
    analogWrite(guit_onof[1], 0);  
    analogWrite(guit_onof[2], 255);  

    switch (pos)
    {
      case EAD:
        digitalWrite(led_GUIT[0], HIGH);
        digitalWrite(led_GUIT[1], HIGH);
        digitalWrite(led_GUIT[2], HIGH);
        digitalWrite(led_GUIT[3], LOW);
        digitalWrite(led_GUIT[4], LOW);
        digitalWrite(led_GUIT[5], LOW);

      break;
      case ADG:
        digitalWrite(led_GUIT[0], LOW);
        digitalWrite(led_GUIT[1], HIGH);
        digitalWrite(led_GUIT[2], HIGH);
        digitalWrite(led_GUIT[3], HIGH);
        digitalWrite(led_GUIT[4], LOW);
        digitalWrite(led_GUIT[5], LOW);
      break;
      case DGB:
        digitalWrite(led_GUIT[0], LOW);
        digitalWrite(led_GUIT[1], LOW);
        digitalWrite(led_GUIT[2], HIGH);
        digitalWrite(led_GUIT[3], HIGH);
        digitalWrite(led_GUIT[4], HIGH);
        digitalWrite(led_GUIT[5], LOW);
      break;
      case GBE:
        digitalWrite(led_GUIT[0], LOW);
        digitalWrite(led_GUIT[1], LOW);
        digitalWrite(led_GUIT[2], LOW);
        digitalWrite(led_GUIT[3], HIGH);
        digitalWrite(led_GUIT[4], HIGH);
        digitalWrite(led_GUIT[5], HIGH);
      break;
    }
  }
  else
  {
    analogWrite(guit_onof[0], 0);  // Color ROJO
    analogWrite(guit_onof[1], 255);    
    analogWrite(guit_onof[2], 255);  

    digitalWrite(led_GUIT[0], LOW);
    digitalWrite(led_GUIT[1], LOW);
    digitalWrite(led_GUIT[2], LOW);
    digitalWrite(led_GUIT[3], LOW);
    digitalWrite(led_GUIT[4], LOW);
    digitalWrite(led_GUIT[5], LOW);

  }

  return pos;
}


/*****************************************************************************/
/*!
    \fn             void determinaTraste()
    \param [in]     none
    \return         none
    \descripcion:   modulo para determinar el valor del traste correspondiente
                    al que se lee de cada uno de los potenciometros.

    \autor          David Garcia Diez
******************************************************************************/
void determinaTraste() 
{
  short s_val = 0; 
  
  for(int i=0; i< N_STR; i++)
  {
    s_val = S_vals[i];

    if(s_val==FRET0)
    {
      fretTouched[i]=0;
    }
    else if (s_val>= FRET1)
    {
      fretTouched[i] = 1;
    }
    else
    {
      for (int j=1; j<=N_FRET; j++) 
      {
        if((s_val<valorString[j-1]) && (s_val>=valorString[j]))
        {
          fretTouched[i] = j;
          break;
        }
      }
    }
  }
}

/*****************************************************************************/
/*!
    \fn             ligadoNotas()
    \param [in]     none
    \return         none
    \descripcion:   modulo que comprueba que esta activado el FSR para hacer 
                    la llamada a la funcion de enviar nota

    \autor          David Garcia Diez
******************************************************************************/
void ligadoNotas(){
  char nota = 0;
  for(int i=0; i<N_STR; i++){
    if(E_notaNueva[i])
    {
      nota = determinaNota(i);
      if(nota != E_notaNueva[i] && (fretTouched[i] || T_activeNew[i]))
      {
        noteOn(nota, T_value[i]);
        //noteOn(nota, 127);
        noteOff(E_notaNueva[i]);
        E_notaNueva[i] = nota;
      }
    }
  }
}


/*****************************************************************************/
/*!
    \fn             rutinaEnvio()
    \param [in]     none
    \return         none
    \descripcion:   modulo que comprueba que esta activado el FSR para hacer 
                    la llamada a la funcion de enviar nota

    \autor          David Garcia Diez
******************************************************************************/
void rutinaEnvio()
{
  //TENGO QUE METER LA MEJORA DE PODER MEDIR LA INTENSIDAD DEL FSR
  for (int i=0; i<N_STR; i++)
  {
    //(T_activeNew[i]!=T_activeOld[i]) && 
    if(T_activeNew[i])
    {
      E_notaNueva[i] = determinaNota(i);
      if(E_notaNueva[i] != E_notaVieja[i])
      { 
        noteOn(E_notaNueva[i], T_value[i]);
        //noteOn(E_notaNueva[i], 127);
        E_notaVieja[i] = E_notaNueva[i]; 
      }   
    }
    else
      E_notaVieja[i]=0;
  }
}

/*****************************************************************************/
/*!
    \fn             unsigned char determinaNota(int i)
    \param [in]     int i: inidice sobre la nota que se desea seleccionar
    \return         none
    \descripcion:   modulo que determina en funcion del potenciometro de
                    seleccion de cuerdas, que nota se envia

    \autor          David Garcia Diez
******************************************************************************/
unsigned char determinaNota(int i)
{
  unsigned char nota=0;
  if(fretless)
  {
    if(bass_guit)
    {
      switch (cuerdas)
      {
        case EAD:
          nota = guit_default[i] + fretTouched[i];
        break;
        case ADG:
          nota = guit_1_down[i] + fretTouched[i];
        break;
        case DGB:
          nota = guit_2_down[i] + fretTouched[i];
        break;
        case GBE:
          nota = guit_3_down[i] + fretTouched[i];
        break;
      }
    }
    else
    {
      switch (cuerdas)
      {
        case EAD:
          nota = bass_default[i] + fretTouched[i];
        break;
        case ADG:
        case DGB:
        case GBE:
          nota = bass_1_down[i] + fretTouched[i];
        break;
      }
    }
  }
  else
  {
    if (!S_vals[i])
      nota=16;
    else
      nota = 123 - (S_vals[i]/10);
  }
  return nota;
}

/*****************************************************************************/
/*!
    \fn             cortarNota()
    \param [in]     none
    \return         none
    \descripcion:   modulo que se encarga de enviar una nota con velocidad 0

    \autor          David Garcia Diez
******************************************************************************/
void cortarNota(){
  for (int i=0; i<N_STR; i++){
    if(E_notaNueva[i] && !fretTouched[i] && !T_activeNew[i]){
        noteOff(E_notaNueva[i]);
        E_notaNueva[i] = 0;
    }
  }
}
/*****************************************************************************/
/*!
    \fn             void noteOn(int cmd, int pitch, int velocity)
    \param [in]     int cmd:      valor 
    \param [in]     int pitch:    valor de la nota midi equivalente
    \param [in]     int velocity: valor de la velocidad de la nota midi
    \return         none
    \descripcion:   funcion que envía por el canal serie la nota midi 
                    correspondiente a la nota tocada.

    \autor          David Garcia Diez
******************************************************************************/
void noteOn(int pitch, int velocity)
{
  Serial.write(byte(0x90));
  Serial.write(byte(pitch));
  Serial.write(byte(velocity));
}

/*****************************************************************************/
/*!
    \fn             void noteOff(int cmd, int pitch)
    \param [in]     int cmd: valor 
    \param [in]     int pitch: valor de la nota
    \return         none
    \descripcion:   funcion que envía por el canal serie el mandato de apagar
                    la nota, para ello envía la velocidad a 0.

    \autor          David Garcia Diez
******************************************************************************/
void noteOff(int pitch) 
{
  Serial.write(byte(0x90));
  Serial.write(byte(pitch));
  Serial.write(byte(0));
}

