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
#include <map>
#include "strings.h"
#include<fstream>
#include <time.h>
using namespace std;
#include "cJSON.h"

static long tryTimes = 0;
static long lastTime = 0;
static long possibility = 1;
static long percent = 0;
#define PrintProgess
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
    bool isZeroAt(int x,int y)
    {
        tryTimes++;
        return (room[y][x] % mod) == 0;
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
        bool ret = block.x + block.w + 1 <= m;
        ret &= (block.y < area.second ? block.x <= m : block.x <= area.first);
        return ret;
    }
    bool canDownWithLimitArea(const Block& block,pair<int,int> area)
    {
        bool ret = block.y + block.h + 1 <= n;
        ret &= (block.y < area.second ? block.x <= m : block.x <= area.first);
        
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
    
    bool isZero()
    {
        bool ret = true;
        for(int j = 0;j < n;++j)
        {
            if(ret == false)
                break;
            for(int i = 0;i < m;++i)
            {
                if((room[j][i] % mod) != 0)
                {
                    ret = false;
                    break;
                }
            }
        }
        tryTimes++;
#ifdef PrintProgess
//        double currPercent = (double)tryTimes * 100 / possibility;
//        if(currPercent >= percent +  1.0)
//        {
//            cout << (int)currPercent << "%," << tryTimes << " / " << possibility << endl;
//            percent = currPercent;
//        }
#endif
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
                int len = (i == area.second ? area.first : m);
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
        //tryTimes++;
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
    
    if(room.canDownWithLimitArea(block, area))
    {
        posX = 0;
        posY = block.y + 1;
        return true;
    }
    return false;
    
}


vector<int> getBlockAtPos(const vector<Block>& blockList,int x,int y)
{
    vector<int> vec;
    for(int i = 0;i < blockList.size();++i)
    {
        if(blockList[i].x == x && blockList[i].y == y)
            vec.push_back(i);
    }
    return vec;
}
vector<int> getMoveAbleBlock(Room& room,vector<Block>& blockList,const vector<int>& vec,int x,int y)
{
    vector<int> vec1;
    
    for(int i = 0;i < vec.size();++i)
    {
        int blockIndex = vec[i];
        Block& block = blockList[blockIndex];
        int posX,posY;
        if(hasNextPos(room, block, posX, posY))
        {
            vec1.push_back(blockIndex);
        }
    }
    return vec1;
}
vector<int> getNonZeroBlock(vector<Block>& blockList,const vector<int>& vec,int x,int y)
{
    vector<int> vec1;
    
    for(int i = 0;i < vec.size();++i)
    {
        int blockIndex = vec[i];
        Block& block = blockList[blockIndex];
        
        if(blockList[blockIndex].block[0][0] != 0)
            vec1.push_back(blockIndex);
    }
    return vec1;
}
vector<vector<int>> getCombis(vector<int> vec,int m)
{
    // 参考 http://mingxinglai.com/cn/2012/09/generate-permutation-use-stl/
    // 这里用的是 next_permutaion,前面补 0
    
    // n 个中取 m 个组合
    vector<vector<int>> result;
    int n = vec.size();
    
    vector<int> selectors;
    for(int i = 0;i < n - m;++i)
    {
        selectors.push_back(0);
    }
    for(int i = 0;i < m;++i)
    {
        selectors.push_back(1);
    }
    
    do
    {
        vector<int> oneComb;
        for (size_t i = 0; i < selectors.size(); ++i)
        {
            if (selectors[i])
            {
                oneComb.push_back(vec[i]);
            }
        }
        result.push_back(oneComb);
    } while (next_permutation(selectors.begin(), selectors.end()));
    
    
    return result;
}
vector<int> getMoveNums(const Room& room,int x,int y,int n)
{
    // 计算(x,y) 位置可以 移除多少个方块达到圆满。排除0（即不移除的情况)
    vector<int> vec;
    
    int val = room.room[y][x];
    int mod = room.mod;
    
    int nextVal = val;
    while(nextVal >= 0)
    {
        int times = nextVal / mod;
        int closest = times * mod;
        int offset = val - closest;
        
        nextVal -= mod;
        
        if(n >= offset)
        {
            vec.push_back(offset);
        }
        else
        {
            break;
        }
        
        
    }
    
    return vec;
}
vector<int> getUnlockBlocks(const vector<Block>& blockList)
{
    vector<int> result;
    for(int i = 0;i < blockList.size();++i)
    {
        if(!blockList[i].isLock())
        {
            result.push_back(i);
        }
    }
    return result;
}
vector<int> getValueBlocks(const vector<Block>& blockList,vector<int> vec,int x,int y)
{
    vector<int> result;
    for(int i = 0;i < vec.size();++i)
    {
        int index = vec[i];
        const Block& block = blockList[index];
        if(!(block.x <= x && x <= block.x + block.w - 1))
            continue;
        if(!(block.y <= y && y <= block.y + block.h - 1))
            continue;
        if(block.block[y - block.y][x - block.x] != 0)
        {
            result.push_back(index);
        }
    }
    return result;
}
bool move(Room& room,vector<Block>& blockList,int x,int y)
{
    if(room.isZeroAt(x,y) && room.isZero())
    {
        return true;
    }
    // 是最后一个位置
    if(x == room.m - 1 && y == room.n - 1)
    {
        return false;
    }
    // 找出所有没被锁的木块
    vector<int> unlockVec = getUnlockBlocks(blockList);
    if(unlockVec.size() == 0)
        return false;
    // 在没被锁的木块里，找出能覆盖到该位置的木块(可以先把没被锁的木块放到左上角
    vector<int> valueVec = getValueBlocks(blockList,unlockVec,x,y);
    
    // 在所有影响该位置的木块中，找出能移动的
    vector<int> moveAbleVec = getMoveAbleBlock(room,blockList,valueVec,x,y);
    
    int n = moveAbleVec.size();
    
    // 对该位置，根据该位置的值和能移动的方块数，计算所有可以移动哪些个数
    vector<int> moveNums = getMoveNums(room,x,y,n);
    
    if(moveNums.size() == 0)
        return false;
    
    
    for(int i = 0;i < moveNums.size();++i)
    {
        int m = moveNums[i];
        // 从 vec1 的 n 个中挑 m 个出来。
        vector<vector<int>> combs = getCombis(moveAbleVec,m);
        
        // 对于每种组合里面的方块，移动到下个位置。
        for(int j = 0;j < combs.size();++j)
        {
            // 对有影响的block，加锁
            for(int i = 0;i < valueVec.size();++i)
            {
                int index = valueVec[i];
                blockList[index].lock();
            }
            // 仅解锁本次移动到下个位置部分的block
            for(int k = 0;k < combs[j].size();++k)
            {
                int index = combs[j][k];
                // 每个block 移动到新的位置
                blockList[index].unLock();
            }
            // 记下这个组合旧的位置
            
            vector<int> vx(combs[j].size());
            vector<int> vy(combs[j].size());
            for(int k = 0;k < combs[j].size();++k)
            {
                int index = combs[j][k];
                Block& block = blockList[index];
                vx[k] = block.x;
                vy[k] = block.y;
            }
            
            Room testRoom = room;
            // 方块移到下个位置
            for(int k = 0;k < combs[j].size();++k)
            {
                int index = combs[j][k];
                Block& block = blockList[index];
                int posX,posY;
                if(!hasNextPos(room,block,posX,posY))
                {
                    cout << "+++++ error +++++" << endl;
                }
                
                testRoom.remove(block);
                block.moveTo(posX,posY);
                testRoom.add(block);
                
            }
            // 得到下个位置的 坐标(x1,y1)
            int x1,y1;
            if(x < room.m - 1)
            {
                x1 = x + 1;
                y1 = y;
            }
            else
            {
                x1 = 0;
                y1 = y + 1;
            }
            
            if(!move(testRoom,blockList,x1,y1))
            {
                for(int k = 0;k < combs[j].size();++k)
                {
                    int index = combs[j][k];
                    Block& block = blockList[index];
                    block.moveTo(vx[k], vy[k]);
                }
            }
            else
            {
                return true;
            }
            
            // 解锁，回复原装
            for(int i = 0;i < valueVec.size();++i)
            {
                int index = valueVec[i];
                blockList[index].unLock();
            }
        }
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
    area.first = w;
    area.second = h;
    return true;
}
bool canZeroWithChildBlock(Room room,vector<Block> blockList,int beginIndex,pair<int,int> area)
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
                    if((newRoom.room[j][i] % newRoom.mod) != 0)
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
            if(k < 0)//debug
            {
                int ddd = 1;
            }
        }
        
    }
    
    return false;
}
void calPossibility(Room& room,vector<Block>& blockList,long& possibility)
{
    for(int i = 0;i < blockList.size();++i)
    {
        const Block& block = blockList[i];
        long rightStep = room.m - block.w + 1;
        long downStep = room.n - block.h + 1;
        
        possibility *= rightStep * downStep;
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
        //block.print();
        for(int j = k + 1; j < n;++j)
        {
            Block& bk = blockList[j];
            room.remove(bk);
            bk.moveTo(0,0);
            room.add(bk);
            
        }
        if(k == 1)
        {
            int j = 0;
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
            if(k == 1 && block.y == 1)  //debug
            {
                int ddd = 0;
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
            if(k < 0)
            {
                int j = 0;
            }
        }
    }
    return false;
}

int main (int argc, const char * argv[]) {
    
    const int MAX_LEVEL = 59;
    const int BEGIN_LEVEL = 1;
    const int END_LEVEL = 59;
    for(int level_it = BEGIN_LEVEL - 1;level_it < MAX_LEVEL;++level_it)
    {
        
        possibility = 1;
        lastTime = clock();
        tryTimes = 0;
        percent = 0;
        
        
        int level = 0;
        int modu = 0;
        string str;
        string output;
        Room room;
        vector<Block> blockList;
        str = strings[level_it];
        
        
        processInput(str,level,modu,room,blockList);
        
        /// 对 Block 进行排序，面积大的在前面
        
        //    std::sort(blockList.begin(),blockList.end(),[](const Block& a,const Block& b){
        //        return a.w * a.h > b.w * b.h;
        //        //return a.w > b.w;
        //    });
        
        
        calPossibility(room,blockList,possibility);
        
        time_t beginTime;
        time(&beginTime);
        
        cout << "Level : " << level << endl;
        cout << "Total: " << possibility << endl;
        cout << "Begin Time :" << ctime(&beginTime);
        
        // 方法1
        //bool suss = calculate(room,blockList);
        // 方法2
        for(int i = 0;i < blockList.size();++i)
        {
            room.add(blockList[i]);
        }
        bool suss = move(room,blockList,0,0);
        if(suss)
        {
            /// 根据 id，排序回来
//            std::sort(blockList.begin(),blockList.end(),[](const Block& a,const Block& b){
//                return a.id < b.id;
//            });
            for(int i = 0;i < blockList.size();++i)
            {
                output += blockList[i].y + '0';
                output += blockList[i].x + '0';
            }
        }
        else
        {
            output = "XXXXXXXXXXXXXXXXXXXXXX";
//            room.print();
            
//            for(int i = 0;i < blockList.size();++i)
//            {
//                blockList[i].print();
//                cout << endl;
//            }
//            /// 根据 id，排序回来
//            std::sort(blockList.begin(),blockList.end(),[](const Block& a,const Block& b){
//                return a.id < b.id;
//            });
//            for(int i = 0;i < blockList.size();++i)
//            {
//                output += blockList[i].y + '0';
//                output += blockList[i].x + '0';
//            }
        }
        
        time_t endTime;
        time(&endTime);
        
        long second = (clock() - lastTime)/CLOCKS_PER_SEC;
        
        cout << "Result:    " << output << endl;
        cout << "tryTimes:  " << tryTimes << endl;
        cout << "Time(s):   " << second << endl;
        cout << "Speed:     " << tryTimes / max((long)1,second) << endl;
        cout << "Begin Time :" << ctime(&beginTime);
        cout << "End Time :" << ctime(&endTime);
        cout << endl;
        
        
        // 写入文件
        ofstream outdata;
        string fileName = std::to_string(level_it + 1) + ".txt";
        outdata.open(fileName,ios::app);//ios::app是尾部追加的意思
        outdata << "Level : " << level << endl;
        outdata << "Total: " << possibility << endl;
        outdata << "Result:    " << output << endl;
        outdata << "tryTimes:  " << tryTimes << endl;
        outdata << "Time(s):   " << (clock() - lastTime)/CLOCKS_PER_SEC << endl;
        outdata << "Speed:     " << tryTimes / max((long)1,second) << endl;
        outdata << "Begin Time :" << ctime(&beginTime);
        outdata << "End Time :" << ctime(&endTime);
        outdata.close();
        
    }
    
    return 0;
}
