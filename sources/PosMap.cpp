/* 
 * File:   PosMap.cpp
 * Author: b4n92uid
 * 
 * Created on 24 mai 2010, 19:44
 */

#include "PosMap.h"
#include "Exception.h"

#include <fstream>

#include <IL/il.h>

using namespace std;

using namespace tbe;
using namespace tbe::scene;

PosMap::PosMap()
{
}

PosMap::PosMap(std::string filename)
{
    Load(filename);
}

PosMap::~PosMap()
{
}

void PosMap::Load(std::string filename)
{
    cout << "Load posmap file : " << filename << endl;

    ilEnable(IL_ORIGIN_SET);
    ilOriginFunc(IL_ORIGIN_UPPER_LEFT);

    ilEnable(IL_FORMAT_SET);
    ilFormatFunc(IL_LUMINANCE);

    ilEnable(IL_TYPE_SET);
    ilTypeFunc(IL_UNSIGNED_BYTE);

    ILuint id = ilGenImage();

    ilBindImage(id);

    ILboolean error = !ilLoadImage(filename.c_str());

    if(error)
        throw Exception("PosMap::Load; Open file error (%s)", filename.c_str());

    m_size.x = ilGetInteger(IL_IMAGE_WIDTH);
    m_size.y = ilGetInteger(IL_IMAGE_HEIGHT);

    m_primiter = m_size;

    ILubyte* pixels = new ILubyte[m_size.x * m_size.y];

    ilCopyPixels(0, 0, 0, m_size.x, m_size.y, 1, IL_LUMINANCE, IL_UNSIGNED_BYTE, pixels);

    // Data

    for(int j = 0; j < m_size.y; j++)
        for(int i = 0; i < m_size.x; i++)
            if(pixels[j * m_size.x + i] == 0)
            {
                push_back(Vector3f(i, 0.0f, j) - Vector3f(m_size.x, 0.0f, m_size.y) / 2.0f);
            }

    delete[] pixels;
}

void PosMap::Process(Mesh::Array array, Mesh* ground)
{
    for(unsigned i = 0; i < array.size() && i < this->size(); i++)
    {
        Vector3f &pos = at(i);

        if(ground)
            pos = ground->GetMatrix() * ground->FindFloor(pos.x, pos.z);

        array[i]->SetPos(pos);
    }
}

Vector2i PosMap::GetSize() const
{
    return m_size;
}

void PosMap::SetPrimiter(Vector2f primiter)
{
    this->m_primiter = primiter;

    for(unsigned i = 0; i < this->size(); i++)
    {
        Vector3f &pos = at(i);

        pos = pos * Vector3f(m_primiter.x, 0.0f, m_primiter.y) / Vector3f(m_size.x, 0.0f, m_size.y);
    }
}

Vector2f PosMap::GetPrimiter() const
{
    return m_primiter;
}
