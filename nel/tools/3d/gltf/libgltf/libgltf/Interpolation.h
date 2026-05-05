#ifndef LIBGLTF_INTERPOLATION_H
#define LIBGLTF_INTERPOLATION_H

#include <string>

namespace gltf {

enum class Interpolation
{
	LINEAR = 0,
	STEP,
	CUBICSPLINE,
	DEFAULT = LINEAR
};
const std::string InterpolationNames[] = { "LINEAR", "STEP", "CUBICSPLINE" };

}

#endif // LIBGLTF_INTERPOLATION_H
