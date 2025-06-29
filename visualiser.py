from collections import defaultdict
import pygame
import math

pygame.init()

#inputFile = "data/RomeSmall.data"
inputFile = "data/BrisbaneCentre.data"

LENGTH, HEIGHT = 800, 600
win = pygame.display.set_mode((LENGTH, HEIGHT))
pygame.display.set_caption("visualiser")
clock = pygame.time.Clock()

PAN_SPEED = 5

# load in the road data
roads = []
with open(inputFile, "r") as f:
    for line in f.readlines():
        roads.append(eval(line[line.index(" ") + 1:-1])) # append the road

# scale it to screen dimensions
scale_factor = HEIGHT
for i, road in enumerate(roads):
    for j, node in enumerate(road):
        roads[i][j] = (roads[i][j][0] * scale_factor, roads[i][j][1] * scale_factor)

cameraX = 0
cameraY = 0
cameraZoom = 1

def get_local_coordinates(coordinates: list[int, int]):
    return (coordinates[0] - cameraX) * cameraZoom, (coordinates[1] - cameraY) * cameraZoom

# accroding to GPT: 4 -> 11m, 5 -> 1.1m, 6 -> 11cm
def round_coord(coord, digits=5):
    return (round(coord[0], digits), round(coord[1], digits))  # (lon, lat)

coord_usage = defaultdict(int)

for road in roads:
    for point in road:
        coord = round_coord(point)
        coord_usage[coord] += 1

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
intersections_set.update(startpoints_set)
intersections_set.update(endpoints_set)
intersections = list(intersections_set)

def on_screen(point):
    return 0 <= point[0] <= LENGTH and 0 <= point[1] <= HEIGHT

def render_screen():
    win.fill((255, 255, 255))

    for i, road in enumerate(roads):
        colour = (0, 0, 0)
        pygame.draw.lines(win, colour, False, list(map(get_local_coordinates, road)), 3);

    if draw_intersections:
        for point in intersections:
            coord = get_local_coordinates(point)
            if on_screen(coord):
                pygame.draw.circle(win, (255, 0, 0), coord, 3)

mouse_down = False
draw_intersections = False

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
            render_screen()

    if mouse_down:
        mouse_movement = pygame.mouse.get_rel()
        if mouse_movement != (0, 0):
            cameraX -= mouse_movement[0]
            cameraY -= mouse_movement[1]
            render_screen()

    keys = pygame.key.get_pressed()
    if (keys[pygame.K_z]):
        cameraZoom += 0.01
        cameraX -= LENGTH/2 * (1 - cameraZoom)
        cameraY -= HEIGHT/2 * (1 - cameraZoom)
        render_screen()
        #        for i, road in enumerate(roads):
#            for j, node in enumerate(road):
#                roads[i][j] = ((node[0] - LENGTH/2 - cameraX)*1.01 + LENGTH/2 + cameraX, 
#                               (node[1] - HEIGHT/2 - cameraY)*1.01 + HEIGHT/2 + cameraY)
    if (keys[pygame.K_x]):
        cameraZoom -= 0.01
        cameraX += LENGTH/2 * (1 - cameraZoom)
        cameraY += HEIGHT/2 * (1 - cameraZoom)
        render_screen()
        #        for i, road in enumerate(roads):
#            for j, node in enumerate(road):
#                roads[i][j] = ((node[0] - LENGTH/2 - cameraX)*0.99 + LENGTH/2 + cameraX, 
#                               (node[1] - HEIGHT/2 - cameraY)*0.99 + HEIGHT/2 + cameraY)
    if (keys[pygame.K_c]):
        for i, point in enumerate(intersections):
            intersections[i] = ((point[0] - LENGTH/2 - cameraX)*1.01 + LENGTH/2 + cameraX,
                                (point[1] - HEIGHT/2 - cameraY)*1.01 + HEIGHT/2 + cameraY)
        for i, road in enumerate(roads):
            for j, node in enumerate(road):
                roads[i][j] = ((node[0] - LENGTH/2 - cameraX)*1.01 + LENGTH/2 + cameraX, 
                               (node[1] - HEIGHT/2 - cameraY)*1.01 + HEIGHT/2 + cameraY)
        render_screen()
    if (keys[pygame.K_v]):
        for i, point in enumerate(intersections):
            intersections[i] = ((point[0] - LENGTH/2 - cameraX)*0.99 + LENGTH/2 + cameraX,
                                (point[1] - HEIGHT/2 - cameraY)*0.99 + HEIGHT/2 + cameraY)
        for i, road in enumerate(roads):
            for j, node in enumerate(road):
                roads[i][j] = ((node[0] - LENGTH/2 - cameraX)*0.99 + LENGTH/2 + cameraX, 
                               (node[1] - HEIGHT/2 - cameraY)*0.99 + HEIGHT/2 + cameraY)
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
