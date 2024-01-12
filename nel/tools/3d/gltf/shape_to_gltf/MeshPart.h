#ifndef MESH_PART_H
#define MESH_PART_H

#include <vector>

#include <nel/misc/types.h>

struct MeshPart
{
	std::vector<uint32> indices;
	std::vector<string> textures;
};

#endif //MESH_PART_H
