extern sf::RenderWindow* pWINDOW;
extern float getRandNum(), getRandNum(int, int);
extern Button minesFlaggedButton;

enum TextureNumbers
{
    BOMB = 9,
    NOT_EXPOSED = 10,
    RED_FLAG = 11,
    GREEN_FLAG = 12
};;

class GameBoard : public sf::Drawable, public sf::Transformable
{
private:
    Tile tileMatrix[BOARD_WIDTH][BOARD_HEIGHT];
    sf::Texture textureSet;
    int minesFlagged = 0;

    void createTileMap()
    {
        int randomX, randomY;
        for (int i=0; i<numberOfMines; i++) {
            do {
                randomX = getRandNum() * (BOARD_WIDTH-1);
                randomY = getRandNum() * (BOARD_HEIGHT-1);
            } while (tileMatrix[randomX][randomY].isMine || tileMatrix[randomX][randomY].isFirstMoveReserved);
            tileMatrix[randomX][randomY].isMine = true;
            tileMatrix[randomX][randomY].textureNumber = BOMB;
            incrementNeighbours(randomX, randomY);
        }
    }

    void incrementNeighbours(const int x, const int y)
    {
        for (int i=-1; i<=1; i++) {
            for (int j=-1; j<=1; j++) {

                int neighbourX = x + i;
                if (neighbourX < 0 || neighbourX > BOARD_WIDTH-1) { continue; }

                int neighbourY = y + j;
                if (neighbourY < 0 || neighbourY > BOARD_HEIGHT-1) { continue; }

                if (tileMatrix[neighbourX][neighbourY].isMine) { continue; }

                tileMatrix[neighbourX][neighbourY].minesAround += 1;
                tileMatrix[neighbourX][neighbourY].textureNumber += 1;
            }
        }
    }

    void textureTile(int textureNumber, int x, int y)
    {
        sf::Vertex* quad = &vertices[ (x+y*BOARD_HEIGHT)*4 ];

        quad[0].position = sf::Vector2f( x*TILE_WIDTH, y*TILE_HEIGHT );
        quad[1].position = sf::Vector2f( (x + 1)*TILE_WIDTH, y*TILE_HEIGHT );
        quad[2].position = sf::Vector2f( (x + 1)*TILE_WIDTH, (y + 1)*TILE_HEIGHT );
        quad[3].position = sf::Vector2f( x*TILE_WIDTH, (y+1)*TILE_HEIGHT );

        quad[0].texCoords = sf::Vector2f( textureNumber * TILE_WIDTH, 0 );
        quad[1].texCoords = sf::Vector2f( (textureNumber+1) * TILE_WIDTH, 0 );
        quad[2].texCoords = sf::Vector2f( (textureNumber+1) * TILE_WIDTH, TILE_HEIGHT );
        quad[3].texCoords = sf::Vector2f( textureNumber * TILE_WIDTH, TILE_HEIGHT );
    }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform *= getTransform();
        states.texture = &textures;

        target.draw(vertices, states);
    }

    void revealSurroundingTiles(int x, int y)
    {
        tileMatrix[x][y].isExposed = true;
        textureTile(tileMatrix[x][y].textureNumber, x, y);

        for (int i=-1; i<=1; i++) {
            for (int j=-1; j<=1; j++) {
                if (!i && !y) { continue; }
                int neighbourX = x + i;
                if (neighbourX < 0 || neighbourX > BOARD_WIDTH-1) { continue; }

                int neighbourY = y + j;
                if (neighbourY < 0 || neighbourY > BOARD_HEIGHT-1) { continue; }

                Tile* neightbourTile = &tileMatrix[neighbourX][neighbourY];
                if (neightbourTile->isExposed) { continue; }
                if (neightbourTile->textureNumber == 0) { revealSurroundingTiles(neighbourX, neighbourY); }
                else {
                    neightbourTile->isExposed = true;
                    textureTile(neightbourTile->textureNumber, neighbourX, neighbourY);
                }
            }
        }
    }

    sf::VertexArray vertices;
    sf::Texture textures;

public:
    int numberOfMines;
    int isFirstMove = true;
    bool isGameOver = false;

    GameBoard(std::string texturesPath, int numOfMines)
    {
        textures.loadFromFile(texturesPath);
        vertices.setPrimitiveType(sf::Quads);
        vertices.resize(BOARD_WIDTH * BOARD_HEIGHT * 4);

        numberOfMines = numOfMines;
    }

    void textureNewBoard()
    {
        for (int i=0; i<BOARD_WIDTH; i++) {
            for (int j=0; j<BOARD_HEIGHT; j++) {
                textureTile(NOT_EXPOSED, i, j);
            }
        }
    }

    void firstMove(sf::Vector2i mousePos)
    {
        isFirstMove = false;

        int x = mousePos.x / TILE_WIDTH;
        int y = mousePos.y / TILE_HEIGHT;

        for (int i=-1; i<=1; i++) {
            for (int j=-1; j<=1; j++) {

                if (!i && !j) {
                    tileMatrix[x][y].isExposed = true;
                    tileMatrix[x][y].isFirstMoveReserved = true;
                }
                int neighbourX = x + i;
                if (neighbourX < 0 || neighbourX > BOARD_WIDTH-1) { continue; }

                int neighbourY = y + j;
                if (neighbourY < 0 || neighbourY > BOARD_HEIGHT-1) { continue; }

                tileMatrix[neighbourX][neighbourY].isFirstMoveReserved = true;
            }
        }
        createTileMap();

        revealTile(mousePos);
    }

    void revealTile(sf::Vector2i mousePos)
    {
        if (isGameOver) { return; }

        int x = mousePos.x / TILE_WIDTH;
        int y = mousePos.y / TILE_HEIGHT;

        Tile* tile = &tileMatrix[x][y];

        if (tile->flagNum) { return; }
        if (tile->isMine) {
            isGameOver = true;
        }

        if (tile->textureNumber == 0) { revealSurroundingTiles(x, y); }
        else {
            textureTile(tile->textureNumber, x, y);
            tile->isExposed = true;
        }
    }

    void flagTile(sf::Vector2i mousePos)
    {
        if (isFirstMove || isGameOver) { return; }
        int x = mousePos.x / TILE_WIDTH;
        int y = mousePos.y / TILE_HEIGHT;

        Tile* tile = &tileMatrix[x][y];

        if (tile->isExposed) { return; }

        switch (tile->flagNum)
        {
        case 0:
            tile->flagNum = RED_FLAG;
            minesFlagged += 1;
            minesFlaggedButton.textObject.setString(std::to_string(minesFlagged) + "/" + std::to_string(numberOfMines));
            break;
        case RED_FLAG:
            minesFlagged -= 1;
            minesFlaggedButton.textObject.setString(std::to_string(minesFlagged) + "/" + std::to_string(numberOfMines));
            tile->flagNum = GREEN_FLAG;
            break;
        case GREEN_FLAG:
            tile->flagNum = 0;
            break;
        }
        if (tile->flagNum) { textureTile(tile->flagNum, x, y); }
        else { textureTile(NOT_EXPOSED, x, y); }


    }
};
