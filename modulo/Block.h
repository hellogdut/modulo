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
struct Block
{
    int w;
    int h;
    int x;
    int y;
    int id;
    bool _lock;
    Block():x(0),y(0),_lock(false){};
    vector<vector<int>> block;
    void init(vector<vector<int>> data,int _id)
    {
        block = data;
        w = block[0].size();
        h = block.size();
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
        for(int i = 0;i < block.size();++i)
        {
            cout << "[";
            for(int j = 0;j < block[0].size();++j)
            {
                cout << block[i][j];
                if(j != block.size() - 1)
                {
                    cout << ",";
                }
            }
            cout << "]" <<endl;
        }
    }
};


#endif /* Block_h */
