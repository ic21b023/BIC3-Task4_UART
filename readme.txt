

					Für das serielle Protokoll wurde eine Bibliothek erstellt (MyLibrary/Inc/mylib_serialprot.h)
					Deteils zur genauen Verwendung und Beschreibung der Bibliothek sind in der MyLibrary/Inc/mylib_serialprot.c Datei
					Folgend ist die Nutzung für Eingabe und Ausgaben mit Putty beschrieben
	
					
  ==============================================================================
                          ##### Syntax - Start #####
  ==============================================================================					
					
Nach dem Upload auf den STM32 und den anschließenden start von Putty muss als erstet die Enter-Taste gedrückt werden, wo anschließend
"Input> " erscheint. Anschließend kann das Kommando eingegeben werden.				


  ==============================================================================
                          ##### Syntax - Eingabe #####
  ==============================================================================

##### Kommandosyntax #####

#befehlsname,Parameter1:Parameter2\r

Der Befehlname besteht aus 3 Buchstaben
Die Parameter können je nach Befehl maximal 4 Stellen (Text oder Zahl) besitzen
Die Bestätigung/Absenden des Befehls erfolgt mit der Enter-Taste(\r)

##### Beschreibung #####

SoF(Zeichen für den Befehlsbeginn)	 						#
CMD(Befehle bestehen aus 3 Buchstaben)						rdm
SEP(Trennzeichen zwischen Befehl und Parameter) 			,
PARAMETER1 (Zahl oder Text)									bl
SEP(Trennzeichen zwischen den 2 Parametern) 				:
PARAMETER2 (Zahl oder Text)									on
EoF(Zeichen für das Begehlsende) 							\r    (drücken der Enter-Taste)

##### Mögliche Befehle #####

*-- Zufallszahl von Zahlenbereich --*						#rdm,zahlmin:zahlmax\r						#rdm,0:10\r
Befehlname=rdm	
Parameter1=Zahlenbereich von 
Parameter2=Zahlenbereich bis 


*-- GIO's Ein/Ausschalten --*
Befehlname=gpo	
Parameter1=definition lt. Callback (Userspez.)
Parameter2=definition lt. Callback (Userspez.)

blaue LED EINschalten										#gpo,bl:on\r								#gpo,bl:on\r
blaue LED AUSschalten										#gpo,bl:off\r								#gpo,bl:off\r						
rote LED EINschalten										#gpo,rt:on\r 								#gpo,rt:on\r
rote LED AUSschalten										#gpo,rt:off\r 								#gpo,rt:off\r
gruene LED EINschalten										#gpo,gn:on\r 								#gpo,gn:on\r
gruene LED AUSschalten										#gpo,gn:off\r 								#gpo,gn:off\r
			

*-- Zwei pos. Zahlen addieren --*
Befehlname=add	
Parameter1=zahl1
Parameter2=zahl2											#add,zahl1:zahl2\r							#add,2:3\r


*-- Ein Dezimalwert eines ASCII-Zeichens lt. Tab. ermitteln	
Befehlname=asc	
Parameter1=ascii-zeichen
Parameter2=0 (immer)										#asc,ascii-zeichen:0\r						#asc,a:0\r


*-- Overflow --*
Sollten mehr als 20 Zeichen eingegeben worden sein,
so ist eine Neueingabe erforderlich, da dies kein
Befehlt mehr sein kann.
Es wird folgendes ausgegeben								-> OV						


  ==============================================================================
                          ##### Syntax - Ausgabe #####
  ==============================================================================

*-- FALSCHEINGABEN --*
Existiert der Befehl nicht oder der Befehl 
wurde falsch Eingegeben wird folgendes Ausgegeben:			STM32-NACK -> "letzte Eingabe"

*-- KORREKTE EINGABEN --*
Werden bei Korrektheit bestätigt durch: 					STM32-ACK -> "letzte Eingabe"

*-- ERWARTUNG EINES ERGEBISSES --*
Wird ein Befehl eingegeben welcher zur Berechnungen 
dient erfolgt zusätzlich das Ergebnis: 						STM32-ACK -> "letzte Eingabe" => #a,ERGEBNIS\r



