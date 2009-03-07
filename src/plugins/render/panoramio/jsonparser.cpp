//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.

#include "jsonparser.h"

jsonParser::jsonParser()
{
//   myEngine.setProcessEventsInterval(10);//this lets the gui remain responsive
}

jsonParser::~jsonParser()
{
}

panoramioDataStructure jsonParser::parseObjectOnPosition(const QString &content , int requiredObjectPosition)
{
    QString temp = "var myJSONObject =" + content;
    myEngine.evaluate(temp);
    myEngine.evaluate(QString("function count(){ return myJSONObject.count };"));
    myEngine.evaluate(QString("function height(x){return myJSONObject.photos[x].height};"));
    myEngine.evaluate(QString("function latitude(x){return myJSONObject.photos[x].latitude};"));
    myEngine.evaluate(QString("function longitude (x){return myJSONObject.photos[x].longitude};"));
    myEngine.evaluate(QString("function owner_id(x){return myJSONObject.photos[x].owner_id};"));
    myEngine.evaluate(QString("function photo_file_url(x){return myJSONObject.photos[x].photo_file_url};"));

	
    dataStorage.count = myEngine.evaluate("count();").toInteger();

/*    dataStorage.height = myEngine.evaluate(QString("return myJSONObject.photos[")
                                           + QString::number(requiredObjectPosition)
                                           + QString("].height;")) .toInteger();
    dataStorage.latitude = myEngine.evaluate(QString("return myJSONObject.photos[")
                           + QString::number(requiredObjectPosition)
                           + QString("].latitude;")).toNumber();
    dataStorage.longitude = myEngine.evaluate(QString("return myJSONObject.photos[")
                            + QString::number(requiredObjectPosition)
                            + QString("].longitude;")).toNumber();
    dataStorage.owner_id = myEngine.evaluate(QString("return myJSONObject.photos[")
                           + QString::number(requiredObjectPosition)
                           + QString("].owner_id;")).toInteger();
    dataStorage.owner_name = myEngine.evaluate(QString("return myJSONObject.photos[")
                             + QString::number(requiredObjectPosition)
                             + QString("].owner_name;")).toString();
    dataStorage.owner_url = myEngine.evaluate(QString("return myJSONObject.photos[")
                            + QString::number(requiredObjectPosition)
                            + QString("].owner_url;")).toString();
    dataStorage.photo_id = myEngine.evaluate(QString("return myJSONObject.photos[")
                           + QString::number(requiredObjectPosition)
                           + QString("].photo_id;")).toInteger();
    dataStorage.photo_title = myEngine.evaluate(QString("return myJSONObject.photos[")
                              + QString::number(requiredObjectPosition)
                              + QString("].photo_title;")).toString();
    dataStorage.photo_url = myEngine.evaluate(QString("return myJSONObject.photos[")
                            + QString::number(requiredObjectPosition)
                            + QString("].photo_url;")).toString();
    dataStorage.photo_file_url = myEngine.evaluate(QString("return myJSONObject.photos[")
                                 + QString::number(requiredObjectPosition)
                                 + QString("].photo_file_url;")).toString();
    dataStorage.upload_date = myEngine.evaluate(QString("return myJSONObject.photos[")
                              + QString::number(requiredObjectPosition)
                              + QString("].upload_date;")).toString();
    dataStorage.width  = myEngine.evaluate(QString("return myJSONObject.photos[")
                                           + QString::number(requiredObjectPosition)
                                           + QString("].width;")).toInteger();

*/
	myEngine.evaluate(QString("var x="+QString::number(requiredObjectPosition)));
	dataStorage.longitude=myEngine.evaluate(QString("longitude(x)")).toNumber();
	dataStorage.latitude=myEngine.evaluate(QString("latitude(x)")).toNumber();
	dataStorage.photo_file_url=myEngine.evaluate(QString("photo_file_url(x)")).toString();

qDebug()<<":::::::"<<myEngine.evaluate("longitude(x)").toNumber();
    return dataStorage;
}

QList <panoramioDataStructure> jsonParser::parseAllObjects(const QString &content , int numberOfObjects)
{
    /*QString temp = "var myJSONObject = " + content;
    int iterator = 0;//the count starts fom one

    myEngine.evaluate(temp);
    while ((iterator) < numberOfObjects) {

        dataStorage.count = myEngine.evaluate("return myJSONObject.count;").toInteger();
        dataStorage.height = myEngine.evaluate(QString("return myJSONObject.photos[")
                                               + QString::number(iterator)
                                               + QString("].height;")) .toInteger();
        dataStorage.latitude = myEngine.evaluate(QString("return myJSONObject.photos[")
                               + QString::number(iterator)
                               + QString("].latitude;")).toNumber();
        dataStorage.longitude = myEngine.evaluate(QString("return myJSONObject.photos[")
                                + QString::number(iterator)
                                + QString("].longitude;")).toNumber();
        dataStorage.owner_id = myEngine.evaluate(QString("return myJSONObject.photos[")
                               + QString::number(iterator)
                               + QString("].owner_id;")).toInteger();
        dataStorage.owner_name = myEngine.evaluate(QString("return myJSONObject.photos[")
                                 + QString::number(iterator)
                                 + QString("].owner_name;")).toString();
        dataStorage.owner_url = myEngine.evaluate(QString("return myJSONObject.photos[")
                                + QString::number(iterator)
                                + QString("].owner_url;")).toString();
        dataStorage.photo_id = myEngine.evaluate(QString("return myJSONObject.photos[")
                               + QString::number(iterator)
                               + QString("].photo_id;")).toInteger();
        dataStorage.photo_title = myEngine.evaluate(QString("return myJSONObject.photos[")
                                  + QString::number(iterator)
                                  + QString("].photo_title;")).toString();
        dataStorage.photo_url = myEngine.evaluate(QString("return myJSONObject.photos[")
                                + QString::number(iterator)
                                + QString("].photo_url;")).toString();
        dataStorage.photo_file_url = myEngine.evaluate(QString("return myJSONObject.photos[")
                                     + QString::number(iterator)
                                     + QString("].photo_file_url;")).toString();
        dataStorage.upload_date = myEngine.evaluate(QString("return myJSONObject.photos[")
                                  + QString::number(iterator)
                                  + QString("].upload_date;")).toString();
        dataStorage.width  = myEngine.evaluate(QString("return myJSONObject.photos[")
                                               + QString::number(iterator)
                                               + QString("].width;")).toInteger();
        parsedJsonOutput.insert(iterator , dataStorage);
        ++iterator;
    }
*/
    return parsedJsonOutput;
}
