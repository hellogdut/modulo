//
//  Strategy.h
//  modulo
//
//  Created by mario on 16/3/2.
//  Copyright © 2016年 mario. All rights reserved.
//

#ifndef Strategy_h
#define Strategy_h
#include "Data.h"
#include "baseMethod.h"
#include "Utility.h"


bool move2(Room room,vector<Block> blockList,BlockValueList blockValueList,BlockPosList blockPosList)
{
    
    vector<vector<int>> combs;
    
    vector<vector<int>*> pCombs;
    vector<vector<int>*> pUnusedComb;
    
    vector<int> unlockVec;
    vector<int> valueVec;
    vector<int> moveAbleVec;
    vector<int> moveNums;
    vector<int> selectors;
    vector<int> oneComb;
    int sumAtXY = 0;
    int blockNums = blockList.size();
    vector<Task> local_queue;
    while(!Data::foundResult)
    {
        Task task;
        
        /**** 保存进度 ********/
        if(Data::saving)
        {
            Data::mtx.lock();
            while(!local_queue.empty())
            {
                queue.push_back(local_queue.back());
                local_queue.pop_back();
            }
            Data::mtx.unlock();
            Data::threadStopCnt++;  // 只加一次
            while(Data::saving)
            {
                mySleep(10);
            }
            
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
            Data::mtx.lock();
            if(!Data::queue.empty())
            {
                // 拿一半，最少一个
                int halfSize = max(int(1),int(Data::queue.size() / 2));
                
                while(halfSize--)
                {
                    local_queue.push_back(Data::queue.back());
                    Data::queue.pop_back();
                }
            }
            Data::mtx.unlock();
            // 如果没取到，过一会再尝试
            if(local_queue.empty())
            {
                mySleep(1);
            }
            continue;
        }
        
        
        int x = task.x;
        int y = task.y;
        
        for(int i = 0;i < blockNums;++i)
        {
            blockList[i].moveTo(task.blocksX[i], task.blocksY[i]);
        }
        
        Data::tryTimes++;
        
        //        if(isZeroAt_lazy(room, blockList, x, y,sumAtXY) && isZeroFrom_lazy(room, blockList, x, y))
        if(isZeroAt_lazy(room, blockList,blockValueList, x, y,sumAtXY) && isZeroFrom_lazy(room, blockList,blockValueList, x, y))
        {
            Data::resultTask = task;
            Data::foundResult = true;
            return true;
        }
        // 是最后一个位置
        if(x == room.w - 1 && y == room.h - 1)
        {
            continue;
        }
        
        unlockVec.clear();
        
        for(int i = 0;i < blockNums;++i)
        {
            if(!task.vecLock[i])
            {
                unlockVec.push_back(i);
            }
        }
        
        // 在没被锁的木块里，找出能覆盖到该位置的木块(值不为0)。可以先把没被锁的木块放到左上角
        //getValueBlocks(blockList,unlockVec,x,y,valueVec);
        getValueBlocks_lazy(blockList,blockValueList,unlockVec,x,y,valueVec);
        // 在所有影响该位置的木块中，找出能移动的
        //getMoveAbleBlock(room,blockList,valueVec,x,y,moveAbleVec);
        getMoveAbleBlock_lazy(room,blockList,blockPosList,valueVec,x,y,moveAbleVec);
        int n = moveAbleVec.size();
        // 对该位置，根据该位置的值和能移动的方块数，计算所有可以移动哪些个数
        getMoveNums_lazy(sumAtXY, room.mod,n,moveNums);
        
        for(int i = moveNums.size() - 1;i >= 0;--i)
        {
            int m = moveNums[i];
            // 从 vec1 的 n 个中挑 m 个出来。
            getCombis_lazy(moveAbleVec,m,pCombs,pUnusedComb,selectors);
            
            // 对于每种组合里面的方块，移动到下个位置。
            for(int j = 0;j < pCombs.size();)
            {
                Task newTask = task;
                
                // 对有影响的block，加锁
                for(int i = 0;i < valueVec.size();++i)
                {
                    int index = valueVec[i];
                    newTask.vecLock[index] = true;
                }
                // 仅解锁本次移动到下个位置部分的block
                for(int k = 0;k < pCombs[j]->size();++k)
                {
                    int index = (*pCombs[j])[k];
                    // 每个block 移动到新的位置
                    
                    newTask.vecLock[index] = false;
                }
                
                // 方块移到下个位置
                for(int k = 0;k < pCombs[j]->size();++k)
                {
                    int index = (*pCombs[j])[k];
                    Block& block = blockList[index];
                    int posX,posY;
                    
                    /******* lazy **************/
                    const Pos& pos = blockPosList[index][block.y][block.x];
                    posX = pos.first;
                    posY = pos.second;
                    
                    //                    if(!hasNextPos(room,block,posX,posY))
                    //                    {
                    //                        cout << "+++++ error +++++" << endl;
                    //                    }
                    newTask.blocksX[index] = posX;
                    newTask.blocksY[index] = posY;
                    
                }
                // 得到下个位置的 坐标(x1,y1)
                
                int x1,y1;
                room.getNextPos(x, y, x1, y1);
                
                newTask.x = x1;
                newTask.y = y1;
                newTask.number = task.number + 1;
                local_queue.push_back(newTask);
                ++j;
            }
        }
        
        
        // 丢一半给主队列，避免其他队列没事干
        if(Data::queue.empty())
        {
            // 拿出一半
            int halfSize = local_queue.size() / 2;
            vector<Task> vecTmp;
            while(halfSize--)
            {
                vecTmp.push_back(local_queue.back());
                local_queue.pop_back();
            }
            Data::mtx.lock();
            for(int i = 0;i < vecTmp.size();++i)
            {
                Data::queue.push_back(vecTmp[i]);
            }
            Data::mtx.unlock();
        }
        
    }
    
    /************* 保存任务 *********************/
    
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



#endif /* Strategy_h */
