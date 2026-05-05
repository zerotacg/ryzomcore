#ifndef MESH_PART_H
#define MESH_PART_H

#include <string>
#include <vector>

#include <nel/misc/types_nl.h>

struct MeshPart
{
	std::vector<uint32> indices;
	std::vector<std::string> textures;
};

#endif //MESH_PART_H
