#include "PingPongPlayer.h"
#include "PlayerSide.h"

//PingPongPlayer :: PingPongPlayer(){}

PingPongPlayer :: PingPongPlayer(PlayerSide side){
    _side = side;
}

void PingPongPlayer :: AddPoint(bool isServing){
    points ++;
//    if(isServing) servingNumber --;
}

void PingPongPlayer :: SubstractPoint(bool isServing){
    points--;
//    if(isServing) servingNumber ++;
}

void PingPongPlayer :: UpdateServings(){
    servingNumber --;
}

void PingPongPlayer :: Reset(){
    points = 0;
    servingNumber = 0;
}

uint8_t PingPongPlayer :: GetPoints(){
    return points;
}

uint8_t PingPongPlayer :: GetServingNumber(){
    return servingNumber;
}

void PingPongPlayer :: InitServings(uint8_t servings){
    servingNumber = servings;
}



