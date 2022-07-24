/**
******************************************************************************
* @file mylib_serialprot. c
* @author Reiter Roman
* @brief mylib-Serielles Protokoll.
* Diese Datei bietet Funktionen zur Verwaltung der folgenden
* Funktionalitäten eines seriellen Protokolls in Verbindung mit dem UART2:
* + IO-Betriebsfunktionen
* + Zustands- und Fehlerfunktionen
*
@verbatim
==============================================================================
###### Wie benutzt man diesen Treiber #####
==============================================================================
[. . ]
Der SERIALPROT MYLIB-Treiber kann wie folgt verwendet werden:

	(#) Einbinden des UART2
		(+) Damit das serielle Protokoll verwendet werden kann muss der UART2 aktiviert werden.
			(++) Der UART2 muss auf den Pins PA4 (TX) und PA15(RX) liegen
			(++) Für den UART2 werden die Standardeinstellungen der IDE-CubeMX verwendet (Baudrate, Wortlänge, Stop-Bit, Parität, Prescaler-Wert..)
			(++) Zusätzlich muss der globale Interrupt in der NVIC-Konfiguration des UART2 aktiviert werden.
			(++) Das Empfangen der Daten erfolgt mit HAL_UART_Receive_IT ()
			(++) Das Senden der Daten erfolgt mit HAL_UART_Transmit ()
			(++) Da der Datenampfang per Interrupt erfolgt, muss die Callback-Methode HAL_UART_TxCpltCallback () definiert werden.

	(#) Verwenden des seriellen Protokolls
		(+) Für das serielle Protokoll wird der UART2 benötigt.
			(++) Die Daten werden mit dem UART2 per Interrupt zeichenweise empfangen. Dafür muss ein Empfangspuffer deklariert und initialisiert werden.
				(+++) z.B.: uint8_t RxBuffer[RxBuffer_SIZE]={0};
			(++) Der Funktion HAL_UART_Receive_IT () muss der Empfangspuffer übergeben werden.
				(+++) z.B.: HAL_UART_Receive_IT(&huart2, RxBuffer, 1)
			(++) Nach einem Zeichenempfang wird die HAL_UART_RxCpltCallback () aufgerufen
				(+++) Um die Eingabe mit dem seriellen Protokoll zu verknüpfen muss ein exchangePuffer angelegt werden,
				 welcher die Antworten zu den getätigten Eingaben in RxBuffer enthält.
					(++++) z.B.: uint8_t exchangedMessage[50] ={0};
				(+++) Als Schnittstelle für die Eingabe (RxBuffer) und der Ausgabe (exchangedMessage) muss die Funktion
					MYLIB_SERIALPROT_XCHANGE() aufgerufen werden.
					(++++) z.B.: MYLIB_SERIALPROT_XCHANGE(&hserialprot,RxBuffer,exchangedMessage);
				(+++) Die erstellte Antwortnachricht muss nun über den UART2 hinausgeschrieben werden HAL_UART_Transmit()
					(++++) z.B.: HAL_UART_Transmit(&huart2, exchangedMessage,(uint16_t)strlen(exchangedMessage), 100)
				(+++) Abschließend muss der Interrupt für den UART2-Empfang wieder aktiviert werden HAL_UART_Receive_IT()
					(++++) z.B.: HAL_UART_Receive_IT(&huart2, RxBuffer, RxBuffer_SIZE)

	(#) Verwenden der Callback-Funktion SERIALPROT_Command_GPO_Callback()
	 	(+) Die Funktion dient dazu, um GPIO's ansteuern zu können.
	 		(++) Dazu wird die Callback-Funktion SERIALPROT_Command_GPO_Callback() in die main.c kopiert
	 		(++) Für die Abfragen bzw. das Festlegen der Kommandos können diese einfach in einer If-Schleige abgefragt werden.
	 			(+++) zur Vereinfachung der Abfrage wird das Makro __SERIALPROT_IS_COMMAND() zu verfügung gestellt,
	 		 		  wodurch die Eingabeparameter des letzten Kommandos abgefragt werden können
	 		 		  (++++) z.B.: __SERIALPROT_IS_COMMAND(hserialprot,"gpo","rt","off")
	 		 	(+++) Je nach Ergebnis muss beim erfüllen der Bedingung eine 0, anderfalls eine 1 zurückgegeben werden

@endverbatim
*/


/* Includes ------------------------------------------------------------------*/

#include "stdint.h"
#include "stddef.h"
#include "mylib_serialprot.h"
#include "stdlib.h"
#include "string.h"

/* Private define ------------------------------------------------------------*/

/** @defgroup SERIALPROT_Private_Constants
  * @{
  */
#define CollectionBuffer_SIZE 20
#define STM32_ACK "STM32-ACK -> "
#define STM32_NACK "STM32-NACK -> "
#define NEW_LINE "\n\r"
/**
  * @}
  */

/* Private variables ---------------------------------------------------------*/
/** @addtogroup SERIALPROT_Private_Variables
  * @{
  */

static uint8_t CollectionBuffer[CollectionBuffer_SIZE];
/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/
/** @addtogroup SERIALPROT_Private_Functions
  * @{
  */
static uint8_t is_number(char string[]);
static uint8_t is_alpha_string(char string[]);
static void cut_input(char * delimiter,char * string, char (*result)[CollectionBuffer_SIZE]);
static void random_number(uint16_t * number_min, uint16_t * number_max, uint16_t * result);
static uint8_t string_char_frequency(char * string, char * spanset);
static void add(uint16_t * number1, uint16_t * number2, uint16_t * result);
static void asc(uint8_t * sign, uint8_t * result);
static void wrong_message(uint8_t * TxBuffer);
static SERIALPROTCOL_StatusTypeDef SERIALPROT_CheckMessage(SERIALPROTOCOL_TypeDef *hserialprot);
static void SERIALPROT_CreateMessage_NUMBER_NUMBER(SERIALPROTOCOL_TypeDef *hserialprot,uint8_t * TxBuffer );
static void SERIALPROT_CreateMessage_TEXT_TEXT(SERIALPROTOCOL_TypeDef *hserialprot, uint8_t * TxBuffer );
static void SERIALPROT_CreateMessage_TEXT_NUMBER(SERIALPROTOCOL_TypeDef *hserialprot, uint8_t * TxBuffer );
static void SERIALPROT_CreateMessage_NUMBER_TEXT(SERIALPROTOCOL_TypeDef *hserialprot,uint8_t * TxBuffer );
static void SERIALPROT_COMMAND_RDM(SERIALPROTOCOL_TypeDef *hserialprot, uint8_t * TxBuffer );
static void SERIALPROT_COMMAND_GPO(SERIALPROTOCOL_TypeDef *hserialprot, uint8_t * TxBuffer );
static void SERIALPROT_COMMAND_ADD(SERIALPROTOCOL_TypeDef *hserialprot, uint8_t * TxBuffer );
static void SERIALPROT_COMMAND_ASC(SERIALPROTOCOL_TypeDef *hserialprot, uint8_t * TxBuffer );
/**
  * @}
  */

/**
  * @brief  Funktion 	überprüft ob ein String eine Zahl enthält
  * @param  string[] 	der zu überprüfende String
  * @retval result 		Ergebnis
  */
static uint8_t is_number(char string[])
{
    for (uint8_t i = 0; string[i]!= '\0'; i++)
    {
        if (isdigit(string[i]) == 0)
              return 0;
    }
    return 1;
}

/**
  * @brief  Funktion 	überprüft ob ein String ein aus Kleinbuchstaben besteht
  * @param  string[] 	der zu überprüfende String
  * @retval result 		Ergebnis
  */
static uint8_t is_alpha_string(char string[])
{
    for (uint8_t i = 0; string[i]!= '\0'; i++)
    {
        if (isalpha(string[i]) == 0)
              return 0;
    }
    return 1;
}

/**
  * @brief  Funktion 	generiert eine zufällige Zahl zwischen "number_min" und "number_max"
  * @param  number_min 	unterer Schwellwert des Zufallszahlenbereiches
  * @param  number_max 	oberer Schwellwert des Zufallszahlenbereiches
  * @param  result 		Ergebnis/Zufallszahl
  * @retval none
  */
static void random_number(uint16_t * number_min, uint16_t * number_max, uint16_t * result){
	srand( HAL_GetTick());
	uint16_t random_number = (rand () % ((atoi(number_max) + 1) - atoi(number_min))) + atoi(number_min);

    itoa(random_number,result,10);
}

/**
  * @brief  Funktion 	addiert die Zahlen "number1" und "number2"
  * @param  number1 	erste Zahl
  * @param  number2 	zweite Zahl
  * @param  result 		Ergebnis
  * @retval none
  */
static void add(uint16_t * number1, uint16_t * number2, uint16_t * result){
    itoa(atoi(number1)+atoi(number2),result,10);
}

/**
  * @brief  Funktion 	liefert von einem ASCII-Zeichen "sign" den Dezimalwert "result" laut ASCII-Tabelle
  * @param  sign 		Zeichen, von dem der ASCII-Wert berechnet werden soll
  * @param  result  	Zeichen, von dem der ASCII-Wert berechnet werden soll
  * @retval none
  */
static void asc(uint8_t * sign, uint8_t * result){
	uint8_t sig =  *sign;
	itoa(sig,result,10);
}

/**
  * @brief  Funktion zerteilt den Inputstring "string" bei jedem Zeichen "delimiter" und speichert diese in "result"
  * @param  string		der zu zerteilende String
  * @param  delimiter	das Zeichen, nachdem der String geteilt werden soll
  * @param  result		zerteilte Strings
  * @retval none
  */
static void cut_input(char * delimiter,char * string, char (*result)[CollectionBuffer_SIZE]){

    char *ptr;

    ptr = strtok(string, delimiter);
    {
    	uint8_t i =0;
        while(ptr != NULL)
        {
            strcpy(result[i++],ptr);
            ptr = strtok(NULL, delimiter);
        }
    }
}

/**
  * @brief  Funktion zählt die die vorkommenden Zeichen "spanset" eines Strings "string" und retourniert die Anzahl
  * @param  string 		String, wo die Zeichen gezählt werden sollen
  * @param  spanset 	Zeichen, welches gezählt werden soll
  * @retval count		Anzahl der Zeichen in dem String
  */
static uint8_t string_char_frequency(char * string, char * spanset){
    char *ptr = string;

    uint8_t count=0;
		for(uint8_t i=0; i<strlen(string);i++)
        {
            ptr = strpbrk(ptr, spanset);

            if(ptr++)
            	count++;
            else
                break;
        }

	return count;
}

/**
  * @brief  Funktion verarbeitet die einzel Eingegebenen Zeichen von RxBuffer und gibt dementsprechend die Antwort im TXBuffer zurück
  * @param  hserialprot SERIALPROT handle
  * @param  RxBuffer 	Ein-Zeichen-Empfangspuffer
  * @param  TxBuffer 	Sendepuffer/Antwortpuffer
  * @retval none
  */
void MYLIB_SERIALPROT_XCHANGE(SERIALPROTOCOL_TypeDef *hserialprot, uint8_t * RxBuffer, uint8_t * TxBuffer ){

	/* Überprüfen eingegebene Zeichen zwischen 32 und 127 oder Enter-Taste sind */
	if(RxBuffer[0]>=32 && RxBuffer[0]<=127 ||RxBuffer[0]=='\r' ){

		/* Wenn Puffergröße überschritten, dann OV ausgeben */
		if(strlen(CollectionBuffer)==CollectionBuffer_SIZE){
			memset(CollectionBuffer,0,strlen(CollectionBuffer));
			strcat(TxBuffer, " -> OV\n\r");
			strcat(TxBuffer, "Input> ");

		}else
		{
			/* Wenn Backspace-Taste gedrückt */
			if(RxBuffer[0]!='\177')
			{
				strcat(CollectionBuffer, RxBuffer);
				strcat(TxBuffer, RxBuffer);
			}
		}

		/* Wenn Backspace-Taste gedrückt */
		if(RxBuffer[0]=='\177')
		{
			/* Verhindern, dass "Input> " überschrieben wird */
			if(strlen(CollectionBuffer)>0){
				CollectionBuffer[strlen(CollectionBuffer)-1]=0;
				strcat(TxBuffer, "\177");
			}else{
				strcat(TxBuffer, "\32");
			}
	}

	/* Wenn Enter-Taste gedrückt */
	if(RxBuffer[0]=='\r')
	{
		/* Eingabe überprüfen ob Kommando-Syntax */
		if(SERIALPROT_CheckMessage(hserialprot) == SERIALPROT_OK )
		{
			/* Nachrichtenarten abragen */
			if(hserialprot->MessageKind == MESSAGEKIND_NUMBER_NUMBER){
				SERIALPROT_CreateMessage_NUMBER_NUMBER(hserialprot,TxBuffer);
			}else if(hserialprot->MessageKind == MESSAGEKIND_TEXT_TEXT){
				SERIALPROT_CreateMessage_TEXT_TEXT(hserialprot,TxBuffer);
			}else if(hserialprot->MessageKind == MESSAGEKIND_TEXT_NUMBER){
				SERIALPROT_CreateMessage_TEXT_NUMBER( hserialprot,TxBuffer);
			}else{
				SERIALPROT_CreateMessage_NUMBER_TEXT(hserialprot,TxBuffer);
			}

		}else if(!strcmp(CollectionBuffer,"\r"))
		{
			strcat(TxBuffer, "\n\r");
		}else{
			wrong_message(TxBuffer);
		}
			strcat(TxBuffer, "Input> ");
			memset(CollectionBuffer,0,strlen(CollectionBuffer));
		}
	}else{
		strcat(TxBuffer, "\32");
	}
}

/**
  * @brief  Funktion 	fügt dem TxBuffer "falsche Nachricht" hinzu
  * @param  TxBuffer 	Sendepuffer/Antwortpuffer
  * @retval none
  */
static void wrong_message(uint8_t * TxBuffer){

	strcat(TxBuffer, "\n\r");
	strcat(TxBuffer, "STM32-NACK -> ");
	strcat(TxBuffer, CollectionBuffer);
	strcat(TxBuffer, "\n\r");
}

/**
  * @brief  Funktion 	überprüft ob das eingegebene Kommando für "MESSAGEKIND_NUMBER_NUMBER" definiert ist
  * @param  hserialprot SERIALPROT handle
  * @param  TxBuffer 	Sendepuffer/Antwortpuffer
  * @retval none
  */
static void SERIALPROT_CreateMessage_NUMBER_NUMBER(SERIALPROTOCOL_TypeDef *hserialprot,uint8_t * TxBuffer ){

	/* Überprüfen ob Kommando "rdm" */
	if(__SERIALPROT_IS_COMMANDNAME(hserialprot,"rdm")){
		SERIALPROT_COMMAND_RDM(hserialprot,TxBuffer);
	/* Überprüfen ob Kommando "add" */
	}else if(__SERIALPROT_IS_COMMANDNAME(hserialprot,"add")){
			SERIALPROT_COMMAND_ADD(hserialprot,TxBuffer);
	/* Überprüfen ob Kommando "asc" */
	}else if(__SERIALPROT_IS_COMMANDNAME(hserialprot,"asc")){
			SERIALPROT_COMMAND_ASC(hserialprot,TxBuffer);
	}else{
		wrong_message(TxBuffer);
	}
}

/**
  * @brief  Funktion 	überprüft ob das eingegebene Kommando für "MESSAGEKIND_TEXT_TEXT" definiert ist
  * @param  hserialprot SERIALPROT handle
  * @param  TxBuffer 	Sendepuffer/Antwortpuffer
  * @retval none
  */
static void SERIALPROT_CreateMessage_TEXT_TEXT( SERIALPROTOCOL_TypeDef *hserialprot,uint8_t * TxBuffer ){
	/* Überprüfen ob Kommando "gpo" */
	if(__SERIALPROT_IS_COMMANDNAME(hserialprot,"gpo")){
		SERIALPROT_COMMAND_GPO(hserialprot,TxBuffer);
	}else{
		wrong_message(TxBuffer);
	}
}

/**
  * @brief  Funktion 	überprüft ob das eingegebene Kommando für "MESSAGEKIND_TEXT_NUMBER" definiert ist
  * @param  hserialprot SERIALPROT handle
  * @param  TxBuffer 	Sendepuffer/Antwortpuffer
  * @retval none
  */
static void SERIALPROT_CreateMessage_TEXT_NUMBER(SERIALPROTOCOL_TypeDef *hserialprot,uint8_t * TxBuffer ){
	/* Überprüfen ob Kommando "asc" */
	if(__SERIALPROT_IS_COMMANDNAME(hserialprot,"asc")){
			SERIALPROT_COMMAND_ASC(hserialprot,TxBuffer);
	}else{
		wrong_message(TxBuffer);
	}
}

/**
  * @brief  Funktion 	überprüft ob das eingegebene Kommando für "MESSAGEKIND_NUMBER_TEXT" definiert ist
  * @param  hserialprot SERIALPROT handle
  * @param  TxBuffer 	Sendepuffer/Antwortpuffer
  * @retval none
  */
static void SERIALPROT_CreateMessage_NUMBER_TEXT(SERIALPROTOCOL_TypeDef *hserialprot,uint8_t * TxBuffer ){

	/* kein Kommando definiert */
	if(0){

	}else{
		wrong_message(TxBuffer);
	}
}

/**
  * @brief  Funktion 	wertet den Rückgabewert der "SERIALPROT_Command_GPO_Callback" aus und erzeugt dementsprechend die Antwort im TxBuffer
  * @param  hserialprot SERIALPROT handle
  * @param  TxBuffer 	Sendepuffer/Antwortpuffer
  * @retval none
  */
static void SERIALPROT_COMMAND_GPO(SERIALPROTOCOL_TypeDef *hserialprot, uint8_t * TxBuffer ){
	/* Erzeugen der Ausgangsnachricht falls Rückgabewerde der Callback 0 ist */
	if (!SERIALPROT_Command_GPO_Callback(hserialprot)){
		strcat(TxBuffer, NEW_LINE);
		strcat(TxBuffer, STM32_ACK);
		strcat(TxBuffer, CollectionBuffer);
		strcat(TxBuffer, NEW_LINE);
	}else{
		wrong_message(TxBuffer);
	}
}

/**
  * @brief  Funktion 	Callback, welche in die main.c kopiert werden kann um die Eingabe des "gpo" Befehls abzufragen
  * @param  hserialprot SERIALPROT handle
  * @retval query
  */
__weak uint8_t SERIALPROT_Command_GPO_Callback(SERIALPROTOCOL_TypeDef *hserialprot)
{
	/* Prevent unused argument(s) compilation warning */
	UNUSED(hserialprot);

	/* NOTE : This function should not be modified, when the callback is needed,
            	the SERIALPROT_Command_GPO_Callback could be implemented in the user file
	 */
}

/**
  * @brief  Funktion 	Überprüft die Eingabeparameter 1 und 2 des Kommandos für die Zuffalszahl und erzeugt dementsprechend die Antwort im TxBuffer
  * @param  hserialprot SERIALPROT handle
  * @param  TxBuffer 	Sendepuffer/Antwortpuffer
  * @retval none
  */
static void SERIALPROT_COMMAND_RDM(SERIALPROTOCOL_TypeDef *hserialprot, uint8_t * TxBuffer ){

	/* Überprüfen ob Parameter1 kleiner als Parameter2 und erzeugen der Ausgangsnachricht */
	if(atoi(hserialprot->Parameter1)<atoi(hserialprot->Parameter2)){
		strcat(TxBuffer, NEW_LINE);
		strcat(TxBuffer, STM32_ACK);
		strcat(TxBuffer, CollectionBuffer);
		TxBuffer[strlen(TxBuffer)-1]=0;
		strcat(TxBuffer," => " );
		uint16_t result[20]={0};
		random_number(hserialprot->Parameter1, hserialprot->Parameter2, result);
		strcat(TxBuffer, "#a,");
		strcat(TxBuffer, result);
		strcat(TxBuffer, NEW_LINE);
	}else{
		wrong_message(TxBuffer);
	}
}

/**
  * @brief  Funktion 	Überprüft die Eingabeparameter 1 und 2 des Kommandos für die Addition und erzeugt dementsprechend die Antwort im TxBuffer
  * @param  hserialprot SERIALPROT handle
  * @param  TxBuffer 	Sendepuffer/Antwortpuffer
  * @retval none
  */
static void SERIALPROT_COMMAND_ADD(SERIALPROTOCOL_TypeDef *hserialprot, uint8_t * TxBuffer ){

	/* Erzeugen der Ausgangsnachricht */
	strcat(TxBuffer, NEW_LINE);
	strcat(TxBuffer, STM32_ACK);
	strcat(TxBuffer, CollectionBuffer);
	TxBuffer[strlen(TxBuffer)-1]=0;
	strcat(TxBuffer," => " );
	uint16_t result[20]={0};
	add(hserialprot->Parameter1, hserialprot->Parameter2, result);
	strcat(TxBuffer, "#a,");
	strcat(TxBuffer, result);
	strcat(TxBuffer, NEW_LINE);
}

/**
  * @brief  Funktion 	Überprüft die Eingabeparameter 1 und 2 des Kommandos für die Ermittlung des Dezimalwertes des ASCII-Zeichens und erzeugt dementsprechend die Antwort im TxBuffer
  * @param  hserialprot SERIALPROT handle
  * @param  TxBuffer 	Sendepuffer/Antwortpuffer
  * @retval none
  */
static void SERIALPROT_COMMAND_ASC(SERIALPROTOCOL_TypeDef *hserialprot, uint8_t * TxBuffer ){

	/* Überprüfen ob Parameter1 ein Zeichen enthält und Parameter2 null ist und erzeugen der Ausgangsnachricht */
	if(strlen(hserialprot->Parameter1) ==1 && atoi(hserialprot->Parameter2)==0){
		strcat(TxBuffer, NEW_LINE);
		strcat(TxBuffer, STM32_ACK);
		strcat(TxBuffer, CollectionBuffer);
		TxBuffer[strlen(TxBuffer)-1]=0;
		strcat(TxBuffer," => " );
		uint8_t result[20]={0};
		asc(hserialprot->Parameter1, result);
		strcat(TxBuffer, "#a,");
		strcat(TxBuffer,result);
		strcat(TxBuffer, NEW_LINE);
	}else{
		wrong_message(TxBuffer);
	}
}

/**
  * @brief  Funktion 	Überprüft vorab ob die Eingabe eine "Kommando-Syntax" ist
  * @param  hserialprot SERIALPROT handle
  * @retval none
  */
static SERIALPROTCOL_StatusTypeDef SERIALPROT_CheckMessage(SERIALPROTOCOL_TypeDef *hserialprot)
{

	uint8_t inputmessage[40] = {0};
	strcat(inputmessage, CollectionBuffer);

	uint8_t  pos_command_start = strcspn( inputmessage, "#" );
	uint8_t  pos_command_split = strcspn( inputmessage, "," );
	uint8_t  pos_command_end = strcspn( inputmessage, "\r" );

	if(pos_command_start==0 && pos_command_split==4  && pos_command_end == strlen(inputmessage)-1  && string_char_frequency(inputmessage,",") ==1 && string_char_frequency(inputmessage,":") ==1 ){

		inputmessage[strlen(inputmessage)-1]=0;

		char command_name[10];
		char command_param1[10];
		char command_param2[10];

		char *ptr;
		char *ptr2;
		char result1[3][CollectionBuffer_SIZE]={0};
		char result2[3][CollectionBuffer_SIZE]={0};

		cut_input(",",inputmessage,result1);
		strcpy(command_name,result1[0]+1);
		cut_input(":",result1[1],result2);

		strcpy(command_param1, result2[0]);
		strcpy(command_param2, result2[1]);

		uint8_t sd = strlen(command_name);
		uint8_t sdd = strlen(command_param1);
		uint8_t sdf = strlen(command_param2);
		if(strlen(command_name)<=3 && strlen(command_param1)<=4 && strlen(command_param2)<=4){

			strcpy(hserialprot->CommandName, command_name);
			strcpy(hserialprot->Parameter1, command_param1);
			strcpy(hserialprot->Parameter2, command_param2);

			if(is_alpha_string(command_name) && is_number(command_param1) && is_number(command_param2)){
				hserialprot->MessageKind = MESSAGEKIND_NUMBER_NUMBER;
				return SERIALPROT_OK;
			}else if(is_alpha_string(command_name) && is_number(command_param2)){
				hserialprot->MessageKind = MESSAGEKIND_TEXT_NUMBER;
				return SERIALPROT_OK;
			}else if(is_alpha_string(command_name) && is_number(command_param1) && is_alpha_string(command_param2)){
				hserialprot->MessageKind = MESSAGEKIND_NUMBER_TEXT;
				return SERIALPROT_OK;
			}else if(is_alpha_string(command_name) && is_alpha_string(command_param1) && is_alpha_string(command_param2)){
				hserialprot->MessageKind = MESSAGEKIND_TEXT_TEXT;
				return SERIALPROT_OK;
			}else{
				return SERIALPROT_ERROR;
			}
		}
		else
		{
			return SERIALPROT_ERROR;
		}

  	  }else{
  		  return SERIALPROT_ERROR;
  	  }
}

//#endif /* MYLIB_SERIALPROT */
