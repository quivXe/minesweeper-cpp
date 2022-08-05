
struct Tile
{
    bool isMine = false;
    bool isExposed = false;
    bool isFirstMoveReserved = false;
    int flagNum = 0;
    int minesAround = 0;
    int textureNumber = 0;
};
