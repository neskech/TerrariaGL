#pragma once

#include "renderer/texture.h"
#include "util/assetPool.h"
#include "pch.h"


struct SpriteSheet{
    Ref<Texture> tex;

    int numRows;
    int numCols;
    float cellWidth, cellHeight;

    SpriteSheet(Ref<Texture>& tex_, int rows_, int cols_) : tex(tex_), numRows(rows_), numCols(cols_){
        cellWidth = tex->getWidth() / numCols;
        cellHeight = tex->getHeight() / numRows;
    }

    SpriteSheet(const SpriteSheet& other): tex(other.tex), numRows(other.numRows), numCols(other.numCols),
                cellWidth(other.cellWidth), cellHeight(other.cellHeight) {}

    SpriteSheet(){}
};

//Setter for the row and col indices, asserts false if invalid index
struct Sprite{
    int row, col;
    Sprite(int row_, int col_): row(row_), col(col_) {}
};