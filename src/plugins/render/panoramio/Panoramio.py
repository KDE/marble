#this is a sample parnomaio query rpely script in python


import httplib
import json

conn = httplib.HTTPConnection("www.panoramio.com",80)
conn.request("GET","/map/get_panoramas.php?order=popularity&set=public&from=0&to=10&minx=-124.29382324218749&miny=36.089060460282006&maxx=-119.8773193359375&maxy=38.724090458956965")

r1 = conn.getresponse()
print r1.status
data1 = r1.read()
#print data1
a=json.loads (data1)
#print a
#print a
for keys,x in a.iteritems() :
	z=x
	print z , ":::::::::::::keys" , keys,"\n"
a=json.load(keys)
