#include "BotController.h"
#include <iostream>
#include <cstring>
#include <cfloat>

//for ai, generally move towards center if player near edge shortest path towards where player is headed or is depending on difficulty
// , if in close proximity to fish or closer to fish than player shortest path to fish.

BotController::BotController() {
  state = 0;

};

void BotController::update(sf::RenderWindow &window, GameLogic &logic, float dSec, int playerNum, Animation &anim) {


    float bot_handicap = 1.0;  // higher number == slower bot

    sf::Vector2f dir = sf::Vector2f(0,0);
    sf::Vector2f w1_pos = logic.walrus1.getPos();
    sf::Vector2f w2_pos = logic.walrus2.getPos();
    sf::Vector2f w1_vel = logic.walrus1.getVel();
    sf::Vector2f w2_vel = logic.walrus2.getVel();


    if (playerNum == 1) {
        //process input for player 1
        if(!logic.walrus2.isDead() && w2_vel.x>=15 || w2_vel.y>=15 && (state != 0)){
          changeState(0);
          calculatePath(logic, playerNum);
        }
        if(!logic.walrus2.isDead() && (state != 1) && (w2_vel.x<15 || w2_vel.y<15)){
          changeState(1);
          calculatePath(logic, playerNum);
        }
        if(logic.walrus2.isDead() && (state != 2)){
          changeState(2);
          calculatePath(logic, playerNum);
        }
        else if(int(w1_pos.x/20)==cellDetails[int(w1_pos.x/20)+1][int(w1_pos.y/20)].pi && int(w1_pos.y/20)==cellDetails[int(w1_pos.x/20)+1][int(w1_pos.y/20)].pj){
          //direction = right
          dir.x +=1;
        }
        else if(int(w1_pos.x/20)==cellDetails[int(w1_pos.x/20)][int(w1_pos.y/20)+1].pi && int(w1_pos.y/20)==cellDetails[int(w1_pos.x/20)][int(w1_pos.y/20)+1].pj){
          //direction = down
          dir.y +=1;
        }
        else if(int(w1_pos.x/20)==cellDetails[int(w1_pos.x/20)][int(w1_pos.y/20)-1].pi && int(w1_pos.y/20)==cellDetails[int(w1_pos.x/20)][int(w1_pos.y/20)-1].pj){
          //direction = up
          dir.y -=1;
        }
        else if(int(w1_pos.x/20)==cellDetails[int(w1_pos.x/20)-1][int(w1_pos.y/20)-1].pi && int(w1_pos.y/20)==cellDetails[int(w1_pos.x/20)-1][int(w1_pos.y/20)-1].pj){
          //direction = left up
          dir.x -=1;
          dir.y -= 1;
        }
        else if(int(w1_pos.x/20)==cellDetails[int(w1_pos.x/20)+1][int(w1_pos.y/20)-1].pi && int(w1_pos.y/20)==cellDetails[int(w1_pos.x/20)+1][int(w1_pos.y/20)-1].pj){
          //direction = right up
          dir.x +=1;
          dir.y-=1;
        }
        else if(int(w1_pos.x/20)==cellDetails[int(w1_pos.x/20)-1][int(w1_pos.y/20)+1].pi && int(w1_pos.y/20)==cellDetails[int(w1_pos.x/20)-1][int(w1_pos.y/20)+1].pj){
          //direction = down left
          dir.x -= 1;
          dir.y +=1;
        }
        else if(int(w1_pos.x/20)==cellDetails[int(w1_pos.x/20)+1][int(w1_pos.y/20)+1].pi && int(w1_pos.y/20)==cellDetails[int(w1_pos.x/20)+1][int(w1_pos.y/20)+1].pj){
          //direction = down right
          dir.y +=1;
          dir.x += 1;
        }
        logic.walrus1.applyActiveForce(dir, dSec/bot_handicap);

    } else {
        //process input for player 2
        if(!logic.walrus1.isDead() && w1_vel.x>=15 || w1_vel.y>=15 && (state != 0)){
          changeState(0);
          calculatePath(logic, playerNum);
        }
        if(!logic.walrus1.isDead() && (state != 1)  && (w1_vel.x<15 || w1_vel.y<15)){
          changeState(1);
          calculatePath(logic, playerNum);
        }
        if(logic.walrus1.isDead() && (state != 2)){
          changeState(2);
          calculatePath(logic, playerNum);
        }
        if(int(w2_pos.x/20)==cellDetails[int(w2_pos.x/20)-1][int(w2_pos.y/20)].pi && int(w2_pos.y/20)==cellDetails[int(w2_pos.x/20)-1][int(w2_pos.y/20)].pj){
          //direction = left
          dir.x -=1;
        }
        else if(int(w2_pos.x/20)==cellDetails[int(w2_pos.x/20)+1][int(w2_pos.y/20)].pi && int(w2_pos.y/20)==cellDetails[int(w2_pos.x/20)+1][int(w2_pos.y/20)].pj){
          //direction = right
          dir.x +=1;
        }
        else if(int(w2_pos.x/20)==cellDetails[int(w2_pos.x/20)][int(w2_pos.y/20)+1].pi && int(w2_pos.y/20)==cellDetails[int(w2_pos.x/20)][int(w2_pos.y/20)+1].pj){
          //direction = down
          dir.y +=1;
        }
        else if(int(w2_pos.x/20)==cellDetails[int(w2_pos.x/20)][int(w2_pos.y/20)-1].pi && int(w2_pos.y/20)==cellDetails[int(w2_pos.x/20)][int(w2_pos.y/20)-1].pj){
          //direction = up
          dir.y -=1;
        }
        else if(int(w2_pos.x/20)==cellDetails[int(w2_pos.x/20)-1][int(w2_pos.y/20)-1].pi && int(w2_pos.y/20)==cellDetails[int(w2_pos.x/20)-1][int(w2_pos.y/20)-1].pj){
          //direction = left up
          dir.x -=1;
          dir.y -= 1;
        }
        else if(int(w2_pos.x/20)==cellDetails[int(w2_pos.x/20)+1][int(w2_pos.y/20)-1].pi && int(w2_pos.y/20)==cellDetails[int(w2_pos.x/20)+1][int(w2_pos.y/20)-1].pj){
          //direction = right up
          dir.x +=1;
          dir.y-=1;
        }
        else if(int(w2_pos.x/20)==cellDetails[int(w2_pos.x/20)-1][int(w2_pos.y/20)+1].pi && int(w2_pos.y/20)==cellDetails[int(w2_pos.x/20)-1][int(w2_pos.y/20)+1].pj){
          //direction = down left
          dir.x -= 1;
          dir.y +=1;
        }
        else if(int(w2_pos.x/20)==cellDetails[int(w2_pos.x/20)+1][int(w2_pos.y/20)+1].pi && int(w2_pos.y/20)==cellDetails[int(w2_pos.x/20)+1][int(w2_pos.y/20)+1].pj){
          //direction = down right
          dir.y +=1;
          dir.x += 1;
        }
        logic.walrus2.applyActiveForce(dir, dSec/bot_handicap);

    }
    anim.updateMovement(dir, dSec/bot_handicap);

    // process events
    sf::Event Event;
    while (window.pollEvent(Event)) {
        switch (Event.type) {
            //window closed
            case sf::Event::Closed:
                window.close();
                break;
                //window out of focus
            case sf::Event::LostFocus:
                logic.togglePause();
                break;
            case sf::Event::GainedFocus:
                break;
            case sf::Event::KeyPressed:
                if (Event.key.code == sf::Keyboard::P) {
                    logic.togglePause();
                }
                break;
        }
    }
};

void BotController::calculatePath(GameLogic &logic, int playerNum){
  sf::Vector2f w_pos;
  sf::Vector2f w_vel;
  if(playerNum==1){
    w_pos = logic.walrus1.getPos();
    w_vel = logic.walrus1.getVel();
  }
  else{
    w_pos = logic.walrus2.getPos();
    w_vel = logic.walrus2.getVel();
  }
  memset(closedList, false, sizeof (closedList));
  int i, j, x, y;
  int newCost;
  if(state == 0){
    //calculate safe positions, for now middle of the stage.
    x = 20;
    y = 15;
  }
  else if(state == 1){
    if(playerNum==1){
      x = logic.walrus2.getPos().x;
      y = logic.walrus2.getPos().y;
    }
    else {
      x = logic.walrus1.getPos().x;
      y = logic.walrus1.getPos().y;
    }
  }
  else{
    if(playerNum==1){
      x = 39;
      y = 15;
    }
    else {
      x = 0;
      y = 15;
    }
  }
  for (i=0; i<40; i++)
  {
      for (j=0; j<30; j++)
      {
          cellDetails[i][j].f = FLT_MAX;
          cellDetails[i][j].g = FLT_MAX;
          cellDetails[i][j].h = FLT_MAX;
          cellDetails[i][j].pos_x = i;
          cellDetails[i][j].pos_y = j;
          cellDetails[i][j].pi = -1;
          cellDetails[i][j].pj = -1;
      }
  }
  i = w_pos.x/20;
  j = w_pos.y/20;
  cellDetails[i][j].f = 0;
  cellDetails[i][j].g = 0;
  cellDetails[i][j].h = 0;
  cellDetails[i][j].pi = i;
  cellDetails[i][j].pj = j;
  openList.push(cellDetails[i][j]);
  while(!openList.empty()){
    cell n = openList.front();
    i = n.pos_x;
    j = n.pos_y;
    openList.pop();
    closedList[i][j] = true;
    if (i == x && j == y){
      break;
    }; //if destination, end search
    newCost = cellDetails[i][j].g+std::abs((i-1)-(w_pos.x/20))+std::abs((j-1)-(w_pos.y/20));
    if(!closedList[i-1][j-1] && newCost<cellDetails[i-1][j-1].g && logic.stage.getTileDura(i-1,j-1,logic.getStageProgression())>=.5){
      cellDetails[i-1][j-1].g = newCost;
      cellDetails[i-1][j-1].h = std::abs((i-1)-(x))+std::abs((j-1)-(y));
      cellDetails[i-1][j-1].f = cellDetails[i-1][j-1].h + cellDetails[i-1][j-1].g;
      cellDetails[i-1][j-1].pi = i;
      cellDetails[i-1][j-1].pj = j;
      openList.push(cellDetails[i-1][j-1]);
    }
    newCost = cellDetails[i][j].g+std::abs((i)-(w_pos.x/20))+std::abs((j-1)-(w_pos.y/20));
    if(!closedList[i][j-1] && newCost<cellDetails[i][j-1].g && logic.stage.getTileDura(i,j-1,logic.getStageProgression())>=.5){
      cellDetails[i][j-1].g = newCost;
      cellDetails[i][j-1].h = std::abs((i)-(x))+std::abs((j-1)-(y));
      cellDetails[i][j-1].f = cellDetails[i][j-1].h + cellDetails[i][j-1].g;
      cellDetails[i][j-1].pi = i;
      cellDetails[i][j-1].pj = j;
      openList.push(cellDetails[i][j-1]);
    }
    newCost = cellDetails[i][j].g+std::abs((i-1)-(w_pos.x/20))+std::abs((j)-(w_pos.y/20));
    if(!closedList[i-1][j] && newCost<cellDetails[i-1][j].g && logic.stage.getTileDura(i-1,j,logic.getStageProgression())>=.5){
      cellDetails[i-1][j].g = newCost;
      cellDetails[i-1][j].h = std::abs((i-1)-(x))+std::abs((j)-(y));
      cellDetails[i-1][j].f = cellDetails[i-1][j].h + cellDetails[i-1][j].g;
      cellDetails[i-1][j].pi = i;
      cellDetails[i-1][j].pj = j;
      openList.push(cellDetails[i-1][j]);
    }
    newCost = cellDetails[i][j].g+std::abs((i-1)-(w_pos.x/20))+std::abs((j+1)-(w_pos.y/20));
    if(!closedList[i-1][j+1] && newCost<cellDetails[i-1][j+1].g && logic.stage.getTileDura(i-1,j+1,logic.getStageProgression())>=.5){
      cellDetails[i-1][j+1].g = newCost;
      cellDetails[i-1][j+1].h = std::abs((i-1)-(x))+std::abs((j+1)-(y));
      cellDetails[i-1][j+1].f = cellDetails[i-1][j+1].h + cellDetails[i-1][j+1].g;
      cellDetails[i-1][j+1].pi = i;
      cellDetails[i-1][j+1].pj = j;
      openList.push(cellDetails[i-1][j+1]);
    }
    newCost = cellDetails[i][j].g+std::abs((i)-(w_pos.x/20))+std::abs((j+1)-(w_pos.y/20));
    if(!closedList[i][j+1] && newCost<cellDetails[i][j+1].g && logic.stage.getTileDura(i,j+1,logic.getStageProgression())>=.5){
      cellDetails[i][j+1].g = newCost;
      cellDetails[i][j+1].h = std::abs((i)-(x))+std::abs((j+1)-(y));
      cellDetails[i][j+1].f = cellDetails[i][j+1].h + cellDetails[i][j+1].g;
      cellDetails[i][j+1].pi = i;
      cellDetails[i][j+1].pj = j;
      openList.push(cellDetails[i][j+1]);
    }
    newCost = cellDetails[i][j].g+std::abs((i+1)-(w_pos.x/20))+std::abs((j+1)-(w_pos.y/20));
    if(!closedList[i+1][j+1] && newCost<cellDetails[i+1][j+1].g && logic.stage.getTileDura(i+1,j+1,logic.getStageProgression())>=.5){
      cellDetails[i+1][j+1].g = newCost;
      cellDetails[i+1][j+1].h = std::abs((i+1)-(x))+std::abs((j+1)-(y));
      cellDetails[i+1][j+1].f = cellDetails[i+1][j+1].h + cellDetails[i+1][j+1].g;
      cellDetails[i+1][j+1].pi = i;
      cellDetails[i+1][j+1].pj = j;
      openList.push(cellDetails[i+1][j+1]);
    }
    newCost = cellDetails[i][j].g+std::abs((i+1)-(w_pos.x/20))+std::abs((j-1)-(w_pos.y/20));
    if(!closedList[i+1][j-1] && newCost<cellDetails[i+1][j-1].g && logic.stage.getTileDura(i+1,j-1,logic.getStageProgression())>=.5){
      cellDetails[i+1][j-1].g = newCost;
      cellDetails[i+1][j-1].h = std::abs((i+1)-(x))+std::abs((j-1)-(y));
      cellDetails[i+1][j-1].f = cellDetails[i+1][j-1].h + cellDetails[i+1][j-1].g;
      cellDetails[i+1][j-1].pi = i;
      cellDetails[i+1][j-1].pj = j;
      openList.push(cellDetails[i+1][j-1]);
    }
    newCost = cellDetails[i][j].g+std::abs((i)-(w_pos.x/20))+std::abs((j+1)-(w_pos.y/20));
    if(!closedList[i][j+1] && newCost<cellDetails[i][j+1].g && logic.stage.getTileDura(i,j+1,logic.getStageProgression())>=.5){
      cellDetails[i][j+1].g = newCost;
      cellDetails[i][j+1].h = std::abs((i)-(x))+std::abs((j+1)-(y));
      cellDetails[i][j+1].f = cellDetails[i][j+1].h + cellDetails[i][j+1].g;
      cellDetails[i][j+1].pi = i;
      cellDetails[i][j+1].pj = j;
      openList.push(cellDetails[i][j+1]);
    }


  }
}

void BotController::changeState(int x){
  state = x;
  std::cout<<state<<"\n";
};
