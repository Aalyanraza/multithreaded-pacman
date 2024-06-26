#ifndef MENU_H
#define MENU_H


#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <time.h>
#include "game.h"
#include "shared_variables.h"
#include "collisionuser.h"
using namespace std;
using namespace sf;

vector<pthread_mutex_t> enemymutexes;
vector<Enemy> enemys;
vector<RectangleShape> wallVector;
vector <Coin> coins;

pthread_mutex_t usermutex;
pthread_mutex_t usermutex2;
pthread_mutex_t enemymutex1;
pthread_mutex_t pelletmutex;
pthread_mutex_t pelletmutex1;
pthread_mutex_t pwermutex;

Coordinates userCoordinates={6,760};
char userDirection = 'L';
bool gamerunning=true;
int score = 0;
int lives = 3;
int counts=0;
int count1=0;
bool pwerTaken=false;
PowerPellet powerPellet;
PowerPellet powerPellet1;
power pwer(500, 500);
power pwer1(500, 400);


Texture coinTexture;
Sprite pacmanSprite;

void* handleUserCollisionWithPwer(void* arg) 
{
    while (gamerunning) 
    {
        pthread_mutex_lock(&pwermutex);

        // Check collision with pwer
        if (pwer.available && pacmanSprite.getGlobalBounds().intersects(pwer.sprite.getGlobalBounds())) 
        {
            pwer.available = false;
            pwerTaken = true;
            pthread_mutex_lock(&pwermutex);
        }

        // Check collision with pwer1
        if (pwer1.available && pacmanSprite.getGlobalBounds().intersects(pwer1.sprite.getGlobalBounds())) 
        {
            pwer1.available = false;
            pwerTaken = true;
            cout << "User collided with pwer1" << endl;
            pthread_mutex_lock(&pwermutex);
        }

        pthread_mutex_unlock(&pwermutex);

        usleep(10000);
    }
    return nullptr; // Return nullptr instead of void pointer
}

bool isCollisionWithWall(int x, int y, const Sprite& sprite) 
{
    FloatRect newBounds = sprite.getGlobalBounds();
    newBounds.left = x;
    newBounds.top = y;

    for (const auto& wall : wallVector) 
    {
        Vector2f wallPosition = wall.getPosition();
        Vector2f wallSize = wall.getSize();

        RectangleShape wallRect(wallSize);
        wallRect.setPosition(wallPosition);

        if (newBounds.intersects(wallRect.getGlobalBounds())) 
            return true;
    }

    return false;
}

void game() 
{
    coinTexture.loadFromFile("coin.jpg");

    pthread_t game;
    pthread_create(&game, NULL, game_thread, NULL);

    pthread_t coll2;
    pthread_create(&coll2, NULL, handleUserCollisionWithPwer, NULL);
    
    while (gamerunning) 
    {
        pthread_mutex_lock(&usermutex);
        char temp = userDirection;
        pthread_mutex_unlock(&usermutex);


        
        pthread_mutex_lock(&usermutex2);
        if (temp == 'U' && userCoordinates.y > 6) 
        {
            if (!isCollisionWithWall(userCoordinates.x, userCoordinates.y - 1, pacmanSprite)) 
                userCoordinates.y -= 1;
        } 
        else if (temp == 'D' && userCoordinates.y < 760) 
        {
            if (!isCollisionWithWall(userCoordinates.x, userCoordinates.y + 1, pacmanSprite)) 
                userCoordinates.y += 1;
        } 
        else if (temp == 'L' && userCoordinates.x > 6) 
        {
            if (!isCollisionWithWall(userCoordinates.x - 1, userCoordinates.y, pacmanSprite)) 
                userCoordinates.x -= 1;
        } 
        else if (temp == 'R' && userCoordinates.x < 1040) 
        {
            if (!isCollisionWithWall(userCoordinates.x + 1, userCoordinates.y, pacmanSprite)) 
                userCoordinates.x += 1;
        }
        pthread_mutex_unlock(&usermutex2);
        
        if (pwerTaken && count1 == 625)
        {
            cout << "Power eaten done" << endl;
            pwerTaken = false;
            pwer.available = true;
            pwer1.available = true;
            pthread_mutex_unlock(&pwermutex);
            count1=0;
        }
        else if (pwerTaken)
            count1++;

        sleep(milliseconds(5));
    }
}

void instructions(RenderWindow& window)
{
    Font font;
    font.loadFromFile("arial.ttf");

    Text instructionsText;
    instructionsText.setFont(font);
    instructionsText.setCharacterSize(24);
    instructionsText.setFillColor(Color::White);
    instructionsText.setString("Instructions:\n\n1. Use the arrow keys to move.\n2. Avoid ghosts.\n3. Eat pellets to score points.\n4. Eat power pellets to temporarily turn ghosts blue and eat them.\n\nPress any key to return to the menu.");

    window.clear();
    window.draw(instructionsText);
    window.display();

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
            if (event.type == Event::Closed)
                window.close();
            else if (event.type == Event::KeyPressed)
                return;
    }
}

void highscores(RenderWindow& window)
{
    Font font;
    font.loadFromFile("arial.ttf");

    Text highscoresText;
    highscoresText.setFont(font);
    highscoresText.setCharacterSize(24);
    highscoresText.setFillColor(Color::White);
    highscoresText.setString("Highscores:\n\n1. Player1 - 1000\n2. Player2 - 800\n3. Player3 - 600\n\nPress any key to return to the menu.");

    window.clear();
    window.draw(highscoresText);
    window.display();
    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
            if (event.type == Event::Closed)
                window.close();
            else if (event.type == Event::KeyPressed)
                return;
    }
}

void* menu_thread(void *arg)
{
    RenderWindow window(VideoMode(1080, 800), "Menu");
    window.setPosition(Vector2i(100, 100));
    window.setFramerateLimit(60);
    Font font;
    font.loadFromFile("arial.ttf");

    // Load the background image
    Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("menu background.jpg")) {
        cerr << "Failed to load background image!" << endl;
        return NULL;
    }
    Sprite backgroundSprite(backgroundTexture);

    Text menuOptions[4];
    string options[4] = {"Start Game", "Instructions", "Highscores", "Exit"};

    for (int i = 0; i < 4; ++i) {
        menuOptions[i].setFont(font);
        menuOptions[i].setString(options[i]);
        menuOptions[i].setCharacterSize(24);
        menuOptions[i].setFillColor(Color::White);
        menuOptions[i].setPosition(50 + i * 300, 600);
    }

    int selectedOption = -1;

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }
            if (event.type == Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == Mouse::Left)
                {
                    Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
                    for (int i = 0; i < 4; ++i)
                    {
                        if (menuOptions[i].getGlobalBounds().contains(mousePos))
                        {
                            selectedOption = i;
                            break;
                        }
                    }
                }
            }
        }

        window.clear();

        // Draw the background image
        window.draw(backgroundSprite);

        for (int i = 0; i < 4; ++i) {
            if (i == selectedOption) {
                menuOptions[i].setFillColor(Color::Green);
            } else {
                menuOptions[i].setFillColor(Color::White);
            }
            window.draw(menuOptions[i]);
        }
        window.display();

        if (selectedOption != -1)
        {
            switch (selectedOption)
            {
                case 0:
                    std::cout << "Starting game..." << std::endl;
                    window.close();
                    game();
                    break;
                case 1:
                    std::cout << "Showing instructions..." << std::endl;
                    instructions(window);
                    break;
                case 2:
                    std::cout << "Showing highscores..." << std::endl;
                    highscores(window);
                    break;
                case 3:
                    window.close();
                    break;
            }
            selectedOption = -1; // Reset selected option
        }
    }

    pthread_exit(NULL);
}

#endif
