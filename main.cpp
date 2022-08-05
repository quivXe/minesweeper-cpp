#include <SFML/Graphics.hpp>
#include <cstdlib> // rand
#include <ctime> // seed for srand
#include <string>
#include "tile.h"
#include "button.h"
#include "gameBoard.h"

#define NUMBER_OF_MINES 40

extern const int BOARD_WIDTH, BOARD_HEIGHT, TILE_WIDTH, TILE_HEIGHT;

std::string TEXTURES_PATH = "./resources/images/textures.png";
std::string FONT_PATH = "./resources/fonts/ComicSansMS.ttf";

sf::RenderWindow WINDOW(sf::VideoMode(BOARD_WIDTH*TILE_WIDTH, BOARD_HEIGHT*TILE_HEIGHT+TILE_HEIGHT), "Minesweeper");
sf::Font FONT;
GameBoard gameBoard(TEXTURES_PATH, NUMBER_OF_MINES);
Button restartButton("Restart", BOARD_WIDTH*TILE_WIDTH/2, BOARD_HEIGHT*TILE_HEIGHT + 2);
Button minesFlaggedButton("0/" + std::to_string(NUMBER_OF_MINES), BOARD_WIDTH*TILE_WIDTH/5, BOARD_HEIGHT*TILE_HEIGHT + 2);

// FUNCTIONS
float getRandNum() { return ((double)std::rand()) / (RAND_MAX); }
float getRandNum(int minNum, int maxNum) { return getRandNum() * (maxNum - minNum + 1) + minNum ; }
void updateWindow();

int main()
{
    std::srand(time(NULL)); // SET SEED FOR RNG
    FONT.loadFromFile(FONT_PATH);

    gameBoard.textureNewBoard();

    updateWindow();

    while (WINDOW.isOpen())
    {
        sf::Event event;
        while (WINDOW.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                WINDOW.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(WINDOW);

                if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    if (mousePos.y < BOARD_HEIGHT * TILE_HEIGHT) {
                        if (gameBoard.isFirstMove) {
                            gameBoard.firstMove(mousePos);
                        }
                        else {
                            gameBoard.revealTile(mousePos);
                        }
                    }
                    else if (restartButton.isPressed(mousePos)) {
                        gameBoard = GameBoard(TEXTURES_PATH, NUMBER_OF_MINES);
                        gameBoard.textureNewBoard();
                        minesFlaggedButton.textObject.setString("0/" + std::to_string(NUMBER_OF_MINES));
                    }
                }
                else {
                    gameBoard.flagTile(mousePos);
                }

                updateWindow();
            }
        }
    }
    return 0;
}

void updateWindow()
{
    WINDOW.clear();
    WINDOW.draw(gameBoard);
    WINDOW.draw(restartButton.textObject);
    WINDOW.draw(minesFlaggedButton.textObject);
    WINDOW.display();
}
