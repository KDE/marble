#!/usr/bin/ruby

require 'net/http'
require 'rubygems'
require 'xmlsimple'
require 'pp'


class Station
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
        @lat = item['lat'].to_s
        @long = item['long'].to_s
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
        string += "    <Point>\n"
        string += "        <coordinates>" + @long + "," + @lat + "</coordinates>\n"
        string += "    </Point>\n"
        string += "</Station>\n"
        return string
    end
end
    
puts "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
puts "<StationList>"
1.upto( 8000 ) do |i| # Should be about 8000
    station = Station.new( i )
    if station.parse
        puts station.to_xml
    end
end
puts "</StationList>"
