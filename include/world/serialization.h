#pragma once
#include "world/world.h"

void serializeChunks();
void serializeWorld(World* world);

Chunk* loadChunks();
World* loadWorld();