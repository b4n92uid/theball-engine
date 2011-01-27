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

    void LoadFromFile(std::string filepath);

private:
    void Format(std::string str);
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
    void Load(std::string font, int size);

    void operator()(std::string font, int size)
    {
        Load(font, size);
    }

    /// Libération mémoire, appler par le déstructeur
    void Delete();

    /// Rendue
    void Display(Vector2f pos, GuiString text);

    /// Renvois la position centrer de text pa rapport a pos et pos+size
    Vector2f CenterOf(GuiString text, Vector2f pos, Vector2f size) const;

    /// Renvois la position centrer de str pa rapport a pos et pos+size
    Vector2f CenterOf(std::string str, Vector2f pos, Vector2f size) const;

    /// Renvois la taille occuper par text avec la police en cour
    Vector2f SizeOf(GuiString text) const;

    /// Renvois la taille occuper par str avec la police en cour
    Vector2f SizeOf(std::string str) const;

    /**
     * Renvoi un la partie de la cahine str qui
     * peut etre contenue dans une largeur rensegner par width
     * avec la police courante
     *
     * @param str Chaine a analyser
     * @param width Largeur a ne pas dépasser
     * @param takeEnd A true la fonction renvois la fin de la chaine
     * @return Résultat
     */
    std::string WrapeLine(std::string str, float width, bool takeEnd = true) const;

    /**
     * Renvoi un tableau de chaine qui est le résultat des retour
     * a la ligne de la chaine qui ne dépasse pas la largeur rensegner par width
     *
     * @param str
     * @param width
     * @return
     */
    std::vector<std::string> GetWrapedLines(std::string str, float width) const;

    /// Renvoi true si le chargement de la police a réussi
    operator bool();
    bool operator!();
    bool IsReady();

    /// Spécifier la taille de la police
    void SetFontSize(int fontsize);
    int GetFontSize() const;

    /// Spécifier la taille d'une tabulation
    void SetTabSize(int tabSize);
    int GetTabSize() const;

    /// Spécifier la taille de l'espace entre les lignes
    void SetLineSpace(int lineSpace);
    int GetLineSpace() const;

    /// Spécifier la couleur d'écriture
    void SetColor(Vector4f color);
    Vector4f GetColor() const;

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
