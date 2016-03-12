﻿//
//  Block.h
//  modulo
//
//  Created by mario on 16/2/24.
//  Copyright © 2016年 mario. All rights reserved.
//

#ifndef Block_h
#define Block_h
#include <iostream>
using namespace std;;
#define MAX_BLOCK_NUMS 40
struct Block
{
    int w;
    int h;
    int x;
    int y;
    int id;
    bool _lock;
    int val;    // 1的个数
    Block():x(0),y(0),_lock(false){};
    //vector<vector<int>> block;
    int block[MAX_BLOCK_NUMS][MAX_BLOCK_NUMS];
    void init(vector<vector<int>> data,int _id)
    {
        //block = data;
        w = data[0].size();
        h = data.size();
        val = 0;
        for(int i = 0;i < data.size();++i)
        {
            for(int j = 0;j <data[0].size();++j)
            {
                block[i][j] = data[i][j];
                if(block[i][j])
                {
                    val++;
                }
            }
        }
//        w = block[0].size();
//        h = block.size();
        id = _id;
    }
    bool isLock() const
    {
        return _lock;
    }
    void lock()
    {
        _lock = true;
    }
    void unLock()
    {
        _lock = false;
    }
    void moveBy(int _x,int _y)
    {
        x += _x;
        y += _y;
    }
    void moveTo(int _x,int _y)
    {
        x = _x;
        y = _y;
    }
    bool canMoveTo(int _x,int _y,int roomW,int roomH)
    {
        
        return (_x + w <= roomW && _y + h <= roomH);
        
    }
    void print()
    {
        cout << "block id = " << id << endl;
        for(int i = 0;i < h;++i)
        {
            cout << "[";
            for(int j = 0;j < w;++j)
            {
                cout << block[i][j];
                if(j != h - 1)
                {
                    cout << ",";
                }
            }
            cout << "]" <<endl;
        }
    }
};


#endif /* Block_h */
