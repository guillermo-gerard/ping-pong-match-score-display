#ifndef _PingPongPlayer_H
#define _PingPongPlayer_H

#include "Arduino.h"
#include "PlayerSide.h"

class PingPongPlayer{
    private:
        uint8_t _points;
        uint8_t _servingNumber;

    public: 
        PingPongPlayer(PlayerSide side);
        
        void AddPoint();
        void SubstractPoint();
        void Reset();
        PlayerSide GetSide();
        uint8_t GetPoints();
        void SetPoints(uint8_t);
        uint8_t GetServingNumber();
        void SetServingNumber(uint8_t, uint8_t);
        void InitServings(uint8_t);
        void UpdateServings();
        void UndoServings();
    private:
        PlayerSide _side;
};

#endif