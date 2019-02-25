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
#define S0        A15
#define S1        A14
#define S2        A13

#define T0        A10
#define T1        A11
#define T2        A12

#define POT_STR   A1
#define POT_VEL   A2
#define POT_WAH   A6
#define LED_EL    50 
#define LED_A     48
#define LED_D     46
#define LED_G     44
#define LED_B     42
#define LED_EH    40

#define GUIT_R    4
#define GUIT_V    3
#define GUIT_A    2

#define SW_ONOFFWAH   32
#define SW_VEL        30
#define SW_FRETLESS   28
#define SW_BASS_GUIT  26

#define N_STR     3
#define N_FRET    13
#define N_KEYS    13
#define N_LEDS    6
#define N_RGB     2

#define VMAXMIDI  127
#define MAXADC    1024

#define EAD       0
#define ADG       1
#define DGB       2
#define GBE       3

#define FRET0     0
#define O_FRET1   550
#define O_FRET2   390
#define O_FRET3   298
#define O_FRET4   238
#define O_FRET5   200
#define O_FRET6   167
#define O_FRET7   145
#define O_FRET8   125
#define O_FRET9   107
#define O_FRET10  90
#define O_FRET11  72
#define O_FRET12  50
#define O_FRET13  7
#define FRETFN    5

#define N_FRET1   830
#define N_FRET2   700
#define N_FRET3   595
#define N_FRET4   510
#define N_FRET5   447
#define N_FRET6   380
#define N_FRET7   330
#define N_FRET8   280
#define N_FRET9   232
#define N_FRET10  184
#define N_FRET11  136
#define N_FRET12  85
#define N_FRET13  10

#define NOTA_E_LOW  40
#define NOTA_A      45
#define NOTA_D      50
#define NOTA_G      55
#define NOTA_B      59
#define NOTA_E_HIGH 64

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


unsigned short n_valorString[N_FRET+2] = {
    FRET0,N_FRET1,N_FRET2,N_FRET3,N_FRET4,N_FRET5,N_FRET6,N_FRET7,
    N_FRET8,N_FRET9,N_FRET10,N_FRET11,N_FRET12,N_FRET13,FRETFN};  // Valor de las cuerdas leidas  

unsigned short O_valorString[N_FRET+2] = {
    FRET0,O_FRET1,O_FRET2,O_FRET3,O_FRET4,O_FRET5,O_FRET6,O_FRET7,
    O_FRET8,O_FRET9,O_FRET10,O_FRET11,O_FRET12,O_FRET13,FRETFN};  // Valor de las cuerdas leidas  


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
bool trastes=true;     // Cambia de modo de funcionamiento de fret o fretless
bool wah_activo=true;    // Cambia cambia al modo activacion del Wah
bool vel_Midi=true; // Cambia de modo de registrar la velocidad del midi
                   // ya sea por la fuerza del sensor o mediante un pot
bool wah_activoP=true;

void setup() 
{

  Serial.begin(115200);

  pinMode(SW_ONOFFWAH, INPUT);
  pinMode(SW_BASS_GUIT, INPUT);
  pinMode(SW_VEL, INPUT);
  pinMode(SW_FRETLESS, INPUT);
  pinMode(POT_STR,INPUT);
  pinMode(POT_VEL,INPUT);
  pinMode(POT_WAH,INPUT);
    
  for(int i=0; i< N_LEDS; i++)
    pinMode(led_GUIT[i],OUTPUT);
  for(int i=0; i< N_RGB; i++)
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
  //imprimir();
  readWah();
  rutinaEnvio();
  cortarNota();
  delay(20);
}

void imprimir()
{

/*
  for (int i=0; i<N_STR; i++)
  {
    Serial.println("CUERDA "+String(i)+" = "+String(S_vals[i]));
    Serial.println("TRASTE "+String(i)+" = "+String(fretTouched[i]));
  }*/

     // Serial.println("TRASTE "+String(i)+" = "+String(fretTouched[i]));
  // Serial.println("CUERDA "+String(i)+" = "+String(S_vals[i]));
     // Serial.println("NOTA "+String(i)+" = "+String(S_active[i]));
    /*
  Serial.println("vel_Midi "+String(vel_Midi));
  Serial.println("trastes "+String(trastes));
  Serial.println("bass_guit "+String(bass_guit));
    Serial.println("POSICION "+String(pos));
    Serial.println("VELOCIDAD "+String(posVel));*/
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
  wah_activo  = digitalRead(SW_ONOFFWAH);
  vel_Midi    = digitalRead(SW_VEL);
  trastes     = digitalRead(SW_FRETLESS);
  bass_guit   = digitalRead(SW_BASS_GUIT);

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
  unsigned short potVel=0; 
  unsigned short valorFSR = analogRead(T_pins[i]);
    
  if (!velMidi)
  {
    potVel = mediaMedidas(POT_VEL);
    T_value[i] = map (potVel, 0, MAXADC, 60, VMAXMIDI);
  }
  else
    T_value[i] = 120;  
  

  if(((valorFSR>70) && (valorFSR<MAXADC)) || !trastes)
    activo=true;

 return activo;
}

/*****************************************************************************/
/*!
    \fn             unsigned short valorCuerda(int i)
    \param [in]     int int   : indice de la cuerda a leer
    \return         short media : valor leido en el softpot
    \descripcion:   Medida de una cuerda
    \autor          David Garcia Diez
******************************************************************************/
unsigned short valorCuerda(int i)
{
  unsigned short medida=0;
  medida = mediaMedidas(S_pins[i]);
  return medida;
}


/*****************************************************************************/
/*!
    \fn             void readWah()
    \param [in]     none
    \return         none
    \descripcion:   Medida de la palanca
    \autor          David Garcia Diez
******************************************************************************/
void readWah()
{
  unsigned short medida=0;
  unsigned short valor=0;
  medida = mediaMedidas(POT_WAH);
  valor = map (medida, 600, MAXADC , 0, VMAXMIDI);
  if(wah_activo)
  {
    if(valor>15)
      controllerChange(valor);
  }
  if(!wah_activo && wah_activoP==1)
    controllerChange(60);
  wah_activoP=wah_activo;
}

/*****************************************************************************/
/*!
    \fn             unsigned short mediaMedidas(int puerto)
    \param [in]     int : puerto analogico del que leer
    \return         short media : valor leido despues de 10 lecturas
    \descripcion:   Media de 10 medidas
    \autor          David Garcia Diez
******************************************************************************/
unsigned short mediaMedidas(int puerto)
{
  unsigned short media=0;
  for(int j=0;j<10;j++)
  {
    media = media+analogRead(puerto);
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
                    una configuracion de cuerdas u otra. Cambia la iluminacion
                    de los leds azules en funcion de la distribucion de cuerdas
                    e indica con el rgb el modo de uso.
    \autor          David Garcia Diez
******************************************************************************/
unsigned char estadoCuerdas()
{
  unsigned short potCuerda=0; 
  unsigned char  pos=0;

  potCuerda = mediaMedidas(POT_STR);
  pos = map (potCuerda, 0, 1018, 0, 3);

  if(trastes)
  {
    if(bass_guit)
    {
      analogWrite(guit_onof[0], 255);  // Color VERDE GUITAR MODE
      analogWrite(guit_onof[1], 0);  // este tiene que estar abajo
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
      analogWrite(guit_onof[0], 0);  // Color AMARILLO BASS MODE
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
        case DGB:
        case GBE:
          digitalWrite(led_GUIT[0], LOW);
          digitalWrite(led_GUIT[1], HIGH);
          digitalWrite(led_GUIT[2], HIGH);
          digitalWrite(led_GUIT[3], HIGH);
          digitalWrite(led_GUIT[4], LOW);
          digitalWrite(led_GUIT[5], LOW);
        break;
      }
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
                    al que se lee de cada uno de los softpot.
    \autor          David Garcia Diez
******************************************************************************/
void determinaTraste() 
{
  unsigned short s_val = 0; 
  
  for(int i=0; i< N_STR; i++)
  {
    s_val = S_vals[i];
    if(i==2)
    {
      if(s_val==FRET0)
      {
        fretTouched[i]=0;
      }
      else if (s_val>= O_FRET1)
      {
        fretTouched[i] = 1;
      }
      else
      {
        for (int j=1; j<=N_FRET; j++) 
        {
          if((s_val<O_valorString[j-1]) && (s_val>=O_valorString[j]))
          {
            fretTouched[i] = j;
            break;
          }
        }
      }
    }

    else
    {
      if(s_val==FRET0)
      {
        fretTouched[i]=0;
      }
      else if (s_val>= N_FRET1)
      {
        fretTouched[i] = 1;
      }
      else
      {
        for (int j=1; j<=N_FRET; j++) 
        {
          if((s_val<n_valorString[j-1]) && (s_val>=n_valorString[j]))
          {
            fretTouched[i] = j;
            break;
          }
        }
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
  for (int i=0; i<N_STR; i++)
  {
    if(T_activeNew[i])
    {
      E_notaNueva[i] = determinaNota(i);
        if(E_notaNueva[i] != E_notaVieja[i])
        { 
          noteOff(E_notaVieja[i]);
          noteOn(E_notaNueva[i], T_value[i]);
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

  if(trastes)
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
    \fn             void noteOn(int pitch, int velocity)
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
    \fn             void noteOff(int pitch)
    \param [in]     int pitch: valor de la nota
    \return         none
    \descripcion:   funcion que envía por el canal serie el mandato de apagar
                    la nota, para ello envía la velocidad a 0.
    \autor          David Garcia Diez
******************************************************************************/
void noteOff(int pitch) 
{
  Serial.write(byte(0x80));
  Serial.write(byte(pitch));
  Serial.write(byte(0));
}

/*****************************************************************************/
/*!
    \fn             void noteOff(int value)
    \param [in]     int value: valor de la palanca enviado al controlador midi
    \return         none
    \descripcion:   funcion que que envia por el canal serie el controlChange
                    correspondiente asignado a la palanca del dispositivo.
    \autor          David Garcia Diez
******************************************************************************/
void controllerChange(int value) 
{
  Serial.write(byte(0xB1));
  Serial.write(byte(5));
  Serial.write(byte(value));
}
