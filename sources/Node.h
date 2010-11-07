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
#include "ParallelScene.h"

namespace tbe
{
namespace scene
{

class ParallelScene;

/**
 * \brief Class de base pour la repr�sentation d'une entit�
 * qui inclus une matrice, un AABB et un nom
 */
class Node
{
public:
    Node();
    Node(const Node& copy);
    virtual ~Node();

    bool operator=(const Node& copy);

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
    void MulMatrix(const Matrix4f& matrix);

    Matrix4f GetAbsoluteMatrix();
    Matrix4f& GetMatrix();

    /// Sp�cifier la scene parallel parent
    void SetParallelScene(ParallelScene* parallelScene);
    ParallelScene* GetParallelScene() const;

    /// Racourcie pour s�pcifier la postion de la matrice du noeud
    void SetPos(Vector3f pos);
    Vector3f GetPos() const;

    bool IsTopLevel();

    void SetParent(Node* parent);
    Node* GetParent();

    void AddChild(Node* child);

    Node* GetChild(unsigned index);

    void ReleaseChild(Node* child);
    Node* ReleaseChild(unsigned index);

    virtual void Process() = 0;
    virtual void Render() = 0;

    virtual Node* Clone() = 0;

    typedef std::map<std::string, Node*> Map;
    typedef std::vector<Node*> Array;

protected:
    ParallelScene* m_parallelScene;
    std::string m_name;
    Matrix4f m_matrix;
    bool m_enable;
    bool m_enableProcess;
    AABB m_aabb;

    Node* m_parent;
    Node::Array m_childs;
};

}
}

#endif // _NODE_H
