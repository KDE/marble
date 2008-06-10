//
// C++ Interface: jsonparser
//
// Description:
//
//
// Author:  <Shashank Singh>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <QtScript/QtScript>
#include <QList>
#include <QObject>
#include <QDebug>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptEngine>
/**
This is a generic class built up for parsing Json that is JavaScript Object Notification 
 
	@author Shashank Singh
*/
struct panoramioDataStructure 
    { 
    static long int count;//total number of photographs will be stored in this int
    long int photo_id ; //id of each photograph
    QString photo_title; //title of each photograph
    QString photo_url; //url of each photograph
    long int longitude;
    long int latitude;
    int width;
    int height;
    QString upload_date;
    long int owner_id;
    QString owner_name;
    QString owner_url; //Url of the User Uplaoded
};

class JsonParser :public QObject
  {
public:
    JsonParser();
    
    ~JsonParser();
    
    QList <panoramioDataStructure> parse(const QString &content);
    
  private:
     QList <panoramioDataStructure> parsedJsonOutput;
     panoramioDataStructure dataStorage;   
//     QScriptEngine myEngine;    //this is Line where i am getting ERROR
    
  };


#endif
