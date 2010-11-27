"""
Name: 'Ball 3d mesh file'
Blender: 249
Group: 'Export'
Tooltip: 'Ball 3d mesh exporter'
"""

import BPyMesh
import Blender
import bpy
import datetime
import os
import shutil

# {name: material, }
gmaterials = {}

# {material_name: list(vertexs), }
gvertexs = {}

gvertexCount = 0
gmaterialCount = 0

blendfilename = Blender.Get('filename')

class Vertex:

    def __init__(self):
        self.x = 0
        self.y = 0
        self.z = 0
        self.nx = 0
        self.ny = 0
        self.nz = 0
        self.r = 1
        self.g = 1
        self.b = 1
        self.a = 1
        self.u = 0
        self.v = 0

def prepare(file):

    for mat in Blender.Material.Get():

        global gmaterialCount

        gmaterials[mat.getName()] = mat
        gmaterialCount += 1

    scene = bpy.data.scenes.active

    for ob in scene.objects:

        if ob.getType() != 'Mesh':
            continue

        omesh = ob.getData(mesh=True).copy()

        tmp_ob = scene.objects.new(omesh)

        mesh = tmp_ob.getData(mesh=True)

        mesh.sel = True
        mesh.quadToTriangle(0)
        mesh.transform(ob.matrix)
        
        BPyMesh.meshCalcNormals(mesh)
        
        for face in mesh.faces:

            matname = mesh.materials[face.mat].getName()

            for i, vertex in enumerate(face.verts):

                ver = Vertex()

                ver.x = vertex.co.x
                ver.y = vertex.co.z
                ver.z = vertex.co.y
                
                if face.smooth:
                    ver.nx = vertex.no.x
                    ver.ny = vertex.no.z
                    ver.nz = vertex.no.y
                else:
                    ver.nx = face.no.x
                    ver.ny = face.no.z
                    ver.nz = face.no.y

                if mesh.vertexColors:
                    ver.r = face.col[i].r / 255.0
                    ver.g = face.col[i].g / 255.0
                    ver.b = face.col[i].b / 255.0
                    ver.a = face.col[i].a / 255.0

                if mesh.faceUV:
                    ver.u = face.uv[i].x
                    ver.v = face.uv[i].y

                if not gvertexs.has_key(matname):
                    gvertexs[matname] = []

                gvertexs[matname].append(ver)

                global gvertexCount

                gvertexCount += 1

        scene.objects.unlink(tmp_ob)

def write(file):

    # Write header -------------------------------------------------------------

    file.write('.ball3d.\n')
    file.write('# Export from blender\n')
    file.write('# Date : %s\n' % datetime.date.today().isoformat())
    file.write('# Vertex count : %d\n' % gvertexCount)
    file.write('# Material count : %d\n' % gmaterialCount)
    file.write('\n')

    # Write material datas -----------------------------------------------------

    ambValue = Blender.World.GetCurrent().getAmb()

    for matname, mat in gmaterials.items():

        textures = mat.getTextures()

        file.write("material %s\n" % matname)

        file.write("alpha %f\n" % mat.getAlpha())

        # ambiant
        file.write("ambient %f %f %f 1.0\n" % (mat.amb, mat.amb, mat.amb))

        # diffuse
        file.write("diffuse %f %f %f 1.0\n" % (mat.R, mat.G, mat.B))

        # specular
        file.write("specular %f %f %f 1.0\n" % (mat.specCol[0], mat.specCol[1], mat.specCol[2]))
        file.write("shininess %f\n" % (mat.getHardness() * 127 / 511))

        for i, mtex in enumerate(textures):

            if mtex:

                filename = mtex.tex.image.getFilename()
                basename = os.path.basename(filename)

                file.write("tex %d %s\n" % (i, basename))

                if filename[:2] != '//' and os.path.isabs(filename):
                    src = filename
                else:
                    src = os.path.join(os.path.dirname(blendfilename), basename)

                dst = os.path.dirname(file.name)

                try:
                    shutil.copy(src, dst)

                except:
                    pass

        file.write("\n")

    # Write vertex datas -------------------------------------------------------

    for id, vertexs in gvertexs.items():

        file.write("build %s\n" % id)

        for v in vertexs:
            datalist = (v.x, v.y, v.z, v.nx, v.ny, v.nz, \
                        v.r, v.g, v.b, v.a, v.u, v.v)

            file.write("%f %f %f %f %f %f %f %f %f %f %f %f\n" % datalist)

        file.write("\n")


def export(filename):

    file = open(filename, "w+")

    prepare(file)

    write(file)

    file.close()

output = os.path.basename(blendfilename).split('.')[0] + ".ball3d"

Blender.Window.FileSelector(export, "Export theBall 3D Mesh", output)
