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
#include <deque>
#include "cJSON.h"
#include <thread>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <stdlib.h>
#include <unistd.h>
#include "Block.h"
#include "Room.h"
#include "Task.h"

#define PrintProgess
using namespace std;

static std::atomic_ullong tryTimes;
static long lastTime = 0;
static unsigned long long possibility = 1;
static long percent = 0;

static Room room;
static vector<Block> BlockList;

static deque<Task> queue;
static bool foundResult = false;
static Task resultTask;
std::mutex mtx;




#ifdef _WIN32
#include <windows.h>
#endif

void mySleep(int millSecond)
{
#ifdef _WIN32
    Sleep(millSecond);
#else
    sleep(millSecond);
#endif
}

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
void getMoveAbleBlock(Room& room,vector<Block>& blockList,const vector<int>& vec,int x,int y,vector<int>& result)
{
    result.clear();
    
    for(int i = 0;i < vec.size();++i)
    {
        int blockIndex = vec[i];
        Block& block = blockList[blockIndex];
        int posX,posY;
        if(hasNextPos(room, block, posX, posY))
        {
            result.push_back(blockIndex);
        }
    }
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
void getCombis(const vector<int>& vec,int m,vector<vector<int>>& result,vector<int>& selectors,vector<int>& oneComb)
{
    // 参考 http://mingxinglai.com/cn/2012/09/generate-permutation-use-stl/
    // 这里用的是 next_permutaion,前面补 0
    
    // n 个中取 m 个组合
    result.clear();
    
    int n = vec.size();
    
    selectors.clear();
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
        //vector<int> oneComb;
        oneComb.clear();
        for (size_t i = 0; i < selectors.size(); ++i)
        {
            if (selectors[i])
            {
                oneComb.push_back(vec[i]);
            }
        }
        result.push_back(oneComb);
    } while (next_permutation(selectors.begin(), selectors.end()));
    
    
    
    
}
void getMoveNums(const Room& room,int x,int y,int n,vector<int>& result)
{
    // 计算(x,y) 位置可以 移除多少个方块达到圆满。排除0（即不移除的情况)
    
    result.clear();
    
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
            result.push_back(offset);
        }
        else
        {
            break;
        }
        
        
    }
    
}
void getMoveNums_lazy(int sum,int mod,int n,vector<int>& result)
{
    // 计算(x,y) 位置可以 移除多少个方块达到圆满。排除0（即不移除的情况)
    
    result.clear();
    
    int val = sum;
    
    int nextVal = val;
    while(nextVal >= 0)
    {
        int times = nextVal / mod;
        int closest = times * mod;
        int offset = val - closest;
        
        nextVal -= mod;
        
        if(n >= offset)
        {
            result.push_back(offset);
        }
        else
        {
            break;
        }
        
        
    }
    
}

void getUnlockBlocks(const vector<Block>& blockList,vector<int>& result)
{
    result.clear();
    for(int i = 0;i < blockList.size();++i)
    {
        if(!blockList[i].isLock())
        {
            result.push_back(i);
        }
    }
    
}
void getValueBlocks(const vector<Block>& blockList,const vector<int>& vec,int x,int y,vector<int>& result)
{
    result.clear();
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
}
int getBlocksValueAt(const vector<Block>& blockList,int x,int y)
{
    int result = 0;
    for(int i = 0;i < blockList.size();++i)
    {
        const Block& block = blockList[i];
        if(!(block.x <= x && x <= block.x + block.w - 1))
            continue;
        if(!(block.y <= y && y <= block.y + block.h - 1))
            continue;
        if(block.block[y - block.y][x - block.x] != 0)
        {
            result++;
        }
    }
    return result;
}
bool isZeroAt_lazy(const Room& room,const vector<Block>& blockList,int x,int y,int& sum)
{
    sum = getBlocksValueAt(blockList, x, y) + room.room[y][x];
    return (sum % room.mod) == 0;
}
bool isZeroFrom_lazy(const Room& room,const vector<Block>& blockList,int x,int y)
{
    // 从 (x,y) 的下一个位置开始,到整个room结束，是否都为 0
    while(1)
    {
        int nextX;
        int nextY;
        int sum;
        bool ret = room.getNextPos(x,y,nextX,nextY);
        x = nextX;
        y = nextY;
        if(!ret)
        {
            // 到结尾了，前面都为0
            return true;
        }
        if(!isZeroAt_lazy(room, blockList, nextX, nextY,sum))
        {
            return false;
        }
    }
}
bool move2(Room room,vector<Block> blockList/*,deque<Task>& queue*/)
{
    vector<vector<int>> combs;
    vector<int> unlockVec;
    vector<int> valueVec;
    vector<int> moveAbleVec;
    vector<int> moveNums;
    vector<int> selectors;
    vector<int> oneComb;
    int sumAtXY = 0;
    int blockNums = blockList.size();
    deque<Task> local_queue;
    while(1)
    {
        Task task;
        
        if(foundResult)
        {
            return true;
        }
        
        /// 先从本线程的队列里面拿
        if(!local_queue.empty())
        {
            task = local_queue.back();
            local_queue.pop_back();
        }
        else
        {
            /// 没有的话再去公用队列中拿
            mtx.lock();
            if(!queue.empty())
            {
                // 拿一半，最少一个
                int halfSize = max(int(1),int(queue.size() / 2));
                
                while(halfSize--)
                {
                    local_queue.push_back(queue.back());
                    queue.pop_back();
                }
            }
            mtx.unlock();
            // 如果没取到，过一会再尝试
            if(local_queue.empty())
            {
                mySleep(1);
            }
            continue;
        }
        //room = RRoom;
        
        int x = task.x;
        int y = task.y;
        
        for(int i = 0;i < blockNums;++i)
        {
            blockList[i].moveTo(task.blocksX[i], task.blocksY[i]);
            //room.add(blockList[i]);
        }
        
        tryTimes++;
        //if(room.isZeroAt(x,y) && room.isZero())
        if(isZeroAt_lazy(room, blockList, x, y,sumAtXY) && isZeroFrom_lazy(room, blockList, x, y))
        {
            resultTask = task;
            foundResult = true;
            return true;
        }
        // 是最后一个位置
        if(x == room.m - 1 && y == room.n - 1)
        {
            continue;
        }
        
        // 找出所有没被锁的木块
        //        vector<int> unlockVec;
        
        unlockVec.clear();
        
        for(int i = 0;i < blockNums;++i)
        {
            if(!task.vecLock[i])
            {
                unlockVec.push_back(i);
            }
        }
        
        // 在没被锁的木块里，找出能覆盖到该位置的木块(值不为0)。可以先把没被锁的木块放到左上角
        getValueBlocks(blockList,unlockVec,x,y,valueVec);
        // 在所有影响该位置的木块中，找出能移动的
        //        moveAbleVec = getMoveAbleBlock(room,blockList,valueVec,x,y);
        getMoveAbleBlock(room,blockList,valueVec,x,y,moveAbleVec);
        
        int n = moveAbleVec.size();
        
        // 对该位置，根据该位置的值和能移动的方块数，计算所有可以移动哪些个数
        //getMoveNums(room,x,y,n,moveNums);
        getMoveNums_lazy(sumAtXY, room.mod,n,moveNums);
        //        if(moveNums.size() == 0)
        //            return false;
//        for(int i = 0;i < moveNums.size();++i)
        for(int i = moveNums.size() - 1;i >= 0;--i)
        {
            int m = moveNums[i];
            // 从 vec1 的 n 个中挑 m 个出来。
            getCombis(moveAbleVec,m,combs,selectors,oneComb);
            // 对于每种组合里面的方块，移动到下个位置。
            for(int j = 0;j < combs.size();/*++j*/)
            {
                Task newTask = task;
                
                // 对有影响的block，加锁
                for(int i = 0;i < valueVec.size();++i)
                {
                    int index = valueVec[i];
                    //blockList[index].lock();
                    newTask.vecLock[index] = true;
                }
                // 仅解锁本次移动到下个位置部分的block
                for(int k = 0;k < combs[j].size();++k)
                {
                    int index = combs[j][k];
                    // 每个block 移动到新的位置
                    //blockList[index].unLock();
                    newTask.vecLock[index] = false;
                }
                
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
                    newTask.blocksX[index] = posX;
                    newTask.blocksY[index] = posY;
                    
                }
                // 得到下个位置的 坐标(x1,y1)
                
                int x1,y1;
                room.getNextPos(x, y, x1, y1);
//                if(x < room.m - 1)
//                {
//                    x1 = x + 1;
//                    y1 = y;
//                }
//                else
//                {
//                    x1 = 0;
//                    y1 = y + 1;
//                }
                
                newTask.x = x1;
                newTask.y = y1;
                newTask.number = task.number + 1;
                local_queue.push_back(newTask);
                //cout << local_queue.size()<<",";
                ++j;
            }
        }
        
        
        // 丢一半给主队列，避免其他队列没事干
        if(queue.empty())
        {
            // 拿出一半
            int halfSize = local_queue.size() / 2;
            vector<Task> vecTmp;
            while(halfSize--)
            {
                vecTmp.push_back(local_queue.back());
                local_queue.pop_back();
            }
            mtx.lock();
            for(int i = 0;i < vecTmp.size();++i)
            {
                queue.push_back(vecTmp[i]);
            }
            mtx.unlock();
        }
        
    }
    return false;
    
}


//bool move(Room& room,vector<Block>& blockList,int x,int y)
//{
//    if(room.isZeroAt(x,y) && room.isZero())
//    {
//        return true;
//    }
//    // 是最后一个位置
//    if(x == room.m - 1 && y == room.n - 1)
//    {
//        return false;
//    }
//    // 找出所有没被锁的木块
//    vector<int> unlockVec = getUnlockBlocks(blockList);
//    if(unlockVec.size() == 0)
//        return false;
//    // 在没被锁的木块里，找出能覆盖到该位置的木块(可以先把没被锁的木块放到左上角
//    vector<int> valueVec = getValueBlocks(blockList,unlockVec,x,y);
//
//    // 在所有影响该位置的木块中，找出能移动的
//    vector<int> moveAbleVec = getMoveAbleBlock(room,blockList,valueVec,x,y);
//
//    int n = moveAbleVec.size();
//
//    // 对该位置，根据该位置的值和能移动的方块数，计算所有可以移动哪些个数
//    vector<int> moveNums = getMoveNums(room,x,y,n);
//
//    if(moveNums.size() == 0)
//        return false;
//
//
//    for(int i = 0;i < moveNums.size();++i)
//    {
//        int m = moveNums[i];
//        // 从 vec1 的 n 个中挑 m 个出来。
//        vector<vector<int>> combs = getCombis(moveAbleVec,m);
//
//        // 对于每种组合里面的方块，移动到下个位置。
//        for(int j = 0;j < combs.size();++j)
//        {
//            // 对有影响的block，加锁
//            for(int i = 0;i < valueVec.size();++i)
//            {
//                int index = valueVec[i];
//                blockList[index].lock();
//            }
//            // 仅解锁本次移动到下个位置部分的block
//            for(int k = 0;k < combs[j].size();++k)
//            {
//                int index = combs[j][k];
//                // 每个block 移动到新的位置
//                blockList[index].unLock();
//            }
//            // 记下这个组合旧的位置
//
//            vector<int> vx(combs[j].size());
//            vector<int> vy(combs[j].size());
//            for(int k = 0;k < combs[j].size();++k)
//            {
//                int index = combs[j][k];
//                Block& block = blockList[index];
//                vx[k] = block.x;
//                vy[k] = block.y;
//            }
//
//            Room testRoom = room;
//            // 方块移到下个位置
//            for(int k = 0;k < combs[j].size();++k)
//            {
//                int index = combs[j][k];
//                Block& block = blockList[index];
//                int posX,posY;
//                if(!hasNextPos(room,block,posX,posY))
//                {
//                    cout << "+++++ error +++++" << endl;
//                }
//
//                testRoom.remove(block);
//                block.moveTo(posX,posY);
//                testRoom.add(block);
//
//            }
//            // 得到下个位置的 坐标(x1,y1)
//            int x1,y1;
//            if(x < room.m - 1)
//            {
//                x1 = x + 1;
//                y1 = y;
//            }
//            else
//            {
//                x1 = 0;
//                y1 = y + 1;
//            }
//
//            if(!move(testRoom,blockList,x1,y1))
//            {
//                for(int k = 0;k < combs[j].size();++k)
//                {
//                    int index = combs[j][k];
//                    Block& block = blockList[index];
//                    block.moveTo(vx[k], vy[k]);
//                }
//            }
//            else
//            {
//                return true;
//            }
//
//            // 解锁，回复原装
//            for(int i = 0;i < valueVec.size();++i)
//            {
//                int index = valueVec[i];
//                blockList[index].unLock();
//            }
//        }
//    }
//    return false;
//
//}
//


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
void calPossibility(Room& room,vector<Block>& blockList,unsigned long long& possibility)
{
    for(int i = 0;i < blockList.size();++i)
    {
        const Block& block = blockList[i];
        long rightStep = room.m - block.w + 1;
        long downStep = room.n - block.h + 1;
        
        possibility *= rightStep * downStep;
    }
}
//bool calculate(Room& room,vector<Block>& blockList)
//{
//    string str;
//    //bool ret = move(room,blockList,0);
//
//    for(int i = 0;i < blockList.size();++i)
//    {
//        room.add(blockList[i]);
//    }
//    int k = blockList.size() - 1;	// 最左边的可以更改的索引
//    int n = blockList.size();
//    while(k >= 0)
//    {
//        if(room.isZero())
//            return true;
//
//        Block& block = blockList[k];
//        //block.print();
//        for(int j = k + 1; j < n;++j)
//        {
//            Block& bk = blockList[j];
//            room.remove(bk);
//            bk.moveTo(0,0);
//            room.add(bk);
//
//        }
//        if(k == 1)
//        {
//            int j = 0;
//        }
//        /*************************************************/
//        //如果是最后一个
//        if(k == n - 1)
//        {
//            bool toContinue = false;
//
//
//            for(int j = 0;j <= block.y;++j)
//            {
//                if(toContinue == true)
//                    break;
//                int w = (j == block.y ? block.x : room.m);
//                for(int i = 0;i < w;++i)
//                {
//                    if((room.room[j][i] % room.mod) != 0)
//                    {
//                        toContinue = true;
//                        break;
//                    }
//                }
//            }
//            if(toContinue)
//            {
//                k--;
//                continue;
//            }
//        }
//        //*********************************************
//        int newX,newY;
//        if(hasNextPos(room,block,newX,newY))
//        {
//            room.remove(block);
//            block.moveTo(newX,newY);
//            room.add(block);
//
//            for(int j = k + 1; j < n;++j)
//            {
//                Block& bk = blockList[j];
//                room.remove(bk);
//                bk.moveTo(0,0);
//                room.add(bk);
//
//            }
//            if(k == 1 && block.y == 1)  //debug
//            {
//                int ddd = 0;
//            }
//            //            if(k != n - 1)
//            //            {
//            //                pair<int,int> area;
//            //                bool ret = hasUntouchableArea(room,block, area);
//            //                if(ret)
//            //                {
//            //                    if(!canZeroWithChildBlock(room,blockList, k + 1, area))
//            //                    {
//            //                        k--;
//            //                        continue;
//            //                    }
//            //                }
//            //            }
//
//            k = n - 1;
//        }
//        else
//        {
//            k--;
//            if(k < 0)
//            {
//                int j = 0;
//            }
//        }
//    }
//    return false;
//}
void threadHelper()
{
    move2(room,BlockList);
    
}
void sendMail(int level,string result,long tryTimes,int second)
{
    
}
int main (int argc, const char * argv[]) {
    
    const int MAX_LEVEL = 59;
    const int BEGIN_LEVEL = 45;
    const int END_LEVEL = 59;
    for(int level_it = BEGIN_LEVEL - 1;level_it < END_LEVEL;++level_it)
    {
        lastTime = clock();
        
        possibility = 1;
        
        tryTimes = 0;
        percent = 0;
        foundResult = false;
        
        int level = 0;
        int modu = 0;
        string str;
        string output;
        
        str = strings[level_it];
        
        room = Room();
        queue.clear();
        BlockList.clear();
        
        processInput(str,level,modu,room,BlockList);
        
        calPossibility(room,BlockList,possibility);
        
        time_t beginTime;
        time(&beginTime);
        lastTime = clock();
        
        cout << "Level : " << level << endl;
        cout << "Total: " << possibility << endl;
        cout << "Begin Time :" << ctime(&beginTime);
        
        
        Task task;
        for(int i = 0;i < BlockList.size();++i)
        {
            task.x = 0;
            task.y = 0;
            task.number = 0;
            task.blocksX[i] = 0;
            task.blocksY[i] = 0;
            task.vecLock[i] = false;
        }
        queue.push_back(task);
        
        //bool suss = move2(room,blockList,queue);
        
        int thread_nums = 2;
        vector<thread> vecThread;
        for(int i =0;i < thread_nums;++i)
        {
            thread t1(threadHelper);
            t1.detach();
        }
        
        long lastReportTime = clock();
        while(!foundResult)
        {
            mySleep(1);
            long currReportTime = clock();
            long interval = (currReportTime - lastReportTime)/CLOCKS_PER_SEC;
            
            // 60秒报告一次
            if(interval > 60)
            {
                lastReportTime = currReportTime;
                long speedTime = (clock() - lastTime)/CLOCKS_PER_SEC;
                cout << "tryTimes:" << tryTimes << " ,speed: " << tryTimes/speedTime << " ,time: " << speedTime << endl;
            }
            
            
        }
        
        if(foundResult)
        {
            for(int i = 0;i < BlockList.size();++i)
            {
                output += resultTask.blocksY[i] + '0';
                output += resultTask.blocksX[i] + '0';
            }
        }
        else
        {
            output = "XXXXXXXXXXXXXXXXXXXXXX";
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
        
        sendMail(level, output, tryTimes, second);
    }
    
    return 0;
}

