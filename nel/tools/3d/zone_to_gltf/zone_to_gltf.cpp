#include <iostream>
#include <nel/misc/types_nl.h>
#include <nel/misc/file.h>
#include <nel/misc/o_xml.h>
#include <nel/misc/common.h>
#include <nel/misc/cmd_args.h>
#include <nel/misc/bitmap.h>
#include <nel/3d/zone.h>
#include <nel/3d/landscape.h>
#include <nel/ligo/zone_region.h>
#include <vector>

using namespace NL3D;
using namespace NLMISC;
using namespace NLLIGO;
using namespace std;


void	buildFaces(CLandscape& landscape, sint zoneId, sint patch, std::vector<CTriangle> &faces)
{
	faces.clear();

	CZone* pZone=landscape.getZone (zoneId);

	// Then trace all patch.
	sint	N= pZone->getNumPatchs();
	nlassert(patch>=0);
	nlassert(patch<N);
	const CPatch	*pa= const_cast<const CZone*>(pZone)->getPatch(patch);

	// Build the faces.
	//=================
	sint	ordS= 4*pa->getOrderS();
	sint	ordT= 4*pa->getOrderT();
	sint	x,y;
	float	OOS= 1.0f/ordS;
	float	OOT= 1.0f/ordT;
	for(y=0;y<ordT;y++)
	{
		for(x=0;x<ordS;x++)
		{
			CTriangle	f;
			f.V0= pa->computeVertex(x*OOS, y*OOT);
			f.V1= pa->computeVertex(x*OOS, (y+1)*OOT);
			f.V2= pa->computeVertex((x+1)*OOS, (y+1)*OOT);
			faces.push_back(f);
			f.V0= pa->computeVertex(x*OOS, y*OOT);
			f.V1= pa->computeVertex((x+1)*OOS, (y+1)*OOT);
			f.V2= pa->computeVertex((x+1)*OOS, y*OOT);
			faces.push_back(f);
		}
	}
}


int main(int argc, char **argv)
{
	try
	{
		NLMISC::CApplicationContext myApplicationContext;
		NLMISC::CCmdArgs args;

		args.addAdditionalArg("input", "Input zone file");
		args.addAdditionalArg("output", "Output directory");

		if (!args.parse(argc, argv))
		{
			args.displayHelp();
			return EXIT_FAILURE;
		}

		std::string inputFilePath = args.getAdditionalArg("input")[0];
		std::string outputDirectory = args.getAdditionalArg("output")[0];
		std::string fileName = CFile::getFilenameWithoutExtension(inputFilePath);
		std::string binFileName = fileName + ".bin";
		std::string outputBinPath = outputDirectory + "/" + binFileName;
		std::string outputGltfPath = outputDirectory + "/" + fileName + ".gltf";

		CIFile zoneFile(inputFilePath);
		CLandscape landscape;
		CZone zone;
		zone.serial(zoneFile);
		landscape.addZone(zone);
		zoneFile.close();
		uint32	triangles=0;
		COFile output;
		if (!output.open(outputBinPath, false, false, false))
		{
			nlwarning("Can't open the file for writing: %s", outputBinPath.c_str());
			return EXIT_FAILURE;
		}

		for (sint patch=0; patch<zone.getNumPatchs(); patch++)
		{
			// vector of triangle
			std::vector<CTriangle> faces;

			// Build a list of triangles at 50 cm
			buildFaces (landscape, zone.getZoneId(), patch, faces);

			// Add to the file
			for (auto & face : faces)
			{
				// Serial the triangle
				face.V0.serial (output);
				face.V1.serial (output);
				face.V2.serial (output);
			}

			// Triangle count
			triangles += faces.size();
		}

		FILE *fp = nlfopen (outputGltfPath, "w");
		if (fp == NULL)
		{
			nlwarning("Can't open the file for writing: %s", outputGltfPath.c_str());
			return EXIT_FAILURE;
		}
		fprintf(fp, "{\n");
		fprintf(fp, "    \"asset\": { \"version\": \"2.0\" },\n");
		fprintf(fp, "    \"meshes\": [\n");
		fprintf(fp, "        {\n");
		fprintf(fp, "			\"primitives\": [\n");
		fprintf(fp, "				{\n");
		fprintf(fp, "					\"attributes\": { \"POSITION\": 0 }\n");
		fprintf(fp, "				}\n");
		fprintf(fp, "			]\n");
		fprintf(fp, "        }\n");
		fprintf(fp, "    ],\n");
		fprintf(fp, "    \"accessors\": [\n");
		fprintf(fp, "        {\n");
		fprintf(fp, "			\"bufferView\": 0,\n");
		fprintf(fp, "			\"componentType\": 5126,\n");
		fprintf(fp, "			\"count\": %i,\n", triangles * 3);
		fprintf(fp, "			\"max\": [1.0, 1.0, 1.0],\n");
		fprintf(fp, "			\"min\": [1.0, 1.0, 1.0],\n");
		fprintf(fp, "			\"type\": \"VEC3\"\n");
		fprintf(fp, "        }\n");
		fprintf(fp, "    ],\n");
		fprintf(fp, "    \"bufferViews\": [\n");
		fprintf(fp, "        {\n");
		fprintf(fp, "			\"buffer\": 0,\n");
		fprintf(fp, "			\"byteLength\": %i\n", output.getPos());
		fprintf(fp, "        }\n");
		fprintf(fp, "    ],\n");
		fprintf(fp, "    \"buffers\": [\n");
		fprintf(fp, "        {\n");
		fprintf(fp, "			\"uri\": \"%s\",\n", binFileName.c_str());
		fprintf(fp, "			\"byteLength\": %i\n", output.getPos());
		fprintf(fp, "        }\n");
		fprintf(fp, "    ]\n");
		fprintf(fp, "}\n");
		fclose (fp);
		output.close();

		return EXIT_SUCCESS;
	}
	catch (const Exception &e)
	{
		nlwarning("Error in writing zone file: %s", e.what());
		return EXIT_FAILURE;
	}
}
