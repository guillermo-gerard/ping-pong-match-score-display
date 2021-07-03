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
    _servingSide = PlayerSide::Left;
    _playerLeft.InitServings(_servingsForEachPlayer);
}

void PingPongMatch ::Reset()
{
    _playerLeft.Reset();
    _playerRight.Reset();
}

void PingPongMatch ::AddPointTo(PlayerSide side)
{
    if (side == PlayerSide::Left)
    {
        _playerLeft.AddPoint(_servingSide == PlayerSide::Left);
    }
    else
    {
        _playerRight.AddPoint(_servingSide == PlayerSide::Right);
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
    if (_servingSide == PlayerSide::Left)
    {
        _servingSide = PlayerSide::Right;
        _playerRight.InitServings(_servingsForEachPlayer);
        return;
    }
    _servingSide = PlayerSide::Left;
    _playerLeft.InitServings(_servingsForEachPlayer);
}

PlayerSide PingPongMatch ::GetServingSide()
{
    return _servingSide;
}

uint8_t PingPongMatch ::GetServingNumber()
{
    return _servingSide == PlayerSide::Left ? _playerLeft.GetServingNumber() : _playerRight.GetServingNumber();
}

void PingPongMatch ::UpdateServings()
{
    if (_servingSide == PlayerSide::Left)
    {
        _playerLeft.UpdateServings();
        return;
    }
    _playerRight.UpdateServings();
}
