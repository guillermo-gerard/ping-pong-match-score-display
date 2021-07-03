#include "PingPongPlayer.h"
#include "PlayerSide.h"

PingPongPlayer ::PingPongPlayer(PlayerSide side)
{
    _side = side;
}

PlayerSide PingPongPlayer :: GetSide(){
    return _side;
}

void PingPongPlayer ::AddPoint()
{
    _points++;
}

void PingPongPlayer ::SubstractPoint()
{
    _points--;
}

void PingPongPlayer ::UpdateServings()
{
    _servingNumber--;
}

void PingPongPlayer ::UndoServings()
{
    _servingNumber++;
}

void PingPongPlayer ::Reset()
{
    _points = 0;
    _servingNumber = 0;
}

uint8_t PingPongPlayer ::GetPoints()
{
    return _points;
}

uint8_t PingPongPlayer ::GetServingNumber()
{
    return _servingNumber;
}

void PingPongPlayer ::InitServings(uint8_t servings)
{
    _servingNumber = servings;
}

void PingPongPlayer ::SetServingNumber(uint8_t number, uint8_t servingsPerPlayer){
    if(number < 0 || number > servingsPerPlayer){
        return;
    }

    _servingNumber = number;
}

void PingPongPlayer ::SetPoints(uint8_t points){
    if(points < 0){
        return;
    }
    _points = points;
}
