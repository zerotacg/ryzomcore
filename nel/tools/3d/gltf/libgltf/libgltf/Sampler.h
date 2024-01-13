#ifndef LIBGLTF_SAMPLER_TARGET_H
#define LIBGLTF_SAMPLER_TARGET_H

#include <string>

#include <libgltf/Interpolation.h>
#include <libgltf/JsonWriter.h>

namespace gltf {

struct Sampler
{
	size_t input;
	Interpolation interpolation;
	size_t output;

	void write(JsonWriter &writer) const
	{
		writer.writeProperty("input", input);
		if (interpolation != Interpolation::DEFAULT)
		{
			writer.writeProperty("interpolation", InterpolationNames[static_cast<int>(interpolation)]);
		}
		writer.writeProperty("output", output);
	}
};

}

#endif // LIBGLTF_SAMPLER_TARGET_H
