# Pathfinder
A pathfined project using real world road data

This project is still a work in process however so far I have implemented:
- geojson data parsing and cleaning
- road partitioning at intersection points and calculating road neighbours
- loading data into C
- rendering road networks using SDL3
- dynamic mouse panning and zooming to examine the road network

See the below image of the ./vis program loading in Brisbane City road data.
<img width="798" height="599" alt="BrisbaneRoadVisualised" src="https://github.com/user-attachments/assets/44368ac8-3f35-45a4-8059-bc6f6abf538b" />

## Usage
- pan around by clicking and dragging the mouse
- zoom using the scroll wheel
- toggle rendering road intersection points with the "n" key
- query the program's memory usage using the "m" key (mostly for debug purposes)

## Loading Other City Data
I have only updated road data for the Brisbane City centre however the program is written in such a way that it will support loading in other data sets. The process of doing this involves download OpenMap data for a particular region as a GeoJson file and then processing it using the formatData.py script. I will add more details on how to do this in future. 
