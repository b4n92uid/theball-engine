/* 
 * File:   GrassLand.h
 * Author: b4n92uid
 *
 * Created on 28 mai 2010, 11:27
 */

#ifndef _GRASSLAND_H
#define	_GRASSLAND_H

#include <Tbe.h>

namespace tbe
{
namespace scene
{

class GrassLand : public Mesh
{
public:

    GrassLand(Mesh* ground)
    {
        PosMap grassPosMap("../../medias/obj/grassposmap.pbm");
        grassPosMap.SetPrimiter(256);

        std::cout << "GrassLand::GrassLand; Generating grass..." << std::endl;

        const Vector3f size(8.0);
        const unsigned totalCount = grassPosMap.size();

        for(unsigned i = 0; i < totalCount; i++)
        {
            Vector3f offset = ground->GetMatrix() * ground->FindFloor(grassPosMap[i].x, grassPosMap[i].z);

            Vector3f diriction(1, 0, 0);

            Vector3f randShift = tools::rand(Vector3f(-2, 0, 2), Vector3f(2, 0, -2));

            Face faces[6];

            for(unsigned j = 0; j < 6; j += 2)
            {
                diriction.Rotate(60.0f * (j / 2), 0);

                Vertex verts[6];

                verts[0] = Vertex(-diriction.x, 1, -diriction.z, 0, 1, 0, 1, 1, 1, 1, 1, 0.0);
                verts[1] = Vertex(-diriction.x, 0, -diriction.z, 0, 1, 0, 1, 1, 1, 1, 1, 1.0);
                verts[2] = Vertex(diriction.x, 0, diriction.z, 0, 1, 0, 1, 1, 1, 1, 0, 1.0);

                verts[3] = Vertex(diriction.x, 0, diriction.z, 0, 1, 0, 1, 1, 1, 1, 0, 1.0);
                verts[4] = Vertex(diriction.x, 1, diriction.z, 0, 1, 0, 1, 1, 1, 1, 0, 0.0);
                verts[5] = Vertex(-diriction.x, 1, -diriction.z, 0, 1, 0, 1, 1, 1, 1, 1, 0.0);


                for(unsigned t = 0; t < 3; t++)
                {
                    verts[t].pos *= size;
                    verts[t].pos += offset + randShift;
                    faces[j].push_back(verts[t]);

                    verts[t + 3].pos *= size;
                    verts[t + 3].pos += offset + randShift;
                    faces[j + 1].push_back(Vertex(verts[t + 3]));
                }

                m_hardwareBuffer.AddFace(faces[j]);
                m_hardwareBuffer.AddFace(faces[j + 1]);
            }
        }

        Texture grassTexel("../../medias/textures/gras_02.png", true);
        grassTexel.SetFiltring(Texture::NEAREST);

        Material* mainMaterial = new Material;
        mainMaterial->SetTexture(grassTexel);
        mainMaterial->SetAmbient(1);
        mainMaterial->SetDiffuse(1);
        mainMaterial->Enable(Material::TEXTURE | Material::ALPHA | Material::LIGHT | Material::COLOR);
        mainMaterial->SetAlphaThershold(0.75);

        AddMaterial("main", mainMaterial);
        ApplyMaterial(mainMaterial, 0, totalCount * 6 * 3);

        m_hardwareBuffer.Compile();

        ComputeAabb();
    }
};

}
}

#endif	/* _GRASSLAND_H */

