//
//  baseMethod.h
//  modulo
//
//  Created by mario on 16/3/2.
//  Copyright © 2016年 mario. All rights reserved.
//

#ifndef baseMethod_h
#define baseMethod_h
#include <vector>
#include "Data.h"


using namespace std;


bool hasNextPos(const Room& room,Block& block,int& posX,int& posY)
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
void getMoveAbleBlock_lazy(Room& room,vector<Block>& blockList,const BlockPosList& blockPosList,const vector<int>& vec,int x,int y,vector<int>& result)
{
    result.clear();
    
    for(int i = 0;i < vec.size();++i)
    {
        int blockIndex = vec[i];
        Block& block = blockList[blockIndex];
        int posX,posY;
        const Pos& pos = blockPosList[blockIndex][block.y][block.x];
        posX = pos.first;
        posY = pos.second;
        if(posX != -1 && posY != -1)
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
inline void getCombis(const vector<int>& vec,int m,vector<vector<int>>& result,vector<int>& selectors,vector<int>& oneComb)
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
void getCombis_lazy(const vector<int>& vec,int m,vector<vector<int>*>& result,vector<vector<int>*>& pUnusedComb,vector<int>& selectors)
{
    // 参考 http://mingxinglai.com/cn/2012/09/generate-permutation-use-stl/
    // 这里用的是 next_permutaion,前面补 0
    
    // n 个中取 m 个组合
    
    
    for(int i = 0;i < result.size();++i)
    {
        pUnusedComb.push_back(result[i]);
    }
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
        vector<int>* pOneComb;
        
        if(pUnusedComb.size() != 0)
        {
            pOneComb = pUnusedComb.back();
            pUnusedComb.pop_back();
        }
        else
        {
            pOneComb = new vector<int>;
        }
        
        pOneComb->clear();
        for (size_t i = 0; i < selectors.size(); ++i)
        {
            if (selectors[i])
            {
                pOneComb->push_back(vec[i]);
            }
        }
        result.push_back(pOneComb);
    } while (next_permutation(selectors.begin(), selectors.end()));
}
inline void getMoveNums(const Room& room,int x,int y,int n,vector<int>& result)
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

inline void getUnlockBlocks(const vector<Block>& blockList,vector<int>& result)
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
void getValueBlocks_lazy(const vector<Block>& blockList,BlockValueList& blockValueList,const vector<int>& vec,int x,int y,vector<int>& result)
{
    result.clear();
    for(int i = 0;i < vec.size();++i)
    {
        int index = vec[i];
        const Block& block = blockList[index];
        if(blockValueList[index][block.y][block.x][y][x] != 0)
        {
            result.push_back(index);
        }
        
    }
}

void preCalculateBlockValue(const Room& room,vector<Block> blockList,BlockValueList& blockValueList,BlockPosList& blockPosList)
{
    for(int i = 0;i < blockList.size();++i)
    {
        Block& block = blockList[i];
        MMap mmap;
        PosMap posMap;
        for(int y = 0;y < room.h;++y)
        {
            vector<Map> vecMap;
            vector<Pos> vecPos;
            for(int x = 0;x < room.w;++x)
            {
                Map map;
                Pos pos;
                vector<int> row(room.w,0);
                for(int k = 0;k < room.h;++k)
                {
                    map.push_back(row);
                }
                Room tmp;
                tmp.init(map, 0);
                if(block.canMoveTo(x, y, room.w,room.h))
                {
                    block.moveTo(x, y);
                    tmp.add(block);
                }
                map = tmp.room;
                vecMap.push_back(map);
                
                int posX,posY;
                if(!hasNextPos(room,block,posX,posY))
                {
                    posX = posY = -1;
                }
                
                pos.first = posX;
                pos.second = posY;
                vecPos.push_back(pos);
            }
            mmap.push_back(vecMap);
            posMap.push_back(vecPos);
        }
        
        blockValueList.push_back(mmap);
        blockPosList.push_back(posMap);
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

// blockList[i] 在自身坐标为(x,y) 时 在 (x,y)位置的值

inline int getBlockValueAt_lazy(const vector<Block>& blockList,const BlockValueList& blockValueList,const int x,int y)
{
    int result = 0;
    int n = blockList.size();
    for(int i = 0;i < n;++i)
    {
        const Block& block = blockList[i];
        result += blockValueList[i][block.y][block.x][y][x];
    }
    return result;
}

bool isZeroAt_lazy(const Room& room,const vector<Block>& blockList,int x,int y,int& sum)
{
    sum = getBlocksValueAt(blockList, x, y) + room.room[y][x];
    //sum = getBlocksValueAt(blockList, x, y);    // room 已经合到 block[0]中了
    return (sum % room.mod) == 0;
}

bool isZeroAt_lazy(const Room& room,const vector<Block>& blockList,const BlockValueList& blockValueList,int x,int y,int& sum)
{
    sum = getBlockValueAt_lazy(blockList, blockValueList, x, y) + room.room[y][x];
    //sum = getBlockValueAt_lazy(blockList, blockValueList, x, y);
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
bool isZeroFrom_lazy(const Room& room,const vector<Block>& blockList,const BlockValueList& blockValueList,int x,int y)
{
    // 从 (x,y) 的下一个位置开始,到整个room结束，是否都为 0
    while(1)
    {
        int nextX;
        int nextY;
        int sum;
        // 优化，先算后面那些非0的位置。
//        for(int i = 0;i < Data::noneZeroPosOfRoom.size();++i)
//        {
//            const Pos& pt = Data::noneZeroPosOfRoom[i];
//            int y1 = pt.second;
//            int x1 = pt.first;
//            // 前面那些位置就不用算了
//            if(y1 <= y && x1 <= x)
//                break;
//            
//            if(!isZeroAt_lazy(room, blockList,blockValueList, x1, y1,sum))
//            {
//                return false;
//            }
//        }
        
                             
           // 如果那些非0的位置都OK，那就按原来的方式算整个矩阵是否OK

        bool ret = room.getNextPos(x,y,nextX,nextY);
        x = nextX;
        y = nextY;
        if(!ret)
        {
            // 到结尾了，前面都为0
            return true;
        }
        if(!isZeroAt_lazy(room, blockList,blockValueList, nextX, nextY,sum))
        {
            return false;
//
//        int result = 0;
//        int n = blockList.size();
//        for(int i = 0;i < n;++i)
//        {
//            const Block& block = blockList[i];
//            result += blockValueList[i][block.y][block.x][y][x];
//        }
//        if((result % room.mod) != 0)
//            return false;
        }
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
                int w = (j == block.y ? block.x : room.w);
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
        }
        
    }
    
    return false;
}


#endif /* baseMethod_h */
