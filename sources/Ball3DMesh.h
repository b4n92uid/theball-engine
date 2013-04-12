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

class MeshParallelScene;

class Ball3DMesh : public Mesh
{
public:
    Ball3DMesh(MeshParallelScene* scene);
    Ball3DMesh(MeshParallelScene* scene, const std::string& filepath);
    Ball3DMesh(const Ball3DMesh& copy);
    ~Ball3DMesh();

    Ball3DMesh & operator=(const Ball3DMesh& copy);

    void open(std::string filepath);

    Ball3DMesh* clone();

    std::string getFilename();

    rtree serialize(std::string root);

    std::vector<std::string> getUsedRessources();

private:
    void readMaterial(std::ifstream& file, Material* mat);
    void readVertexs(std::ifstream& file);

private:
    std::string m_filename;
};

}
}

#endif	/* BALL3DMESH_H */

