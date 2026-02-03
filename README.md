# Pathfinder
A pathfined project using real world road data and written entierly in C. 

Program implements Dijkstra's Algorithm and A* search optimised to run efficiently on large real world road networks with tens of thousands of nodes.

See below an image of the programming loading Brisbane City road data.

<img width="400" alt="BrisbaneRoadVisualised" src="https://github.com/user-attachments/assets/44368ac8-3f35-45a4-8059-bc6f6abf538b" />

## Features
This project is still a work in process however so far I have implemented:
- Geojson data parsing and cleaning (including road partitioning and neighbour parsing)
- Graphics rendering with SDL3
- Dynamic mouse panning and zooming
- Toggleable node visibility
- Efficient pathfinding using a binary heap implementation
- Animated search algorithms **(work still in progress)**
- Smooth and responsive performance even on large road networks

## Running the Code
Use the provided [Makefile](Makefile) however ensure that the following dependencies are satisffied:
- `pkg-config` (used by the Makefile to automatically pass c flags and linker flags)
- SDL3 + SDL_ttf (see [here](https://wiki.libsdl.org/SDL3/FrontPage))

## Usage

Most things are quite self explanatory but some additional information is here:
- Pan around with the mouse and zoom with the scroll wheel
- Toggle node rendering with the "n" key
- Click to select the pathfinding start and end points
- Press space to then run the pathfinder

## Loading Other City Data
I have only updated road data for the Brisbane City centre however the program is written in such a way that it will support loading in other data sets. The process of doing this involves download OpenMap data for a particular region as a GeoJson file and then processing it using the formatData.py script. I will add more details on how to do this in future. 
