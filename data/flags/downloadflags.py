#! /usr/bin/env python
# -*- coding: iso-8859-1 -*-

import urllib, re

class AppURLopener(urllib.FancyURLopener):
    version = "Mozilla/5.0"
urllib._urlopener = AppURLopener()

def suche( source , country, code ):
	for line in source:
		result = re.match ( '(.*)(upload.wikimedia)(.*)'+ country +'(.*)', line )
		if result > -1 :
			svgurl = line[ line.find("http") : line.find(".svg") + 4 ].strip()
			if svgurl.find('thumb') == -1 and len(svgurl) > 3 :
				svgsource = urllib.urlopen(svgurl)
				data = svgsource.read()
				svgsource.close()
				
				out_file = open('flag_' + code.strip().lower() + '.svg','w')
				out_file.write(data)
				out_file.close()
				print svgurl
				break



isourlstring = 'http://www.iso.org/' + 'iso/en/prods-services/iso3166ma/02iso-3166-code-lists/list-en1-semic.txt'
isosource = urllib.urlopen(isourlstring).readlines()

for line in isosource:
	if len(line) < 80 and len(line) > 5 :
		print line		
		linelist = line.split(';')
		linelist[0] = linelist[0].replace("KOREA, DEMOCRATIC PEOPLE'S REPUBLIC OF","NORTH KOREA")
		linelist[0] = linelist[0].replace("KOREA, REPUBLIC OF","SOUTH KOREA")
		linelist[0] = linelist[0].replace("CONGO","REPUBLIC OF THE CONGO")
		linelist[0] = linelist[0].replace("REPUBLIC OF THE CONGO, THE DEMOCRATIC REPUBLIC OF THE","DEMOCRATIC REPUBLIC OF THE CONGO")
		linelist[0] = linelist[0].replace("KOREA, REPUBLIC OF","SOUTH KOREA")
		linelist[0] = linelist[0].replace('VIRGIN ISLANDS, BRITISH','BRITISH VIRGIN ISLANDS')
		linelist[0] = linelist[0].replace('VIRGIN ISLANDS, U.S.','UNITED STATES VIRGIN ISLANDS')
		
		linelist[0] = linelist[0].split(',')[0].rstrip()
		linelist[0] = linelist[0].split('(')[0].rstrip()
		
		namelist = linelist[0].split(' ')
		fullname = ""
		for word in namelist:
			if fullname != "" :
				fullname = fullname + "_"
			if word == 'AND' or word == 'THE' or word == 'OF' or word.find('D\'') > -1:
				word = word.lower()
			else :
				word = word.capitalize()
			if word.find('\'') > -1 :
				word = word.split('\'')[0] + '%27' + word.split('\'')[1].capitalize() 
			if word.find('-') > -1 :
				word = word.split('-')[0] + '-' + word.split('-')[1].capitalize() 
			fullname = fullname + word
		
		fullname.strip()
		if fullname.find('Islands') > -1 or fullname.find('United') > -1 or fullname.find('Antilles') > -1  or fullname.find('Seychelles') > -1 or fullname.find('Philippines') > -1 or fullname.find('Republic') > -1 or fullname.find('Bahamas') > -1 or fullname.find('Territory') > -1  or fullname.find('Comoros') > -1 or fullname.find('Netherlands') > -1 or fullname.find('Isle') > -1:
			fullname = 'the_' + fullname
								
		if fullname.find("land_Islands") > -1 :
			fullname ='Aaland'

		fullname = fullname.replace('Timor-Leste','East_Timor')
		fullname = fullname.replace('the_Syrian_Arab_Republic','Syria')
		fullname = fullname.replace('Svalbard_and_Jan_Mayen','Norway')
		fullname = fullname.replace('Saint_Pierre','Saint-Pierre')
		fullname = fullname.replace('Russian_Federation','Russia')
		fullname = fullname.replace('Libyan_Arab_Jamahiriya','Libya')
		fullname = fullname.replace('the_Lao_People\'S_Democratic_Republic','Laos')
		fullname = fullname.replace('Holy_See','')
		fullname = fullname.replace('the_Heard_Island_and_Mcdonald_Islands','Australia')
		fullname = fullname.replace('French_Southern_Territories','France')
		fullname = fullname.replace('Mayotte','France')
		fullname = fullname.replace('Guadeloupe','France')
		fullname = fullname.replace('Reunion','France')
		fullname = fullname.replace('Gambia','The_Gambia')
		fullname = fullname.replace('Tokelau','New_Zealand')
		fullname = fullname.replace('Taiwan','the_Republic_of_China')
		fullname = fullname.replace('Viet_Nam','Vietnam')
		fullname = fullname.replace('French_Guiana','France')
		fullname = fullname.replace('Brunei_Darussalam','Brunei')
		fullname = fullname.replace('Pitcairn','the_Pitcairn_Islands')
		fullname = fullname.replace('Macao','Macau')
		fullname = fullname.replace('Bouvet_Island','Norway')
		fullname = fullname.replace('the_Palestinian_Territory','Palestine')
		fullname = fullname.replace('the_United_States_Minor_Outlying_Islands','the_United_States')
		fullname = fullname.replace('the_South_Georgia_and_the_South_Sandwich_Islands','South_Georgia_and_the_South_Sandwich_Islands')
		fullname = fullname.replace('Cocos','the_Cocos_%28Keeling%29_Islands')
				
		wpurlstring = 'http://de.wikipedia.org/wiki/Bild:Flag_of_'.strip() + fullname.strip() + '.svg'.strip()

		wpsource = urllib.urlopen(wpurlstring).readlines()
		if fullname !='' :
			print wpurlstring
			suche( wpsource, fullname, linelist[1] )