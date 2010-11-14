//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Shashank Singh <shashank.personal@gmail.com>
//

#include "jsonparser.h"

jsonParser::jsonParser()
{
  myEngine.setProcessEventsInterval(10);//this lets the gui remain responsive
}

jsonParser::~jsonParser()
{
}

twitterDataStructure jsonParser::parseObjectOnPosition(const QString &content , int requiredObjectPosition)
{
    return dataStorage;
}

QList <twitterDataStructure> jsonParser::parseAllObjects(const QString &content , int numberOfObjects)
{
    QString temp = "var myJSONObject =  { \"twitter\":" + content + "}";
    int iterator = 0;//the count starts fom one
    if (temp != "Twitter is down for database maintenance. It will return in about 30 minutes")
    {
	myEngine.evaluate(QString("function userName(k){return myJSONObject.twitter[k].user.name};"));
	myEngine.evaluate(QString("function userLocation(k){return myJSONObject.twitter[k].user.location};"));
	myEngine.evaluate(QString("function userText(k){return myJSONObject.twitter[k].text};"));

    myEngine.evaluate(temp);
    while ((iterator) < numberOfObjects) {
	myEngine.evaluate(QString("var a ="+QString::number(iterator) )).toString();
        dataStorage.user=myEngine.evaluate(QString("userName(a)")).toString();
 	dataStorage.location=myEngine.evaluate(QString("userLocation(a)")).toString();
 	dataStorage.text=myEngine.evaluate(QString("userText(a)")).toString();
        
	parsedJsonOutput.insert(iterator , dataStorage);

        ++iterator;
      }
    }
   else 
    {
//twitter is down :(
      dataStorage.user="@Twitter Plugin";
      dataStorage.location="Equator";
      dataStorage.text="Twitter is down , Please try in 30 minutes " ;
      parsedJsonOutput<<dataStorage;

    }
   return parsedJsonOutput;
}

googleMapDataStructure jsonParser::geoCodingAPIparseObject(QString content)
{
    mDebug() << "in geoCodingAPIparseObject";
    QString temp = "var myJSONObject = " + content;

    googleMapDataStructure returnStructure;
    myEngine.evaluate("function lat(){return myJSONObject.Placemark[0].Point.coordinates[0]}");
    myEngine.evaluate("function lon(){return myJSONObject.Placemark[0].Point.coordinates[1]}");
    myEngine.evaluate(temp);

    returnStructure.lon = myEngine.evaluate("lon()").toNumber();
    returnStructure.lat = myEngine.evaluate("lat()").toNumber();
    mDebug() << "twitter lan lon text=" << returnStructure.lat << returnStructure.lon;
    return returnStructure;
}


