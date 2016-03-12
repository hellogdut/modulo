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
#include<fstream>
#include <time.h>
#include <deque>
#include <thread>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <stdlib.h>

#include "cJSON.h"
#include "strings.h"

#include "baseMethod.h"
#include "Strategy.h"
#include "Utility.h"
#include <map>

using namespace std;


void threadHelper()
{
#ifdef _WIN32
	// 绑定线程到指定CPU，跳过CPU 0
	Data::cpuMask = Data::cpuMask * 2;
	SetThreadAffinityMask(GetCurrentThread(), Data::cpuMask);
#endif
    move2(Data::room, Data::BlockList, Data::blockValueList, Data::blockPosList);
}



void createThreads()
{
    for(int i =0;i < Data::threadNum;++i)
    {
        thread* pThread = new thread(threadHelper);
        pThread->detach();
        Data::vecThread.push_back(pThread);
    }
}

int main (int argc, const char * argv[])
{

    Data::reset();

    Data::saveNums = 5;			// 保存最近5个备份
    Data::curr_level = 45;      // 从第x题开始
    Data::saveInterval = 120;   // 300秒保存一次
	Data::threadNum = 1;		// 开6个线程
    /******* 先判断是否重回 ************/
    string path = getSavePath();
    
    for(int i = Data::saveNums;i > 0;--i)
    {
        string tmp = path + "_" + to_string(i);
        ifstream fin(tmp);
        if(fin)
        {
            Data::isContinued = true;
            path = tmp;
            fin.close();
            break;
        }
    }

    if(Data::isContinued)
    {
        //readFromDisk(path);
        readFromDisk_rapid(path);
        cout << "===== Continue ====" << endl;
        cout << "tryTime : " << Data::tryTimes << endl;
        cout << "skipCnt : " << Data::skipCnt << endl;
        cout << "thread : " << Data::threadNum << endl;
        cout << "===================" << endl;
        
    }
    /*********************************/

//    string str;
//    str = strings[44];
//    
//    processInput(str,Data::curr_level,Data::mod,Data::room,Data::BlockList);
//    
//    cout << "room" << endl;
//    room.print();
//
//    for(int i = 0;i < Data::BlockList.size();++i)
//    {
//        cout << "block " << i << endl;
//        BlockList[i].print();
//    }
    
    for(int level_it = Data::curr_level - 1;level_it < Data::end_level;++level_it)
    {
        if(!Data::isContinued)
        {
            Data::reset();
        }
        
        string str;
        str = strings[level_it];
        
        processInput(str,Data::curr_level,Data::mod,Data::room,Data::BlockList);
        assert(room.w <= MAX_ROOM_WIDTH && room.h <= MAX_ROOM_HEIGHT);
        
        calPossibility(Data::room,Data::BlockList,Data::possibility);
        calNoneZeroPos(room);
        calMinValue(room);
        // 预先计算Block在每个位置的覆盖范围以及下一个位置的值
        preCalculateBlockValue(Data::room, Data::BlockList, Data::blockValueList,Data::blockPosList);
        
        // 如果是重回，则任务已经设置好了。否则，初始化当前任务
        if(!Data::isContinued)
        {
            initTask();
        }
        else
        {
            calTaskDetail();
        }
        
        createThreads();
        
        // 每隔一段时间报告一次进度
        reportTaskBegin();
        while(!Data::foundResult)
        {
            reportProgress();
            checkSaveProgress();
            mySleep(1);
            
        }
        
        reportTaskFinish();
        saveResult();

        // 重回后如果算出结果，会接着算下个题目
        Data::isContinued = false;
    }

    
    return 0;
}

