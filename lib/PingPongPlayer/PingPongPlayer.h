#ifndef _PingPongPlayer_H
#define _PingPongPlayer_H

#include "Arduino.h"
#include "PlayerSide.h"

class PingPongPlayer{
    private:
        uint8_t points;
        uint8_t servingNumber;

    public: 
    //PingPongPlayer();
        PingPongPlayer(PlayerSide side);
        
        void AddPoint(bool);
        void SubstractPoint(bool);
        void Reset();
        uint8_t GetPoints();
        uint8_t GetServingNumber();
        void InitServings(uint8_t);
        void UpdateServings();

    private:
        PlayerSide _side;
};

#endif