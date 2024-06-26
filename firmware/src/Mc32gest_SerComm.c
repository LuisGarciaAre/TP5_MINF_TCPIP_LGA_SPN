// Mc32Gest_SerComm.C
// fonction d'�mission et de r�ception des message
// transmis en USB CDC
// Canevas TP4 SLO2 2015-2015


#include "appGen.h"
#include "Mc32gest_SerComm.h"
#include "DefMenuGen.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


/**********************************************************************************************
 * Nom de fonction: GetMessage
 * Auteur: LGA & JAR
 * Parametres:
 *  Entr�e: poiteur: int8_t USBReadBuffer, S_ParamGen pParam, bool SaveTodo
 *  Sortie: -
 * 
 * Description: Fonction de reception  d'un  message
 * Met � jour les param�tres du generateur a partir du message recu
 * Format du message
 *  !S=CF=2000A=10000O=+5000W=0#
 *  !S=CF=2000A=10000O=-5000D=100W=0#
 **********************************************************************************************/
bool GetMessage(int8_t *USBReadBuffer, S_ParamGen *pParam, bool *SaveTodo)
{
    uint8_t lenMsg = 0; // Variable de la longeur du message
    bool askSave = false;   // Variable de demande de sauvegarde
    // Variables pour les diff�rents index
    char* indexForme;
    char* indexFrequence;
    char* indexAmplitude;
    char* indexOffset;
    char* indexSave;

    bool stateMsg = false;  // Variable de l'�tat du message

    lenMsg = (uint8_t)strlen((char*)USBReadBuffer); // Sauvegarde la taille du message re�u

    // Test de la taille du message
    if (lenMsg > APP_GEN_TAILLE_MAX_STR)
    {
        stateMsg = false;
    }
    // V�rifie les d�buts et fin de trame
    else if(USBReadBuffer[0] == '!')
    {
        // R�cup�ration de index
        indexForme = strstr((char*)USBReadBuffer, "S");
        indexFrequence = strstr((char*)USBReadBuffer, "F");
        indexAmplitude = strstr((char*)USBReadBuffer, "A");
        indexOffset = strstr((char*)USBReadBuffer, "O");
        indexSave = strstr((char*)USBReadBuffer, "W");

        // R�cup�ration et sauvegarde des valeurs re�ues
        pParam->Forme = convertCharToForme(indexForme + 2);
        pParam->Frequence = atoi(indexFrequence + 2);
        pParam->Amplitude = atoi(indexAmplitude + 2);
        pParam->Offset = atoi(indexOffset + 2);
        askSave = atoi(indexSave + 2);
        
        // Traite la demande de sauvegarde
        if(askSave == true)
        {
            appgenData.flagAskToSave = true;
            *SaveTodo = true;
        }
        else
        {
            appgenData.flagAskToSave = false;
            *SaveTodo = false;
        }
        appgenData.updateParams = true;
        appgenData.flagUpdateAffichageLCD = true;
        stateMsg = true;
    }
    
    return stateMsg;
} // GetMessage


/**********************************************************************************************
 * Nom de fonction: SendMessage
 * Auteur: LGA & JAR
 * Parametres:
 *  Entr�e: poiteur: int8_t USBReadBuffer, S_ParamGen pParam, bool Saved
 *  Sortie: -
 * 
 * Description: Fonction d'envoi d'un  message
 * Rempli le tampon d'�mission pour USB en fonction des param�tres du g�n�rateur
 * Format du message
 *  !S=TF=2000A=10000O=+5000WP=0#
 *  !S=TF=2000A=10000O=-5000WP=1#
 **********************************************************************************************/
void SendMessage(int8_t *USBSendBuffer, S_ParamGen *pParam, bool Saved )
{ 
    // Ajout de lettre P en fonction de parametre "saved" pour aquiter message re�u
    sprintf((char*)USBSendBuffer, "!S=%sF=%dA=%dO=%dWP=%d#\n\r", ConvertFormeToChar(pParam->Forme), pParam->Frequence, pParam->Amplitude, pParam->Offset, Saved);
} // SendMessage

/**********************************************************************************************
 * Nom de fonction: convertCharToForme
 * Auteur: LGA & JAR
 * Parametres:
 *  Entr�e: pointeur: char valChar
 *  Sortie: E_FormesSignal
 * 
 * Description: Converti les lettres en formes
 **********************************************************************************************/
E_FormesSignal convertCharToForme(char* valChar)
{
    E_FormesSignal forme = ERROR;

    switch (*valChar)
    {
        case 'S':
        {
            forme = Sinus;
            break;
        }
        case 'T':
        {
            forme = Triangle;
            break;
        }
        
        case 'C':
        {
            forme = Carre;
            break;
        }

        case 'D':
        {
            forme = DentDeScie;
            break;
        }
        default:
        {
            forme = ERROR;
            break;
        }
    }

    return forme;   // Renvoi la forme correspondant � la lettre
}

/**********************************************************************************************
 * Nom de fonction: ConvertFormeToChar
 * Auteur: LGA & JAR
 * Parametres:
 *  Entr�e: E_FormesSignal forme
 *  Sortie: pointeur: char
 * 
 * Description: Converti les formes en caract�re
 **********************************************************************************************/
char* ConvertFormeToChar(E_FormesSignal forme)
{
    char* charConvert;

    switch (forme)
    {
    case Sinus:
    {
        charConvert = "S";
        break;
    }
    case Triangle:
    {
        charConvert = "T";
        break;
    }

    case Carre:
    {
        charConvert = "C";
        break;
    }

    case DentDeScie:
    {
        charConvert = "D";
        break;
    }
    default:
    {
        charConvert = "E";
        break;
    }
    }

    return charConvert; // Renvoi la lettre correspondant � la forme
}

