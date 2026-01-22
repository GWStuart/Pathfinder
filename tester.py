import pygame
pygame.init()

"""
Simple program to test out some of the mechanics quickly in python before
C implementation.

This file is only for personal testing.
"""

win = pygame.display.set_mode((800, 600))
pygame.display.set_caption("Program Tester")

SCALE = 1000

class Node:
    def __init__(self, pos, edges, num_edges):
        self.pos = pos
        self.edges = edges
        self.num_edges = num_edges

    def __repr__(self):
        return f"Node({self.pos}"

nodes = []
with open("assets/data/BrisbaneCentreV2.nodes", "r") as f:
    f.readline()
    for line in f.readlines():
        sep = line.rindex(" ")
        pos = eval(line[:sep])
        pos = (pos[0] * SCALE, pos[1] * SCALE)
        nodes.append(Node(pos, [], int(line[sep+1:-1])))

run = True
while run:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            run = False
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_q:
                run = False

    win.fill((40, 40, 40))

    for node in nodes:
        pygame.draw.circle(win, (255, 0, 0), node.pos, 2)

    pygame.display.update()
