//
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
#define BLOCK_SIZE 10
struct Block
{
    int w;
    int h;
    int x;
    int y;
    int id;
    bool _lock;
    Block():x(0),y(0),_lock(false){};
    //vector<vector<int>> block;
    int block[BLOCK_SIZE][BLOCK_SIZE];
    void init(vector<vector<int>> data,int _id)
    {
        //block = data;
        w = data[0].size();
        h = data.size();
        for(int i = 0;i < data.size();++i)
        {
            for(int j = 0;j <data[0].size();++j)
            {
                block[i][j] = data[i][j];
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
