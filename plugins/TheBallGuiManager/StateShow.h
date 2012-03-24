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

    void setCurState(int curState);
    int getCurState() const;

protected:
    void objectRender();

    int m_curState;
};

}
}

#endif	/* _STATESHOW_H */

