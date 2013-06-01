/*
 * File:   Material.cpp
 * Author: b4n92uid
 *
 * Created on 16 novembre 2009, 16:56
 */

#include "Material.h"

using namespace tbe;
using namespace scene;

Material::Material()
{
    m_ambient = 1;
    m_diffuse = 1;
    m_specular = 0.5;
    m_renderFlags = COLORED | TEXTURED | LIGHTED | FOGED;
    m_drawPass = 1;
    m_shininess = 16;
    m_lineWidth = 0;
    m_alphaThershold = 0;
    m_frameSortWait = 0;
    m_faceType = TRIANGLES;
    m_polygoneMode = GL_FILL;
    m_depthTest = true;
    m_depthWrite = true;
    m_color = 1;
    m_clockCycle = 0;
}

Material::Material(const Material& copy)
{
    *this = copy;
}

Material::~Material() { }

Material& Material::operator=(const Material& copy)
{
    m_name = copy.m_name;

    m_ambient = copy.m_ambient;
    m_diffuse = copy.m_diffuse;
    m_specular = copy.m_specular;
    m_shininess = copy.m_shininess;

    m_textures = copy.m_textures;
    m_shader = copy.m_shader;

    m_faceType = copy.m_faceType;
    m_polygoneMode = copy.m_polygoneMode;

    m_renderFlags = copy.m_renderFlags;
    m_drawPass = copy.m_drawPass;

    m_color = copy.m_color;

    m_clock = copy.m_clock;
    m_clockCycle = copy.m_clockCycle;

    m_frameSortWait = copy.m_frameSortWait;

    m_lineWidth = copy.m_lineWidth;

    m_alphaThershold = copy.m_alphaThershold;

    m_depthTest = copy.m_depthTest;
    m_depthWrite = copy.m_depthWrite;

    m_texApply = copy.m_texApply;

    return *this;
}

void Material::setFrameSortWait(unsigned frameSortWait)
{
    this->m_frameSortWait = frameSortWait;
}

unsigned Material::getFrameSortWait() const
{
    return m_frameSortWait;
}

bool Material::isTransparent()
{
    return (m_renderFlags & ADDITIVE)
            || (m_renderFlags & MULTIPLY)
            || (m_renderFlags & MODULATE)
            || (m_renderFlags & ALPHA);
}

void Material::setAlphaThershold(float alphaThershold)
{
    this->m_alphaThershold = alphaThershold;
}

float Material::getAlphaThershold() const
{
    return m_alphaThershold;
}

void Material::setRenderFlags(unsigned renderFlags)
{
    this->m_renderFlags = renderFlags;
}

unsigned Material::getRenderFlags() const
{
    return m_renderFlags;
}

void Material::setShader(const Shader& shader)
{
    m_shader = shader;
}

Shader Material::getShader() const
{
    return m_shader;
}

void Material::setLineWidth(float lineWidth)
{
    this->m_lineWidth = lineWidth;
}

float Material::getLineWidth() const
{
    return m_lineWidth;
}

void Material::setName(std::string name)
{
    this->m_name = name;
}

std::string Material::getName() const
{
    return m_name;
}

void Material::setTexture(Texture texture, unsigned index)
{
    m_textures[index] = texture;

    if(!m_texApply.count(index))
    {
        TextureApply apply = {Material::MODULATE, false, 128, 0, 0};
        m_texApply[index] = apply;
    }
}

Texture Material::getTexture(unsigned index) const
{
    return m_textures.find(index)->second;
}

unsigned Material::getTexturesCount() const
{
    return m_textures.size();
}

void Material::dropTexture(unsigned index)
{
    m_textures.erase(index);
}

void Material::dropAllTexture()
{
    m_textures.clear();
}

void Material::setShininess(float shininess)
{
    m_shininess = shininess;
}

float Material::getShininess() const
{
    return m_shininess;
}

void Material::setSpecular(Vector4f specular)
{
    m_specular = specular;
}

Vector4f Material::getSpecular() const
{
    return m_specular;
}

void Material::setDiffuse(Vector4f diffuse)
{
    m_diffuse = diffuse;
}

Vector4f Material::getDiffuse() const
{
    return m_diffuse;
}

void Material::setAmbient(Vector4f ambient)
{
    m_ambient = ambient;
}

Vector4f Material::getAmbient() const
{
    return m_ambient;
}

bool Material::isEnable(unsigned flag)
{
    return (m_renderFlags & flag);
}

void Material::enable(unsigned flag)
{
    m_renderFlags |= flag;
}

void Material::disable(unsigned flag)
{
    m_renderFlags &= ~flag;
}

void Material::setFaceType(FaceType faceType)
{
    this->m_faceType = faceType;
}

Material::FaceType Material::getFaceType() const
{
    return m_faceType;
}

void Material::setPolygoneMode(GLenum polygoneMode)
{
    this->m_polygoneMode = polygoneMode;
}

GLenum Material::getPolygoneMode() const
{
    return m_polygoneMode;
}

void Material::setDepthWrite(bool depthWrite)
{
    this->m_depthWrite = depthWrite;
}

bool Material::isDepthWrite() const
{
    return m_depthWrite;
}

void Material::setDepthTest(bool depthTest)
{
    this->m_depthTest = depthTest;
}

bool Material::isDepthTest() const
{
    return m_depthTest;
}

void Material::setTextureBlend(unsigned type, unsigned index)
{
    m_texApply[index].blend = type;
}

unsigned Material::getTextureBlend(unsigned index)
{
    return m_texApply[index].blend;
}

void Material::setTextureFrameSize(Vector2i size, unsigned index)
{
    m_texApply[index].frameSize = size;
}

Vector2i Material::getTextureFrameSize(unsigned index)
{
    return m_texApply[index].frameSize;
}

void Material::setTexturePart(Vector2i part, unsigned index)
{
    m_texApply[index].part = part;
}

Vector2i Material::getTexturePart(unsigned index)
{
    return m_texApply[index].part;
}

void Material::setTextureAnimation(unsigned msec, unsigned index)
{
    m_texApply[index].animation = msec;
}

unsigned Material::getTextureAnimation(unsigned index)
{
    return m_texApply[index].animation;
}

void Material::setTextureClipped(bool stat, unsigned index)
{
    m_texApply[index].clipped = stat;
}

bool Material::isTextureClipped(unsigned index)
{
    return m_texApply[index].clipped;
}

void Material::setOpacity(float opacity)
{
    this->m_color.w = opacity;
}

float Material::getOpacity() const
{
    return m_color.w;
}

void Material::setColor(Vector4f color)
{
    m_color = color;
}

Vector4f Material::getColor()
{
    return m_color;
}

void Material::setClockCycle(long clockCycle)
{
    this->m_clockCycle = clockCycle;
}

long Material::getClockCycle() const
{
    return m_clockCycle;
}

ticks::Clock Material::getClock() const
{
    return m_clock;
}

void Material::setDrawPass(unsigned drawPass)
{
    this->m_drawPass = drawPass;
}

unsigned Material::getDrawPass() const
{
    return m_drawPass;
}

MaterialManager* MaterialManager::m_matMng = NULL;

MaterialManager* MaterialManager::get()
{
    if(!m_matMng)
        m_matMng = new MaterialManager;

    return m_matMng;
}

void MaterialManager::clear()
{
    delete m_matMng, m_matMng = NULL;
}

MaterialManager::MaterialManager() { }

MaterialManager::~MaterialManager()
{
    BOOST_FOREACH(MaterialManager::Map::value_type&v, m_materials)
            delete v.second;

    BOOST_FOREACH(MaterialManager::Map::value_type&v, m_materialsBackup)
            delete v.second;
}

Material* MaterialManager::newMaterial(std::string name)
{
    if(m_materials.count(name))
        return m_materials[name];

    Material* mat = new Material;
    mat->setName(name);

    m_materials[name] = mat;

    return mat;
}

Material* MaterialManager::getMaterial(std::string name)
{
    if(m_materials.count(name))
        return m_materials[name];

    return NULL;
}

Material* MaterialManager::backupMaterial(std::string name)
{
    if(m_materialsBackup.count(name))
        return m_materialsBackup[name];

    return NULL;
}

Material* MaterialManager::loadMaterial(std::string path, bool reload)
{
    using namespace std;
    using namespace boost;

    if(!reload && m_materialsFromFile.count(path))
        return m_materialsFromFile[path];

    cout << "[Material] " << path << endl;

    rtree data;
    property_tree::read_info(path, data);

    string matname = data.get<string>("name");

    Material* mat = NULL;

    if(m_materialsFromFile.count(path) && reload)
    {
        mat = m_materialsFromFile[path];
    }
    else if(m_materials.count(matname) && !reload)
    {
        mat = m_materials[matname];
        m_materialsBackup[matname] = new Material(*mat);
    }
    else
    {
        mat = new Material;
        m_materials[matname] = mat;
    }

    mat->setName(matname);

    m_materialsFromFile[path] = mat;

    VectorTranslator<Vector4f> v4tr;

    mat->setAmbient(data.get<Vector4f>("ambient", Vector4f(1), v4tr));
    mat->setDiffuse(data.get<Vector4f>("diffuse", Vector4f(1), v4tr));
    mat->setSpecular(data.get<Vector4f>("specular", Vector4f(0.5), v4tr));
    mat->setShininess(data.get<float>("shininess", 16));
    mat->setClockCycle(data.get<long>("clockCycle", 1000));

    if(data.get<bool>("alpha", false))
    {
        mat->enable(Material::ALPHA);
        mat->setAlphaThershold(data.get<float>("alphaThershold", 0.0f));
    }

    if(data.get<bool>("colored", true))
    {
        mat->enable(Material::COLORED);
        mat->setColor(data.get<Vector4f>("color", Vector4f(1, 1, 1, 1), VectorTranslator<Vector4f>()));
    }

    if(data.get<string>("blend", "none") != "none")
    {
        string blend = data.get<string>("blend");

        if(blend == "additive")
            mat->enable(Material::ADDITIVE);

        else if(blend == "modulate")
            mat->enable(Material::MODULATE);

        else if(blend == "multiply")
            mat->enable(Material::MULTIPLY);
    }

    if(data.get<string>("faceCull", "none") != "none")
    {
        string cull = data.get<string>("faceCull");

        if(cull == "back")
            mat->enable(Material::BACKFACE_CULL);

        else if(cull == "front")
            mat->enable(Material::FRONTFACE_CULL);
    }

    if(data.get<bool>("lighted", true))
        mat->enable(Material::LIGHTED);
    else
        mat->disable(Material::LIGHTED);

    if(data.get<bool>("foged", true))
        mat->enable(Material::FOGED);
    else
        mat->disable(Material::FOGED);

    if(data.get<bool>("textured", true))
        mat->enable(Material::TEXTURED);
    else
        mat->disable(Material::TEXTURED);

    if(data.count("textures"))
    {
        rtree textures = data.get_child("textures");

        rtree::iterator it = textures.begin();
        for(int i = 0; it != textures.end(); i++)
        {
            rtree& value = it->second;

            string texpath = value.get<string>("path");

            if(!tools::isAbsoloutPath(texpath))
                texpath = tools::resolvePath(texpath, path);

            bool mipmap = value.get<bool>("mipmap", true);
            int origin = value.get<int>("origin", 1);

            mat->setTexture(Texture(texpath, mipmap, origin, true), i);

            string blend = value.get<string>("blend", "modulate");

            if(blend == "modulate")
                mat->setTextureBlend(Material::MODULATE, i);

            else if(blend == "additive")
                mat->setTextureBlend(Material::ADDITIVE, i);

            else if(blend == "replace")
                mat->setTextureBlend(Material::REPLACE, i);

            else if(blend == "alpha")
                mat->setTextureBlend(Material::ALPHA, i);

            optional<rtree&> transform = value.get_child_optional("transform");

            if(transform)
            {
                VectorTranslator<Vector2i> v2itr;

                bool clipped = transform->get("clipped", false);
                Vector2i framesize = transform->get("framesize", Vector2i(), v2itr);
                Vector2i part = transform->get("part", Vector2i(), v2itr);

                mat->setTextureClipped(clipped, i);
                mat->setTextureFrameSize(framesize, i);
                mat->setTexturePart(part, i);
            }

            it++;
        }
    }

    if(data.count("shader"))
    {
        string shaderpath = data.get<string>("shader");

        if(!tools::isAbsoloutPath(shaderpath))
            shaderpath = tools::resolvePath(shaderpath, path);

        Shader shader;
        shader.parseShaderFile(shaderpath);

        mat->setShader(shader);
    }

    return mat;
}

rtree MaterialManager::serialize(std::string name, std::string root)
{
    using namespace std;
    using namespace boost;

    if(!m_materials.count(name))
        return rtree();

    Material* mat = m_materials[name];

    rtree matscheme;

    matscheme.put("name", name);

    matscheme.put("ambient", mat->getAmbient());
    matscheme.put("diffuse", mat->getDiffuse());
    matscheme.put("specular", mat->getSpecular());
    matscheme.put("shininess", mat->getShininess());
    matscheme.put("clockCycle", mat->getClockCycle());

    matscheme.put("alpha", mat->isEnable(Material::ALPHA));
    matscheme.put("alphaThershold", mat->getAlphaThershold());

    if(mat->isEnable(Material::MODULATE))
        matscheme.put("blend", "modulate");
    else if(mat->isEnable(Material::ADDITIVE))
        matscheme.put("blend", "additive");
    else if(mat->isEnable(Material::MULTIPLY))
        matscheme.put("blend", "multiplty");

    matscheme.put("color", mat->getColor());
    matscheme.put("cullTrick", mat->isEnable(Material::VERTEX_SORT_CULL_TRICK));

    if(mat->isEnable(Material::FRONTFACE_CULL))
        matscheme.put("faceCull", "front");
    if(mat->isEnable(Material::BACKFACE_CULL))
        matscheme.put("faceCull", "back");

    matscheme.put("lighted", mat->isEnable(Material::LIGHTED));
    matscheme.put("textured", mat->isEnable(Material::TEXTURED));
    matscheme.put("colored", mat->isEnable(Material::COLORED));

    Shader shade = mat->getShader();

    if(shade.isEnable())
    {
        string shaderpath = shade.getShaderFile();

        if(!shaderpath.empty())
        {
            // rtree shadertree = shade.serialize(shaderpath);
            // boost::property_tree::write_info(shaderpath, shadertree);

            shaderpath = tools::relativizePath(shaderpath, root);

            matscheme.put("shader", shaderpath);
        }
        else
            cout << "/!\\ WARNING: Mesh::serializeMaterial; Shader has not been loaded from file (" << shaderpath << ")" << endl;
    }

    unsigned txcount = mat->getTexturesCount();

    for(unsigned i = 0; i < txcount; i++)
    {
        rtree texscheme;

        Texture tex = mat->getTexture(i);

        texscheme.put("path", tools::relativizePath(tex.getFilename(), root));
        texscheme.put("origin", tex.getOrigin());
        texscheme.put("mipmap", tex.isGenMipMap());

        unsigned blend = mat->getTextureBlend(i);

        if(blend == Material::MODULATE)
            texscheme.put("blend", "modulate");
        if(blend == Material::ADDITIVE)
            texscheme.put("blend", "additive");
        if(blend == Material::REPLACE)
            texscheme.put("blend", "replace");

        matscheme.add_child("textures.unit", texscheme);
    }

    return matscheme;
}
