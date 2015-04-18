#ifndef DEVILLOADER_H
#define DEVILLOADER_H

#include <IL/il.h>
#include <Texture.h>

namespace tbe
{

class DevilLoader : public TextureLoader
{
public:
  DevilLoader()
  {
    ilInit();
  }

  TextureData* load(std::string filename, int origin)
  {
    if(origin == 1)
    {
        ilEnable(IL_ORIGIN_SET);
        ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
    }

    else if(origin == 2)
    {
        ilEnable(IL_ORIGIN_SET);
        ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
    }

    ilEnable(IL_FORMAT_SET);
    ilFormatFunc(IL_RGBA);

    ilEnable(IL_TYPE_SET);
    ilTypeFunc(IL_UNSIGNED_BYTE);

    ILuint id = ilGenImage();
    ilBindImage(id);

    ILboolean error = !ilLoadImage(filename.c_str());

    if(error)
        throw Exception("DevilLoader::load; Open file error (%1%)") % filename;

    TextureData* tdata = new TextureData;

    tdata->width = ilGetInteger(IL_IMAGE_WIDTH);
    tdata->height = ilGetInteger(IL_IMAGE_HEIGHT);

    tdata->pixels = new ILubyte[tdata->width * tdata->height * 4];

    ilCopyPixels(0, 0, 0, tdata->width, tdata->height, 1, IL_RGBA, IL_UNSIGNED_BYTE, (ILubyte*)tdata->pixels);

    return tdata;
  }

  void release(TextureData* tdata)
  {
    // ilDeleteImage(id);

    delete[] tdata->pixels;
    delete tdata;
  }
};

}

#endif // DEVILLOADER_H
