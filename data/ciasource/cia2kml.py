#! /usr/bin/env python
# -*- coding: iso-8859-1 -*-
# This file is part of the Marble Virtual Globe.
#
# This program is free software licensed under the GNU LGPL. You can
# find a copy of this license in LICENSE.txt in the top directory of
# the source code.
#
# This python script parses the CIA Factbook 2007 for country data and 
# saves it in KML format
# Place this script in the root directory of the CIA Factbook 
# (usually "factbook") and run it.
#
# Copyright 2004-2007 Torsten Rahn  <tackat@kde.org>"
#

import urllib, urllib2, re

class AppURLopener(urllib.FancyURLopener):
    version = "Mozilla/5.0"
urllib._urlopener = AppURLopener()

# This method converts the representation of the coordinate values
# on the CIA webpages to the representation we need in the KML file.

def convertCoordinates( ciaString ):
#    print "cia: " + ciaString 
    
    commentStart=ciaString.find(";")
    coordinateString=ciaString
    if commentStart!=-1 :
        coordinateString = ciaString[ 0 : ciaString.find(";") ].strip()
    
    prefixEnd=coordinateString.find("</i> ")
    if prefixEnd!=-1 :
        coordinateString = coordinateString[ prefixEnd + 5 : len(coordinateString) ].strip()
    
    coordinateList = coordinateString.split(", ")
    latCoordinate=0.0
    lonCoordinate=0.0
    coordinateValue=0.0
    
    for coordinate in coordinateList :
        coordinateComponents = coordinate.split(' ')
#        print coordinateComponents[0] + " " + coordinateComponents[1]+ " " + coordinateComponents[2]
        coordinateValue = float(coordinateComponents[0]) + float(coordinateComponents[1])/60.0
        if coordinateComponents[2]=='W' or coordinateComponents[2]=='S' :
            coordinateValue=-coordinateValue
        if coordinate==coordinateList[0] :
            latCoordinate=coordinateValue
        else :
            lonCoordinate=coordinateValue
    latString="%04.3f" % latCoordinate
    lonString="%04.3f" % lonCoordinate
    coordinateString=lonString + "," + latString
    return coordinateString

# This method converts the representation of the population value
# on the CIA webpages to the representation we need in the KML file.
def convertPopulation( ciaString ):
#    print ciaString
    if ciaString=="uninhabited" :
        return 0
    prefixEnd=ciaString.find("</i> ")
    if prefixEnd!=-1 :
        ciaString = ciaString[ prefixEnd + 5 : len(ciaString) ].strip()
    populationString=ciaString.split(' ')[0]
    if populationString=="no" :
        return 0
    populationString=populationString.replace(",", "")
    population=int(populationString)
    populationString="%s" % (populationString)
    return populationString

def convertArea( ciaString ):
#    print ciaString
#    print "..."
    prefixEnd=ciaString.find("</i> ")
    if prefixEnd!=-1 :
        ciaString = ciaString[ prefixEnd + 5 : len(ciaString) ].strip()
    prefixEnd=ciaString.find("(")
    if prefixEnd!=-1 :
        ciaString = ciaString[ 0 : prefixEnd ].strip()
    prefixEnd=ciaString.find(";")
    if prefixEnd!=-1 :
        ciaString = ciaString[ prefixEnd + 1 : len(ciaString) ].strip()
    prefixEnd=ciaString.find("land -")
    if prefixEnd!=-1 :
        ciaString = ciaString[ prefixEnd + 6 : len(ciaString) ].strip()
    
    areaString=ciaString.split(' ')[0]
#    print areaString
    areaString=areaString.replace(",", "")
    area=float(areaString)
    areaString="%02.1f" % area
#    print areaString
    return areaString


# First build two hashes that map FIPS10 to ISO country codes
# and FIPS10 to country names. As a source for data we use 
# the appendix-d.html of the CIA Factbook.

countryCodeSource = urllib.urlopen("./appendix/appendix-d.html").readlines()

startTag="<td width=\"15%\" align=\"left\" valign=top class=\"Normal\"><b>"
stopTag="</b></td>"
finishTag="</tr>"
subStartTag="<td width=\"10%\" valign=top class=\"Normal\">"
subStopTag="</td>"
fipsString=""
country=""

inside=0
subcount=0

countryDict={}
countryCodeDict={}

for line in countryCodeSource:
    result = re.match ( '(.*)'+ startTag +'(.*)' + stopTag, line )
    if result > -1 :
        country = line[ line.find(startTag) + len(startTag) : line.find(stopTag) ].strip()
        country = country.split(',')[0]
        inside=1
    else :
        if inside==1 :
            result = re.match ( '(.*)'+ subStartTag +'(.*)' + subStopTag, line )
            if result > -1 :
                sub = line[ line.find(subStartTag) + len(subStartTag) : line.find(subStopTag) ].strip()
                subcount+=1
                if subcount == 1 :
                    fipsString=sub
                if subcount == 2 :
                    if fipsString.strip()!="-" and sub.strip()!="-" :
                        countryCodeDict[fipsString]=sub
                        countryDict[fipsString]=country
            result = re.match ( '(.*)'+ finishTag +'(.*)', line )
            if result > -1 :
                inside = 0
                subcount = 0

# Now that the hashes are in place we go through each
# website named after the fips10 code to gather the data
# we need for the KML file.

startTopicTag="<div align=\"right\">"
stopTopicTag="</div>"
topicTagList=["Background:", "Geographic coordinates:", "Area:", "Population:"]
topicBackgroundString=""
topicCoordinatesString=""
topicAreaString=""
topicPopulationString=""
placemarkFolder=""
valueTag="<br> "
finishTag="</table>"
inside=0
subcount=-1

print "Parsing ..."

for item in countryCodeDict:
    print item 
#    print item + " " + countryCodeDict.get( item, "-" ) + " " + countryDict.get( item, "-" )
    fileUrlString="./geos/%s.html" % (item.lower())
#    print fileUrlString
    try: 
        handle = urllib.urlopen(fileUrlString)
    except IOError:
        print "Error: No file for " + countryDict.get( item, "-" )
        continue
    countrySource = handle.readlines()
    for topicTag in topicTagList:
        for line in countrySource:
            result = re.match ( '(.*)'+ startTopicTag + topicTag + stopTopicTag +'(.*)', line )
            if result > -1 :
#                print line
                inside=1
                subcount=-1
            else :
                if inside==1 :
                    result = re.match ( '(.*)'+ valueTag, line )
                    if result > -1 :
#                       print line
                        subcount=0
                    if subcount>1 and line.strip()!="" :
                        if topicTag=="Background:" :
                            topicBackgroundString=line.strip()
                        if topicTag=="Geographic coordinates:" :
                            topicCoordinatesString=line.strip()
                        if topicTag=="Area:" :
                            topicAreaString=line.strip()
                        if topicTag=="Population:" :
                            topicPopulationString=line.strip()
                        inside=0
                        subcount=-1
                        break
                    if subcount>-1 :
                        subcount+=1
    
    topicCoordinatesString=convertCoordinates( topicCoordinatesString )
    topicAreaString=convertArea( topicAreaString )
    topicPopulationString=convertPopulation( topicPopulationString )
    countryNameString=countryDict.get( item, "-" )
    countryCodeString=countryCodeDict.get( item, "-" )

    topicBackgroundString=topicBackgroundString + "  <i>Source: CIA - The World Factbook 2007</i> " 
   
# Now that we have all the data needed we create a KML snippet that
# represents the country data and add it to the previous ones that 
# we've generated already.

    placemarkString='''
    <Placemark> 
        <name>%s</name> 
        <description>%s</description> 
        <countrycode>%s</countrycode>
        <role>S</role> 
        <area>%s</area> 
        <pop>%s</pop> 
        <Point>
            <coordinates>%s</coordinates> 
        </Point> 
    </Placemark>''' %( countryNameString, topicBackgroundString, countryCodeString, topicAreaString, topicPopulationString, topicCoordinatesString )
    placemarkFolder += placemarkString

# Now we insert all our KML code for the countries into a KML 
# document

kmlDocument = '''<?xml version="1.0" encoding="UTF-8"?> 
<kml xmlns="http://earth.google.com/kml/2.0"> 
<Document> 
    <SimpleField> 
        <name>pop</name> 
        <type>int</type> 
    </SimpleField> 
    <SimpleField> 
        <name>area</name> 
        <type>int</type> 
    </SimpleField> 
    <SimpleField> 
        <name>state</name> 
        <type>string</type> 
    </SimpleField> 
    <SimpleField> 
        <name>countrycode</name> 
        <type>string</type> 
    </SimpleField> 
    <SimpleField> 
        <name>role</name> 
        <type>string</type> 
    </SimpleField>
%s
</Document> 
</kml>''' %( placemarkFolder)

# Finally we save the KML document we have created

out_file = open('ciacountries.kml','w')
out_file.write(kmlDocument)
out_file.close()

