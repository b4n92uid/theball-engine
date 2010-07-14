/* 
 * File:   Palm.h
 * Author: b4n92uid
 *
 * Created on 28 mai 2010, 11:28
 */

#ifndef _PALM_H
#define	_PALM_H

#include <Tbe.h>

namespace tbe
{
namespace scene
{

class Palm : public OBJMesh
{
public:

    Palm()
    {
        Open("../../medias/obj/palm.obj");

        Material* Core_palm = GetMaterial("Core_palm");
        Core_palm->Enable(Material::BACKFACE_CULL);
        
        Material* Header_palm = GetMaterial("Header_palm");
        Header_palm->Enable(Material::ALPHA);
        Header_palm->SetAlphaThershold(0.5);
    }

};

}
}

#endif	/* _PALM_H */

