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


void	buildFaces(CLandscape& landscape, sint zoneId, sint patch, std::vector<CTriangle> &faces, std::vector<CUV> &textureCordinates)
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
	sint	ordS= pa->getOrderS();
	sint	ordT= pa->getOrderT();
	sint	x,y;
	float	OOS= 1.0f/ordS;
	float	OOT= 1.0f/ordT;
	for(y=0;y<ordT;y++)
	{
		for(x=0;x<ordS;x++)
		{
			CTriangle	f;
			// CUV a(x*OOS, y*OOT), b(x*OOS, (y+1)*OOT), c((x+1)*OOS, (y+1)*OOT), d((x+1)*OOS, y*OOT);
			CUV a(0, 0), b(0, 1), c(1, 1), d(1, 0);

			f.V0= pa->computeContinousVertex(x*OOS, y*OOT); textureCordinates.push_back(a);
			f.V1= pa->computeContinousVertex(x*OOS, (y+1)*OOT); textureCordinates.push_back(b);
			f.V2= pa->computeContinousVertex((x+1)*OOS, (y+1)*OOT); textureCordinates.push_back(c);
			faces.push_back(f);
			f.V0= pa->computeContinousVertex(x*OOS, y*OOT); textureCordinates.push_back(a);
			f.V1= pa->computeContinousVertex((x+1)*OOS, (y+1)*OOT); textureCordinates.push_back(c);
			f.V2= pa->computeContinousVertex((x+1)*OOS, y*OOT); textureCordinates.push_back(d);
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
		args.addAdditionalArg("tile-bank", "[name.smallbank] TileBank to load");
		args.addAdditionalArg("output", "Output directory");

		if (!args.parse(argc, argv))
		{
			args.displayHelp();
			return EXIT_FAILURE;
		}

		std::string inputFilePath = args.getAdditionalArg("input").front();
		std::string bankFilePath = args.getAdditionalArg("tile-bank").front();
		std::string outputFilePath = args.getAdditionalArg("output").front();
		std::string outputDirectory = CFile::getPath(outputFilePath);
		std::string fileName = CFile::getFilenameWithoutExtension(outputFilePath);
		std::string positionFileName = fileName + ".position.bin";
		std::string positionFilePath = outputDirectory + "/" + positionFileName;
		std::string textureCordinateFileName = fileName + ".texcoord.bin";
		std::string textureCordinateFilePath = outputDirectory + "/" + textureCordinateFileName;

		CIFile zoneFile(inputFilePath);
		CLandscape landscape;
		CTileBank tile_bank;
		bool firstVertex = true;
		CAABBox bbox;
		CZone zone;
		zone.serial(zoneFile);
		landscape.addZone(zone);
		zoneFile.close();
		uint32	triangleCount=0;
		uint32	textrueCordinateCount=0;
		COFile outputPosition;
		try
		{
			// CIFile bankFile(bankFilePath);
			// landscape.TileBank.serial(bankFile);
		}
		catch(const Exception &)
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

		for (sint patch=0; patch<zone.getNumPatchs(); patch++)
		{
			// vector of triangle
			std::vector<CTriangle> faces;
			std::vector<CUV> textureCordinates;

			buildFaces (landscape, zone.getZoneId(), patch, faces, textureCordinates);

			// Add to the file
			for (auto & face : faces)
			{
				// Serial the triangle
				face.V0.serial (outputPosition);
				face.V1.serial (outputPosition);
				face.V2.serial (outputPosition);
				if ( firstVertex)
				{
					firstVertex = false;
					bbox.setCenter(face.V0);
				} else
				{
					bbox.extend(face.V0);
				}
				bbox.extend(face.V1);
				bbox.extend(face.V2);
			}

			for (auto & uv : textureCordinates)
			{
				// Serial the triangle
				uv.serial (outputTextureCordinate);
			}

			triangleCount += faces.size();
			textrueCordinateCount += textureCordinates.size();
		}

		FILE *fp = nlfopen (outputFilePath, "w");
		if (fp == NULL)
		{
			nlwarning("Can't open the file for writing: %s", outputFilePath.c_str());
			return EXIT_FAILURE;
		}
		fprintf(fp, "{\n");
		fprintf(fp, "    \"asset\": { \"version\": \"2.0\" },\n");
		fprintf(fp, "    \"meshes\": [\n");
		fprintf(fp, "        {\n");
		fprintf(fp, "			\"primitives\": [\n");
		fprintf(fp, "				{\n");
		fprintf(fp, "					\"attributes\": { \"POSITION\": 0, \"TEXCOORD_0\": 1 }\n");
		fprintf(fp, "				}\n");
		fprintf(fp, "			]\n");
		fprintf(fp, "        }\n");
		fprintf(fp, "    ],\n");
		fprintf(fp, "    \"accessors\": [\n");
		fprintf(fp, "        {\n");
		fprintf(fp, "			\"bufferView\": 0,\n");
		fprintf(fp, "			\"componentType\": 5126,\n");
		fprintf(fp, "			\"count\": %i,\n", triangleCount * 3);
		fprintf(fp, "			\"max\": [%f, %f, %f],\n", bbox.getMax().x, bbox.getMax().y, bbox.getMax().z);
		fprintf(fp, "			\"min\": [%f, %f, %f],\n", bbox.getMin().x, bbox.getMin().y, bbox.getMin().z);
		fprintf(fp, "			\"type\": \"VEC3\"\n");
		fprintf(fp, "        },\n");
		fprintf(fp, "        {\n");
		fprintf(fp, "			\"bufferView\": 1,\n");
		fprintf(fp, "			\"componentType\": 5126,\n");
		fprintf(fp, "			\"count\": %i,\n", textrueCordinateCount);
		fprintf(fp, "			\"max\": [1.0, 1.0],\n");
		fprintf(fp, "			\"min\": [1.0, 1.0],\n");
		fprintf(fp, "			\"type\": \"VEC2\"\n");
		fprintf(fp, "        }\n");
		fprintf(fp, "    ],\n");
		fprintf(fp, "    \"bufferViews\": [\n");
		fprintf(fp, "        {\n");
		fprintf(fp, "			\"buffer\": 0,\n");
		fprintf(fp, "			\"byteLength\": %i\n", outputPosition.getPos());
		fprintf(fp, "        },\n");
		fprintf(fp, "        {\n");
		fprintf(fp, "			\"buffer\": 1,\n");
		fprintf(fp, "			\"byteLength\": %i\n", outputTextureCordinate.getPos());
		fprintf(fp, "        }\n");
		fprintf(fp, "    ],\n");
		fprintf(fp, "    \"buffers\": [\n");
		fprintf(fp, "        {\n");
		fprintf(fp, "			\"uri\": \"%s\",\n", positionFileName.c_str());
		fprintf(fp, "			\"byteLength\": %i\n", outputPosition.getPos());
		fprintf(fp, "        },\n");
		fprintf(fp, "        {\n");
		fprintf(fp, "			\"uri\": \"%s\",\n", textureCordinateFileName.c_str());
		fprintf(fp, "			\"byteLength\": %i\n", outputTextureCordinate.getPos());
		fprintf(fp, "        }\n");
		fprintf(fp, "    ]\n");
		fprintf(fp, "}\n");
		fclose (fp);
		outputPosition.close();

		return EXIT_SUCCESS;
	}
	catch (const Exception &e)
	{
		nlwarning("Error in writing zone file: %s", e.what());
		return EXIT_FAILURE;
	}
}
