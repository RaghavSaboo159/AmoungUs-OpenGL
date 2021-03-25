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
// #include "text_renderer.h"
#include "ball_object_collisions.h"

#include <sstream>
#include <iostream>
#include<bits/stdc++.h>
using namespace std;


// Game-related State data
SpriteRenderer  *Renderer;
GameObject      *Player;
BallObject     *Ball,*Ball2; 
// TextRenderer  *Text;


  
  

Game::Game(unsigned int width, unsigned int height) 
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{ 

}

Game::~Game()
{
    delete Renderer;
    delete Player;
    delete Ball;
    delete Ball2;

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
    glm::vec2 ballPos2 = glm::vec2(700.0f, 116.805f);
    Ball2 = new BallObject(ballPos2, BALL_RADIUS, INITIAL_BALL_VELOCITY,ResourceManager::GetTexture("paddle"));
    // Text = new TextRenderer(this->Width, this->Height);
    // Text->Load("fonts/ocraext.TTF", 24);
}

void Game::Update(float dt)
{
    // Ball2->Move(dt, this->Width);
    this->DoCollisions();
    this->Bfs(dt);
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
        // std::stringstream ss; ss << this->Lives;
        // Text->RenderText("Lives:" + ss.str(), 5.0f, 5.0f, 1.0f);

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
        Ball2->Draw(*Renderer);

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
    this->Lives = 3;

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
Collision CheckCollision(glm :: vec2 &position , GameObject &two) // AABB - AABB collision
{
    // collision x-axis?
    glm::vec2 center(position + BALL_RADIUS);
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
            Collision collision = CheckCollision(Ball->Position, box);
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
            collision = CheckCollision(Ball2->Position, box);
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
                    Ball2->Velocity.x = -Ball2->Velocity.x; // reverse horizontal velocity
                    // relocate
                    float penetration = Ball2->Radius - std::abs(diff_vector.x);
                    if (dir == LEFT)
                        Ball2->Position.x += penetration; // move ball2 to right
                    else
                        Ball2->Position.x -= penetration; // move ball2 to left;
                }
                else // vertical collision
                {
                    Ball2->Velocity.y = -Ball2->Velocity.y; // reverse vertical velocity
                    // relocate
                    float penetration = Ball2->Radius - std::abs(diff_vector.y);
                    if (dir == UP)
                        Ball2->Position.y -= penetration; // move ball2 back up
                    else
                        Ball2->Position.y += penetration; // move ball2 back down
                }
            }
            
        }
    }

} 

void Game :: Bfs(float dt){
    // dt=0.0001;
    // set<pair<float,pair<float,float>>>s;
    // s.insert({0,{Ball2->Position.x,Ball2->Position.y}});
    // map<pair<float,float>,float>mp;
    // map<pair<float,float>,pair<float,float>>par;
    int dx[]={0,0,1,-1};
    int dy[]={1,-1,0,0};
    float velocity = PLAYER_VELOCITY * (dt/5);
    // // cout<<dt<<" "<<PLAYER_VELOCITY<<endl;
    // par[{Ball2->Position.x,Ball2->Position.y}]={-1,-1};
    // bool flag=false;
    // while(!s.empty()){
    //     auto it=*s.begin();
    //     float x=it.second.first;
    //     float y=it.second.second;
    //     s.erase(it);
    //     for(int i=0;i<4;i++){
    //         float nx=x+(dx[i]*velocity);
    //         float ny=y+(dy[i]*velocity);
    //         // cout<<nx<<" "<<ny<<endl;
    //         if(nx>=0 && nx<this->Width && ny>=0 && ny<this->Height)
    //         {
    //             if(mp.find({nx,ny})==mp.end() || mp[{nx,ny}]>1+mp[{x,y}])
    //             {
    //                 glm::vec2 newpos(nx,ny);
    //                 for (GameObject &box : this->Levels[this->Level].Bricks)
    //                 {
    //                     if (!box.Destroyed)
    //                     {
    //                         Collision collision = CheckCollision(newpos, box);
    //                         if (std::get<0>(collision)) // if collision is true
    //                             continue;
    //                         else{
    //                             mp[{nx,ny}]=mp[{x,y}]+1;
    //                             par[{nx,ny}]={x,y};
    //                             s.insert({mp[{nx,ny}],{nx,ny}});
    //                             if(fabs(nx-Ball->Position.x)<=velocity && fabs(ny-Ball->Position.y)<=velocity){
    //                                 flag=true;
    //                                 par[{Ball->Position.x,Ball->Position.y}]={nx,ny};
    //                                 mp[{Ball->Position.x,Ball->Position.y}]=mp[{nx,ny}]+1;

    //                             }
    //                         }
    //                     }
    //                 }
    //             }
    //         }

    //     }
    //     if(flag)
    //         break;
    // }
    // cout<<"here"<<endl;
    // if(flag)
    // {
    //     float x=Ball->Position.x;
    //     float y=Ball->Position.y;
    //     pair <float,float> coord={Ball2->Position.x,Ball2->Position.y};
    //     while(par[{x,y}]!=coord)
    //     {
    //         x=par[{x,y}].first;
    //         y=par[{x,y}].second;
    //     }
    //     Ball2->Position.x=x;
    //     Ball2->Position.y=y;

    // }
    float mn=1e5;
    float x=Ball2->Position.x;
    float y=Ball2->Position.y;
    float xx=Ball->Position.x;
    float yy=Ball->Position.y;
    float tempx=-1;
    float tempy=-1;

    for(int i=0;i<4;i++){
        // float nx =
        float nx=x+(dx[i]*velocity);
        float ny=y+(dy[i]*velocity);
        if(nx>=0 && nx<this->Width && ny>=0 && ny<this->Height){
            glm::vec2 newpos(nx,ny);
            for (GameObject &box : this->Levels[this->Level].Bricks)
            {
                if (!box.Destroyed)
                {
                    Collision collision = CheckCollision(newpos, box);
                    if (std::get<0>(collision)) // if collision is true
                        continue;
                    else{
                        if((fabs(nx-xx) + fabs(ny-yy))<=mn){
                            mn=(fabs(nx-xx) + fabs(ny-yy));
                            tempx=nx;
                            tempy=ny;
                        }
                    }
                }
            }
        }
    }
    if(tempx!=-1 && tempy!=-1){
        Ball2->Position.x=tempx;
        Ball2->Position.y=tempy;

    }
}