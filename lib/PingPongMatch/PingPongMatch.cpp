#include "PingPongMatch.h"
#include "Arduino.h"
#include "PlayerSide.h"

PingPongMatch ::PingPongMatch(uint8_t pointsToWin, uint8_t servingsForEachPlayer, uint8_t differenceToWin) : _playerLeft(PlayerSide::Left), _playerRight(PlayerSide::Right)
{
    _pointsToWin = pointsToWin;
    _servingsForEachPlayer = servingsForEachPlayer;
    _differenceToWin = differenceToWin;
}

void PingPongMatch ::Start()
{
    Reset();
    _lastPointSide = PlayerSide::Undefined;
    _playerServing = &_playerLeft;
    _playerLeft.InitServings(_servingsForEachPlayer);
}

void PingPongMatch ::Reset()
{
    _playerLeft.Reset();
    _playerRight.Reset();
}

void PingPongMatch ::AddPointTo(PlayerSide side)
{
    _lastPointSide = side;
    if (side == PlayerSide::Left)
    {
        _playerLeft.AddPoint();
    }
    else
    {
        _playerRight.AddPoint();
    }

    UpdateServings();

    if (GetServingNumber() <= 0)
    {
        ChangeServingSide();
    }
}

bool PingPongMatch ::IsWinner(PlayerSide side)
{
    return side == PlayerSide::Left ? 
        _playerLeft.GetPoints() >= _pointsToWin && _playerLeft.GetPoints() >= _playerRight.GetPoints() + _differenceToWin : 
        _playerRight.GetPoints() >= _pointsToWin && _playerRight.GetPoints() >= _playerLeft.GetPoints() + _differenceToWin;
}

uint8_t PingPongMatch ::GetPoints(PlayerSide side)
{
    return side == PlayerSide::Left ? _playerLeft.GetPoints() : _playerRight.GetPoints();
}

void PingPongMatch ::ChangeServingSide()
{
    PlayerSide servingSide = _playerServing->GetSide();
    if(servingSide == PlayerSide::Left){
        _playerServing = &_playerRight; 
    }else{
        _playerServing = &_playerLeft;
    }

    _playerServing->InitServings(_servingsForEachPlayer);
}

PlayerSide PingPongMatch ::GetServingSide()
{
    return _playerServing->GetSide();
}

uint8_t PingPongMatch ::GetServingNumber()
{
    return _playerServing->GetServingNumber();
}

void PingPongMatch ::UpdateServings()
{
    _playerServing->UpdateServings();
}

void PingPongMatch ::UndoLastPoint()
{
    if (_lastPointSide == PlayerSide::Undefined || _playerLeft.GetPoints() + _playerRight.GetPoints() == 0)
    {
        return;
    }

    UndoServings();

    if (GetServingNumber() > _servingsForEachPlayer)
    {
        ChangeServingSide();
    }

    if (_lastPointSide == PlayerSide::Left)
    {
        _playerLeft.SubstractPoint();
    }else{
        _playerRight.SubstractPoint();    
    }
    //this is done to "disable" the undo. I'm not storing any history (other than the last point side)
    _lastPointSide = PlayerSide::Undefined;
}

void PingPongMatch ::UndoServings()
{
    _playerServing->UndoServings();
}

void PingPongMatch ::ChangeSides(){
    ChangeServingSidesOnly();
    uint8_t tempLeftPoints = _playerLeft.GetPoints();
    _playerLeft.SetPoints(_playerRight.GetPoints());
    _playerRight.SetPoints(tempLeftPoints);
    
}

void PingPongMatch ::ChangeServingSidesOnly(){
    _playerServing = _playerServing->GetSide() == PlayerSide::Left ?  &_playerRight : &_playerLeft;
 
    uint8_t tempLeftServings = _playerLeft.GetServingNumber();
    _playerLeft.SetServingNumber(_playerRight.GetServingNumber(), _servingsForEachPlayer);
    _playerRight.SetServingNumber(tempLeftServings, _servingsForEachPlayer);
}
