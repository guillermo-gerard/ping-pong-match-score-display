#include "PingPongPlayer.h"
#include "PlayerSide.h"

//PingPongPlayer :: PingPongPlayer(){}

PingPongPlayer :: PingPongPlayer(PlayerSide side){
    _side = side;
}

void PingPongPlayer :: AddPoint(){
    points ++;
}

void PingPongPlayer :: SubstractPoint(){
    points--;
}

void PingPongPlayer :: UpdateServings(){
    servingNumber --;
}

void PingPongPlayer :: UndoServings(){
    servingNumber ++;
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



