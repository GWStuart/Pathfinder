from collections import defaultdict
import pygame
import math
import random

pygame.init()

#inputFile = "data/RomeSmall.data"
#inputFile = "data/v5.data"
#inputFile = "data/BrisbaneCentre.data"
inputFile = "data/temp.data"
nodeFile = "data/temp.nodes"

LENGTH, HEIGHT = 800, 600
win = pygame.display.set_mode((LENGTH, HEIGHT))
pygame.display.set_caption("visualiser")
clock = pygame.time.Clock()

PAN_SPEED = 5
ZOOM_FACTOR = 1.01
ZOOM_DIFFERENCE = ZOOM_FACTOR - 1
ZOOM_FACTOR_MOUSE = 1.1
ZOOM_DIFFERENCE_MOUSE = ZOOM_FACTOR_MOUSE - 1

# load in the road data
roads = []
with open(inputFile, "r") as f:
    f.readline() # skip header line

    # read in each road
    for line in f.readlines():
        roads.append(eval(line[line.index("["):-1]))

SCALE_FACTOR = HEIGHT
def scale_to_screen(pos):
    return pos[0] * SCALE_FACTOR, pos[1] * SCALE_FACTOR

# scale it to screen dimensions
for i, road in enumerate(roads):
    for j, node in enumerate(road):
        roads[i][j] = scale_to_screen(roads[i][j])

"""
Object used to store all intersection points in the map. Each node is uniquely identified by its pos (a tuple
representing x,y position). All nodes are connected to one or more other nodes as specified by the connections
list.
"""
class Node():
    def __init__(self, pos, neighbours):
        self.pos = pos
        self.neighbours = neighbours

    def __repr__(self):
        return f"Node({self.pos})"

# load in the intersection nodes
nodes = set()
with open(nodeFile, "r") as f:
    for line in f.readlines():
        pos = scale_to_screen(eval(line[line.index(" ")+1:line.index(")")+1]))
        neighbours = list(map(scale_to_screen, eval(line[line.index("["):-1])))
        nodes.add(Node(pos, neighbours))

cameraX = 0
cameraY = 0
cameraZoom = 1

def get_local_coordinates(coordinates: list[int, int]):
    return (coordinates[0] - cameraX) * cameraZoom, (coordinates[1] - cameraY) * cameraZoom

def on_screen(point):
    return 0 <= point[0] <= LENGTH and 0 <= point[1] <= HEIGHT

def render_screen():
    win.fill((255, 255, 255))

    random.seed(0)
    colour = (0, 0, 0)
    for road in roads:
        if randomise_road_colour:
            colour = (random.randint(0, 255), random.randint(0, 255), random.randint(0, 255))
        pygame.draw.lines(win, colour, False, list(map(get_local_coordinates, road)), 3);

    if draw_intersections:
        for node in nodes:
            coord = get_local_coordinates(node.pos)
            if on_screen(coord):
                pygame.draw.circle(win, (255, 0, 0), coord, 3)

mouse_down = False
draw_intersections = False
randomise_road_colour = False

render_screen() # initial screen render
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
            if event.key == pygame.K_h:
                draw_intersections = not draw_intersections
                render_screen()
            if event.key == pygame.K_k:
                randomise_road_colour = not randomise_road_colour
                render_screen()
        if event.type == pygame.MOUSEBUTTONDOWN:
            if event.button == 1: # left click
                mouse_down = True
                pygame.mouse.get_rel() # idk why this is needed but stops a bug
            elif event.button == 2: # right click
                pass
        if event.type == pygame.MOUSEBUTTONUP:
            mouse_down = False
        if event.type == pygame.MOUSEWHEEL:
            mouse = pygame.mouse.get_pos()

            # update the zoom
            direction = 1
            if (event.y > 0):
                cameraZoom *= ZOOM_FACTOR_MOUSE
            else:
                direction = -1
                cameraZoom /= ZOOM_FACTOR_MOUSE

            # adjust camera centre
            cameraX += mouse[0] / cameraZoom * ZOOM_DIFFERENCE_MOUSE * direction
            cameraY += mouse[1] / cameraZoom * ZOOM_DIFFERENCE_MOUSE * direction

            render_screen()
            render_screen()

    if mouse_down:
        mouse_movement = pygame.mouse.get_rel()
        if mouse_movement != (0, 0):
            cameraX -= mouse_movement[0] / cameraZoom
            cameraY -= mouse_movement[1] / cameraZoom
            render_screen()

    keys = pygame.key.get_pressed()

    if keys[pygame.K_z]:
        cameraZoom *= ZOOM_FACTOR

        # adjust camera centre
        cameraX += LENGTH / 2 / cameraZoom * ZOOM_DIFFERENCE
        cameraY += HEIGHT / 2 / cameraZoom * ZOOM_DIFFERENCE

        render_screen()

    if (keys[pygame.K_x]):
        cameraZoom /= ZOOM_FACTOR

        # adjust camera centre
        cameraX -= LENGTH / 2 / cameraZoom * ZOOM_DIFFERENCE
        cameraY -= HEIGHT / 2 / cameraZoom * ZOOM_DIFFERENCE

        render_screen()

    if (keys[pygame.K_w]):
        cameraY -= PAN_SPEED
    if (keys[pygame.K_a]):
        cameraX -= PAN_SPEED
    if (keys[pygame.K_s]):
        cameraY += PAN_SPEED
    if (keys[pygame.K_d]):
        cameraX += PAN_SPEED

    #render_screen()

    # render live fps count
    fps = clock.get_fps()
    font = pygame.font.SysFont("Arial", 18)
    fps_text = font.render(f"FPS: {int(fps)}", True, (255, 255, 255))
    pygame.draw.rect(win, (0, 0, 0), (0, 0, 50, 18))
    win.blit(fps_text, (3, 3))

    pygame.display.update()

    clock.tick(60)
