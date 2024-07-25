#include <mapper/TrackSampledVectorMapper.h>

using namespace NL3D;
using namespace NLMISC;
using namespace std;

void TrackSampledVectorMapper::map(SampleData &output)
{
	output.type = gltf::AccessorType::VEC3;
}
