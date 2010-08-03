/* 
 * File:   Clock.h
 * Author: b4n92uid
 *
 * Created on 26 novembre 2009, 22:39
 */

#ifndef _CLOCK_H
#define	_CLOCK_H

#include <string>

namespace tbe
{
namespace ticks
{

/**
 * \brief Gestion du temps
 * Inclus des fonctions statique pour le formatage des dates
 * et de controle du processus courant
 */
class Clock
{
public:
    Clock();
    virtual ~Clock();

    /// Renvois 'true' si 'time' est écouler
    bool IsEsplanedTime(long time);

    /// Renvois le temp depuis le dernier appelle de la fonction
    long GetEsplanedTime();

    /// Enregistre le temp courant pour le comparer a l'appele de la fonction IsEsplanedTime
    void SnapShoot();

    /// Fonction qui suspend le processus courant (valeur en millisecond)
    static void Sleep(unsigned v);

private:
    clock_t m_lastTime;

};

}
}

#endif	/* _CLOCK_H */

