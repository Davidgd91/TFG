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
#include <EEPROM.h>
#include <Wire.h>


/*****************************************************************************/
/*                               DEFINES                                     */
/*****************************************************************************/
#define S0        A0
#define S1        A1
#define S2        A2

#define T0        A3
#define T1        A4
#define T2        A5

#define N_STR     3
#define N_FRET    13
#define N_KEYS    13

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


/*****************************************************************************/
/*                         PROTOTIPO DE FUNCIONES                            */
/*****************************************************************************/
void imprimir();
void readControls();
unsigned short valorCuerda(int i);
bool comprobarPulsado(int i);
void determinaTraste ();
void ligadoNotas();
void rutinaEnvio();
void noteOn(int pitch, int velocity);
void noteOff(int pitch);
void cortarNota();


/*****************************************************************************/
/*                          PARAMETROS GLOBALES                              */
/*****************************************************************************/
unsigned char fretDefs[N_STR][N_FRET+1] = {
    {40,41,42,43,44,45,46,47,48,49,50,51,52,53}, // Cuerda E
    {45,46,47,48,49,50,51,52,53,54,55,56,57,58}, // Cuerda A
    {50,51,52,53,54,55,56,57,58,59,60,61,62,63}, // Cuerda D
};  /* Defincion de las notas midi asignada a cada traste
       3 cuerdas y 16 trastes, siendo la tercera cuerda E
       la segunda A y la primera D */

unsigned char notas_default[N_STR]  = {40, 45, 50};

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
bool primera=true;

void setup() 
{

  Serial.begin(115200);

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
  ligadoNotas();
  rutinaEnvio();
  cortarNota();
  delay(5);
}

void imprimir()
{
    for (int i=0; i<N_STR; i++)
  {
    Serial.println("TRASTE "+String(i)+" = "+String(fretTouched[i]));
  }
   /* output each array element's value */

     
     // Serial.println("CUERDA "+String(i)+" = "+String(S_vals[i]));
     // Serial.println("TRASTE "+String(i)+" = "+String(fretTouched[i]));
     // Serial.println("NOTA "+String(i)+" = "+String(S_active[i]));
     // Serial.println("\n"); 
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
  for (int i=0; i<N_STR; i++)
  {
    T_activeOld[i] = T_activeNew[i];
    T_activeNew[i] = comprobarPulsado(i);
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
bool comprobarPulsado(int i)
{
  bool  activo=false;
  short valorFSR = analogRead(T_pins[i]);
  T_value[i] = map (valorFSR, 0, 980, 60, 127);//mapeo los valores
  if((valorFSR>15) && (valorFSR<950))
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
      nota = notas_default[i] + fretTouched[i];
      /* hay dos formas de ver este ligado, si hay un || se puede generar
         la nota con que solo se active una vez el touch y si tenemos un 
         && hay que mantener pulsado */
      if(nota != E_notaNueva[i] && (fretTouched[i] || T_activeNew[i]))
      {
        noteOn(nota, 100);
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
      E_notaNueva[i] = notas_default[i] + fretTouched[i];
      if(E_notaNueva[i] != E_notaVieja[i])
      { 
        //noteOn(E_notaNueva[i], T_value[i]);
        noteOn(E_notaNueva[i], 127);
        E_notaVieja[i] = E_notaNueva[i]; 
      }   
    }
    else
      E_notaVieja[i]=0;
  }
}


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


