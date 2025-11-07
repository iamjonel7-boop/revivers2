#include <SFML/Graphics.hpp>
#include <iostream>

int main()
{
  sf::RenderWindow window(sf::VideoMode({800, 600}), "My window");

  window.setFramerateLimit(60);
  std::srand(static_cast<unsigned>(std::time(nullptr)));

  int tileSize = 16;
  int mapWidth = 40;
  int mapHeight = 30;

  std::vector<int> tileMap(mapWidth * mapHeight);

  for(int i = 0; i < mapWidth * mapHeight; ++i)
    tileMap[i] = rand() % 4;

  sf::RectangleShape tile(sf::Vector2f(tileSize, tileSize));
  tile.setFillColor(sf::Color::Transparent);
  tile.setOutlineThickness(1);

  std::string inputString;
  sf::Text text;
  sf::Font font;
  font.loadFromFile("/usr/local/share/fonts/noto/NotoSans-Condensed.ttf");
  text.setFont(font);

  sf::VertexArray triangle(sf::Triangles, 3);
  triangle[0].position = sf::Vector2f(10.f, 10.f);
  triangle[1].position = sf::Vector2f(100.f, 10.f);
  triangle[2].position = sf::Vector2f(100.f, 100.f);

  triangle[0].color = sf::Color::Red;
  triangle[1].color = sf::Color::Blue;
  triangle[2].color = sf::Color::Green;

  // run the program as long as the window is open
  while(window.isOpen())
    {
      sf::Event event;

      // check all the window events that were triggered since the last iteration of the loop
      while(window.pollEvent(event))
        {
          switch(event.type)
            {
            case sf::Event::Closed:
              window.close();
              break;
            case sf::Event::TextEntered:
              if(event.text.unicode < 128)
                {
                  char c = static_cast<char>(event.text.unicode);
                  if( c == '\b')
                    {
                      if(!inputString.empty())
                        inputString.pop_back();
                    }
                  else if(c != '\r' && c != '\n')
                    {
                      inputString += c;
                      std::cout << "ASCII character typed: " << static_cast<char>(event.text.unicode) << std::endl;
                    }
                }
              break;
            case sf::Event::KeyPressed:
              if(event.key.scancode == sf::Keyboard::Scan::Escape)
                window.close();
              if(event.key.scancode == sf::Keyboard::Scan::Enter)
                text.setString(inputString);
                std::cout << inputString << std::endl;
              break;
            default:
              break;
            }
        }
      window.clear();
      // window.draw(text);
      //window.draw(triangle);
      for(int y = 0; y < mapHeight; ++y)
        {
          for(int x = 0; x < mapWidth; ++x)
            {
              int id = tileMap[y * mapWidth + x];

              switch(id){
              case 0: tile.setOutlineColor(sf::Color(100,100,100)); break;
              case 1: tile.setOutlineColor(sf::Color::Green); break;
              case 2: tile.setOutlineColor(sf::Color::Cyan); break;
              case 3: tile.setOutlineColor(sf::Color::Yellow); break;
              }

              tile.setPosition(x*tileSize, y*tileSize);
              window.draw(tile);
            }
        }

      window.display();
    }
}
