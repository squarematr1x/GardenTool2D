#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "Vec2.h"
#include "Game.h"

//void checkCollision(sf::Shape& shape, sf::Vector2f& speed, const int width, const int height)
//{
//    sf::Vector2f pos = shape.getPosition();
//
//    if (pos.y < 0)
//        speed.y *= -1.0f;
//    else if (pos.x < 0)
//        speed.x *= -1.0f;
//    else if (pos.x + + shape.getLocalBounds().width > width)
//        speed.x *= -1.0f;
//    else if (pos.y + shape.getLocalBounds().height > height)
//        speed.y *= -1.0f;
//
//    shape.setPosition(pos + sf::Vector2f(speed.x, speed.y));
//}
//
//int readConfig(std::vector<sf::CircleShape>& shapes, std::vector<sf::Text>& texts, std::vector<sf::Vector2f>& speeds, sf::Font& font, int& width, int& height)
//{
//    std::ifstream inf{ "config.txt" };
//    sf::Color text_color;
//    int font_size = 0;
//
//    if (!inf)
//    {
//        std::cerr << "Config file not found\n";
//        return 1;
//    }
//
//    while (inf)
//    {
//        std::string line;
//        std::getline(inf, line);
//        std::stringstream ssin(line);
//        std::vector<std::string> args;
//
//        while (ssin.good())
//        {
//            std::string arg;
//            ssin >> arg;
//            args.push_back(arg);
//        }
//        
//        if (args[0] == "Window")
//        {
//            if (args.size() < 3)
//                return -1;
//
//            width = stoi(args[1]);
//            height = stoi(args[2]);
//        }
//        else if (args[0] == "Font")
//        {
//            if (args.size() < 6)
//                return -1;
//
//            if (!font.loadFromFile(args[1]))
//            {
//                std::cerr << "Font not found\n";
//                exit(-1);
//            }
//
//            font_size = stoi(args[2]);
//            text_color = sf::Color(stoi(args[3]), stoi(args[4]), stoi(args[5]));
//        }
//        else if (args[0] == "Circle")
//        {
//            if (args.size() < 10)
//                return -1;
//
//            sf::Text text(args[1], font, font_size);
//            text.setFillColor(text_color);
//            texts.push_back(text);
//           
//            sf::CircleShape shape(stof(args[9]));
//            shape.setPosition(stof(args[2]), stof(args[3]));
//            shape.setFillColor(sf::Color(stoi(args[6]), stoi(args[7]), stoi(args[8])));
//            shapes.push_back(shape);
//
//            speeds.push_back(sf::Vector2f(stof(args[4]), stof(args[5])));
//        }
//    }
//
//    return 0;
//}

int main()
{
    Game game("config.txt");
    game.run();
    //Vec2 p1(10, 20);
    //Vec2 p2(10, 20);
    //Vec2 p3(p1 + p2);

    //std::cout << p1 << '\n';
    //std::cout << p3 << '\n';

    //p3.add(10).add(10);

    //std::vector<sf::CircleShape> shapes;
    //std::vector<sf::Text> texts;
    //std::vector<sf::Vector2f> speeds;

    //sf::Font font;

    //int width = 800;
    //int height = 600;

    //readConfig(shapes, texts, speeds, font, width, height);

    //sf::RenderWindow window(sf::VideoMode(width, height), "Engine");

    //if (!font.loadFromFile("fonts/arial.ttf"))
    //{
    //    std::cerr << "Font not found\n";
    //    exit(-1);
    //}

    //while (window.isOpen())
    //{
    //    sf::Event event;
    //    while (window.pollEvent(event))
    //    {
    //        if (event.type == sf::Event::Closed)
    //            window.close();
    //    }

    //    window.clear();

    //    for (unsigned int i = 0; i < shapes.size(); i++)
    //    {
    //        checkCollision(shapes[i], speeds[i], width, height);
    //        texts[i].setPosition(shapes[i].getPosition().x + shapes[i].getLocalBounds().width / 2 - texts[i].getLocalBounds().width / 2, shapes[i].getPosition().y + shapes[i].getLocalBounds().height / 2 - texts[i].getLocalBounds().height / 2);

    //        window.draw(shapes[i]);
    //        window.draw(texts[i]);
    //    }
   
    //    window.display();
    //}

    return 0;
}
