/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "game_object.h"
#include "ball_object_collisions.h"
#include <iostream>


// Game-related State data
SpriteRenderer  *Renderer;
GameObject      *Player;
BallObject     *Ball; 


  
  

Game::Game(unsigned int width, unsigned int height) 
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{ 

}

Game::~Game()
{
    delete Renderer;
    delete Player;
    delete Ball;
}

void Game::Init()
{
    // load shaders
    ResourceManager::LoadShader("../source/shaders/sprite.vs", "../source/shaders/sprite.frag", nullptr, "sprite");
    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), 
        static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    // set render-specific controls
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
    // load ../source/textures
    ResourceManager::LoadTexture("../source/textures/background.jpg", false, "background");
    ResourceManager::LoadTexture("../source/textures/awesomeface.png", true, "face");
    ResourceManager::LoadTexture("../source/textures/block.png", false, "block");
    ResourceManager::LoadTexture("../source/textures/block_solid.png", false, "block_solid");
    ResourceManager::LoadTexture("../source/textures/pink_player.png", true, "paddle");
    // load levels
    GameLevel one; one.Load("../source/levels/one.lvl", this->Width, this->Height / 2);
    GameLevel two; two.Load("../source/levels/two.lvl", this->Width, this->Height / 2);
    GameLevel three; three.Load("../source/levels/three.lvl", this->Width, this->Height / 2);
    GameLevel four; four.Load("../source/levels/four.lvl", this->Width, this->Height / 2);
    this->Levels.push_back(one);
    this->Levels.push_back(two);
    this->Levels.push_back(three);
    this->Levels.push_back(four);
    this->Level = 0;
    // configure game objects
    glm::vec2 playerPos = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
    Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));
    glm::vec2 ballPos = glm::vec2(-0.102f, 116.805f);
    Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY,ResourceManager::GetTexture("face"));
}

void Game::Update(float dt)
{
    // Ball->Move(dt, this->Width);
    this->DoCollisions();
    //     if (Ball->Position.y >= this->Height) // did ball reach bottom edge?
    // {
    //     this->ResetLevel();
    //     this->ResetPlayer();
    // }

}

void Game::ProcessInput(float dt)
{
    if (this->State == GAME_ACTIVE)
    {
        float velocity = PLAYER_VELOCITY * dt;
        // move playerboard
        if (this->Keys[GLFW_KEY_A])
        {
            if (Ball->Position.x >= 0.0f){
                Ball->Position.x -= velocity;
                // if (Ball->Stuck)
                    // Ball->Position.x -= velocity;
            }
        }
        if (this->Keys[GLFW_KEY_D])
        {
            if (Ball->Position.x <= this->Width - Ball->Size.x){
                Ball->Position.x += velocity;
                //  if (Ball->Stuck)
                    // Ball->Position.x += velocity;
            }
        }
        if (this->Keys[GLFW_KEY_W])
        {
            if (Ball->Position.y >= 0.0f)
                Ball->Position.y -= velocity;
        }
        if (this->Keys[GLFW_KEY_S])
        {
            if (Ball->Position.y <= this->Height - Ball->Size.y)
                Ball->Position.y += velocity;
        }
        // if (this->Keys[GLFW_KEY_SPACE])
        //     Ball->Stuck = false;

    }
//  std::cout<<Player->Position.x<<" "<<Player->Position.y<<std::endl;
}

void Game::Render()
{
    if(this->State == GAME_ACTIVE)
    {
        // draw background
        Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
        // draw level
        this->Levels[this->Level].Draw(*Renderer);
        // draw player
        Player->Draw(*Renderer);
        Ball->Draw(*Renderer);

    }
}
void Game::ResetLevel()
{
    if (this->Level == 0)
        this->Levels[0].Load("../source/levels/one.lvl", this->Width, this->Height / 2);
    else if (this->Level == 1)
        this->Levels[1].Load("../source/levels/two.lvl", this->Width, this->Height / 2);
    else if (this->Level == 2)
        this->Levels[2].Load("../source/levels/three.lvl", this->Width, this->Height / 2);
    else if (this->Level == 3)
        this->Levels[3].Load("../source/levels/four.lvl", this->Width, this->Height / 2);
}

void Game::ResetPlayer()
{
    // reset player/ball stats
    Player->Size = PLAYER_SIZE;
    Player->Position = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
    Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -(BALL_RADIUS * 2.0f)), INITIAL_BALL_VELOCITY);
}

// Collision CheckCollision(BallObject &one, GameObject &two);
// Direction VectorDirection(glm::vec2 closest);
Direction VectorDirection(glm::vec2 target)
{
    glm::vec2 compass[] = {
        glm::vec2(0.0f, 1.0f),	// up
        glm::vec2(1.0f, 0.0f),	// right
        glm::vec2(0.0f, -1.0f),	// down
        glm::vec2(-1.0f, 0.0f)	// left
    };
    float max = 0.0f;
    unsigned int best_match = -1;
    for (unsigned int i = 0; i < 4; i++)
    {
        float dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max)
        {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
}    
Collision CheckCollision(GameObject &one, GameObject &two) // AABB - AABB collision
{
    // collision x-axis?
    glm::vec2 center(one.Position + BALL_RADIUS);
    // calculate AABB info (center, half-extents)
    glm::vec2 aabb_half_extents(two.Size.x / 2.0f, two.Size.y / 2.0f);
    glm::vec2 aabb_center(
        two.Position.x + aabb_half_extents.x, 
        two.Position.y + aabb_half_extents.y
    );
    // get difference vector between both centers
    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    // add clamped value to AABB_center and we get the value of box closest to circle
    glm::vec2 closest = aabb_center + clamped;
    // retrieve vector between center circle and closest point AABB and check if length <= radius
    difference = closest - center;
    if (glm::length(difference) <= BALL_RADIUS)
        return std::make_tuple(true, VectorDirection(difference), difference);
    else
        return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
    // return glm::length(difference) < BALL_RADIUS;
}  
void Game::DoCollisions()
{
    for (GameObject &box : this->Levels[this->Level].Bricks)
    {
        if (!box.Destroyed)
        {
            Collision collision = CheckCollision(*Ball, box);
            if (std::get<0>(collision)) // if collision is true
            {
                // destroy block if not solid
                if (!box.IsSolid)
                    box.Destroyed = true;
                // collision resolution
                Direction dir = std::get<1>(collision);
                glm::vec2 diff_vector = std::get<2>(collision);
                if (dir == LEFT || dir == RIGHT) // horizontal collision
                {
                    Ball->Velocity.x = -Ball->Velocity.x; // reverse horizontal velocity
                    // relocate
                    float penetration = Ball->Radius - std::abs(diff_vector.x);
                    if (dir == LEFT)
                        Ball->Position.x += penetration; // move ball to right
                    else
                        Ball->Position.x -= penetration; // move ball to left;
                }
                else // vertical collision
                {
                    Ball->Velocity.y = -Ball->Velocity.y; // reverse vertical velocity
                    // relocate
                    float penetration = Ball->Radius - std::abs(diff_vector.y);
                    if (dir == UP)
                        Ball->Position.y -= penetration; // move ball back up
                    else
                        Ball->Position.y += penetration; // move ball back down
                }
            }
            
        }
    }
     Collision result = CheckCollision(*Ball, *Player);
            if (!Ball->Stuck && std::get<0>(result))
            {
                // check where it hit the board, and change velocity based on where it hit the board
                float centerBoard = Player->Position.x + Player->Size.x / 2.0f;
                float distance = (Ball->Position.x + Ball->Radius) - centerBoard;
                float percentage = distance / (Player->Size.x / 2.0f);
                // then move accordingly
                float strength = 2.0f;
                glm::vec2 oldVelocity = Ball->Velocity;
                Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength; 
                Ball->Velocity.y = -Ball->Velocity.y;
                Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
            } 
} 
