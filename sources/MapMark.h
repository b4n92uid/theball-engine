/*
 * File:   MapMark.h
 * Author: b4n92uid
 *
 * Created on 9 juin 2011, 13:59
 */

#ifndef MAPMARK_H
#define	MAPMARK_H

#include "Node.h"

namespace tbe
{
namespace scene
{

class MapMark : public Node
{
public:
    MapMark();
    MapMark(const MapMark& orig);
    virtual ~MapMark();

    MapMark& operator=(const MapMark& orig);

    MapMark* clone();

    void process();
    void render();

    void setColor(Vector3f color);
    Vector3f getColor() const;

    void setSize(float size);
    float getSize() const;

    enum Type
    {
        Sphere, Box
    };

    void setType(Type type);
    Type getType() const;

    Node::CtorMap constructionMap(std::string root);

    typedef std::vector<MapMark*> Array;

private:
    Type m_type;
    float m_size;
    Vector3f m_color;

private:
    void copy(const MapMark& m);
};

}
}

#endif	/* MAPMARK_H */
