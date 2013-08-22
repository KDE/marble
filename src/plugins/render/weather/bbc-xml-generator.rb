#!/usr/bin/ruby

#
# This file is part of the Marble Virtual Globe.
#
# This program is free software licensed under the GNU LGPL. You can
# find a copy of this license in LICENSE.txt in the top directory of
# the source code.
#
# Copyright 2009 Bastian Holst <bastianholst@gmx.de>
#

require 'net/http'
require 'rubygems'
require 'xmlsimple'
require 'pp'


class Station
    attr_reader :long, :lat, :priority, :station
    attr_writer :priority
    def initialize(number)
	@id = number.to_s
        @url = 'http://newsrss.bbc.co.uk/weather/forecast/' + @id + '/ObservationsRSS.xml'
    end
    
    def parse()
	xmlData = Net::HTTP.get_response(URI.parse(@url)).body
        begin
            data = XmlSimple.xml_in(xmlData, { 'KeyAttr' => 'name' })
        rescue REXML::ParseException
            return false
        end
        
        channel = (data["channel"])[0]
        
        name = channel["title"].to_s.sub( /BBC - Weather Centre - Latest Observations for /, "" )
        nameList = name.split( /, / )
        @station = nameList[0].to_s
        @country = nameList[1].to_s
        
        item = channel["item"][0]
        @lat = item['lat'].to_s.to_f
        @long = item['long'].to_s.to_f
        if @lat == "N/A" or @long == "N/A"
            return false
        end
        
        return true
    end
	
    def to_s()
        string = "Id: "
        string += @id.to_s
        string += "\n"
        string += "Station: "
        string += @station.to_s
        string += "\n"
        string += "Country: "
        string += @country.to_s
        string += "\n"
        string += "lat: "
        string += @lat.to_s
        string += "\n"
        string += "lon: "
        string += @long.to_s
        string += "\n"
        return string
    end
    
    def to_xml()
        string = "<Station>\n"
        string += "    <name>" + @station + "</name>\n"
        string += "    <Country>" + @country + "</Country>\n"
        string += "    <id>" + @id.to_s + "</id>\n"
        string += "    <priority>" + @priority.to_s + "</priority>\n"
        string += "    <Point>\n"
        string += "        <coordinates>" + @long.to_s + "," + @lat.to_s + "</coordinates>\n"
        string += "    </Point>\n"
        string += "</Station>\n"
        return string
    end
    
#    def priority=(newPriority)
#        @priority = newPriority
#    end
    
    def angelDistance(other)
        begin
            distance = Math.acos( Math.sin( @lat / 180 * Math::PI ) * Math.sin( other.lat / 180 * Math::PI ) + Math.cos( @lat / 180 * Math::PI ) * Math.cos( other.lat / 180 * Math::PI ) * Math.cos( other.long / 180 * Math::PI - @long / 180 * Math::PI ) ).abs
        rescue
            distance = 0
        end
        return distance
    end
end
    
puts "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
puts "<StationList>"
stations = Array.new()
1.upto( 8000 ) do |i| # Should be about 8000
    station = Station.new( i )
    if station.parse
        minDist = 2.0 * Math::PI
        minDistStation = "none"
        stations.each do |other|
            distance = station.angelDistance( other )
            if distance < minDist
                minDist = distance
                minDistStation = other.station
            end
        end
        
        priority = 32
        min = Math::PI / 8.0
        while minDist < min and priority > 0
            priority -= 1
            min = min / 1.17
        end
        
        station.priority = priority
        
        puts station.to_xml
        
        stations = stations << station
    end
end
puts "</StationList>"
