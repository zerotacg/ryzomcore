#ifndef SHAPE_TO_GLTF_H
#define SHAPE_TO_GLTF_H

#include <nel/misc/types_nl.h>
#include <nel/misc/file.h>
#include <nel/misc/o_xml.h>
#include <nel/misc/common.h>
#include <nel/misc/cmd_args.h>
#include <nel/misc/bitmap.h>
#include <nel/3d/landscape.h>
#include <nel/3d/texture_file.h>
#include <vector>
#include <nel/3d/mesh.h>
#include <nel/3d/mesh_mrm.h>
#include <nel/3d/mesh_mrm_skinned.h>
#include <nel/3d/scene.h>
#include <nel/3d/register_3d.h>


#include "MeshPart.h"

using namespace NL3D;
using namespace NLMISC;
using namespace std;

bool processMesh(IShape *shape, vector<CVector> &vertices, vector<CVector> &normals, vector<CUV> &textureCoordinates, vector<MeshPart> &parts);
bool processMeshMRMSkinned(IShape *shape, vector<CVector> &vertices, vector<CVector> &normals, vector<CUV> &textureCoordinates, vector<MeshPart> &parts);
std::string getLongArgFirstValue(const NLMISC::CCmdArgs &args, const std::string &argName);
uint32 getIndexAt(const CIndexBufferRead &buffer, const int index);
void logIndexBufferFormat(const CIndexBuffer::TFormat format);

#endif // SHAPE_TO_GLTF_H
