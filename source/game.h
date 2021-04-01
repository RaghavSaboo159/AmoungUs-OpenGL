/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "game_level.h"
#include <bits/stdc++.h>
#include <ctime>
//   clock_t begin = clock();
 using namespace std;
// Represents the current state of the game
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};
enum Direction {
	UP,
	RIGHT,
	DOWN,
	LEFT
};

typedef std::tuple<bool, Direction, glm::vec2> Collision;    


// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(25.0f, 20.0f);
// Initial velocity of the player paddle
const float PLAYER_VELOCITY(500.0f);

const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
// Radius of the ball object
const float BALL_RADIUS = 11.5f;

// Game holds all game-related state and functionality.
// Combines all game-related data into a single class for
// easy access to each of the components and manageability.
class Game
{
public:
    // game state
    GameState               State;	
    bool                    Keys[1024];
    unsigned int            Width, Height;
    std::vector<GameLevel>  Levels;
    unsigned int            Level;
    unsigned int            Health;
    float                   Score;
    bool                    Win;
    bool                    Destroy;
    bool                    Display;
    bool                    lose;
    unsigned int            move;
    unsigned int            fx;
    unsigned int            fy;
    unsigned int            task;
    clock_t                 begin;
    clock_t                 now;
    bool                    light;
    unsigned int            dir;
    bool                    dirv;

    vector<pair<int,int>>   coords;
    



    // constructor/destructor
    Game(unsigned int width, unsigned int height);
    ~Game();
    // initialize game state (load all shaders/textures/levels)
    void Init();
    void MazeGen();

    // game loop
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();
    bool DoCollisions();
    void ResetLevel();
    void ResetPlayer();
    void BFS(float dt);



};

#endif