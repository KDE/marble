//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.

#include "jsonparser.h"

jsonParser::jsonParser()
{
  myEngine.setProcessEventsInterval(10);//this lets the gui remain responsive
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
    myEngine.evaluate(QString("function photo_title(x){return myJSONObject.photos[x].photo_title};"));
    dataStorage.count = myEngine.evaluate("count();").toInteger();
    myEngine.evaluate(QString("var x="+QString::number(requiredObjectPosition)));
    dataStorage.longitude=myEngine.evaluate(QString("longitude(x)")).toNumber();
    dataStorage.latitude=myEngine.evaluate(QString("latitude(x)")).toNumber();
    dataStorage.photo_file_url=myEngine.evaluate(QString("photo_file_url(x)")).toString();
    dataStorage.photo_title=myEngine.evaluate(QString("photo_title(x)")).toString();

//     qDebug()<<":::::::"<<__func__<<myEngine.evaluate("longitude(x)").toNumber();
    return dataStorage;
}

QList <panoramioDataStructure> jsonParser::parseAllObjects(const QString &content , int numberOfObjects)
{
    QList <panoramioDataStructure> returnStructure;
    return returnStructure;
}
