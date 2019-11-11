#include <iostream>
#include <cmath>
#include "GameLogic.h"
#include "Definitions.h"

GameLogic::GameLogic() {
    state = mainMenu;
    walrus1 = Player();
    walrus2 = Player();
    progression = 0;
    stage = Stage();
    stage.generateMap();
    accumulator = 0;
    bump = 0;
    splash = 0;
    fish_num = 0;
    fish_accumulator = 0.0f;
}

void GameLogic::update(float dSec) {

    if (state == playing) {

        //fish powerup generation
        //need to fine tune these numbers, not sure where we want the fish to be generated
        //rand_create is just a simple way to randomize when fish are created
        fish_accumulator += dSec;
        if (fish_num < 3 && fish_accumulator > 3.0) {
            fish_accumulator = 0;
            //sf::Vector2f stage_bounds = stage.getFishBounds(progression);
            //std::cout<<stage_bounds.y;
            int rand_x = rand() % 700 + 100;
            int rand_y = rand() % 500 + 100;
            int rand_color = rand() % 10;
            //make sure not on water
            float tile_dur = stage.getTileDura(rand_x / 20, rand_y / 20, progression);
            while (tile_dur <= 0) {
                rand_x = rand() % 700 + 100;
                rand_y = rand() % 500 + 100;
                tile_dur = stage.getTileDura(rand_x / 20, rand_y / 20, progression);
            }
            fish_list.push_back(std::unique_ptr<Fish>(new Fish()));
            fish_list.back()->setPosition(sf::Vector2f(rand_x, rand_y));
            if (rand_color < 5)
                fish_list.back()->setColor(0);
            else
                fish_list.back()->setColor(1);
            std::cout<<fish_list.back()->getPosition().x<<"\n";
            //curr_fish_pos = fish_list.back()->getPosition();
            fish_num++;
            std::cout<<fish_num;
        }

        // process movement
        if (!walrus1.isDead()) {
            walrus1.tickUpdate(dSec);
            walrus1.applyPassiveForce(dSec);
        }
        if (!walrus2.isDead()) {
            walrus2.tickUpdate(dSec);
            walrus2.applyPassiveForce(dSec);
        }

        // melt stage
        accumulator += dSec;
        if(accumulator >= 1){
          stage.tickMelt(progression);
          accumulator -= 1;
        }

        // *check collisions* //
        sf::Vector2f w1_pos = walrus1.getPos();
        sf::Vector2f w2_pos = walrus2.getPos();

        // player1 - water collision
        if (stage.getTileDura((w1_pos.x)/20, (w1_pos.y)/20, progression) <= 0) {
            if (!walrus1.isDead()) {
                handlePlayerDeath(1);
            }
        }
        // player2 - water collision
        if (stage.getTileDura((w2_pos.x)/20, (w2_pos.y)/20, progression) <= 0) {
            if (!walrus2.isDead()) {
                handlePlayerDeath(2);
            }
        }

        // player - boundary collision
        if (w1_pos.x >= 800 || w1_pos.x <= 0) {
            handleBoundaryCollision(1, w1_pos.x);
        } else if (w2_pos.x >= 800 || w2_pos.x <= 0) {
            handleBoundaryCollision(2, w2_pos.x);
        }

        // player1 - player2 collision
        sf::Vector2f posDiff = w1_pos - w2_pos;
        float dist = sqrt((posDiff.x * posDiff.x) + (posDiff.y * posDiff.y));
        if (dist < 6.5*(walrus1.getMass() + walrus2.getMass()) && !(walrus1.isDead() || walrus2.isDead())) {
            handlePlayerCollision();
        }

        // fish collisions
        //have list of no more than 3 fish
        //check for collision of each fish
        sf::Vector2f fish_pos [3];
        int fish_col [3];
        int idx = 0;
        std::list<std::unique_ptr<Fish>>::iterator it;
        for (it = fish_list.begin(); it != fish_list.end() && idx <= fish_list.size(); it++) {
            fish_pos[idx] = (*it)->getPosition();
            fish_col[idx] = (*it)->getColor();
            idx++;
        }

        for (idx = 0; idx < fish_list.size(); idx++) {
            // fish - player1 collision
            posDiff = w1_pos - fish_pos[idx];
            dist = sqrt((posDiff.x * posDiff.x) + (posDiff.y * posDiff.y));
            if (dist < 6.5*walrus1.getMass() + 15 && !walrus1.isDead()) {
                handleFishCollision(1, idx, fish_col[idx]);
                break;
            }
            // fish - player 2 collision
            posDiff = w2_pos - fish_pos[idx];
            dist = sqrt((posDiff.x * posDiff.x) + (posDiff.y * posDiff.y));
            if (dist < 6.5*walrus2.getMass() + 15 && !walrus2.isDead()) {
                handleFishCollision(2, idx, fish_col[idx]);
                break;
            }
            // fish - water collision
            if (stage.getTileDura(fish_pos[idx].x/20, fish_pos[idx].y/20, progression) <= 0) {
                handleFishCollision(0, idx, fish_col[idx]);
                break;
            }

        }



    }


}

void GameLogic::handleBoundaryCollision(int walrus, float xpos) {

    if (walrus == 1 && xpos <= 0) {
        if (walrus2.isDead()) {
            progression--;
            walrus1.spawn(sf::Vector2f(750.0f, 300.0f));
            walrus2.spawn(sf::Vector2f(400.0f, 300.0f));
        } else {
            sf::Vector2f newVel = walrus1.getVel();
            newVel.x *= -1;
            walrus1.setVel(newVel);
            walrus1.tickUpdate(0.04);
        }
    }

    else if (walrus == 2 && xpos >= 800) {
        if (walrus1.isDead()) {
            progression++;
            walrus1.spawn(sf::Vector2f(400.0f, 300.0f));
            walrus2.spawn(sf::Vector2f(0.0f, 300.0f));
        } else {
            sf::Vector2f newVel = walrus2.getVel();
            newVel.x *= -1;
            walrus2.setVel(newVel);
            walrus2.tickUpdate(0.04);
        }
    }

    else if (walrus == 1 && xpos >= 800) {
        sf::Vector2f newVel = walrus1.getVel();
        newVel.x *= -1;
        walrus1.setVel(newVel);
        walrus1.tickUpdate(0.04);
    }

    else if (walrus == 2 && xpos <= 0) {
        sf::Vector2f newVel = walrus2.getVel();
        newVel.x *= -1;
        walrus2.setVel(newVel);
        walrus2.tickUpdate(0.04);
    }

}

void GameLogic::handleFishCollision(int player, int fish_idx, int fish_color) {

    if (player == 1) {
        walrus1.handlePowerUp(fish_color);
    } else if (player == 2) {
        walrus2.handlePowerUp(fish_color);
    }

    //delete correct fish from list
    if (fish_idx == 0) {
        fish_list.pop_front();
    }
    else if (fish_idx == 2) {
        fish_list.pop_back();
    }
    //deleting middle is harder
    else {
        //pop last element and store it.
        sf::Vector2f stored_pos = fish_list.back()->getPosition();
        unsigned int stored_color = fish_list.back()->getColor();
        fish_list.pop_back();
        //pop new last (middle) element
        fish_list.pop_back();
        //push the stored element to back of list
        fish_list.push_back(std::unique_ptr<Fish>(new Fish()));
        fish_list.back()->setPosition(stored_pos);
        fish_list.back()->setColor(stored_color);
    }
    fish_num--;

}

void GameLogic::handlePlayerCollision() {

  float knockback = 0.04; // tunable

  //find the velocity of collision along the line of collision
  sf::Vector2f w1_vel = walrus1.getVel();
  sf::Vector2f w2_vel = walrus2.getVel();
  sf::Vector2f w1_pos = walrus1.getPos();
  sf::Vector2f w2_pos = walrus2.getPos();
  float w1_mass = walrus1.getMass();
  float w2_mass = walrus2.getMass();

  // calculate point of collision for potentially adding a collision animation later
  playerCollisionPoint = sf::Vector2f(((w1_pos.x * w2_mass*20) + (w2_pos.x * w1_mass*20)) / (w1_mass*20 + w2_mass*20), ((w1_pos.y * w2_mass*20) + (w2_pos.y * w1_mass*20)) / (w1_mass*20 + w2_mass*20));

  // Elastic Collision handling algorithm implemented from:
  // http://cobweb.cs.uga.edu/~maria/classes/4070-Spring-2017/Adam%20Brookes%20Elastic%20collision%20Code.pdf

  // calculate the difference in positions and normalize into a unit vector
  sf::Vector2f posDiff = w1_pos - w2_pos;
  float length = sqrt((posDiff.x * posDiff.x) + (posDiff.y * posDiff.y));
  sf::Vector2f normal = sf::Vector2f(posDiff.x / length, posDiff.y / length);
  // calculate the tangent unit vector
  sf::Vector2f tangent = sf::Vector2f(normal.y*-1, normal.x);
  // dot product of normal and velocities
  float walrus1ScalarNorm = (normal.x*w1_vel.x)+(normal.y*w1_vel.y);
  float walrus2ScalarNorm = (normal.x*w2_vel.x)+(normal.y*w2_vel.y);
  // dot product of tangent and velocities
  float walrus1ScalarTan = (tangent.x*w1_vel.x)+(tangent.y*w1_vel.y);
  float walrus2ScalarTan = (tangent.x*w2_vel.x)+(tangent.y*w2_vel.y);
  // calculate elastic collision
  float walrus1NewScalarNorm = (walrus1ScalarNorm * (w1_mass - w2_mass) + 2 * w2_mass * walrus2ScalarNorm) / (w1_mass + w2_mass);
  float walrus2NewScalarNorm = (walrus2ScalarNorm * (w2_mass - w1_mass) + 2 * w1_mass * walrus1ScalarNorm) / (w1_mass + w2_mass);

  sf::Vector2f walrus1NewVecNorm = normal * walrus1NewScalarNorm;
  sf::Vector2f walrus2NewVecNorm = normal * walrus2NewScalarNorm;
  sf::Vector2f walrus1NewVecTan = tangent * walrus1ScalarTan;
  sf::Vector2f walrus2NewVecTan = tangent * walrus2ScalarTan;

  walrus1.setVel(walrus1NewVecTan + walrus1NewVecNorm);
  walrus2.setVel(walrus2NewVecTan + walrus2NewVecNorm);
  // avoid walrus sticking together occasionally
  walrus1.tickUpdate(knockback);
  walrus2.tickUpdate(knockback);

  // power of collision
  sf::Vector2f velDiff = walrus1.getVel() - walrus2.getVel();
  float magnitude = sqrt((velDiff.x * velDiff.x) + (velDiff.y * velDiff.y));
  bump = (int) (magnitude*0.2);
  std::cout<<bump<<"\n";
}

void GameLogic::handlePlayerAttack(int playerNum, sf::Vector2f dir) {
    std::cout << "playerNum" << playerNum << std::endl;
    sf::Vector2f w1_pos = walrus1.getPos();
    sf::Vector2f w2_pos = walrus2.getPos();
    float w1_mass = walrus2.getMass();
    float w1_radius = (w1_mass*20) + 1; //+1 to avoid a regular collision
    float w2_mass = walrus2.getMass();
    float w2_radius = (w2_mass*20) + 1; //+1 to avoid a regular collision

    if (playerNum == 2){

        //determine direction of attack, direction conversion copied from animation
        attackCollisionPoint = w2_pos;
        int acpAdjustment = w2_radius / 2; //range for slash attack
        sf::Vector2f attackKnockBackDir;
        float slashAttackPower = SLASH_ATTACK_POWER;

        int hash = dir.x * 17 + dir.y * 7;
        std::cout << "hash:"<< hash << "\n" << std::endl;
        switch (hash) {
            case 17 + 7: //right down
                //adjust attack hitbox (hit coordinate) based on direction
                attackCollisionPoint.x += acpAdjustment;
                attackCollisionPoint.y += acpAdjustment;
                //variable used to determine knockback direction if hit
                attackKnockBackDir = (sf::Vector2f(slashAttackPower, slashAttackPower));
                break;
            case 17 - 7: //right up
                attackCollisionPoint.x += acpAdjustment;
                attackCollisionPoint.y -= acpAdjustment;
                attackKnockBackDir = (sf::Vector2f(slashAttackPower, -slashAttackPower));
                break;
            case -17 + 7: //left down
                attackCollisionPoint.x -= acpAdjustment;
                attackCollisionPoint.y += acpAdjustment;
                attackKnockBackDir = (sf::Vector2f(-slashAttackPower, slashAttackPower));
                break;
            case -17 - 7: //left up
                attackCollisionPoint.x -= acpAdjustment;
                attackCollisionPoint.y -= acpAdjustment;
                attackKnockBackDir = (sf::Vector2f(-slashAttackPower, -slashAttackPower));
                break;
            case 17: //right
                attackCollisionPoint.x += acpAdjustment;
                attackKnockBackDir = (sf::Vector2f(slashAttackPower, 0));
                break;
            case 7: //down
                attackCollisionPoint.y += acpAdjustment;
                attackKnockBackDir = (sf::Vector2f(0, slashAttackPower));
                break;
            case -17: //left
                attackCollisionPoint.x -= acpAdjustment;
                attackKnockBackDir = (sf::Vector2f(-slashAttackPower, 0));
                break;
            case -7: //up
                attackCollisionPoint.y -= acpAdjustment;
                attackKnockBackDir = (sf::Vector2f(0, -slashAttackPower));
                break;
            case 0:
                break;
        }
        //if collision point inside other walrus hitbox, apply collision force
        std::cout << "x:" <<attackCollisionPoint.x <<"\n" << "y:" <<attackCollisionPoint.y <<"\n"<< std::endl;
        //follow circle formula to determine if point is inside other walrus hitbox
        if ((attackCollisionPoint.x - w1_pos.x) * (attackCollisionPoint.x - w1_pos.x) +
            (attackCollisionPoint.y - w1_pos.y) * (attackCollisionPoint.y - w1_pos.y) <= w1_radius * w1_radius) {
            //knock walrus
            std::cout << "SLASHED" <<"\n"<< std::endl;
            walrus1.setVel(attackKnockBackDir);
        }
        else{
            std::cout << "MISSED SLASH" <<"\n"<< std::endl;
         }
    }
    if (playerNum == 1) {

        //determine direction of attack, direction conversion copied from animation
        attackCollisionPoint = w1_pos;
        int acpAdjustment = w1_radius / 2; //range for slash attack
        sf::Vector2f attackKnockBackDir;
        float slashAttackPower = SLASH_ATTACK_POWER;

        int hash = dir.x * 17 + dir.y * 7;
        std::cout << "hash:" << hash << "\n" << std::endl;
        switch (hash) {
            case 17 + 7: //right down
                //adjust attack hitbox (hit coordinate) based on direction
                attackCollisionPoint.x += acpAdjustment;
                attackCollisionPoint.y += acpAdjustment;
                //variable used to determine knockback direction if hit
                attackKnockBackDir = (sf::Vector2f(slashAttackPower, slashAttackPower));
                break;
            case 17 - 7: //right up
                attackCollisionPoint.x += acpAdjustment;
                attackCollisionPoint.y -= acpAdjustment;
                attackKnockBackDir = (sf::Vector2f(slashAttackPower, -slashAttackPower));
                break;
            case -17 + 7: //left down
                attackCollisionPoint.x -= acpAdjustment;
                attackCollisionPoint.y += acpAdjustment;
                attackKnockBackDir = (sf::Vector2f(-slashAttackPower, slashAttackPower));
                break;
            case -17 - 7: //left up
                attackCollisionPoint.x -= acpAdjustment;
                attackCollisionPoint.y -= acpAdjustment;
                attackKnockBackDir = (sf::Vector2f(-slashAttackPower, -slashAttackPower));
                break;
            case 17: //right
                attackCollisionPoint.x += acpAdjustment;
                attackKnockBackDir = (sf::Vector2f(slashAttackPower, 0));
                break;
            case 7: //down
                attackCollisionPoint.y += acpAdjustment;
                attackKnockBackDir = (sf::Vector2f(0, slashAttackPower));
                break;
            case -17: //left
                attackCollisionPoint.x -= acpAdjustment;
                attackKnockBackDir = (sf::Vector2f(-slashAttackPower, 0));
                break;
            case -7: //up
                attackCollisionPoint.y -= acpAdjustment;
                attackKnockBackDir = (sf::Vector2f(0, -slashAttackPower));
                break;
            case 0:
                break;
        }
        //if collision point inside other walrus hitbox, apply collision force
        std::cout << "x:" << attackCollisionPoint.x << "\n" << "y:" << attackCollisionPoint.y << "\n" << std::endl;
        //follow circle formula to determine if point is inside other walrus hitbox
        if ((attackCollisionPoint.x - w2_pos.x) * (attackCollisionPoint.x - w2_pos.x) +
            (attackCollisionPoint.y - w2_pos.y) * (attackCollisionPoint.y - w2_pos.y) <= w2_radius * w2_radius) {
            //knock walrus
            std::cout << "SLASHED" << "\n" << std::endl;
            walrus2.setVel(attackKnockBackDir);
        } else {
            std::cout << "MISSED SLASH" << "\n" << std::endl;
        }
    }
}

void GameLogic::returnToMenu() {
  state = mainMenu;
  stage.generateMap();
}

/*
 *1 param: walrus1 died
 * 2 param: walrus2 died
 * */
void GameLogic::handlePlayerDeath(int walrus) {

  //will have more need for separate cases later on to adjust the screen transition
	if (walrus == 1) {

	  // check for game over
      if (progression == 2) {
          winner1 = false;
          state = gameOverMenu;
          //reset progression
          progression = 0;
      }

      walrus1.kill();
      if(walrus2.isDead()){
        resetGame();
      }
      splash = 1;

	}
	else if (walrus == 2) {

	  //check for game over
      if (progression == -2) {
          winner1 = true;
          state = gameOverMenu;
          //reset progression
          progression = 0;
      }

      walrus2.kill();
      if(walrus1.isDead()){
        resetGame();
      }
      splash = 1;
	}

}

void GameLogic::togglePause() {
  if (state == pauseMenu) {
      state = playing;
  } else if (state == playing) {
      state = pauseMenu;
  }
}

void GameLogic::resetGame() {
    state = playing;
    walrus1.spawn(sf::Vector2f(500.0f, 300.0f));
    walrus2.spawn(sf::Vector2f(300.0f, 300.0f));
}

GameLogic::GameState GameLogic::getState() {
    return state;
}

int GameLogic::getStageProgression() {
    return progression;
}
sf::Vector2f GameLogic::getAttackCollisionPoint() {
    return attackCollisionPoint;
}
