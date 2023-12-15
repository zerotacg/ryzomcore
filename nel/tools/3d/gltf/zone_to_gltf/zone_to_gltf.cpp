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

#include "../common/gltf.h"

using namespace NL3D;
using namespace NLMISC;
using namespace NLLIGO;
using namespace std;

void buildFaces(CLandscape &landscape, sint zoneId, sint patch, std::vector<CTriangle> &faces, std::vector<CUV> &textureCordinates)
{
	faces.clear();

	CZone *pZone = landscape.getZone(zoneId);

	// Then trace all patch.
	sint N = pZone->getNumPatchs();
	nlassert(patch >= 0);
	nlassert(patch < N);
	const CPatch *pa = const_cast<const CZone *>(pZone)->getPatch(patch);

	// Build the faces.
	//=================
	sint ordS = pa->getOrderS();
	sint ordT = pa->getOrderT();
	sint x, y;
	float OOS = 1.0f / ordS;
	float OOT = 1.0f / ordT;
	for (y = 0; y < ordT; y++)
	{
		for (x = 0; x < ordS; x++)
		{
			CTriangle f;
			// CUV a(x*OOS, y*OOT), b(x*OOS, (y+1)*OOT), c((x+1)*OOS, (y+1)*OOT), d((x+1)*OOS, y*OOT);
			CUV a(0, 0), b(0, 1), c(1, 1), d(1, 0);

			f.V0 = pa->computeContinousVertex(x * OOS, y * OOT);
			textureCordinates.push_back(a);
			f.V1 = pa->computeContinousVertex(x * OOS, (y + 1) * OOT);
			textureCordinates.push_back(b);
			f.V2 = pa->computeContinousVertex((x + 1) * OOS, (y + 1) * OOT);
			textureCordinates.push_back(c);
			faces.push_back(f);
			f.V0 = pa->computeContinousVertex(x * OOS, y * OOT);
			textureCordinates.push_back(a);
			f.V1 = pa->computeContinousVertex((x + 1) * OOS, (y + 1) * OOT);
			textureCordinates.push_back(c);
			f.V2 = pa->computeContinousVertex((x + 1) * OOS, y * OOT);
			textureCordinates.push_back(d);
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

		args.addAdditionalArg("input", ".zonel Input zone file");
		args.addAdditionalArg("output", "Output gltf file");
		args.addArg("", "tile-bank", "[name.smallbank]", "TileBank to load");

		if (!args.parse(argc, argv))
		{
			args.displayHelp();
			return EXIT_FAILURE;
		}

		std::string inputFilePath = args.getAdditionalArg("input").front();
		std::vector<std::string> tileBanks = args.getLongArg("tile-bank");
		std::string bankFilePath;
		if (!tileBanks.empty())
		{
			bankFilePath = tileBanks.front();
		}
		std::string outputFilePath = args.getAdditionalArg("output").front();
		std::string outputDirectory = CFile::getPath(outputFilePath);
		std::string fileName = CFile::getFilenameWithoutExtension(outputFilePath);
		std::string positionFileName = fileName + ".position.bin";
		std::string positionFilePath = outputDirectory + "/" + positionFileName;
		std::string textureCordinateFileName = fileName + ".texcoord.bin";
		std::string textureCordinateFilePath = outputDirectory + "/" + textureCordinateFileName;

		CIFile zoneFile;
		if (!zoneFile.open(inputFilePath))
		{
			nlwarning("Can't open the file for reading: %s", inputFilePath.c_str());
			return EXIT_FAILURE;
		}
		CLandscape landscape;
		CTileBank tile_bank;
		bool firstVertex = true;
		CAABBox bbox;
		CZone zone;
		zone.serial(zoneFile);
		landscape.addZone(zone);
		zoneFile.close();
		uint32 triangleCount = 0;
		uint32 textrueCordinateCount = 0;
		COFile outputPosition;
		try
		{
			// CIFile bankFile(bankFilePath);
			// FIXME paths in bankfile are using forward slashes and are in wrong caseing causing textures fail to load which causes the fallback (arrows) used for displacement
			// landscape.TileBank.serial(bankFile);
		}
		catch (const Exception &)
		{
			nlerror("Can't load bankfile: %s", bankFilePath.c_str());
			return EXIT_FAILURE;
		}
		if (!outputPosition.open(positionFilePath, false, false, false))
		{
			nlwarning("Can't open the file for writing: %s", positionFilePath.c_str());
			return EXIT_FAILURE;
		}
		COFile outputTextureCordinate;
		if (!outputTextureCordinate.open(textureCordinateFilePath, false, false, false))
		{
			nlwarning("Can't open the file for writing: %s", textureCordinateFilePath.c_str());
			return EXIT_FAILURE;
		}

		for (sint patch = 0; patch < zone.getNumPatchs(); patch++)
		{
			// vector of triangle
			std::vector<CTriangle> faces;
			std::vector<CUV> textureCordinates;

			buildFaces(landscape, zone.getZoneId(), patch, faces, textureCordinates);

			// Add to the file
			for (auto &face : faces)
			{
				// Serial the triangle
				face.V0.serial(outputPosition);
				face.V1.serial(outputPosition);
				face.V2.serial(outputPosition);
				if (firstVertex)
				{
					firstVertex = false;
					bbox.setCenter(face.V0);
				}
				else
				{
					bbox.extend(face.V0);
				}
				bbox.extend(face.V1);
				bbox.extend(face.V2);
			}

			for (auto &uv : textureCordinates)
			{
				// Serial the triangle
				uv.serial(outputTextureCordinate);
			}

			triangleCount += faces.size();
			textrueCordinateCount += textureCordinates.size();
		}

		FILE *fp = nlfopen(outputFilePath, "w");
		if (fp == NULL)
		{
			nlwarning("Can't open the file for writing: %s", outputFilePath.c_str());
			return EXIT_FAILURE;
		}
		gltf::JsonWriter gltfWriter = { .file = fp };
		gltf::Mesh mesh = {
			.primitives = {
				{ .attributes = { .position = 0, .texcoord0 = 1, .hasPosition = true, .hasTexcoord0 = true } }
			},
		};
		gltf::Asset asset = {
			.meshes = { mesh },
			.accessors = {
				// fprintf(fp, "			\"max\": [%f, %f, %f],\n", bbox.getMax().x, bbox.getMax().y, bbox.getMax().z);
				// fprintf(fp, "			\"min\": [%f, %f, %f],\n", bbox.getMin().x, bbox.getMin().y, bbox.getMin().z);
				{ .bufferView = 0, .componentType = gltf::ComponentType::FLOAT, .count = triangleCount * 3, .type = gltf::AccessorType::VEC3},
				{ .bufferView = 1, .componentType = gltf::ComponentType::FLOAT, .count = textrueCordinateCount, .type = gltf::AccessorType::VEC2}
			},
			.bufferViews = {
				{ .buffer = 0, .byteLength = outputPosition.getPos() },
				{ .buffer = 1, .byteLength = outputTextureCordinate.getPos() }
			},
			.buffers = {
				{ .uri = positionFileName, .byteLength = outputPosition.getPos() },
				{ .uri = textureCordinateFileName, .byteLength = outputTextureCordinate.getPos() }
			}
		};
		gltfWriter.write(asset);
		fclose(fp);
		outputPosition.close();
		outputTextureCordinate.close();

		return EXIT_SUCCESS;
	}
	catch (const Exception &e)
	{
		nlwarning("Error in writing zone file: %s", e.what());
		return EXIT_FAILURE;
	}
}
