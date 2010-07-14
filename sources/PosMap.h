/* 
 * File:   PosMap.h
 * Author: b4n92uid
 *
 * Created on 24 mai 2010, 19:44
 */

#ifndef _POSMAP_H
#define	_POSMAP_H

#include <string>

#include "Mesh.h"
#include "Mathematics.h"

namespace tbe
{
namespace scene
{

/**
 * \brief Construit un tableau de position
 * depuis un fichier image sur 8-bit (noir et blanc)
 *
 * Les position sont projeté sur la zone défine par la taille
 * Vector2f m_primiter
 */
class PosMap : public Vector3f::Array
{
public:
    PosMap();
    PosMap(std::string filename);

    ~PosMap();

    void Load(std::string filename);

    void Process(Mesh::Array array, Mesh* ground = NULL);

    Vector2i GetSize() const;

    void SetPrimiter(Vector2f primiter);
    Vector2f GetPrimiter() const;

protected:
    Vector2f m_primiter;
    Vector2i m_size;
};

}
}

#endif	/* _POSMAP_H */
