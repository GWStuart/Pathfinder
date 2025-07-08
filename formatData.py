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

.data file:
    This file contains all road segments partitioned so that each segment starts and ends at 
    an intersection point. The first line of this file is a single number representing the total
    number of roads. Subsequent lines represent each road and start with a number representing the
    number of nodes in that given road.
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
    outputFile = "data/" + args.output
else:
    outputFile = inputFile[:inputFile.index(".")]

# open the file to be read from
try:
    with open(inputFile) as f:
        data = json.load(f)
except FileNotFoundError:
    print(f"Failed reading from {inputFile}")
    print("No such file")
    quit()

print("Processing Data")

ids = []
names = []
roads = []

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

# partition the road data at intersection points
# this ensures that each road only contains 2 intersection points (one at its start and one at its end)
partitioned_roads = []
for road in roads:
    index = 0
    for i in range(1, len(road) - 1):
        if road[i] in intersections_set:
            partitioned_roads.append(road[index:i+1])
            index = i
    partitioned_roads.append(road[index:])

# save the road data
print(f"Saving full road data to {outputFile}.data")
with open(outputFile + ".data", "w") as f:
    f.write(f"{len(partitioned_roads)}\n")
    for road in partitioned_roads:
        f.write(f"{len(road)} {str(road)}\n")

# find all node connections
neighbours = dict(zip(intersections_set, [list() for _ in range(len(intersections_set))]))
for road in partitioned_roads:
    neighbours[road[0]].append(road[-1])
    neighbours[road[-1]].append(road[0])

# save the intersection data
print(f"Saving road neighbours to {outputFile}.nodes")
with open(outputFile + ".nodes", "w") as f:
    f.write(f"{len(neighbours)}\n")
    for node, connections in neighbours.items():
        f.write(f"{str(node)} {str(connections)}\n")

# conclude program
print("Operation completed successfully")
