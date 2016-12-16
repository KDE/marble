#!/usr/bin/python3
 
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

fixed_tags = {}
feat_dict = {}
node_dict = {}
non_geom = 0
eflag = False
 
nodes = []  #(id, lon, lat, tags)
ways = []  #(id, node_refs, tags)
relations = []  #(id, ways)

element_indices = []
 
non_polygons = ['Admin-1 aggregation', 'Admin-1 minor island', 'Admin-1 scale rank']
 
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

def bathymetry_map(data):
    elevation = 0
    if 'depth' in data:
        elevation = data['depth']
    tag = [('marble:feature', 'bathymetry'), ('ele', elevation)]
    return tag

 
def road_map(data):
    keys = {
    'Ferry Route': [('route','ferry')],
    'Major Highway': [('highway','motorway')],
    'Beltway': [('highway','primary')],
    'Track': [('highway','tertiary')],
    'Unknown': [('highway','unclassified')],
    'Secondary Highway': [('highway','trunk')],
    'Bypass': [('highway','secondary')],
    'Road': [('highway','primary')]
    }
    if 'type' in data:
        if data['type'] in keys:
            return keys[data['type']]
 
def city_map(data):
    population = 0
    capital = 'no'
    country = ''
    if data['featurecla'] == 'Admin-0 capital' or data['featurecla'] == 'Admin-1 capital' or data['featurecla'] == 'Admin-0 region capital' or data['featurecla'] == 'Admin-1 region capital':
        capital = 'yes'
    if 'pop_max' in data:
        population = data['pop_max']
    if 'adm0name' in data:
        country = data['adm0name']

    # National capitals in OSM are recongised by tag "admin_level"="2" ( http://wiki.openstreetmap.org/wiki/Key:capital#Using_relations_for_capitals ).
    # In Natural Earth .shp files these capitals are tagged as "Admin-0 capital".
    if data['featurecla'] == 'Admin-0 capital':
        admin_level = "2"
        temp = [('is_in:country', country), ('capital', capital), ('admin_level', admin_level), ('population', population), ('place', 'city')]
    else:
        temp = [('is_in:country', country), ('capital', capital), ('population', population), ('place', 'city')]

    return temp

def mountain_map(data):
    elevation = 0
    if 'elevation' in data:
        elevation = data['elevation']
    temp = [('natural', 'peak'), ('ele', elevation)]
    return temp
 
def feature_class(data):
    global non_fcla_dict
    keys = {
    'Lake': [('natural', 'water')],
    'Alkaline Lake': [('natural', 'water')],
    'Reservoir': [('natural', 'water')],
    'Road': [(road_map,None)],
    'Ferry': [('route','ferry')],
    'River': [('waterway', 'river')],
    'Coastline': [('natural', 'coastline')],
    'Minor coastline': [('natural', 'coastline')],
    'Ocean': [('natural', 'water')],
    'Land': [('marble_land', 'landmass')],
    'Minor island': [('marble_land', 'landmass'), ('place', 'island')],
    'Reefs': [('natural', 'reef')],
    'Admin-0 country': [('marble_land', 'landmass')],
    'Admin-0 sovereignty': [('marble_land', 'landmass')],
    'Admin-0 map unit': [('marble_land', 'landmass')],
    'Adm-0 scale ranks': [('marble_land', 'landmass')],
    'International boundary (verify)': [('boundary', 'administrative'), ('admin_level', '2')],
    'Overlay limit': [('boundary', 'administrative'), ('admin_level', '2')],
    'Disputed (please verify)': [('boundary', 'administrative'), ('admin_level', '2')],
    'Line of control (please verify)': [('boundary', 'administrative'), ('admin_level', '2')],
    'Indefinite (please verify)': [('boundary', 'administrative'), ('admin_level', '2')],
    'Lease limit': [('boundary', 'administrative'), ('admin_level', '2')],
    'Indeterminant frontier': [('boundary', 'administrative'), ('admin_level', '2')],
    'Admin-0 lease': [('marble_land', 'landmass')],
    'Admin-0 claim area': [('marble_land', 'landmass')],
    'Admin-0 breakaway and disputed': [('marble_land', 'landmass')],
    'Admin-0 overlay': [('marble_land', 'landmass')],
    'Admin-0 indeterminant': [('marble_land', 'landmass')],
    'Admin-1 aggregation': [('boundary', 'administrative'), ('admin_level', '4')],
    'Admin-1 minor island': [('boundary', 'administrative'), ('admin_level', '4')],
    'Admin-1 scale rank': [('boundary', 'administrative'), ('admin_level', '4')],
    'Admin-1 region boundary': [('boundary', 'administrative'), ('admin_level', '4')],
    'Railroad': [('railway', 'rail')],
    'Railroad ferry': [('route', 'ferry')],
    'Urban area': [('settlement', 'yes')],
    'Timezone': [('marble_land', 'landmass')],
    'Historic place': [(city_map,None)],
    'Populated place': [(city_map,None)],
    'Scientific station': [(city_map,None)],
    'Meteorological Station': [(city_map,None)],
    'Admin-0 capital': [(city_map,None)],
    'Admin-1 capital': [(city_map,None)],
    'Admin-0 region capital': [(city_map,None)],
    'Admin-1 region capital': [(city_map,None)],
    'Admin-0 capital alt': [(city_map,None)],
    'Lake Centerline': [('waterway', 'river')],
    'Port': [('harbour', 'yes')],
    'Island': [('marble_land', 'landmass'), ('place', 'island')],
    'Island group': [('marble_land', 'landmass'), ('place', 'island')],
    'Wetlands': [('natural', 'wetland')],
    'Basin': [('landuse', 'basin')],
    'Desert': [('natural', 'desert')],  #no tag support in marble, no clue as to how to render
    'Depression': [('natural', 'sinkhole')],  #no tag support in marble, no clue as to how to render
    'Range/mtn': [('marble_land', 'landmass')], # no clue as to how to render
    'Geoarea': [('marble_land', 'landmass')], # no clue as to how to render
    'Plain' : [('marble_land', 'landmass')], # no clue as to how to render
    'Tundra': [('natural', 'tundra')], #no tag support in marble, no clue as to how to render
    'Foothills' : [('marble_land', 'landmass')], #no tag support in OSM, nothing in Marble, no clue as to how to render
    'Lowland' : [('marble_land', 'landmass')], #no tag support in OSM, nothing in Marble, no clue as to how to render
    'Dragons-be-here': [('marble_land', 'landmass')], #no tag support in OSM, nothing in Marble, no clue as to how to render
    'Delta': [('marble_land', 'landmass')], #no tag support in OSM, nothing in Marble, no clue as to how to render
    'Peninsula':  [('marble_land', 'landmass')],
    'Plateau' : [('marble_land', 'landmass')],
    'Pen/cape' : [('natural', 'cape')], #osm tag still in proposal stage, no clue as to how to render
    'Gorge': [('marble_land', 'landmass')],  #no tag support in OSM, nothing in Marble, no clue as to how to render
    'Coast':  [('marble_land', 'landmass')], #no tag support in OSM, nothing in Marble, no clue as to how to render
    'Continent':  [('marble_land', 'landmass')], #no tag support in OSM, nothing in Marble, redundant data
    'Isthmus': [('marble_land', 'landmass')], #no tag support in OSM, nothing in Marble, no clue as to how to render
    'Valley': [('natural', 'valley')],  #no tag support in marble
    'waterfall' : [('waterway', 'waterfall')], #no tag support in marble
    'cape': [('natural', 'cape')], #osm tag still in proposal stage, no clue as to how to render      'NONE'
    'pole': [],
    'plain': [],
    'island group': [],
    'island': [],
    'mountain': [(mountain_map,None)], #this tag has to be mapped to Mountain feature of Marble
    'spot elevation': [],
    'plateau': [],
    'depression': [],
    'pass': [],
    'sound': [('natural', 'water')],
    'river':  [('waterway', 'river')],
    'generic': [('natural', 'water')], #still have to confirm what generic means exactly in marine_polygons?
    'lagoon': [('natural', 'water'), ('water', 'lagoon')], 
    'reef': [('natural', 'reef')],
    'gulf': [('natural', 'water')],
    'inlet': [('natural', 'water')],
    'strait': [('natural', 'water')],
    'bay': [('natural', 'water')],
    'fjord': [('natural', 'water')],
    'sea': [('natural', 'water')],  
    'ocean': [('natural', 'water')],
    'channel': [('natural', 'water')],
    'Playa': [('natural', 'water'), ('water', 'lake'), ('salt', 'yes')],
    #glacier:type is a proposed OSM tag - http://wiki.openstreetmap.org/wiki/Proposed_features/Glaciers_tags
    'Antarctic Ice Shelf': [('natural', 'glacier'), ('glacier:type','shelf')], 
    'Antarctic Ice Shelf Edge': [('glacier:edge', 'calving_line')], #marble does not support this osm tag
    'Glaciated areas': [('natural', 'glacier')],
    'Admin-0 Tiny Countries': [],
    'Admin-0 Tiny GeoUnit': [],
    'Admin-0 Tiny GeoSubunit': [],
    'Admin-0 Tiny Countries Pacific': [],
    'Pacific Groupings': [],
    'Admin-1 boundary': [('boundary', 'administrative'), ('admin_level', '4')],
    'Map unit boundary':[],
    'Marine Indicator Treaty':[('boundary', 'administrative'), ('admin_level', '2'), ('maritime', 'yes'), ('border_type', 'territorial')],
    'Marine Indicator Median':[('boundary', 'administrative'), ('admin_level', '2'), ('maritime', 'yes'), ('border_type', 'territorial')],
    'Boundary Treaty':[('boundary', 'administrative'), ('admin_level', '2'), ('maritime', 'yes'), ('border_type', 'territorial')],
    'Marine Indicator 200 mi nl':[('boundary', 'maritime'), ('border_type', 'eez')],
    'Marine Indicator Disputed':[('boundary', 'administrative'), ('admin_level', '2'), ('maritime', 'yes'), ('border_type', 'territorial'), ('marble:disputed', 'yes')],
    'Claim boundary': [('boundary', 'administrative'), ('admin_level', '2')],
    'Reference line': [('boundary', 'administrative'), ('admin_level', '2')],
    'Breakaway': [('boundary', 'administrative'), ('admin_level', '2')],
    'Elusive frontier': [('boundary', 'administrative'), ('admin_level', '2')],
    'Country': [('marble_land', 'landmass')],
    '1st Order Admin Lines': [('boundary', 'administrative'), ('admin_level', '4')],
    'Claim': [('boundary', 'administrative'), ('admin_level', '4')],
    'Airport': [('aeroway', 'aerodrome')],
    'Date line': [('marble_line', 'date')],
    'Bathymetry': [(bathymetry_map,None)]
    }

    if 'featurecla' in data:
        if data['featurecla'] in feat_dict:
            feat_dict[data['featurecla']] += 1
        else:
            feat_dict[data['featurecla']] = 1
        if data['featurecla'] in keys:
            if len(keys[data['featurecla']]) == 0:
                return keys[data['featurecla']]
            if hasattr(keys[data['featurecla']][0][0], '__call__'):
                return keys[data['featurecla']][0][0](data)
            else:
                return keys[data['featurecla']]
        else:
            if data['featurecla'] in non_fcla_dict:
                non_fcla_dict[data['featurecla']] += 1
            else:
                non_fcla_dict[data['featurecla']] = 1
 
 
 
def name_map(data):
    if 'name' in data:
        return [('name', data['name'])]
 
 
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
        print("Invalid date: %s" % date)
        eflag = True
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
    ('featurecla', feature_class),
    ('name', name_map)
]    
 
# These tags are not exported, even with the source data; this should be
# used for tags which are usually calculated in a GIS. AREA and LEN are
# common.
 
boring_tags = [ 'AREA', 'LEN', 'GIS_ACRES']
 
# Namespace is used to prefix existing data attributes. If 'None', or 
# '--no-source' is set, then source attributes are not exported, only
# attributes in tag_mapping.
 
namespace = "natural_earth"
#namespace = None 
 
# Uncomment the "DONT_RUN = False" line to get started. 
 
#DONT_RUN = True
DONT_RUN = False
 
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
        eflag = True
except ImportError:
    __doc__ += gdal_install 
    if DONT_RUN:
        print(__doc__)
        sys.exit(2)
    print("OGR Python Bindings not installed.\n%s" % gdal_install)
    sys.exit(1)
    eflag = True
 
def close_file():
    """ Internal. Close an open file."""
    global open_file
    if not open_file.closed: 
        open_file.write("</osm>")
        open_file.close()
 
def start_new_file():
    """ Internal. Open a new file, closing existing file if neccesary."""
    global open_file, file_counter, node_dict, file_name
    file_counter += 1
    if open_file:
        close_file()
    open_file = open("%s.%s.osm" % (file_name, file_counter), "w")
    print("<?xml version='1.0' encoding='UTF-8'?>" , end = '\n', file = open_file) 
    print("<osm version='0.5'>" , end = '\n', file = open_file)  
    node_dict = {}

def write_osm_files():
    global nodes, ways, relations, element_indices, open_file, file_counter, file_name
    current = [0, 0, 0]
    previous = [0, 0, 0]
    for indices in element_indices:
        start_new_file()
        current[0] = indices[0] - 1
        current[1] = indices[1] - 1
        current[2] = indices[2] - 1
        while current[0] >= previous[0]:
            write_node(nodes[current[0]])
            current[0] -= 1
        while current[1] >= previous[1]:
            write_way(ways[current[1]])
            current[1] -= 1
        while current[2] >= previous[2]:
            write_relation_multipolygon(relations[current[2]])
            current[2] -= 1
        previous = indices[:]
        close_file()
    element_indices = []
 
def clean_attr(val):
    """Internal. Hacky way to make attribute XML safe."""
    val = str(val)
    val = val.replace("&", "&amp;").replace("'", "&quot;").replace("<", "&lt;").replace(">", "&gt;").strip()
    return val

def check_featurecla(f):
    """
     Checks if featurecla field is present in the feature f.
     If present it implies that shp data is from Natural Earth dataset
    """
    if 'featurecla' in f.keys():
        return True
    else:
        return False
 
def add_point(f):
    """Adds a point geometry to the OSM file"""
    global id_counter
    airport_metadata = None
    pt = f.GetGeometryRef()
    if check_featurecla(f):
        if f['featurecla'] == 'Airport':
            airport_metadata = f
            f = None
    node_id = add_node(id_counter, pt.GetX(0), pt.GetY(0), 'POINT', f)
    if node_id == id_counter:
        id_counter += 1
    if airport_metadata != None:
        add_way_around_node(airport_metadata)
    
    
 
def add_relation_multipolygon(geom, f):
    """ Writes the multipolygon relation to the OSM file, returns 0 if no relation is formed"""
    global id_counter, file_counter, counter, file_name, open_file, namespace
    rel_ways = []
    rel_id = 0
    way_id = add_way(geom.GetGeometryRef(0), f, True)
    if way_id == None:
        print('Error in writing relation')
        return None
    rel_ways.append(way_id)
 
    if geom.GetGeometryCount() > 1:
        for i in range(1, geom.GetGeometryCount()):
            way_id = add_way(geom.GetGeometryRef(i), f, False)
            if way_id == None:
                print('Error in writing relation')
                return None
            rel_ways.append(way_id)
        rel_id = id_counter
        if check_featurecla(f):
            if f['featurecla'] in non_polygons:
                return 0 #means no relation is there
        relations.append((rel_id, rel_ways))
        id_counter += 1
 
    return rel_id   #if rel_id return 0, means no relations is there
 
def write_relation_multipolygon(relation):
    global open_file
    print("<relation id='-%s'><tag k='type' v='multipolygon' />" % relation[0] , end = '\n', file = open_file)  
    print('<member type="way" ref="-%s" role="outer" />' % relation[1][0] , end = '\n', file = open_file)    
    for way in relation[1][1:]:
        print('<member type="way" ref="-%s" role="inner" />' % way  , end = '\n', file = open_file)
    print("</relation>" , end = '\n', file = open_file)
 
def write_tags(f):
    """Writes the tags associated with a way or a relation"""
    global id_counter, file_counter, counter, file_name, open_file, namespace
    field_count = f.GetFieldCount()
    fields  = {}
    for field in range(field_count):
        value = f.GetFieldAsString(field)
        name = f.GetFieldDefnRef(field).GetName()
        if namespace and name and value and name not in boring_tags:
            print(" <tag k='%s:%s' v='%s' />" % (namespace, name, clean_attr(value)) , end = '\n', file = open_file)
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
            print(" <tag k='%s' v='%s' />" % (key, clean_attr(value))  , end = '\n', file = open_file)   
    for name, value in fixed_tags.items():
        print(" <tag k='%s' v='%s' />" % (name, clean_attr(value)) , end = '\n', file = open_file)   
    if f.GetGeometryRef().GetGeometryName() == 'POLYGON' or f.GetGeometryRef().GetGeometryName() == 'MULTIPOLYGON':
        if check_featurecla(f):
            if f['featurecla'] not in non_polygons:
                print(" <tag k='area' v='yes' />" , end = '\n', file = open_file)  
 
def add_way(geom, f, tag_flag):
    """ Writes the way of a particular geometry to the OSM file"""
    global open_file, id_counter, ways
    ids = add_way_nodes(geom, f)
    if len(ids) == 0:
        print('Error in writing way')
        return None
    way_id = id_counter
    id_counter += 1
    node_refs = ids
    if tag_flag:
        tags = f
    else:
        tags = None
    ways.append((way_id, node_refs, tags))
    return way_id
 
def write_way(way):
    global open_file
    print("<way id='-%s'>" % way[0] , end = '\n', file = open_file)  
    for i in way[1]:
        print(" <nd ref='-%s' />" % i , end = '\n', file = open_file)  
    if way[2]:
        write_tags(way[2])
    print("</way>", end = '\n', file = open_file)  
 
def add_way_nodes(geom, f):
    """Writes the nodes of a particular way"""
    global open_file, id_counter
    ids = []
    geom_name = geom.GetGeometryName()
    pt_count = geom.GetPointCount()
    if geom_name == 'LINESTRING':
        range_count = range(geom.GetPointCount())
    else:
        range_count = range(geom.GetPointCount() - 1)
    if range_count == 0 or pt_count == 0:
        print( "Degenerate ", geom_name , end = '\n', file = sys.stderr)  
        return
    #if geom_name != 'LINESTRING':
    #    pt_count -= 1
    for count in range(pt_count):
        node_id = add_node(id_counter, geom.GetX(count), geom.GetY(count), geom_name, f)
        if node_id == id_counter:   #means a new node is created, if not means node already exists
            id_counter += 1
        ids.append(node_id)
    return ids   
 
 
def add_node(num_id, lon, lat, geom_name, f):
    """ Writes the node to the OSM file"""
    global open_file, node_dict
    key = (lon, lat)
    if geom_name == 'POINT':
        nodes.append((num_id, lon, lat, f))
        node_dict[key] = num_id
    else:
        if key in node_dict:
            num_id = node_dict[key]
        else:
            nodes.append((num_id, lon, lat, None))
            node_dict[key] = num_id
    return num_id
 
def write_node(node):
    global open_file
    if node[3] == None:
        print("<node id='-{}' visible='true' lon='{:.10f}' lat='{:.10f}' />".format(node[0], node[1], node[2]), end = '\n', file = open_file)   
    else:
        print("<node id='-{}' visible='true' lon='{:.10f}' lat='{:.10f}' >".format(node[0], node[1], node[2]), end = '\n', file = open_file)
        write_tags(node[3]) 
        print("</node>", end = '\n', file = open_file)
 
def add_way_around_node(f):
    """ Writes a way around a single point"""
    global id_counter, ways
    nid = id_counter - 1
    ways.append((id_counter, [nid], f))
    id_counter += 1
    
 
open_file = None
 
file_name = None 
 
id_counter = 1
 
file_counter = 0
counter = 0
 
geom_counter = {}
 
class AppError(Exception): pass
 
def run(filenames, slice_count=1, obj_count=5000000, output_location=None, no_source=False):
    """Run the converter. Requires open_file, file_name, id_counter,
    file_counter, counter to be defined in global space; not really a very good
    singleton."""
    global id_counter, file_counter, counter, file_name, open_file, namespace, non_geom, non_fcla_dict, nodes, ways, relations, geom_counter
    open_file = None
 
    file_name = None 
 
    id_counter = 1
 
    file_counter = 0
    counter = 0
 
    geom_counter = {}
    node_dict = {}
    if output_location:
       file_name = output_location
    # start_new_file()
    for filename in filenames:
        non_geom = 0
        non_fcla_dict = {}
        
        if no_source:
            namespace=None
 
        ds = ogr.Open(filename)
        if not ds:
            raise AppError("OGR Could not open the file %s" % filename)
            eflag = True
        l = ds.GetLayer(0)
       
        max_objs_per_file = obj_count 
 
        extent = l.GetExtent()
        #if extent[0] < -180 or extent[0] > 180 or extent[2] < -90 or extent[2] > 90:
        #   raise AppError("Extent does not look like degrees; are you sure it is? \n(%s, %s, %s, %s)" % (extent[0], extent[2], extent[1], extent[3]))  
        slice_width = (extent[1] - extent[0]) / slice_count
 
        seen = {}
 
        print("Running %s slices with %s base filename against shapefile %s" % (
                slice_count, file_name, filename))
 
        for i in range(slice_count): 
 
            l.ResetReading()
            l.SetSpatialFilterRect(extent[0] + slice_width * i, extent[2], extent[0] + (slice_width * (i + 1)), extent[3])
 
            #start_new_file()
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
                    print("Splitting file with %s objs" % (obj_counter - last_obj_split))
                    #start_new_file()
                    last_obj_split = obj_counter
                    element_indices.append((len(nodes), len(ways), len(relations)))
                    node_dict = {} 
                feat_dict = f.items()
                geom = f.GetGeometryRef()
                geom_name = geom.GetGeometryName()
                if geom_name in geom_counter:
                    geom_counter[geom_name] += 1
                else:
                    geom_counter[geom_name] = 1
                if geom_name == 'POLYGON':
                    rel_id = add_relation_multipolygon(geom, f)
                    if rel_id == None:
                        f = l.GetNextFeature()
                        continue
                elif geom_name == 'LINESTRING':
                    way_id = add_way(geom, f, True)
                    if way_id == None:
                        f = l.GetNextFeature()
                        continue
                elif geom_name == 'MULTILINESTRING':
                    for i in range(geom.GetGeometryCount()):
                        way_id = add_way(geom.GetGeometryRef(i), f, True)
                        if way_id == None:
                            f = l.GetNextFeature()
                            continue
                elif geom_name == 'MULTIPOLYGON':
                    for i in range(geom.GetGeometryCount()):
                        rel_id = add_relation_multipolygon(geom.GetGeometryRef(i), f)
                        if rel_id == None:
                            f = l.GetNextFeature()
                            continue
                elif geom_name == 'POINT':
                    add_point(f)
                else:
                    ids = []
                    non_geom += 1       
                        
                counter += 1
                f = l.GetNextFeature()
                obj_counter += (id_counter - start_id_counter)
 
    # for node in nodes:
    #     write_node(node)
    # for way in ways:
    #     write_way(way)
    # for relation in relations:
    #     write_relation_multipolygon(relation)

    element_indices.append((len(nodes), len(ways), len(relations))) 
    write_osm_files()
                    
    # close_file()
    nodes = []  #(id, lon, lat, tags)
    ways = []  #(id, node_refs, tags)
    relations = []  #(id, ways)
 
if __name__ == "__main__":
    if DONT_RUN:
        print(__doc__)
        sys.exit(2)
    
    from optparse import OptionParser
    
    parse = OptionParser(usage="%prog [args] filename.shp", version=__version__)
    parse.add_option("-s", "--slice-count", dest="slice_count", 
                     help="Number of horizontal slices of data", default=1, 
                     action="store", type="int")
    parse.add_option("-o", "--obj-count", 
                     dest="obj_count", 
                     help="Target Maximum number of objects in a single .osm file", 
                     default=5000000, type="int")
    parse.add_option("-n", "--no-source", dest="no_source", 
                     help="Do not store source attributes as tags.",
                     action="store_true", default=False)
    parse.add_option("-l", "--output-location", 
                        dest="output_location", help="base filepath for output files.", 
                        default="poly_output") 
    (options, args) = parse.parse_args()
    
    if not len(args):
        print("No shapefile name given!")
        parse.print_help()
        sys.exit(3)
 
    kw = {}
    for key in  ('slice_count', 'obj_count', 'output_location', 'no_source'):
        kw[key] = getattr(options, key)
    
    try:
        run(args, **kw)
    except AppError as E:
        print("An error occurred: \n%s" % E)
        eflag = True
 
    print() 
    print('Geometry types present: ')
    for key in geom_counter:
        print(key, geom_counter[key])
    print() 
    print('Feature type present: ')
    for key in feat_dict:
        print(key, feat_dict[key])
    print()
 
    
    if eflag:
        print('Conversion not Successful :')
    else:
        if len(non_fcla_dict) == 0 and non_geom == 0:
            print('Conversion Successful')
        else:
            print('Conversion not Successful :')
            if len(non_fcla_dict) != 0:
                print('Unknown features present in SHP file: ', len(non_fcla_dict))
                print() 
                for key in non_fcla_dict:
                    print(key, non_fcla_dict[key])
            if non_geom != 0:
                print('Unknown geometry present in SHP file: ', non_geom)
    
 
