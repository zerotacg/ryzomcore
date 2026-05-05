#ifndef SAMPLE_DATA_H
#define SAMPLE_DATA_H

#include <vector>

#include <nel/misc/vector.h>
#include <nel/misc/uv.h>
#include <nel/misc/quat.h>

#include <libgltf/ChannelTargetPath.h>
#include <libgltf/Interpolation.h>
#include <libgltf/other.h>

struct ChannelData
{
	std::vector<float> time;
	gltf::Interpolation interpolation;
	gltf::AccessorType type;

	std::vector<NLMISC::CVector> vector;
	std::vector<NLMISC::CQuat> quaternion;
};

#endif // SAMPLE_DATA_H
