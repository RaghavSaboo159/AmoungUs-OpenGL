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
#include "text_renderer.h"
#include "ball_object_collisions.h"

#include <sstream>
#include <iostream>
#include<bits/stdc++.h>
#include <ctime>

using namespace std;


// Game-related State data
SpriteRenderer  *Renderer;
// GameObject      *Player;
BallObject     *Ball,*Ball2; 
TextRenderer  *Text;


  
  

Game::Game(unsigned int width, unsigned int height) 
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height){ 
    this->Win=false;
    this->Health=5;
    this->Destroy=false;
    this->Display=false;
    this->Score=0;
    this->lose=false;
    this->move=0;
    this->task=0;
    this->light=true;
    this->dir=0;
    this->dirv=false;
}   

Game::~Game()
{
    delete Renderer;
    // delete Player;
    delete Ball;
    delete Ball2;
    delete Text;

}
bool CheckCollision(GameObject &one, GameObject &two) // AABB - AABB collision
{
    // collision x-axis?
    bool collisionX = one.Position.x + one.Size.x > two.Position.x &&
        two.Position.x + two.Size.x > one.Position.x;
    // collision y-axis?
    bool collisionY = one.Position.y + one.Size.y > two.Position.y &&
        two.Position.y + two.Size.y > one.Position.y;
    // collision only if on both axes
    return collisionX && collisionY;
} 
bool Game::DoCollisions()
{
    bool result=false;
    for (GameObject &box : this->Levels[this->Level].Bricks)
    {
        if (!box.Destroyed)
        {
            bool collision = CheckCollision(*Ball, box);
            if (collision) // if collision is true
            {
                if(box.val==1)
                    result=true;
                if(box.val==4){
                    box.ChangeSprite(ResourceManager::GetTexture("green"));
                    if(!this->Destroy)
                        this->task++;
                    this->Destroy=true;
                }
                if(box.val==2)
                    this->Score++;
                if(box.val==6)
                    this->Health--;
                if(box.val==3)
                    this->Win=true;
                if(box.val==5){
                    this->Display=true;
                    this->task++;
                }
                if (!box.IsSolid && box.val!=4)
                    box.Destroyed = true;
            }
        }
    }
    return result;
} 
void Game::MazeGen(){
        int n=25;
        vector<vector<int>> vec( n, vector<int> (n, 1));
        stack <pair<int,int>>st;
        vector<pair<int,int>>tempc;
        int x=0;
        int y=0;
        // cout<<x<<" "<<y<<endl;
        st.push({x,y});
        vec[x][y]=0;
        int dx[]={0,0,2,-2};
        int dy[]={2,-2,0,0};
        srand(time(NULL));
        while(!st.empty()){
            pair<int ,int > p= st.top();
            st.pop();
            int x=p.first;
            int y=p.second;
            bool f=false;
            vector<pair<int,int>>vp;
            for(int i=0;i<4;i++)
            {
                int xx=x+dx[i];
                int xy=y+dy[i];
                if(xx>=0 && xx<n && xy>=0 && xy<n)
                {
                    if(vec[xx][xy]==1){
                        vp.push_back({xx,xy});
                        f=true;
                    }
                }

            }
            if(f){
                st.push(p);
                int itr=rand()%vp.size();
                // cout<<itr<<" "<<vp.size()<<endl;
                vec[(x+vp[itr].first)/2][(y+vp[itr].second)/2]=0;
                vec[(vp[itr].first)][(vp[itr].second)]=0;
                st.push(vp[itr]);

            }
        }
        vec[24][24]=3;
        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++)
                if(vec[i][j]==0)
                    tempc.push_back({i,j});
        }
        srand(time(NULL));
        for(int i=0;i<3;i++)
        {
            int itr=rand()%tempc.size();
            if(vec[tempc[itr].first][tempc[itr].second]==0)
                vec[tempc[itr].first][tempc[itr].second]=2;
            else 
            i--;

        }
        for(int i=0;i<3;i++)
        {
            // srand(time(NULL));
            int itr=rand()%tempc.size();
            if(vec[tempc[itr].first][tempc[itr].second]==0)
                vec[tempc[itr].first][tempc[itr].second]=6;
            else 
            i--;

        }
        for(int i=0;i<1;i++)
        {
            // srand(time(NULL));
            int itr=rand()%tempc.size();
            if(vec[tempc[itr].first][tempc[itr].second]==0)
                vec[tempc[itr].first][tempc[itr].second]=5;
            else 
            i--;

        }
        for(int i=0;i<1;i++)
        {
            int itr=rand()%tempc.size();
            if(vec[tempc[itr].first][tempc[itr].second]==0)
                vec[tempc[itr].first][tempc[itr].second]=4;
            else 
            i--;

        }

        ofstream MyFile("../source/levels/one.lvl");

        // Write to the file
        for(int i=0;i<n;i++)
        {
            for(int j=0;j<n;j++){
            MyFile<<vec[i][j]<<" ";
            if(vec[i][j]!=1)
            this->coords.push_back({i,j});
            }
            MyFile <<endl;

        }
        MyFile.close();
        //     for(int i=0;i<this->coords.size();i++){
        // cout<<this->coords[i].first<<" "<<this->coords[i].second<<endl;
    // }


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
    ResourceManager::GetShader("sprite").SetFloat("lightCutOff", 5000.0f);
    // ResourceManager::GetShader("sprite").SetVector3f("lightColor", 1.0f,1.0f,1.0f);
    // set render-specific controls
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
    // load ../source/textures
    ResourceManager::LoadTexture("../source/textures/download.png", false, "background");
    ResourceManager::LoadTexture("../source/textures/f11.png", true, "face");
    ResourceManager::LoadTexture("../source/textures/f12.png", true, "face2");
    ResourceManager::LoadTexture("../source/textures/f13.png", true, "face3");
    ResourceManager::LoadTexture("../source/textures/f14.png", true, "face4");
    ResourceManager::LoadTexture("../source/textures/r11.png", true, "fac");
    ResourceManager::LoadTexture("../source/textures/r12.png", true, "fac2");
    ResourceManager::LoadTexture("../source/textures/r13.png", true, "fac3");
    ResourceManager::LoadTexture("../source/textures/r14.png", true, "fac4");

    ResourceManager::LoadTexture("../source/textures/block.png", false, "block");
    ResourceManager::LoadTexture("../source/textures/block_solid.png", false, "block_solid");
    ResourceManager::LoadTexture("../source/textures/pink_player.png", true, "paddle");
    ResourceManager::LoadTexture("../source/textures/coin.png", true, "coin");
    // ResourceManager::LoadTexture("../source/textures/win.jpeg", false, "win");
    ResourceManager::LoadTexture("../source/textures/awesomeface.png", true, "win");
    ResourceManager::LoadTexture("../source/textures/red.png", true, "red");
    ResourceManager::LoadTexture("../source/textures/green.png", true, "green");
    ResourceManager::LoadTexture("../source/textures/poweron.png", true, "poweron");
    ResourceManager::LoadTexture("../source/textures/spike.png", true, "spike");
    
    Text = new TextRenderer(this->Width, this->Height);
    Text->Load("../source/fonts/OCRAEXT.TTF", 24);
    // load levels
    GameLevel one; one.Load("../source/levels/one.lvl", this->Width, this->Height );
    this->Levels.push_back(one);
    this->Level = 0;
    glm::vec2 ballPos = glm::vec2(0.5, 0.4);
    Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY,ResourceManager::GetTexture("face"));
    int random=this->coords.size()-2;
    // if (random==0)
    // random++;
    glm::vec2 ballPos2 = glm::vec2(this->coords[random].second*32,this->coords[random].first*24);
    Ball2 = new BallObject(ballPos2, 10.0f, INITIAL_BALL_VELOCITY,ResourceManager::GetTexture("paddle"));
    ResourceManager::GetShader("sprite").Use().SetVector3f("lightPos", Ball->Position.x,Ball->Position.y,0.0f);

    // Text = new TextRenderer(this->Width, this->Height);
    // Text->Load("fonts/ocraext.TTF", 24);
    this->begin=clock();
}

void Game::Update(float dt)
{
    this->BFS(dt);
    // cout<<Ball->Position.x<<" "<<Ball->Position.y<<endl;
    ResourceManager::GetShader("sprite").Use().SetVector3f("lightPos", Ball->Position.x,Ball->Position.y,0.0f);
    if(!this->dirv){
    if(this->dir%4==0)
        Ball->ChangeSprite(ResourceManager::GetTexture("face"));
    else if(this->dir%4==1)
        Ball->ChangeSprite(ResourceManager::GetTexture("face2"));
    else if(this->dir%4==2)
        Ball->ChangeSprite(ResourceManager::GetTexture("face3"));
    else if(this->dir%4==3)
        Ball->ChangeSprite(ResourceManager::GetTexture("face4"));
    }
    else if(this->dirv){
    if(this->dir%4==0)
        Ball->ChangeSprite(ResourceManager::GetTexture("fac"));
    else if(this->dir%4==1)
        Ball->ChangeSprite(ResourceManager::GetTexture("fac2"));
    else if(this->dir%4==2)
        Ball->ChangeSprite(ResourceManager::GetTexture("fac3"));
    else if(this->dir%4==3)
        Ball->ChangeSprite(ResourceManager::GetTexture("fac4"));
    }

    if(this->Win==true && this->State==GAME_ACTIVE){
        this->State = GAME_WIN;

    }
    if(this->Display==true){
         for (GameObject &box : this->Levels[this->Level].Bricks){
             if(box.val==2 || box.val==6)
                box.Destroyed=false;
         }
         this->Display=false;
    }
    if(!this->Destroy && this->State==GAME_ACTIVE && CheckCollision(*Ball,*Ball2) ){
        this->State = GAME_WIN;
        this->lose=true;
    
    }
    if(this->Health==0 && this->State==GAME_ACTIVE ){
        this->State = GAME_WIN;
        this->lose=true;
    
    }
    this->now = clock();
    double elapsed_secs = double(this->now - this->begin) / CLOCKS_PER_SEC;
    double time_left=100-elapsed_secs;
    if(time_left<0 && this->State==GAME_ACTIVE){
        this->State = GAME_WIN;
        this->lose=true;

    }
    if(!this->light){
    ResourceManager::GetShader("sprite").Use().SetFloat("lightCutOff", 50.0f);
    this->Score+=dt;
    }
    else{
    ResourceManager::GetShader("sprite").Use().SetFloat("lightCutOff", 5000.0f);

    }
    if(this->task==2){
        for (GameObject &box : this->Levels[this->Level].Bricks){
            if(box.val==3)
                box.Destroyed=false;
       }
    }


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
                bool ans=this->DoCollisions();
                if(ans)
                Ball->Position.x += velocity;
                else {
                this->dir++;
                this->dirv=true;
                }
            }
            
        }
        if (this->Keys[GLFW_KEY_D])
        {
            if (Ball->Position.x <= this->Width - Ball->Size.x){
                Ball->Position.x += velocity;
                bool ans=this->DoCollisions();
                if(ans)
                Ball->Position.x -= velocity;
                else {
                this->dir++;
                this->dirv=false;
                }

            }
            
        }
        if (this->Keys[GLFW_KEY_W])
        {
            if (Ball->Position.y >= 0.0f){
                Ball->Position.y -= velocity;
                bool ans=this->DoCollisions();
                if(ans)
                Ball->Position.y += velocity;
                else 
                this->dir++;

                }
        }
        if (this->Keys[GLFW_KEY_S])
        {
            if (Ball->Position.y <= this->Height - Ball->Size.y){
                Ball->Position.y += velocity;
                bool ans=this->DoCollisions();
                if(ans)
                Ball->Position.y -= velocity;
                else 
                this->dir++;

            }
        }
        if (this->Keys[GLFW_KEY_L])
        {
            if(this->light==false)
                this->light=true;
            else
                this->light=false;

        }
        // ResourceManager::GetShader("sprite").SetVector3f("lightPos", Ball->Position.x,Ball->Position.y,0.0f);


    }
}

void Game::Render()
{
    if(this->State == GAME_ACTIVE){
        // draw background
        // ResourceManager::GetShader("sprite").SetVector3f("lightPos", Ball->Position.x,Ball->Position.y,0.0f);

        Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f,glm::vec3(0.0f,0.0f,0.0f));
        // draw level
        this->Levels[this->Level].Draw(*Renderer);
        Ball->Draw(*Renderer);
        if(this->Destroy==false)
            Ball2->Draw(*Renderer);
        this->now = clock();
        double elapsed_secs = double(this->now - this->begin) / CLOCKS_PER_SEC;
        unsigned int time_left=100-elapsed_secs;
        std::stringstream sh; sh << this->Health;
        std::stringstream ss; ss << this->Score;
        std::stringstream st; st << this->task;
        std::stringstream sti; sti << time_left;

        if(!this->light){
            Text->RenderText("Lives:" + sh.str()+ " Score:" +  ss.str()+ " Tasks: " + st.str()+ "/2 Time Left:" + sti.str()+ " Light: OFF" , 5.0f, 5.0f, 0.8f,glm::vec3(1.0f, 1.0f, 1.0f));
        }
        else{
            Text->RenderText("Lives:" + sh.str()+ " Score:" +  ss.str()+ " Tasks: " + st.str()+ "/2 Time Left:" + sti.str() + " Light: ON" , 5.0f, 5.0f, 0.8f,glm::vec3(1.0f, 1.0f, 1.0f));
        }

    }
    if (this->State == GAME_WIN && this->Win)
        Text->RenderText("You WON!!!", 320.0f, this->Height / 2.0f - 20.0f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    // Text->RenderText("Press ENTER to retry or ESC to quit", 130.0f, this->Height / 2.0f, 1.0f, glm::vec3(1.0f, 1.0f, 0.0f));
    if (this->State == GAME_WIN && this->lose)
        Text->RenderText("You LOST!!!", 320.0f, this->Height / 2.0f - 20.0f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));

}
void Game::ResetLevel()
{
    this->Levels[0].Load("../source/levels/one.lvl", this->Width, this->Height / 2);
    this->Health = 5;

}

void Game::ResetPlayer(){
    Ball->Reset(glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -(BALL_RADIUS * 2.0f)), INITIAL_BALL_VELOCITY);
}
void Game:: BFS(float dt){
    float velocity = (PLAYER_VELOCITY * dt)/10;
    unsigned int height = 25;
    unsigned int width = 25; // note we can index vector at [0] since this function is only called if height > 0
    int x_coord = this->Width / width, y_coord= this->Height / height; 
    int destx=(int )Ball->Position.x/(int )x_coord;
    int desty=(int )Ball->Position.y/(int)y_coord;
    int xx=(int )Ball2->Position.x/(int )x_coord;
    int yy=(int )Ball2->Position.y/(int )y_coord;
    float kx=xx*x_coord;
    float yx=yy*y_coord;
    // cout<<this->move<<"moveval"<<endl;
    if(this->move==0){
    set<pair<int ,pair<int ,int >>>s;
    // cout<<x_coord<<" "<<y_coord<<endl;
    // cout<<destx<<" "<<desty<<" "<<xx<<" "<<yy<<endl;
    // cout<<(int )Ball2->Position.x/(int )x_coord<<" "<<(int)Ball2->Position.y/(int)y_coord<<endl;
    // cout<<Ball2->Position.x<<"CCC "<<Ball2->Position.y<<endl;
    map<pair<int,int>,int>mp;
    map<pair<int,int>,pair<int,int>>par;
    int dx[]={0,0,1,-1};
    int dy[]={1,-1,0,0};

    s.insert({0,{xx,yy}});
    pair <int,int> initt={-1,-1};
    par[{xx,yy}]=initt;
    bool flag=false;
    mp[{xx,yy}]=0;
    while(!s.empty()){
        auto it=*s.begin();
        int  x=it.second.first;
        int  y=it.second.second;
        s.erase(it);
        for(int i=0;i<4;i++){
            int  nx=x+(dx[i]);
            int  ny=y+(dy[i]);
            // cout<<nx<<" "<<ny<<endl;
            if(nx>=0 && nx<25 && ny>=0 && ny<25){
                if(mp.find({nx,ny})==mp.end() || mp[{nx,ny}]>1+mp[{x,y}]){
                    pair<int ,int > cd= {ny,nx};
                    bool fnd=false;
                    for(int i=0;i<this->coords.size();i++){
                        if(this->coords[i]==cd){
                            fnd=true;
                        }
                    }
                    if (fnd){
                        pair<int ,int > ncd= {x,y};
                        if(s.find({mp[{nx,ny}],{nx,ny}})!=s.end())
                        s.erase({mp[{nx,ny}],{nx,ny}});
                        mp[{nx,ny}]=mp[{x,y}]+1;
                        par[{nx,ny}]=ncd;
                        
                        s.insert({mp[{nx,ny}],{nx,ny}});
                        if(destx==nx && desty==ny){
                            flag=true;
                        }
                    }
                }

            }
        }
        // if(flag)
        //     break;
    }
    // cout<<flag<<"====C"<<endl;
    if(flag)
    {
        int x=destx;
        int y=desty;
        // pair <float,float> coord={Ball2->Position.x,Ball2->Position.y};
        pair <int,int>pp={xx,yy};
        
            // cout<<pp.first<<" "<<pp.second<<endl;
            // cout<<x<<" "<<y<<endl;
        while(par[{x,y}]!=pp )
        {
            // cout<<"innn";
            pair<int,int> tp=par[{x,y}];
            x=tp.first;
            y=tp.second;
        }
        // cout<<Ball2->Position.x<<"AAA "<<Ball2->Position.y<<endl;
        this->fx=x;
        this->fy=y;
        if(x!=xx)
        {
            if(x>xx &&  (Ball2->Position.x+velocity)<=this->Width){
                // cout<<"INCX"<<endl;
                // Ball2->Position.x+=velocity;
                // bool ans=this->DoCollisionsimp();
                // if(ans)
                // Ball2->Position.x -= velocity;
                this->move=1;
                }
            else if (x<xx &&  (Ball2->Position.x-velocity)>=0){
                // cout<<"DECX"<<endl;
                // Ball2->Position.x-=velocity;
                // bool ans=this->DoCollisionsimp();
                // if(ans)
                // Ball2->Position.x += velocity;
                this->move=2;
            }
        }
        if(y!=yy)
        {
            if(y>yy &&  (Ball2->Position.y+velocity)<=this->Height){
                // cout<<"INCY"<<endl;
                // Ball2->Position.y+=velocity;
                // bool ans=this->DoCollisionsimp();
                // if(ans)
                // Ball2->Position.y -= velocity;
                this->move=3;
            }
            else if (y<yy &&  (Ball2->Position.y-velocity)>=0){
                // cout<<"DECY"<<endl;
                // Ball2->Position.y-=velocity;
                // bool ans=this->DoCollisionsimp();
                // if(ans)
                // Ball2->Position.y += velocity;
                this->move=4;
            }

        }
        // cout<<Ball2->Position.x<<"BBB "<<Ball2->Position.y<<endl;

        }
    // }
    }
    else if(this->move==1){
        // float kkx=xx*x_coord;
        // float yx=yy*y_coord;

        if(xx!=this->fx)
        {
            Ball2->Position.x+=velocity;
        }
        // else if ()
        else
            this->move=0;
    }
    else if(this->move==2){

        if((this->fx!=xx))
            Ball2->Position.x-=velocity;
        else if (this->fx==xx && (Ball2->Position.x-kx)>=8)
            Ball2->Position.x-=velocity;
        else
            this->move=0;
    }
    else if(this->move==3){

        if(this->fy!=yy)
        {
            Ball2->Position.y+=velocity;
        }
        else
            this->move=0;
    }
    else if(this->move==4){

        if(this->fy!=yy)
        {
            Ball2->Position.y-=velocity;
        }
        else if (this->fy==yy && (Ball2->Position.y-yx)>=1)
            Ball2->Position.y-=velocity;
        else
            this->move=0;
    }

}