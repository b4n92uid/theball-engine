/*
 * File:   AABB.h
 * Author: b4n92uid
 *
 * Created on 24 novembre 2009, 18:53
 */

#ifndef _AABB_H
#define	_AABB_H

#include "Mathematics.h"
#include "HardwareBuffer.h"

namespace tbe
{

namespace scene
{
class Node;
}

/// \brief Axis Aligned Bounding Box

class AABB
{
public:
    AABB();
    AABB(float minmax);
    AABB(Vector3f min, Vector3f max);
    virtual ~AABB();

    /**
     * Prend en compte le point définie par le vecteur pos, si le point
     *  est a l'interieur du AABB rien n'est fait, sinon le AABB est agrandie
     *  pour le prendre en compte
     */
    AABB& count(const Vector3f& pos);

    /**
     * Prend en compte le AABB définie par 'aabb'
     * @see count(const Vector3f& pos)
     */
    AABB& count(const AABB& aabb);

    /**
     * Prend en compte le AABB absolue du noeud 'node'
     * @see count(const Vector3f& pos)
     */
    AABB& count(scene::Node* node);

    /// Assigne les dimmension du AABB
    AABB & operator()(const Vector3f& min, const Vector3f& max);

    /**
     */
    AABB & add(float pos);

    /**
     * Etire la valeur min et max du AABB par pos
     * AABB(-2,2).add(3) == AABB(-5,5)
     */
    AABB & add(const Vector3f& pos);

    /**
     * Etire la valeur min et max du AABB par les valeur min et max de 'aabb'
     * @see add(const Vector3f& pos)
     */
    AABB & add(const AABB& aabb);

    /**
     * Rétrécie la valeur min et max du AABB par pos
     * AABB(-5,5).sub(3) == AABB(-2,2)
     */
    AABB & sub(const Vector3f& pos);

    /**
     * Rétrécie la valeur min et max du AABB par les valeur min et max de 'aabb'
     * @see sub(const Vector3f& pos)
     */
    AABB & sub(const AABB& aabb);

    /// @see add(float pos)
    AABB & operator+=(float pos);

    /// @see add(const Vector3f& pos)
    AABB & operator+=(const Vector3f& pos);

    /// @see add(const AABB& pos)
    AABB & operator+=(const AABB& aabb);

    /// @see sub(const Vector3f& pos)
    AABB & operator-=(const Vector3f& pos);

    /// @see sub(const AABB& pos)
    AABB & operator-=(const AABB& aabb);

    /// @see add(const Vector3f& pos)
    AABB operator+(const Vector3f& value);

    /// @see add(const AABB& pos)
    AABB operator+(const AABB& aabb);

    /// @see sub(const Vector3f& pos)
    AABB operator-(const Vector3f& value);

    /// @see sub(const AABB& pos)
    AABB operator-(const AABB& aabb);

    /// Renvois 'true' si le noeud 'node' se trouve a l'interieur du AABB
    bool isInner(scene::Node* node) const;

    /// Renvois 'true' si la boite englobant 'aabb' se trouve a l'interieur du AABB
    bool isInner(const AABB& aabb) const;

    /// Renvois 'true' si le boite point 'pos' se trouve a l'interieur du AABB
    bool isInner(const Vector3f& point) const;

    /// Réinitialise les valeurs de la boite englobant a 0
    void clear();

    /// Renvois la longeur de la boite englobant (max - min).getMagnitude()
    float getLength() const;

    /// Renvois la taille de la boite englobant (max - min).getMagnitude()
    Vector3f getSize() const;

    /// Renvois le centre de la boite englobant
    Vector3f getCenter() const;

    /// Renvois une position aléatoire a l'interieur de la boite englobant
    Vector3f randPos() const;

    // Renvois les 8 points qui compose la boite englobant
    Vector3f::Array getPoints() const;

    /// Déplace la boite englobant
    AABB& translate(Vector3f pos);

    Vector3f min;
    Vector3f max;
};

}

#endif	/* _AABB_H */

