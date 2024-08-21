
#include <QTextStream>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <nel/misc/file.h>
#include <nel/3d/zone.h>
#include <nel/misc/path.h>

using namespace NL3D;
using namespace NLMISC;
using namespace std;

QJsonValue toJson(const int source);
QJsonObject toJson(const CZone &source);
QJsonArray toJson(const CVector &source);
QJsonObject toJson(const CBorderVertex &source);
QJsonObject toJson(const CAABBoxExt &source);
QJsonObject toJson(const CPatch &source);
QJsonObject toJson(const CTileElement &source);
QJsonValue toJson(const CTileElement::TVegetableInfo source);
QJsonArray patchesToJson(const CZone &source);
QJsonArray toJson(const CVector3s &source);

template <class T>
QJsonArray toJson(const std::vector<T> &source)
{
	QJsonArray json;
	for (auto &element : source)
	{
		json.append(toJson(element));
	}

	return json;
}

int main(int argc, char **argv)
{
	QCoreApplication app(argc, argv);
	QCoreApplication::setApplicationName("zone_info");
	QCoreApplication::setApplicationVersion("0.1");

	QCommandLineParser parser;
	parser.addHelpOption();
	parser.addVersionOption();
	parser.addPositionalArgument("input", ".zonel Input zone file");
	parser.process(app);

	auto inputFilePath(parser.positionalArguments().at(0));

	CIFile zoneFile;
	if (!zoneFile.open(inputFilePath.toStdString()))
	{
		nlerror("Can't open the file for reading: %s", inputFilePath.constData());
		return EXIT_FAILURE;
	}
	CZone zone;
	CZoneInfo zoneInfo;
	uint version = zoneFile.serialVersion(0xffff);
	zoneFile.close();
	zoneFile.open(inputFilePath.toStdString());
	zone.serial(zoneFile);
	zoneFile.close();
	zone.retrieve(zoneInfo);
	auto zoneName(QFileInfo(inputFilePath).baseName());

	auto zone_info = toJson(zone);
	zone_info["version"] = toJson(version);
	zone_info["name"] = zoneName;
	zone_info["borderVertices"] = toJson(zoneInfo.BorderVertices);

	QByteArray byteArray(QJsonDocument(zone_info).toJson(QJsonDocument::Compact));
	QTextStream textStream(stdout);
	textStream << byteArray << Qt::endl;

	return EXIT_SUCCESS;
}

QJsonValue toJson(const int source)
{
	return { source };
}

QJsonObject toJson(const CZone &source)
{
	QJsonObject json;

	json["zoneId"] = source.getZoneId();
	json["bbox"] = toJson(source.getZoneBB());
	json["patchBias"] = toJson(source.getPatchBias());
	json["patchScale"] = source.getPatchScale();
	json["patches"] = patchesToJson(source);

	return json;
}

QJsonArray patchesToJson(const CZone &source)
{
	QJsonArray json;
	for (auto i = 0; i < source.getNumPatchs(); ++i)
	{
		json.append(toJson(*source.getPatch(i)));
	}

	return json;
}

QJsonObject toJson(const CPatch &source)
{
	QJsonObject json;

	json["orderS"] = source.getOrderS();
	json["orderT"] = source.getOrderT();
	json["vertices"] = toJson(std::vector(source.Vertices, source.Vertices + (sizeof source.Vertices / sizeof source.Vertices[0])));
	json["tangents"] = toJson(std::vector(source.Tangents, source.Tangents + (sizeof source.Tangents / sizeof source.Tangents[0])));
	json["interiors"] = toJson(std::vector(source.Interiors, source.Interiors + (sizeof source.Interiors / sizeof source.Interiors[0])));
	json["tiles"] = toJson(source.Tiles);

	return json;
}

QJsonObject toJson(const CTileElement &source)
{
	QJsonObject json;

	QJsonObject flags;
	bool is256x256;
	uint8 uvOff;
	source.getTile256Info(is256x256, uvOff);

	flags["is256x256"] = is256x256;
	flags["uvOff"] = uvOff;
	flags["subNoise"] = source.getTileSubNoise();
	flags["vegetableState"] = toJson(source.getVegetableState());

	json["flags"] = flags;
	json["tile"] = toJson(std::vector(source.Tile, source.Tile + (sizeof source.Tile / sizeof source.Tile[0])));

	json["orientation"] = toJson((std::vector<uint8>) { source.getTileOrient(0), source.getTileOrient(1), source.getTileOrient(2) });

	return json;
}

QJsonValue toJson(const CTileElement::TVegetableInfo source)
{
	switch (source)
	{
	case CTileElement::AboveWater: return { "AboveWater" };
	case CTileElement::UnderWater: return { "UnderWater" };
	case CTileElement::IntersectWater: return { "IntersectWater" };
	case CTileElement::VegetableDisabled: return { "VegetableDisabled" };
	case CTileElement::VegetInfoLast: return { "VegetInfoLast" };
	default: return {};
	}
}

QJsonObject toJson(const CAABBoxExt &source)
{
	QJsonObject json;

	json["center"] = toJson(source.getCenter());
	json["halfSize"] = toJson(source.getHalfSize());

	return json;
}

QJsonArray toJson(const CVector &source)
{
	QJsonArray json;

	json.append(source.x);
	json.append(source.y);
	json.append(source.z);

	return json;
}

QJsonArray toJson(const CVector3s &source)
{
	QJsonArray json;

	json.append(source.x);
	json.append(source.y);
	json.append(source.z);

	return json;
}

QJsonObject toJson(const CBorderVertex &source)
{
	QJsonObject json;

	json["CurrentVertex"] = source.CurrentVertex;
	json["NeighborZoneId"] = source.NeighborZoneId;
	json["NeighborVertex"] = source.NeighborVertex;

	return json;
}