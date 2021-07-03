#ifndef _PingPongMatch_H
#define _PingPongMatch_H

#include "PingPongPlayer.h"
#include "Arduino.h"

class PingPongMatch
{

private:
    PingPongPlayer _playerLeft;
    PingPongPlayer _playerRight;
    uint8_t _pointsToWin;
    uint8_t _servingsForEachPlayer;
    uint8_t _differenceToWin;
    PlayerSide _servingSide;
    PlayerSide _lastPointSide;
    void UpdateServings();
    void ChangeServingSide();
    void Reset();
    void UndoServings();

public:
    PingPongMatch(
        uint8_t pointsToWin,
        uint8_t servingsForEachPlayer,
        uint8_t differenceToWin);

    void Start();
    void AddPointTo(PlayerSide side);
    bool IsWinner(PlayerSide side);
    uint8_t GetPoints(PlayerSide side);
    PlayerSide GetServingSide();
    uint8_t GetServingNumber();
    void UndoLastPoint();
    void ChangeSides();
    void ChangeServingSidesOnly();
};

#endif