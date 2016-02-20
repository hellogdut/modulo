//
//  main.cpp
//  Modulo
//
//  Created by mario on 16/2/18.
//  Copyright © 2016年 mario. All rights reserved.
//

#include <iostream>
#include <string>
#include <vector>

using namespace std;
#include "cJSON.h"

static long tryTimes = 0;
static long lastTime = 0;
struct Block
{
    int w;
    int h;
    int x;
    int y;
    Block():x(0),y(0){};
    vector<vector<int>> block;
    void init(vector<vector<int>> data)
    {
        block.swap(data);
        w = block[0].size();
        h = block.size();
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
};

struct Room
{
    void init(vector<vector<int>> data,int modu)
    {
        room = data;
        m = room[0].size();
        n = room.size();
        mod = modu;
    }
    bool isFit(const Block& block)
    {
        if(m >= block.w && n >= block.h)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    bool canRight(const Block& block)
    {
        return block.x + block.w + 1 <= m;
    }
    bool canDown(const Block& block)
    {
        return block.y + block.h + 1 <= n;
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
    
    bool isZero()
    {
        bool ret = true;
        for(int i = 0;i < m;++i)
        {
            for(int j = 0;j < n;++j)
            {
                if((room[j][i] % mod) != 0)
                {
                    ret = false;
                    break;
                }
            }
        }
        tryTimes++;
        return ret;
    };
    
    int m;
    int n;
    int mod;
    vector<vector<int>> room;
};

bool hasNextPos(Room& room,Block& block,int& posX,int& posY)
{
    if(room.canRight(block))
    {
        posX = block.x + 1;
        posY = block.y;
        return true;
    }
    
    if(room.canDown(block))
    {
        posX = 0;
        posY = block.y + 1;
        return true;
    }
    return false;
    
}
void processInput(string str,int& level,int& modu,Room& room,vector<Block>& blockList)
{
    
    const char* my_json_string = str.c_str();
    cJSON *root = cJSON_Parse(my_json_string);
    
    level = cJSON_GetObjectItem(root,"level")->valueint;
    modu = atoi(cJSON_GetObjectItem(root, "modu")->valuestring);
    
    
    cJSON * vecMap = cJSON_GetObjectItem(root,"map");
    int size = cJSON_GetArraySize(vecMap);
    vector<vector<int>> vecRoom;
    for(int i =0;i < size;++i)
    {
        cJSON* item = cJSON_GetArrayItem(vecMap, i);
        vector<int> row;
        string strRow = item->valuestring;
        for(int j = 0;j < strRow.length();++j)
        {
            row.push_back(strRow[j] - '0');
        }
        vecRoom.push_back(row);
    }
    room.init(vecRoom,modu);
    
    
    cJSON * vecBlock = cJSON_GetObjectItem(root,"pieces");
    size = cJSON_GetArraySize(vecBlock);
    for(int i =0;i < size;++i)
    {
        cJSON* item = cJSON_GetArrayItem(vecBlock, i);
        vector<vector<int>> data;
        vector<int> row;
        string strRow = item->valuestring;
        for(int j = 0;j < strRow.length();++j)
        {
            if(strRow[j] == '.')
            {
                row.push_back(0);
            }
            else if(strRow[j] == 'X')
            {
                row.push_back(1);
            }
            else if(strRow[j] == ',')
            {
                data.push_back(row);
                row.clear();
            }
        }
        if(!row.empty())
        {
            data.push_back(row);
        }
        Block block;
        block.init(data);
        blockList.push_back(block);
    }
}

bool calculate(Room& room,vector<Block>& blockList)
{
    string str;
    //bool ret = move(room,blockList,0);
    
    for(int i = 0;i < blockList.size();++i)
    {
        room.add(blockList[i]);
    }
    int k = blockList.size() - 1;	// 最左边的可以更改的索引
    int n = blockList.size();
    while(k >= 0)
    {
        
        if(room.isZero())
            return true;
        
        Block& block = blockList[k];
        int newX,newY;
        if(hasNextPos(room,block,newX,newY))
        {
            room.remove(block);
            block.moveTo(newX,newY);
            room.add(block);
            
            for(int j = k + 1; j < n;++j)
            {
                Block& bk = blockList[j];
                room.remove(bk);
                bk.moveTo(0,0);
                room.add(bk);
                
            }
            
            k = n - 1;
        }
        else
        {
            k--;
        }
    }
    return false;
}

int main (int argc, const char * argv[]) {
    
    int level = 0;
    int modu = 0;
    string str;
    string output;
    Room room;
    vector<Block> blockList;
    str = string("{\"level\":26,\"modu\":\"4\",\"map\":[\"032200\",\"100310\",\"232330\",\"210230\",\"232333\",\"213230\"],\"pieces\":[\"XX,.X\",\".XX,XX.\",\"..X.,..X.,.XXX,XXXX,X...\",\"XXX.,..XX,..X.\",\"..X..,..X..,.XX..,XXXXX,..XX.\",\"...X,.XXX,XX..\",\"XX..,.XXX,.XX.,.X..\",\"XXX,XXX,.XX,XX.,.X.\",\".XX,..X,XXX,XX.,.X.\",\"..XX.,.XXXX,.XX..,XX...\",\".X...,XXXXX,...XX,...XX\",\".XX,XX.,XX.,.X.,.X.\"]}");
    
    processInput(str,level,modu,room,blockList);
    
    lastTime = clock();
    tryTimes = 0;
    
    
    if(calculate(room,blockList))
    {
        for(int i = 0;i < blockList.size();++i)
        {
            output += blockList[i].y + '0';
            output += blockList[i].x + '0';
        }
        
        cout << "Result:    " << output << endl;
        cout << "tryTimes:  " << tryTimes << endl;
        cout << "Time(s):   " << (clock() - lastTime)/CLOCKS_PER_SEC << endl;
    }
    else
    {
         cout << "no answer" << endl;
    }
    
    return 0;
}
