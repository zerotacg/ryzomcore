#include <gltf-convert/zone/utils.h>

#include <sstream>

using std::ostringstream;
using std::string;

string zoneName(const sint x, const sint y)
{
	ostringstream name;

	name << y + 1 << "_" << static_cast<char>('A' + (x / 26)) << static_cast<char>('A' + (x % 26));

	return name.str();
}

string zoneNameLowerCase(const sint x, const sint y)
{
	ostringstream name;

	name << y + 1 << "_" << static_cast<char>('a' + (x / 26)) << static_cast<char>('a' + (x % 26));

	return name.str();
}

string materialName(const uint16 tileId)
{
	ostringstream name;

	name << "M_tile_id_" << tileId;

	return name.str();
}
