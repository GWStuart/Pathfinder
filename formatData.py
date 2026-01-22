from argparse import ArgumentParser, RawDescriptionHelpFormatter
from collections import defaultdict
from sys import argv
import json
import math
"""
This file process a given geojson file and prepares it to be rendered by the visualiser.
This envolves extracting only the relevant information and scaling GPS coordinates so that all
coordinates are floating point value between 0 and 1. Additional scaling factors were needed to
prevent distortion caused by rendering spherical GPS coordinates on a 2D screen.

Both files start with a line containing a single integer that represents the total number of lines in the file.

# .nodes file:
File used to contain data for all nodes in the network.
File is structured so that it contains:
    1. a tuple containing the nodes position
    2. an array containing the indexes of all neighbouring nodes
    3. an array containing the weighting representing the distance to that node
# .roads file:
File used to contain data for all roads in the network.
File is structured as follows:
    1. two integers represeting the indexes of the start and end nodes
    2. an array of coordinates representing the road path.


I AM NOW WORKING ON AN ALTERNATIVE FILE STRUCTURE. HERE IS HOW THE DATA WILL BE
FORMATTED IN THIS NEW STRUCTURE.

# .nodes file
File used to contain all nodes in the program.
- First line contains the total number of nodes.
- Each proceeding line contains two numbers representing the nodes position.

+ also tempted to include an integer here that represents the number of edges
  that this node is attached to. Will make things easier later.

# .roads file
This file is used purely for road geometry
- First line contains the total number of roads.
- Each proceeding line contains an array of all the points along the road.

# .edges file
This is the file that ties everything together. Edges glue two nodes together
and also contain information about the road that joins them.
- Subsequent lines are formatted as: node_start_id node_end_id road_id weight

(note that with the edges the number of edges is equal to the number of roads
and so this number is only recorded in the .roads file)

"""
description = """
Tool used to process and extract road networks for openMap geojson data.
"""

parser = ArgumentParser(prog="formatData", description=description, 
                        formatter_class=RawDescriptionHelpFormatter)

parser.add_argument("file", help="geojson file to be processed")
parser.add_argument("-o", "--output", help="where to save output file")

args = parser.parse_args()

inputFile = args.file # file to be loaded

# location of output file
if args.output:
    outputFile = "assets/data/" + args.output
else:
    outputFile = inputFile[:inputFile.index(".")]

outputFile += "V2"

# open the file to be read from
try:
    with open(inputFile) as f:
        data = json.load(f)
except FileNotFoundError:
    print(f"Failed reading from {inputFile}")
    print("No such file")
    quit()

print("\nProcessing Data\n")

ids = []    # GeoJSON road id of the form wXXXXXXX
names = []  # Road names
roads = []  # Each element is a list containing tuples of road vertex coords

# loop over all entries and extract the information
for i in range(len(data["features"])):
    name = data["features"][i]["properties"]["name"]

    # check for roads without names
    if not name:
        name = ""

    # append the relevant information
    names.append(name)
    ids.append(data["features"][i]["properties"]["full_id"])
    roads.append(data["features"][i]["geometry"]["coordinates"])

# rescale the data
min_lat = min([min(line, key=lambda x: x[1])[1] for line in roads], key=lambda x: x)
max_lat = max([max(line, key=lambda x: x[1])[1] for line in roads], key=lambda x: x)
min_lon = min([min(line, key=lambda x: x[0])[0] for line in roads], key=lambda x: x)
max_lon = max([max(line, key=lambda x: x[0])[0] for line in roads], key=lambda x: x)

lat_range = max_lat - min_lat
lon_range = max_lon - min_lon

# correct aspect ratio by scaling longitude by cos(latitude)
mean_lat = (min_lat + max_lat) / 2
lon_scale_factor = math.cos(math.radians(mean_lat))
adj_lon_range = lon_range * lon_scale_factor

# scale to fit screen with padding
scale_x = 1 / adj_lon_range
scale_y = 1 / lat_range
scale = min(scale_x, scale_y)  # preserve aspect ratio

# apply the transformations to all points
for i, road in enumerate(roads):
    for j, node in enumerate(road):
        lon, lat = node
        x = (lon - min_lon) * lon_scale_factor * scale
        y = (max_lat - lat) * scale # flip Y
        roads[i][j] = x, y

# find all intersection points
coord_usage = defaultdict(int)
for road in roads:
    for point in road:
        coord_usage[point] += 1

intersections_set = {
    coord for coord, count in coord_usage.items()
    if count > 1
}
startpoints_set = {
    road[0] for road in roads
}
endpoints_set = {
    road[-1] for road in roads
}

# start and end  points are counted as "intersection" points
intersections_set.update(startpoints_set)
intersections_set.update(endpoints_set)
nodes = list(intersections_set)

# save the node data
print(f"Saving node data to {outputFile}.nodes")
with open(outputFile + ".nodes", "w") as f:
    f.write(f"{len(nodes)}\n")
    for node in nodes:
        f.write(f"{node}\n")
print("Node data saved successfully\n")

# partition the road data at intersection points
# this ensures that each road only contains 2 intersection points
# (one at its start and one at its end)
partitioned_roads = []
for road in roads:
    index = 0
    for i in range(1, len(road) - 1):
        if road[i] in intersections_set:
            partitioned_roads.append(road[index:i+1])
            index = i
    partitioned_roads.append(road[index:])

# save the road data
print(f"Saving road geometry data to {outputFile}.roads")
with open(outputFile + ".roads", "w") as f:
    f.write(f"{len(partitioned_roads)}\n")
    for road in partitioned_roads:
        f.write(f"{str(road)}\n")
print("Road data saved successfully\n")

# calculates the weight of a given road which is just the roads length then 
# scaled up arbitrarily and rounded.
def calculate_weight(road):
    prev = road[0]
    distance = 0
    for part in road[1:]:
        distance += math.dist(prev, part)
        prev = part
    return round(distance * 1000, 6)

# save the edge data
print(f"Saving edge data to {outputFile}.edges")
with open(outputFile + ".edges", "w") as f:
    for road in partitioned_roads:
        from_node = nodes.index(road[0])
        to_node = nodes.index(road[-1])
        weight = calculate_weight(road)
        f.write(f"{from_node} {to_node} {weight}\n")
print("Edge data saved successfully\n")

# conclude program
print("Operation completed successfully\n")
