#!/usr/bin/python

"""
This script is designed to act as assistance in converting shapefiles
to OpenStreetMap data. This file is optimized and tested with MassGIS
shapefiles, converted to EPSG:4326 before being passed to the script.
You can perform this conversion with 

   ogr2ogr -t_srs EPSG:4326 new_file.shp old_file.shp

It is expected that you will modify the fixed_tags, tag_mapping, and
boring_tags attributes of this script before running. You should read,
or at least skim, the code up until it says:

  DO NOT CHANGE AFTER THIS LINE.

to accomodate your own data. 
"""

__author__ = "Christopher Schmidt <crschmidt@crschmidt.net>"
__version__ = "$Id$"

gdal_install = """
Installing GDAL depends on your platform. Information is available at:
   
   http://trac.osgeo.org/gdal/wiki/DownloadingGdalBinaries

For Debian-based systems:

   apt-get install python-gdal

will usually suffice. 
"""

# These tags are attached to all exterior ways. You can put any key/value pairs
# in this dictionary. 

fixed_tags = {
  'source': 'MassGIS OpenSpace (http://www.mass.gov/mgis/osp.htm)',
  'area': 'yes',
  'created_by': 'polyshp2osm'
}  

# Here are a number of functions: These functions define tag mappings. The API
# For these functions is that they are passed the attributes from a feature,
# and they return a list of two-tuples which match to key/value pairs.

def access(data):
    """Access restrictions."""  
    keys = {
        'Y': 'yes',
        'N': 'private',
        'L': 'restricted'
    }
    if 'pub_access' in data:
        if data['pub_access'] in keys:
            return [('access', keys[data['pub_access']])]
    return None        

def protection(data):
    keys = {
        'P': 'perpetuity',
        'T': 'temporary',
        'L': 'limited',
    }
    if 'lev_prot' in data:
        if data['lev_prot'] in keys:
            return [('protected', keys[data['lev_prot']])]
    return None

def owner_type(data):
    """See wiki:Key:ownership""" 
    keys = {
        'F': 'national',
        'S': 'state',
        'C': 'county',
        'M': 'municipal',
        'N': 'private_nonprofit',
        'P': 'private',
        'B': 'public_nonprofit',
        'L': 'land_trust',
        'G': 'conservation_rganization',
        'I': 'inholding',
    }
    if 'owner_type' in data:
        if data['owner_type'] in keys:
            return [['ownership', keys[data['owner_type']]]]

def purpose(data):
    """Based on a discussion on IRC"""
    keys = {
        'R': [('leisure', 'recreation_ground')],
        'C': [('leisure', 'nature_reserve'), ('landuse', 'conservation')],
        'B': [('landuse','conservation'), ('leisure','recreation_ground')],
        'H': [('historical', 'yes')],
        'A': [('agricultural', 'yes'), ('landuse','farm')], 
        'W': [('landuse', 'resevoir')],
        'S': [('scenic','yes')],
        'F': [('landuse','land')],
        'Q': [('landuse','conservation')],
        'U': [('water','yes')]
    }
    if 'prim_purp' in data:
        if data['prim_purp'] in keys:
            return keys[data['prim_purp']]

def name_tags(data):
    """This function returns two things: a 'pretty' name to use, and
       may return a landuse of either 'cemetery' or 'forest' if the name
       contains those words; based on evaluation the dataset in question."""
    tags = [] 
    name = data.get('site_name', None)
    if not name: 
        return
    name = name.title()
    
    if "cemetery" in name.lower():
        tags.append(['landuse', 'cemetery']) 
    elif "forest" in name.lower():
        tags.append(['landuse', 'forest']) 

    tags.append(['name', name])
    return tags

def cal_date(data):
    """Return YYYY-MM-DD or YYYY formatted dates, based on 
       (m)m/(d)d/yyyy dates"""
    date = data.get('cal_date_r', None)
    if not date: return
    try:
        m, d, y = map(int, date.split("/"))
        if m == 1 and d == 1:
            return [['start_date', '%4i' % y]]
        return [['start_date', '%04i-%02i-%02i' % (y, m, d)]] 
    except:
        print "Invalid date: %s" % date
        return None

# The most important part of the code: define a set of key/value pairs
# to iterate over to generate keys. This is a list of two-tuples: first
# is a 'key', which is only used if the second value is a string. In
# that case, it is a map of lowercased fielnames to OSM tag names: so
# fee_owner maps to 'owner' in the OSM output.

# if the latter is callable (has a __call__; is a function), then that
# method is called, passing in a dict of feature attributes with
# lowercased key names. Those functions can then return a list of
# two-tuples to be used as tags, or nothin' to skip the tags.  


tag_mapping = [ 
    ('fee_owner', 'owner'),
    ('cal_date', cal_date),
    ('pub_access', access),
    ('lev_prot', protection),
    ('owner_type', owner_type),
    ('prim_purp', purpose),
    ('site_name', name_tags),
]    

# These tags are not exported, even with the source data; this should be
# used for tags which are usually calculated in a GIS. AREA and LEN are
# common.

boring_tags = [ 'AREA', 'LEN', 'GIS_ACRES' ]

# Namespace is used to prefix existing data attributes. If 'None', or 
# '--no-source' is set, then source attributes are not exported, only
# attributes in tag_mapping.

namespace = "massgis"
#namespace = None 

# Uncomment the "DONT_RUN = False" line to get started. 

DONT_RUN = True
#DONT_RUN = False

# =========== DO NOT CHANGE AFTER THIS LINE. ===========================
# Below here is regular code, part of the file. This is not designed to
# be modified by users.
# ======================================================================

import sys

try:
    try:
        from osgeo import ogr
    except ImportError:
        import ogr
except ImportError:
    __doc__ += gdal_install 
    if DONT_RUN:
        print __doc__
        sys.exit(2)
    print "OGR Python Bindings not installed.\n%s" % gdal_install
    sys.exit(1)

def close_file():
    """ Internal. Close an open file."""
    global open_file
    if not open_file.closed: 
        open_file.write("</osm>")
        open_file.close()

def start_new_file():
    """ Internal. Open a new file, closing existing file if neccesary."""
    global open_file, file_counter
    file_counter += 1
    if open_file:
        close_file()
    open_file = open("%s.%s.osm" % (file_name, file_counter), "w")
    print >>open_file, "<osm version='0.5'>"

def clean_attr(val):
    """Internal. Hacky way to make attribute XML safe."""
    val = str(val)
    val = val.replace("&", "&amp;").replace("'", "&quot;").replace("<", "&lt;").replace(">", "&gt;").strip()
    return val

def add_ring_nodes(ring):
    """Internal. Write the first ring nodes."""
    global open_file, id_counter
    ids = []
    if range(ring.GetPointCount() - 1) == 0 or ring.GetPointCount() == 0:
        print >>sys.stderr, "Degenerate ring." 
        return    
    for count in range(ring.GetPointCount()-1):
        ids.append(id_counter)
        print >>open_file, "<node id='-%s' lon='%s' lat='%s' />" % (id_counter, ring.GetX(count), ring.GetY(count)) #print count
        id_counter += 1
    return ids    

def add_ring_way(ring): 
    """Internal. write out the 'holes' in a polygon."""
    global open_file, id_counter
    ids = []
    for count in range(ring.GetPointCount()-1):
        ids.append(id_counter)
        print >>open_file, "<node id='-%s' lon='%s' lat='%s' />" % (id_counter, ring.GetX(count), ring.GetY(count)) #print count
        id_counter += 1
    if len(ids) == 0:
        return None
    print >>open_file, "<way id='-%s'>" % id_counter
    way_id = id_counter
    id_counter += 1
    for i in ids:
        print >>open_file, "<nd ref='-%s' />" % i
    print >>open_file, "<nd ref='-%s' />" % ids[0]
    print >>open_file, "</way>"

    return way_id

open_file = None

file_name = None 

id_counter = 1

file_counter = 0
counter = 0

class AppError(Exception): pass

def run(filename, slice_count=1, obj_count=50000, output_location=None, no_source=False):
    """Run the converter. Requires open_file, file_name, id_counter,
    file_counter, counter to be defined in global space; not really a very good
    singleton."""
    global id_counter, file_counter, counter, file_name, open_file, namespace
    
    if no_source:
        namespace=None

    if output_location:
       file_name = output_location

    ds = ogr.Open(filename)
    if not ds:
        raise AppError("OGR Could not open the file %s" % filename)
    l = ds.GetLayer(0)
   
    max_objs_per_file = obj_count 

    extent = l.GetExtent()
    if extent[0] < -180 or extent[0] > 180 or extent[2] < -90 or extent[2] > 90:
        raise AppError("Extent does not look like degrees; are you sure it is? \n(%s, %s, %s, %s)" % (extent[0], extent[2], extent[1], extent[3]))  
    slice_width = (extent[1] - extent[0]) / slice_count

    seen = {}

    print "Running %s slices with %s base filename against shapefile %s" % (
            slice_count, file_name, filename)

    for i in range(slice_count): 

        l.ResetReading()
        l.SetSpatialFilterRect(extent[0] + slice_width * i, extent[2], extent[0] + (slice_width * (i + 1)), extent[3])

        start_new_file()
        f = l.GetNextFeature()
        
        obj_counter = 0
        last_obj_split = 0

        while f:
            start_id_counter = id_counter
            if f.GetFID() in seen:
                f = l.GetNextFeature()
                continue
            seen[f.GetFID()] = True             
            
            if (obj_counter - last_obj_split) > max_objs_per_file:
                print "Splitting file with %s objs" % (obj_counter - last_obj_split)
                start_new_file()
                last_obj_split = obj_counter

            ways = []
        
            geom = f.GetGeometryRef()
            ring = geom.GetGeometryRef(0)
            ids = add_ring_nodes(ring)
            if not ids or len(ids) == 0: 
                f = l.GetNextFeature()
                continue
            print >>open_file, "<way id='-%s'>" % id_counter
            ways.append(id_counter) 
            id_counter += 1
            for i in ids:
                print >>open_file, "<nd ref='-%s' />" % i
            print >>open_file, "<nd ref='-%s' />" % ids[0]
            field_count = f.GetFieldCount()
            fields  = {}
            for field in range(field_count):
                value = f.GetFieldAsString(field)
                name = f.GetFieldDefnRef(field).GetName()
                if namespace and name and value and name not in boring_tags:
                    print >>open_file, "<tag k='%s:%s' v='%s' />" % (namespace, name, clean_attr(value))
                fields[name.lower()] = value
            tags={}
            for tag_name, map_value in tag_mapping:
                if hasattr(map_value, '__call__'):
                    tag_values = map_value(fields)
                    if tag_values:
                        for tag in tag_values:
                            tags[tag[0]] = tag[1]

                else:
                    if tag_name in fields:
                        tags[map_value] = fields[tag_name].title()
            for key, value in tags.items():
                if key and value:
                    print >>open_file, "<tag k='%s' v='%s' />" % (key, clean_attr(value))
                    
            for name, value in fixed_tags.items():
                print >>open_file, "<tag k='%s' v='%s' />" % (name, clean_attr(value))
            print >>open_file, "</way>"   
            if geom.GetGeometryCount() > 1:
                for i in range(1, geom.GetGeometryCount()):
                    ways.append(add_ring_way(geom.GetGeometryRef(i)))
                print >>open_file, "<relation id='-%s'><tag k='type' v='multipolygon' />" % id_counter
                id_counter += 1
                print >>open_file, '<member type="way" ref="-%s" role="outer" />' % ways[0]    
                for way in ways[1:]:
                    print >>open_file, '<member type="way" ref="-%s" role="inner" />' % way 
                print >>open_file, "</relation>"    
                
            counter += 1
            f = l.GetNextFeature()
            obj_counter += (id_counter - start_id_counter)
        
        close_file()

if __name__ == "__main__":
    if DONT_RUN:
        print __doc__
        sys.exit(2)
    
    from optparse import OptionParser
    
    parse = OptionParser(usage="%prog [args] filename.shp", version=__version__)
    parse.add_option("-s", "--slice-count", dest="slice_count", 
                     help="Number of horizontal slices of data", default=1, 
                     action="store", type="int")
    parse.add_option("-o", "--obj-count", 
                     dest="obj_count", 
                     help="Target Maximum number of objects in a single .osm file", 
                     default=50000, type="int")
    parse.add_option("-n", "--no-source", dest="no_source", 
                     help="Do not store source attributes as tags.",
                     action="store_true", default=False)
    parse.add_option("-l", "--output-location", 
                        dest="output_location", help="base filepath for output files.", 
                        default="poly_output") 
    (options, args) = parse.parse_args()
    
    if not len(args):
        print "No shapefile name given!"
        parse.print_help()
        sys.exit(3)

    kw = {}
    for key in  ('slice_count', 'obj_count', 'output_location', 'no_source'):
        kw[key] = getattr(options, key)
    
    try:
        run(args[0], **kw)   
    except AppError, E:
        print "An error occurred: \n%s" % E  
