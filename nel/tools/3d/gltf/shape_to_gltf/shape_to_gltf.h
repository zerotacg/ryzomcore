#ifndef SHAPE_TO_GLTF_H
#define SHAPE_TO_GLTF_H

#include <nel/misc/types_nl.h>
#include <nel/misc/cmd_args.h>
#include <vector>
#include <nel/3d/mesh.h>

#include "MeshPart.h"

bool processMesh(NL3D::IShape *shape, std::vector<NLMISC::CVector> &vertices, std::vector<NLMISC::CVector> &normals, std::vector<NLMISC::CUV> &textureCoordinates, std::vector<MeshPart> &parts);
std::string getLongArgFirstValue(const NLMISC::CCmdArgs &args, const std::string &argName);
uint32 getIndexAt(const NL3D::CIndexBufferRead &buffer, int index);
void fillTextureFileNames(std::vector<std::string> &textures, const NL3D::CMaterial &material);
void logIndexBufferFormat(NL3D::CIndexBuffer::TFormat format);

#endif // SHAPE_TO_GLTF_H
