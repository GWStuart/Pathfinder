import pygame
import math
from collections import defaultdict

inputFile = "output2.data"

pygame.init()

LENGTH, HEIGHT = 800, 600
win = pygame.display.set_mode((LENGTH, HEIGHT))
pygame.display.set_caption("visualiser")
clock = pygame.time.Clock()

roads = []
with open(inputFile, "r") as f:
    for line in f.readlines():
        roads.append(eval(line[line.index(" ") + 1:-1])) # append the road

#minX = min([min(line, key=lambda x: x[0])[0] for line in roads], key=lambda x: x)
#maxX = max([max(line, key=lambda x: x[0])[0] for line in roads], key=lambda x: x)
#minY = min([min(line, key=lambda x: x[1])[1] for line in roads], key=lambda x: x)
#maxY = max([max(line, key=lambda x: x[1])[1] for line in roads], key=lambda x: x)

#def prepare_gps_to_screen(roads, screen_width, screen_height, padding=50):
#    all_lats = []
#    all_lons = []
#
#    for road in roads:
#        for lon, lat in road:  # OpenStreetMap uses (lon, lat)
#            all_lats.append(lat)
#            all_lons.append(lon)
#
#    min_lat = min(all_lats)
#    max_lat = max(all_lats)
#    min_lon = min(all_lons)
#    max_lon = max(all_lons)
#
#    lat_range = max_lat - min_lat
#    lon_range = max_lon - min_lon
#
#    # Scale to fit screen while preserving aspect ratio
#    scale_x = (screen_width - 2 * padding) / lon_range
#    scale_y = (screen_height - 2 * padding) / lat_range
#    scale = min(scale_x, scale_y)
#
#    def gps_to_screen(point):
#        lon, lat = point
#        x = (lon - min_lon) * scale + padding
#        y = (max_lat - lat) * scale + padding  # Flip Y so north is up
#        return int(x), int(y)
#
#    return gps_to_screen

def prepare_gps_to_screen(roads, screen_width, screen_height, padding=50):
    all_lats = []
    all_lons = []

    for road in roads:
        for lon, lat in road:  # lon, lat
            all_lats.append(lat)
            all_lons.append(lon)

    min_lat = min(all_lats)
    max_lat = max(all_lats)
    min_lon = min(all_lons)
    max_lon = max(all_lons)

    lat_range = max_lat - min_lat
    lon_range = max_lon - min_lon

    # Correct aspect ratio by scaling longitude by cos(latitude)
    mean_lat = (min_lat + max_lat) / 2
    lon_scale_factor = math.cos(math.radians(mean_lat))

    adj_lon_range = lon_range * lon_scale_factor

    # Scale to fit screen with padding
    scale_x = (screen_width - 2 * padding) / adj_lon_range
    scale_y = (screen_height - 2 * padding) / lat_range
    scale = min(scale_x, scale_y)  # preserve aspect ratio

    def gps_to_screen(point):
        lon, lat = point
        x = (lon - min_lon) * lon_scale_factor * scale + padding
        y = (max_lat - lat) * scale + padding  # flip Y
        return int(x), int(y)

    return gps_to_screen

convert = prepare_gps_to_screen(roads, 800, 600)

for i, road in enumerate(roads):
    for j, node in enumerate(road):
        roads[i][j] = convert(node)

## transform the points
## in future it would probably be best to do this in the format data file
#scale = max(maxX - minX, maxY - minY)
#for i, road in enumerate(roads):
#    for j, node in enumerate(road):
#        roads[i][j] = [(node[0] - minX) / scale * HEIGHT, (node[1] - minY) / scale * HEIGHT]
#        roads[i][j][1] = HEIGHT - roads[i][j][1]
#
cameraX = 0
cameraY = 0
#
def get_local_coordinates(coordinates: list[int, int]):
    return coordinates[0] - cameraX, coordinates[1] - cameraY


# accroding to GPT: 4 -> 11m, 5 -> 1.1m, 6 -> 11cm
def round_coord(coord, digits=5):
    return (round(coord[0], digits), round(coord[1], digits))  # (lon, lat)

coord_usage = defaultdict(int)

for road in roads:
    for point in road:
        coord = round_coord(point)
        coord_usage[coord] += 1

intersections = {
    coord for coord, count in coord_usage.items()
    if count > 1
}

run = True
while run:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            quit()
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_q:
                pygame.quit()
                quit()

    keys = pygame.key.get_pressed()
    if (keys[pygame.K_UP]):
        for i, road in enumerate(roads):
            for j, node in enumerate(road):
                roads[i][j] = [node[0], node[1] * 0.999]
    if (keys[pygame.K_DOWN]):
        for i, road in enumerate(roads):
            for j, node in enumerate(road):
                roads[i][j] = [node[0], node[1] * 1.001]
    if (keys[pygame.K_LEFT]):
        for i, road in enumerate(roads):
            for j, node in enumerate(road):
                roads[i][j] = [node[0] * 0.999, node[1]]
    if (keys[pygame.K_RIGHT]):
        for i, road in enumerate(roads):
            for j, node in enumerate(road):
                roads[i][j] = [node[0] * 1.001, node[1]]

    if (keys[pygame.K_z]):
        for i, road in enumerate(roads):
            for j, node in enumerate(road):
                roads[i][j] = [node[0] * 1.001, node[1] * 1.001]
    if (keys[pygame.K_x]):
        for i, road in enumerate(roads):
            for j, node in enumerate(road):
                roads[i][j] = [node[0] * 0.999, node[1] * 0.999]

    if (keys[pygame.K_w]):
        cameraY += 1
    if (keys[pygame.K_a]):
        cameraX += 1
    if (keys[pygame.K_s]):
        cameraY -= 1
    if (keys[pygame.K_d]):
        cameraX -= 1

    win.fill((255, 255, 255))

    for i, road in enumerate(roads):
        colour = (0, 0, 0)
        pygame.draw.lines(win, colour, False, list(map(get_local_coordinates, road)), 3);
#        for node in road:
#            pygame.draw.circle(win, (255, 0, 0), get_local_coordinates(node), 1)
        #pygame.draw.lines(win, colour, False, road, 3);

    for point in intersections:
        x, y = get_local_coordinates(point)
        pygame.draw.circle(win, (255, 0, 0), (x, y), 3)


    pygame.display.update()
    clock.tick(60)
