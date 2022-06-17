#include "scripts/Inventory.h"
#include "core/input.h"
#include "util/assetPool.h"
#include "renderer/debugRenderer.h"
#include "renderer/fontRenderer.h"
#include "renderer/renderer.h"

#define INVENTORY_SLOT_SCALE 2.25f
#define INVENTORY_ITEM_SCALE 1.3f
#define INVENTORY_GAP_FROM_TOP 2.25f
#define SLOT_GAP 2.25f

#define ITEM_GAP 0.02f

#define SPRITE_SHEET_CELLS 8

#define MIN_DROP_TIME 1.0f //seconds
#define DROPPED_ITEM_SCALE 1.0f
#define DROPPED_ITEM_MAGNET_RADIUS 15.0f
#define DROPPED_ITEM_PICKUP_RADIUS 1.0f
#define DROPPED_ITEM_MAGNET_VELOCITY 2.5f

#define TILE_BREAK_DELAY 0.1f
#define DROP_ITEM_DELAY 0.8f
#define TILE_PLACE_DELAY 0.1f

#define MIN_PLACE_AND_BREAK_DISTANCE 10.0f

#define MAX_ITEMS 99

void Inventory::start(){
    player = scene->getEntityByTagName("Player");
    assert(player);
    playerTransform = &(player->getComponent<Component::Transform>());

    Renderer::getLightingData().playerTransform = playerTransform;

    initInventoryGUI();

    for (int i = 0; i < 8; i++)
        inventoryItems[i] = nullptr;

    for (int i = 0; i < 2; i++){
        items[i].tile = TileType::planks;
        items[i].count = 99;
    }
    items[3].tile = TileType::torch;
    items[3].count = 10;

    inventorySlots[selectedItem]->getComponent<Component::SpriteRenderer>().changeColor(glm::vec4(0.8f, 0.3f, 0.8f, 1.0f));
    updateInventory();
}

void Inventory::update(float timeStep){
    updateInventoryPos();

    renderItemText();

    int key = -1;
    for (int i = 0; i < 8; i++){
        if (KeyListener::isKeyPressed(GLFW_KEY_1 + i)){
            key = i;
            break;
        }
    }
    //Highlight the corresponding inventory slot
    if (key != -1 && key != selectedItem){
        inventorySlots[selectedItem]->getComponent<Component::SpriteRenderer>().changeColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        selectedItem = key;
        inventorySlots[selectedItem]->getComponent<Component::SpriteRenderer>().changeColor(glm::vec4(0.8f, 0.3f, 0.8f, 1.0f));

        if (items[selectedItem].tile == TileType::torch)
            Renderer::getLightingData().addPlayerLight();
        else
            Renderer::getLightingData().removePlayerLight();
    }

    if (KeyListener::isKeyPressed(GLFW_KEY_Q) && elapsedTimeItemDrop >= DROP_ITEM_DELAY){
        dropItem();
        updateInventory();
        elapsedTimeItemDrop = 0.0f;
    }

    if (MouseListener::isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT) && !MouseListener::isMouseButtonModdedBy(GLFW_MOUSE_BUTTON_LEFT, GLFW_KEY_LEFT_SHIFT) && elapsedTimeTileBreak >= TILE_BREAK_DELAY){
             breakTile();
             updateInventory();
             elapsedTimeTileBreak = 0.0f;
    }

    if (MouseListener::isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT) && elapsedTimeItemPlace >= TILE_PLACE_DELAY){
        placeItem();
        updateInventory();
        elapsedTimeItemPlace = 0.0f;
    }
    
    elapsedTimeTileBreak += timeStep;
    elapsedTimeItemDrop += timeStep;
    elapsedTimeItemPlace += timeStep;
}

   
void Inventory::dropItem(){
     if (items[selectedItem].count > 0)
        items[selectedItem].count--;
}

void Inventory::placeItem(){
    if (items[selectedItem].count <= 0)
        return;

    int x = MouseListener::GetMousex();
    int y = MouseListener::GetMousey();
    if (x < 0 || x >= Window::getWidth() || y < 0 || y >= Window::getHeight())
        return;

    const glm::vec2& p = Camera::getPosition();
    x = (int) floor((float) x / (Window::getWidth() / 2.0f) * CAM_WIDTH + p.x - CAM_WIDTH / 2 );
    y = (int) floor((1.0f - ((float)y / (Window::getHeight()/2.0f))) * CAM_HEIGHT + p.y - CAM_HEIGHT / 2);
    y += 1;

    //check if the block intersects the player's collider
    auto& playerCollider = player->getComponent<Component::AABB>();
    glm::vec2 dimensions((int) floor(playerCollider.extentsX* 2.0f + 1), (int) floor(playerCollider.extentsY * 2.0f + 1));
    glm::vec2 start((int) floor(playerTransform->position.x - playerCollider.extentsX), (int) floor(playerTransform->position.y - playerCollider.extentsY));
    for (int xx = start.x; xx < start.x + dimensions.x; xx++){
        for (int yy = start.y; yy < start.y + dimensions.y; yy++){
            if (x == xx && y == yy)
                return;
        }
    }
    
    if (glm::length(glm::vec2(x, y) - playerTransform->position) > MIN_PLACE_AND_BREAK_DISTANCE)    
        return;

    TileType t = scene->getWorld()->tileTypeAtWorldCoordinate(x, y);
    if (t != TileType::air || t >= TileType::background)
        return;

    //Check if theres a solid block on either of the four sides
    bool valid = false;
    for (int i = 0; i < 4; i++){
        int shiftX = i == 0 ? -1 : (i == 1 ? 1 : 0);
        int shiftY = i == 2 ? -1 : (i == 3 ? 1 : 0);

        TileType t = scene->getWorld()->tileTypeAtWorldCoordinate(x + shiftX, y + shiftY);
        if (t != TileType::air){
            valid = true;
            break;
        }
    }

    if (!valid) return;

    if (items[selectedItem].tile == TileType::torch)
        Renderer::getLightingData().addTorch(glm::vec2(x, y));

    scene->getWorld()->placeTileAtWorldCoordinate(x, y, items[selectedItem].tile);
    items[selectedItem].count--;
}

void Inventory::breakTile(){
    int x = MouseListener::GetMousex();
    int y = MouseListener::GetMousey();
    if (x < 0 || x >= Window::getWidth() || y < 0 || y >= Window::getHeight())
        return;

    const glm::vec2& p = Camera::getPosition();
    x = (int) floor((float) x / (Window::getWidth() / 2.0f) * CAM_WIDTH + p.x - CAM_WIDTH / 2 );
    y = (int) floor((1.0f - ((float)y / (Window::getHeight()/2.0f))) * CAM_HEIGHT + p.y - CAM_HEIGHT / 2);
    y += 1;

    if (glm::length(glm::vec2(x, y) - playerTransform->position) > MIN_PLACE_AND_BREAK_DISTANCE)    
        return;
 

    TileType t = scene->getWorld()->tileTypeAtWorldCoordinate(x, y);
    if (t != TileType::air && t < TileType::background){
        scene->getWorld()->removeTileAtWorldCoordinate(x, y);
        
    if (t == TileType::torch)
         Renderer::getLightingData().removeTorch(glm::vec2(x, y));

        //add to the inventory
        for (int i = 0; i < 8; i++){
            if (items[i].count < MAX_ITEMS && items[i].tile == t){
                items[i].count++;
                break;
            }
            else if (items[i].count == 0){
                items[i].count = 1;
                items[i].tile = t;
                break;
            }
        }
    }

}


void Inventory::updateInventory(){

    for (int i = 0; i < 8; i++){
        //If the inventory icon isn't being rendered       
        if (items[i].count != 0 && inventoryItems[i].null()){
            Ref<Texture>& tex = AssetPool::getTexture("../assets/img/SpriteSheet.png");
            int spriteX = (items[i].tile - 1) % SPRITE_SHEET_CELLS;
            int spriteY = (items[i].tile - 1) / SPRITE_SHEET_CELLS;

            Terra::Entity& ent = scene->createEntity();
            ent.addComponent<Component::SpriteRenderer>(SpriteSheet(tex, 8, 8), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), Sprite{spriteY, spriteX});

            auto& trans = ent.getComponent<Component::Transform>();
            trans.zIndex = 3;
            trans.setScale(glm::vec2(INVENTORY_ITEM_SCALE, INVENTORY_ITEM_SCALE));

            inventoryItems[i].ent = &ent;
            inventoryItems[i].display(scene);
        }
        //If we ran out of items in an inventory slot and the item is still being rendered
        else if (items[i].count == 0 && inventoryItems[i].displaying){
            inventoryItems[i].stopDisplaying(scene);
        }
        else if (items[i].count != 0 && !inventoryItems[i].displaying){
            auto& spr = inventoryItems[i].ent->getComponent<Component::SpriteRenderer>();
            int spriteX = (items[i].tile - 1) % SPRITE_SHEET_CELLS;
            int spriteY = (items[i].tile - 1) / SPRITE_SHEET_CELLS;
            spr.sprite = Sprite(spriteY, spriteX);
            inventoryItems[i].display(scene);
        }
    }
}

void Inventory::updateInventoryPos(){
    const glm::vec2& p = Camera::getPosition();
    float y = p.y + CAM_HEIGHT / 2 - INVENTORY_GAP_FROM_TOP;
    for (int i = 0; i < 8; i++){

        auto& transSlot = inventorySlots[i]->getComponent<Component::Transform>();
        transSlot.setPos(glm::vec2(inventoryOffsets[i] + p.x, y));

        if (!inventoryItems[i].null() && inventoryItems[i].displaying){
            auto& transItem = inventoryItems[i].ent->getComponent<Component::Transform>();
            transItem.setPos(glm::vec2(inventoryOffsets[i] + p.x + ITEM_GAP, y - ITEM_GAP));
        }
    }
}

void Inventory::initInventoryGUI(){
    Ref<Texture>& tex = AssetPool::getTexture("../assets/img/Slot.png");

    float startX = -CAM_WIDTH / 2 + 2.0f;
    for (int i = 0; i < 8; i++){
        Terra::Entity& ent = scene->createEntity();
        Component::SpriteRenderer& spr = ent.addComponent<Component::SpriteRenderer>(SpriteSheet(tex, 1, 1));

        Component::Transform& trans = ent.getComponent<Component::Transform>();
        trans.setScale(glm::vec2(INVENTORY_SLOT_SCALE, INVENTORY_SLOT_SCALE));
        trans.setPos(glm::vec2(startX + SLOT_GAP * i, 0.0f));
        trans.zIndex = 1;
        inventoryOffsets[i] = startX + SLOT_GAP * i;

        inventorySlots[i] = &ent;
        scene->addToRenderer(ent);
    }
}

void Inventory::renderItemText(){
    const float textGap = 1.0f;
    const glm::vec2& p = Camera::getPosition();
    float y = p.y + CAM_HEIGHT / 2 - INVENTORY_GAP_FROM_TOP;

    for (int i = 0; i < 8; i++){
        if (!inventoryItems[i].null() && inventoryItems[i].displaying)
            FontRenderer::drawText(std::to_string(items[i].count), glm::vec2(inventoryOffsets[i] + p.x - textGap, y), 1.0f, 0.05f, glm::vec3(1.0f, 1.0f, 1.0f));
    }
}

void Inventory::onDestroy(){
 
}

void Inventory::onCollision(const CollisionData& collision){
 
}