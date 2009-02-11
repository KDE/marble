//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.

#include "jsonparser.h"

jsonParser::jsonParser()
{
}

jsonParser::~jsonParser()
{
}

twitterDataStructure jsonParser::parseObjectOnPosition(const QString &content , int requiredObjectPosition)
{

 /*   QString temp = "var myJSONObject =" + content;
    myEngine.evaluate(temp);

    dataStorage.user = myEngine.evaluate(QString("return myJSONObject.photos[")
                                         + QString::number(requiredObjectPosition)
                                         + QString("].user;")).toString();
    dataStorage.location = myEngine.evaluate(QString("return myJSONObject.photos[")
                           + QString::number(requiredObjectPosition)
                           + QString("].location;")).toString();
    dataStorage.text = myEngine.evaluate(QString("return myJSONObject.photos[")
                                         + QString::number(requiredObjectPosition)
                                         + QString("].text;")).toString();

*/
    return dataStorage;
}

QList <twitterDataStructure> jsonParser::parseAllObjects(const QString &content , int numberOfObjects)
{
    QString temp = "var myJSONObject =  { \"twitter\":" + content + "}";
    int iterator = 0;//the count starts fom one
//qDebug()<<"::::::::::::::::::::::::"<<temp;
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
//qDebug()<<"::::::::::::::::::::::"<<dataStorage.user;

//qDebug()<<":::::::::::::::::parsed Output"<<parsedJsonOutput[0].location;
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
     qDebug() << "in geoCodingAPIparseObject";
    QString temp = "var myJSONObject = " + content;

    googleMapDataStructure returnStructure;
	myEngine.evaluate("function lat(){return myJSONObject.Placemark[0].Point.coordinates[0]}");
	myEngine.evaluate("function lon(){return myJSONObject.Placemark[0].Point.coordinates[1]}");
    myEngine.evaluate(temp);

    returnStructure.lon = myEngine.evaluate("lon()").toNumber();
 returnStructure.lat = myEngine.evaluate("lat()").toNumber();
     qDebug() << "twitter lan lon text=" << returnStructure.lat << returnStructure.lon;
    return returnStructure;
}


