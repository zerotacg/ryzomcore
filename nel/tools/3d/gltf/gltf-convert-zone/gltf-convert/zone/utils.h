#ifndef GLTF_CONVERT_ZONE_UTILS_H
#define GLTF_CONVERT_ZONE_UTILS_H

#include <string>

#include <nel/misc/types_nl.h>

std::string zoneName(sint x, sint y);

std::string zoneNameLowerCase(sint x, sint y);

std::string materialName(uint16 tileId);

#endif // GLTF_CONVERT_ZONE_UTILS_H
