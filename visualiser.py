import pygame
import math
from collections import defaultdict

inputFile = "output3.data"

pygame.init()

LENGTH, HEIGHT = 800, 600
win = pygame.display.set_mode((LENGTH, HEIGHT))
pygame.display.set_caption("visualiser")
clock = pygame.time.Clock()

roads = []
with open(inputFile, "r") as f:
    for line in f.readlines():
        roads.append(eval(line[line.index(" ") + 1:-1])) # append the road

scale_factor = HEIGHT
for i, road in enumerate(roads):
    for j, node in enumerate(road):
        roads[i][j] = (roads[i][j][0] * scale_factor, roads[i][j][1] * scale_factor)

cameraX = 0
cameraY = 0

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

intersections = [
    coord for coord, count in coord_usage.items()
    if count > 1
]

mouse_down = False
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
        if event.type == pygame.MOUSEBUTTONDOWN:
            mouse_down = True
            pygame.mouse.get_rel() # idk why this is needed but stops a bug
        if event.type == pygame.MOUSEBUTTONUP:
            mouse_down = False
        if event.type == pygame.MOUSEWHEEL:
            mouse = pygame.mouse.get_pos()
            scale = 1 + event.y / 10
            for i, point in enumerate(intersections):
                intersections[i] = ((point[0] - mouse[0] - cameraX)*scale + mouse[0] + cameraX,
                                    (point[1] - mouse[1] - cameraY)*scale + mouse[1] + cameraY)

            for i, road in enumerate(roads):
                for j, node in enumerate(road):
                    roads[i][j] = ((node[0] - mouse[0] - cameraX)*scale + mouse[0] + cameraX, 
                                   (node[1] - mouse[1] - cameraY)*scale + mouse[1] + cameraY)

    if mouse_down:
        mouse_movement = pygame.mouse.get_rel()
        if mouse_movement != (0, 0):
            cameraX -= mouse_movement[0]
            cameraY -= mouse_movement[1]

    keys = pygame.key.get_pressed()
    if (keys[pygame.K_z]):
        for i, point in enumerate(intersections):
            intersections[i] = (point[0] * 1.01, point[1] * 1.01)

        for i, road in enumerate(roads):
            for j, node in enumerate(road):
                roads[i][j] = (node[0] * 1.01, node[1] * 1.01)
    if (keys[pygame.K_x]):
        for i, point in enumerate(intersections):
            intersections[i] = (point[0] * 0.99, point[1] * 0.99)

        for i, road in enumerate(roads):
            for j, node in enumerate(road):
                roads[i][j] = (node[0] * 0.99, node[1] * 0.99)

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

    for point in intersections:
        x, y = get_local_coordinates(point)
        pygame.draw.circle(win, (255, 0, 0), (x, y), 3)

    pygame.display.update()
    clock.tick(60)
