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
    int id;
    Block():x(0),y(0){};
    vector<vector<int>> block;
    void init(vector<vector<int>> data,int _id)
    {
        block = data;
        w = block[0].size();
        h = block.size();
        id = _id;
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
    bool canRightWithLimitArea(const Block& block,pair<int,int> area)
    {
        return block.y + block.h + 1 <= n && block.x <= area.first && block.y <= area.second;
    }
    bool canDownWithLimitArea(const Block& block,pair<int,int> area)
    {
        return block.y + block.h + 1 <= n && block.x <= area.first && block.y <= area.second;
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
    bool isZeroWithLimitArea(pair<int,int> area)
    {
        bool ret = true;
        
        for(int j = 0;j <= area.second;++j)
        {
            int len = (j == area.second ? area.first : m);
            for(int i = 0;i < len;++i)
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
    }
    
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

bool hasNextPosWithArea(Room& room,Block& block,int& posX,int& posY,pair<int,int> area)
{
    if(room.canRightWithLimitArea(block, area))
    {
        posX = block.x + 1;
        posY = block.y;
        return true;
    }
    
    if(room.canRightWithLimitArea(block, area))
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
        block.init(data,i);
        blockList.push_back(block);
    }
}
bool hasUntouchableArea(const Room& room,const Block& block,pair<int,int>& area)
{
    if(block.x == 0 && block.y == 0)
        return false;
    int w = block.x;
    int h = block.y;
    return true;
}
bool canZeroWithChildBlock(Room& room,vector<Block>& blockList,int beginIndex,pair<int,int> area)
{

    int n = blockList.size();
    int k = n - 1;

    Room newRoom;
    newRoom.init(room.room, room.mod);
    
    while(k >= beginIndex)
    {
        if(newRoom.isZeroWithLimitArea(area))
            return true;
        
        Block& block = blockList[k];
        
        int newX,newY;
        if(hasNextPosWithArea(newRoom,block,newX,newY,area))
        {
            newRoom.remove(block);
            block.moveTo(newX,newY);
            newRoom.add(block);
            
            for(int j = k + 1; j < n;++j)
            {
                Block& bk = blockList[j];
                newRoom.remove(bk);
                bk.moveTo(0,0);
                newRoom.add(bk);
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
        
        for(int j = k + 1; j < n;++j)
        {
            Block& bk = blockList[j];
            room.remove(bk);
            bk.moveTo(0,0);
            room.add(bk);
            
        }
        
        /*************************************************/
        //如果是最后一个
        if(k == n - 1)
        {
            bool toContinue = false;
            
            
            for(int j = 0;j <= block.y;++j)
            {
                if(toContinue == true)
                    break;
                int w = (j == block.y ? block.x : room.m);
                for(int i = 0;i < w;++i)
                {
                    if((room.room[j][i] % room.mod) != 0)
                    {
                        toContinue = true;
                        break;
                    }
                }
            }
            if(toContinue)
            {
                k--;
                continue;
            }
        }
//        else
//        {
//            pair<int,int> area;
//            bool ret = hasUntouchableArea(room,block, area);
//            if(ret)
//            {
//                if(!canZeroWithChildBlock(room,blockList, k + 1, area))
//                {
//                    k--;
//                    continue;
//                }
//            }
//        }
        //*********************************************
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
//            if(k != n - 1)
//            {
//                pair<int,int> area;
//                bool ret = hasUntouchableArea(room,block, area);
//                if(ret)
//                {
//                    if(!canZeroWithChildBlock(room,blockList, k + 1, area))
//                    {
//                        k--;
//                        continue;
//                    }
//                }
//            }
            
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
    str = string("{\"level\":25,\"modu\":\"3\",\"map\":[\"1222\",\"0200\",\"1012\",\"2222\",\"2121\"],\"pieces\":[\".X.,.XX,XXX\",\"X.,XX,X.,X.\",\"XXX,..X\",\".X,XX,X.,XX\",\"XXXX\",\"X.,X.,XX,X.,X.\",\"X..,XXX\",\".X,XX\",\".X,XX,X.\",\"X.,X.,X.,XX,.X\",\"X..,X..,XXX\",\".X,XX,.X\"]}");
    
    processInput(str,level,modu,room,blockList);
    
    lastTime = clock();
    tryTimes = 0;
    
    
    /// 对 Block 进行排序，面积大的在前面
    
    std::sort(blockList.begin(),blockList.end(),[](const Block& a,const Block& b){
        return a.w * a.h > b.w * b.h;
    });
    
    if(calculate(room,blockList))
    {
        /// 根据 id，排序回来
        std::sort(blockList.begin(),blockList.end(),[](const Block& a,const Block& b){
            return a.id < b.id;
        });
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
