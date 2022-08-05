extern sf::Font FONT;
extern std::string FONT_PATH;
extern const int BOARD_WIDTH, BOARD_HEIGHT;

struct Button
{
    std::string text;

    sf::Text textObject;
    sf::FloatRect bb;

    Button(std::string txt, int x, int y)
    {
        FONT.loadFromFile(FONT_PATH);
        textObject.setFont(FONT);
        textObject.setCharacterSize(20);
        text = txt;
        textObject.setString(txt);
        textObject.setFillColor(sf::Color(255, 255, 255));

        textObject.setPosition(x, y);
        textObject.setOrigin(textObject.getGlobalBounds().width/2, 0);
        bb = textObject.getGlobalBounds();
    }

    bool isPressed(sf::Vector2i mousePos)
    {
        int x = mousePos.x, y = mousePos.y;
        if (x > bb.left && x < (bb.left + bb.width) && y > bb.top && y < (bb.top + bb.height)) { return true; }
        return false;
    }
};
