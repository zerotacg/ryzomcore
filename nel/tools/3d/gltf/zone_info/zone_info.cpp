
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

QJsonObject toJson( const CZone& source );
QJsonArray toJson( const CVector& source );
QJsonArray toJson( const std::vector<CBorderVertex>& source );
QJsonObject toJson( const CBorderVertex& source );
QJsonObject toJson( const CAABBoxExt& source );
QJsonObject toJson( const CPatch& source );
QJsonArray patchesToJson( const CZone& source );


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
	zone.serial(zoneFile);
	zoneFile.close();
	zone.retrieve(zoneInfo);
	auto zoneName(QFileInfo(inputFilePath).baseName());

	QJsonObject json(toJson(zone));
	json["name"] = zoneName;
	json["borderVertices"] = toJson(zoneInfo.BorderVertices);

	QByteArray byteArray(QJsonDocument(json).toJson(QJsonDocument::Compact));
	QTextStream textStream(stdout);
	textStream << byteArray << Qt::endl;

	return EXIT_SUCCESS;
}

QJsonObject toJson( const CZone& source )
{
	QJsonObject json;

	json["zoneId"] = source.getZoneId();
	json["bbox"] = toJson(source.getZoneBB());
	json["patchBias"] = toJson(source.getPatchBias());
	json["patchScale"] = source.getPatchScale();
	json["patches"] = patchesToJson( source );

	return json;
}

QJsonArray patchesToJson( const CZone& source )
{
	QJsonArray json;
	for(auto i = 0; i < source.getNumPatchs(); ++i) {
		json.append(toJson(*source.getPatch(i)));
	}

	return json;
}

QJsonObject toJson(const CPatch &source )
{
	QJsonObject json;

	json["orderS"] = source.getOrderS();
	json["orderT"] = source.getOrderT();

	return json;
}

QJsonObject toJson( const CAABBoxExt& source )
{
	QJsonObject json;

	json["center"] = toJson(source.getCenter());
	json["halfSize"] = toJson(source.getHalfSize());

	return json;
}

QJsonArray toJson( const CVector& source )
{
	QJsonArray json;

	json.append(source.x);
	json.append(source.y);
	json.append(source.z);

	return json;
}

QJsonArray toJson( const std::vector<CBorderVertex>& source )
{
	QJsonArray json;

	for(auto& element: source)
	{
		json.append(toJson(element));
	}

	return json;

}

QJsonObject toJson( const CBorderVertex& source )
{
	QJsonObject json;

	json["CurrentVertex"] = source.CurrentVertex;
	json["NeighborZoneId"] = source.NeighborZoneId;
	json["NeighborVertex"] = source.NeighborVertex;

	return json;
}