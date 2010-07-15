/* 
 * File:   StateShow.h
 * Author: Administrateur
 *
 * Created on 11 septembre 2009, 21:40
 */

#ifndef _STATESHOW_H
#define	_STATESHOW_H

#include "Control.h"
#include "Skin.h"

namespace tbe
{
namespace gui
{

/**
 * \brief Afficheur d'état
 */
class StateShow : public Control
{
public:
    StateShow();
    ~StateShow();

    void SetSkin(const GuiSkin& skin);

    bool OnEvent(const EventManager& event);

    void SetCurState(unsigned curState);
    unsigned GetCurState() const;

    void SetStateCount(unsigned stateCount);
    unsigned GetStateCount() const;

protected:
    void ObjectRender();

    unsigned m_curState;
    unsigned m_stateCount;
};

}
}

#endif	/* _STATESHOW_H */

