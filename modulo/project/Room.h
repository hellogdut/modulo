//
//  Room.h
//  modulo
//
//  Created by mario on 16/2/24.
//  Copyright © 2016年 mario. All rights reserved.
//
#ifndef Room_h
#define Room_h
#include "Block.h"

struct Room
{
    void init(vector<vector<int>> data,int modu)
    {
        room = data;
        w = room[0].size();
        h = room.size();
        mod = modu;
    }
    bool isFit(const Block& block) const
    {
        if(w >= block.w && h >= block.h)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    bool isZeroAt(int x,int y) const
    {
        return (room[y][x] % mod) == 0;
    }
    bool canRight(const Block& block) const
    {
        return block.x + block.w + 1 <= w;
    }
    bool canDown(const Block& block) const
    {
        return block.y + block.h + 1 <= h;
    }
    bool canRightWithLimitArea(const Block& block,pair<int,int> area) const
    {
        bool ret = block.x + block.w + 1 <= w;
        ret &= (block.y < area.second ? block.x <= w : block.x <= area.first);
        return ret;
    }
    bool canDownWithLimitArea(const Block& block,pair<int,int> area) const
    {
        bool ret = block.y + block.h + 1 <= h;
        ret &= (block.y < area.second ? block.x <= w : block.x <= area.first);
        
        return ret;
    }
    void add(const Block& block)
    {
        for(int i = 0; i < block.w; ++i)
        {
            for(int j = 0; j < block.h;++j)
            {
                room[j + block.y][i + block.x] += block.block[j][i];
            }
        }
    }
    void remove(const Block& block)
    {
        for(int i = 0; i < block.w; ++i)
        {
            for(int j = 0; j < block.h;++j)
            {
                room[j + block.y][i + block.x] -= block.block[j][i];
            }
        }
    }
    
    bool isZero() const
    {
        bool ret = true;
        for(int j = 0;j < h;++j)
        {
            if(ret == false)
                break;
            for(int i = 0;i < w;++i)
            {
                if((room[j][i] % mod) != 0)
                {
                    ret = false;
                    break;
                }
            }
        }
        return ret;
    };
    bool isZeroWithLimitArea(pair<int,int> area)
    {
        bool ret = true;
        if(area.second == 0)
        {
            for(int i = 0;i < area.first;++i)
            {
                if((room[0][i] % mod) != 0)
                {
                    ret =false;
                    break;
                }
            }
        }
        else
        {
            for(int i = 0;i <= area.second && ret;++i)
            {
                int len = (i == area.second ? area.first : w);
                for(int j = 0;j < len;++j)
                {
                    if((room[i][j] % mod) != 0)
                    {
                        ret =false;
                        break;
                    }
                }
            }
        }
        return ret;
    }
    void print()
    {
        cout << "room" << endl;
        for(int i = 0;i < room.size();++i)
        {
            cout << "[";
            for(int j = 0;j < room[0].size();++j)
            {
                cout << room[i][j];
                if(j != room.size())
                {
                    cout << ",";
                }
            }
            cout << "]" <<endl;
        }
    }
    bool getNextPos(int oldX,int oldY,int& newX,int& newY) const
    {
        if(oldX < this->w - 1)
        {
            newX = oldX + 1;
            newY = oldY;
        }
        else
        {
            newX = 0;
            newY = oldY + 1;
        }
        return newY < h;
    }
    
    int w;
    int h;
    int mod;
    vector<vector<int>> room;
};


#endif /* Room_h */
