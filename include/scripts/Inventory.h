#pragma once
#include "components/component.h"

struct Item{
    TileType tile;
    int count;

    Item(): tile(TileType::air), count(0){}
};

struct DroppedItem{
    Terra::Entity* ent;
    TileType tile;
    float droppedTime;

    DroppedItem(const DroppedItem& other): ent(other.ent), tile(other.tile), droppedTime(other.droppedTime) {}
    DroppedItem(Terra::Entity* ent_, TileType tile_): ent(ent_), tile(tile_), droppedTime(0.0f) {}
    DroppedItem(): ent(nullptr), tile(TileType::air), droppedTime(0.0f){}
};

struct ItemRenderable{
    Terra::Entity* ent = nullptr;
    bool displaying = false;

    ItemRenderable(const ItemRenderable& other): ent(other.ent), displaying(other.displaying) {}
    ItemRenderable(Terra::Entity* ent_): ent(ent_), displaying(false){}
    ItemRenderable(): ent(nullptr), displaying(false){}

    void display(Scene* scene){
        scene->addToRenderer(*ent);
        displaying = true;
    }

    void stopDisplaying(Scene* scene){
        scene->removeFromRenderer(*ent);
        displaying = false;
    }

    bool null(){
        return ent == nullptr;
    }
};

//Handles inventory management + block breaking and placing
class Inventory : ScriptableObject{
    public:
            Inventory(Terra::Entity& entity_, Scene* scene_) : ScriptableObject(entity_, scene_) {}
           ~Inventory(){}
            void start() override;
            void update(float timeStep) override;
            void onDestroy() override;
            void onCollision(const CollisionData& collision) override;
     private:
            void dropItem();
            void breakTile();
            void placeItem();

            void renderItemText();

            void initInventoryGUI();
            void updateInventory();
            void updateInventoryPos();

            Terra::Entity* player;
            Component::Transform* playerTransform;
            
            Terra::Entity* inventorySlots[8];
            ItemRenderable inventoryItems[8];
            float inventoryOffsets[8];

            Item items[8];
            int selectedItem = 0;

            std::vector<DroppedItem> groundItems;

            float elapsedTimeTileBreak = 0.0f;
            float elapsedTimeItemDrop = 0.0f;
            float elapsedTimeItemPlace = 0.0f;
};