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

    void setSkin(const GuiSkin& skin);

    bool onEvent(const EventManager& event);

    void setCurState(unsigned curState);
    unsigned getCurState() const;

    void setStateCount(unsigned stateCount);
    unsigned getStateCount() const;

protected:
    void objectRender();

    unsigned m_curState;
    unsigned m_stateCount;
};

}
}

#endif	/* _STATESHOW_H */

