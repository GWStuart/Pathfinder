import pygame
import math

pygame.init()

win = pygame.display.set_mode((800, 600))
pygame.display.set_caption("Shape Cutting")
clock = pygame.time.Clock()

roads = []
with open("roads.data", "r") as f:
    for line in f.readlines():
        roads.append(eval(line[line.index(" ") + 1:-1])) # append the road

minX = min([min(line, key=lambda x: x[0])[0] for line in roads], key=lambda x: x)
maxX = max([max(line, key=lambda x: x[0])[0] for line in roads], key=lambda x: x)
minY = min([min(line, key=lambda x: x[1])[1] for line in roads], key=lambda x: x)
maxY = max([max(line, key=lambda x: x[1])[1] for line in roads], key=lambda x: x)

print(minX, maxX, minY, maxY)
#
#def get_scale(min_lat, max_lat, min_lon, max_lon, screen_width, screen_height):
#    lat_center = (min_lat + max_lat) / 2
#    lat_scale = screen_height / (max_lat - min_lat)
#    lon_scale = screen_width / ((max_lon - min_lon) * math.cos(math.radians(lat_center)))
#    return min(lat_scale, lon_scale)
#
#def gps_to_screen(lat, lon, min_lat, min_lon, max_lat, scale, screen_width, screen_height):
#    lat_center = (min_lat + max_lat) / 2
#    x = (lon - min_lon) * scale * math.cos(math.radians(lat_center))
#    y = (lat - min_lat) * scale
#    y = screen_height - y  # Flip y-axis
#
#    return int(y), 600 - int(x)
#
#scale = get_scale(minX, maxX, minY, maxY, 800, 600)
        #lines[i][j] = [(point[0] - minX) / diff * 800, (point[1] - minY) / diff * 800]
        #gps_to_screen(point[0], point[1], minX, minY, maxX, scale, 800, 600)
        #lines[i][j] = [point[0], point[1] / math.cos(math.radians(lines[0][0][0]))]

# transform the points
# in future it would probably be best to do this in the format data file
scale = max(maxX - minX, maxY - minY)
for i, road in enumerate(roads):
    for j, node in enumerate(road):
        roads[i][j] = [(node[0] - minX) / scale * 600, (node[1] - minY) / scale * 600]

#
#print(lines)
#
#
#diffX = maxX - minX
#diffY = maxY - minY
#diff = max(diffX, diffY)
#
##for i, line in enumerate(lines):
##    for j, point in enumerate(line):
##        #lines[i][j] = [(point[0] - minX) / diff * 800, (point[1] - minY) / diff * 800]
##        lines[i][j] = [(point[0] - minX) / diff * 800, (-point[1] + maxY) / diff * 800]
#
#dx = 0
#dy = 0
#
run = True
while run:
    for events in pygame.event.get():
        if events.type == pygame.QUIT:
            pygame.quit()
            quit()

    keys = pygame.key.get_pressed()
#    if (keys[pygame.K_UP]):
#        for i, line in enumerate(lines):
#            for j, point in enumerate(line):
#                dy = point[1]*1.001 - point[1]
#                lines[i][j] = [point[0] * 1.001, point[1] * 1.001]
#    if (keys[pygame.K_DOWN]):
#        for i, line in enumerate(lines):
#            for j, point in enumerate(line):
#                dy = point[1]*0.999 - point[1]
#                lines[i][j] = [point[0] * 0.999, point[1] * 0.999]
#    if (keys[pygame.K_LEFT]):
#        for i, line in enumerate(lines):
#            for j, point in enumerate(line):
#                dx = point[0]*0.999 - point[0]
#                lines[i][j] = [point[0] * 0.999, point[1]]
#    if (keys[pygame.K_RIGHT]):
#        for i, line in enumerate(lines):
#            for j, point in enumerate(line):
#                dx = point[0]*1.001 - point[0]
#                lines[i][j] = [point[0] * 1.001, point[1]]
#    if (keys[pygame.K_SPACE]):
#        print(dx, dy)

    win.fill((255, 255, 255))

    for i, road in enumerate(roads):
        colour = (0, 0, 0)
        if (i == 0): colour = (255, 0, 0)
        if (i == 1): colour = (0, 255, 0)
        if (i == 49): colour = (0, 0, 255)
        pygame.draw.lines(win, colour, False, road, 3);

    pygame.display.update()
    clock.tick(60)
