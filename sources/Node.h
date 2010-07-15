#ifndef _NODE_H
#define _NODE_H

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "Mathematics.h"
#include "AABB.h"
#include "Texture.h"
#include "Exception.h"

namespace tbe
{
namespace scene
{

class ParallelScene;

/**
 * \brief Class de base pour la représentation d'une entité
 * qui inclus une matrice, un AABB et un nom
 */
class Node
{
public:
    Node();
    virtual ~Node();

    /// Operateur de comparaison par nom (m_name)
    bool operator==(Node* node);
    bool operator==(std::string name);

    /// Renvois le AABB du noeud
    AABB GetAabb() const;
    AABB GetAbsolutAabb() const;

    /// Specifier si l'objet est activer
    void SetEnable(bool enbale);
    bool IsEnable() const;

    void SetEnableProcess(bool enableProcess);
    bool IsEnableProcess() const;

    /// Identifiant du noeud
    void SetName(std::string name);
    std::string GetName() const;

    /// Matrice du noeud
    void SetMatrix(const Matrix4f& matrix);
    Matrix4f& GetMatrix();

    /// Spécifier la scene parallel parent
    void SetParallelScene(ParallelScene* parallelScene);
    ParallelScene* GetParallelScene() const;

    /// Racourcie pour sépcifier la postion de la matrice du noeud
    void SetPos(Vector3f pos);
    Vector3f GetPos() const;

    typedef std::map<std::string, Node*> Map;
    typedef std::vector<Node*> Array;

protected:
    virtual void Process();

protected:
    ParallelScene* m_parallelScene;
    std::string m_name;
    Matrix4f m_matrix;
    bool m_enable;
    bool m_enableProcess;
    AABB m_aabb;
};

}
}

#endif // _NODE_H
