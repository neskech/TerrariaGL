#pragma once

#include "renderer/texture.h"
#include "util/assetPool.h"
#include <array>


struct SpriteSheet{
    Ref<Texture> tex;

    int numRows;
    int numCols;
    float cellWidth, cellHeight;

    SpriteSheet(Ref<Texture>& tex_, int rows_, int cols_) : tex(tex_), numRows(rows_), numCols(cols_){
        cellWidth = tex->getWidth() / numCols;
        cellHeight = tex->getHeight() / numRows;
    }
};

//Setter for the row and col indices, asserts false if invalid index
struct Sprite{
    int row, col;
};