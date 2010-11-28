/* 
 * File:   Ball3DMesh.h
 * Author: b4n92uid
 *
 * Created on 25 novembre 2010, 17:45
 */

#ifndef BALL3DMESH_H
#define	BALL3DMESH_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "Mesh.h"

namespace tbe
{
namespace scene
{

class Ball3DMesh : public Mesh
{
public:
    Ball3DMesh();
    Ball3DMesh(std::string filepath);
    Ball3DMesh(const Ball3DMesh& copy);
    ~Ball3DMesh();

    Ball3DMesh & operator=(const Ball3DMesh& copy);

    void Open(std::string filepath);

    std::string GetFilePath();

private:
    void ReadMaterial(std::ifstream& file, Material* mat);
    void ReadVertexs(std::ifstream& file);

private:
    std::string m_filepath;
};

}
}

#endif	/* BALL3DMESH_H */
