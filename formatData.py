from argparse import ArgumentParser, RawDescriptionHelpFormatter
from sys import argv
import json
import math
"""
This file process a given geojson file and prepares it to be rendered by the visualiser.
This envolves extracting only the relevant information and scaling GPS coordinates so that all
coordinates are floating point value between 0 and 1. Additional scaling factors were needed to
prevent distortion caused by rendering spherical GPS coordinates on a 2D screen.
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
    outputFile = inputFile[:inputFile.index(".")] + ".data"

# open the file to be read from
try:
    with open(inputFile) as f:
        data = json.load(f)
except FileNotFoundError:
    print(f"Failed reading from {inputFile}")
    print("No such file")
    quit()

print("Processing Data")

keys = ["ids", "names", "coordinates"]
ids = []
names = []
coordinates = []

# loop over all entries and extract the information
for i in range(len(data["features"])):
    name = data["features"][i]["properties"]["name"]

    # check for roads without names
    if not name:
        name = ""

    # append the relevant information
    names.append(name)
    ids.append(data["features"][i]["properties"]["full_id"])
    coordinates.append(data["features"][i]["geometry"]["coordinates"])

# process the data
min_lat = min([min(line, key=lambda x: x[1])[1] for line in coordinates], key=lambda x: x)
max_lat = max([max(line, key=lambda x: x[1])[1] for line in coordinates], key=lambda x: x)
min_lon = min([min(line, key=lambda x: x[0])[0] for line in coordinates], key=lambda x: x)
max_lon = max([max(line, key=lambda x: x[0])[0] for line in coordinates], key=lambda x: x)

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
for i, road in enumerate(coordinates):
    for j, node in enumerate(road):
        lon, lat = node
        x = (lon - min_lon) * lon_scale_factor * scale
        y = (max_lat - lat) * scale # flip Y
        coordinates[i][j] = x, y

print(f"Saving output to {outputFile}")

with open(outputFile, "w") as f:
    for i in range(len(ids)):
        f.write(f"{ids[i]} {str(coordinates[i])}\n")

print("Operation completed successfully")
