/*
 * File:   Pencil.h
 * Author: b4n92uid
 *
 * Created on 11 mai 2010, 19:12
 */

#ifndef _PENCIL_H
#define	_PENCIL_H

#include <string>
#include <vector>
#include <algorithm>

#include "Mathematics.h"
#include "Exception.h"

namespace tbe
{
namespace gui
{

/**
 * \brief Gestion du texte multi-ligne
 */
class GuiString : public std::vector<std::string>
{
public:
    GuiString();
    GuiString(std::string str, ...);
    GuiString(const char* str, ...);

    void loadFromFile(std::string filepath);

private:
    void format(std::string str);
};

/**
 * \brief Gestion du texte dans l'interface graphique
 * contine la couleur, taille et police du texte
 */
class Pencil
{
public:
    Pencil();
    Pencil(std::string path, int size);
    ~Pencil();

    /// Charge une police .ttf
    void load(std::string font, int size);

    void operator()(std::string font, int size)
    {
        load(font, size);
    }

    /// Lib�ration m�moire, appeller par le d�structeur
    void remove();

    /// Rendue
    void display(Vector2f pos, GuiString text);

    /// Renvois la position centrer de text pa rapport a pos et pos+size
    Vector2f centerOf(GuiString text, Vector2f pos, Vector2f size) const;

    /// Renvois la position centrer de str pa rapport a pos et pos+size
    Vector2f centerOf(std::string str, Vector2f pos, Vector2f size) const;

    /// Renvois la taille occuper par text avec la police en cour
    Vector2f sizeOf(GuiString text) const;

    /// Renvois la taille occuper par str avec la police en cour
    Vector2f sizeOf(std::string str) const;

    /**
     * Renvoi un la partie de la cahine str qui
     * peut etre contenue dans une largeur rensegner par width
     * avec la police courante
     *
     * @param str Chaine a analyser
     * @param width Largeur a ne pas d�passer
     * @param takeEnd A true la fonction renvois la fin de la chaine
     * @return R�sultat
     */
    std::string wrapeLine(std::string str, float width, bool takeEnd = true) const;

    /**
     * Renvoi un tableau de chaine qui est le r�sultat des retour
     * a la ligne de la chaine qui ne d�passe pas la largeur rensegner par width
     *
     * @param str
     * @param width
     * @return
     */
    std::vector<std::string> getWrapedLines(std::string str, float width) const;

    /// Renvoi true si le chargement de la police a r�ussi
    operator bool();
    bool operator!();
    bool isReady();

    /// Sp�cifier la taille de la police
    void setFontSize(int fontsize);
    int getFontSize() const;

    /// Sp�cifier la taille d'une tabulation
    void setTabSize(int tabSize);
    int getTabSize() const;

    /// Sp�cifier la taille de l'espace entre les lignes
    void setLineSpace(int lineSpace);
    int getLineSpace() const;

    /// Sp�cifier la couleur d'�criture
    void setColor(Vector4f color);
    Vector4f getColor() const;

protected:
    std::vector<unsigned > m_charsWidth;

    std::string m_fontPath;
    int m_fontSize;
    int m_tabSize;
    int m_lineSpace;

    GLuint m_charList;

    Vector4f m_color;
};

}
}

#endif	/* _PENCIL_H */
