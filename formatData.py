import json

inputFile = "romeRoads2.geojson"  # file to be loaded

# open the file to be read from
with open(inputFile) as f:
    data = json.load(f)

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

# convert the data back to a dictionary
result = dict(zip(keys, [ids, names, coordinates]))

# write the output to a file
with open("output.json", "w") as f:
    json.dump(result, f)
