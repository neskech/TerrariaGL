#pragma once
#include "renderer/texture.h"
#include "util/assetPool.h"
#include <array>


struct SpriteSheet{
    Ref<Texture> tex;

    int numRows;
    int numCols;
    float cellWidth, cellHeight;

    SpriteSheet(const char* path, int rows, int cols, TexParams* params = nullptr, bool mipMaps = false) : numRows(rows), numCols(cols){
        if (params != nullptr)
             tex = AssetPool::getTexture(path, *params, mipMaps);
        tex = AssetPool::getTexture(path);

        cellWidth = tex->getWidth() / numCols;
        cellHeight = tex->getHeight() / numRows;
    }
};

//Setter for the row and col indices, asserts false if invalid index
struct Sprite{
    int row, col;
};

class SpriteRenderer{
    public:
    private:
        SpriteSheet sheet;
        Sprite sprite;
};