//
//  Data.h
//  modulo
//
//  Created by mario on 16/3/2.
//  Copyright © 2016年 mario. All rights reserved.
//

#ifndef Data_h
#define Data_h

#include <vector>

#include "Block.h"
#include "Room.h"
#include "Task.h"

using namespace std;

typedef vector<vector<int>> Map;
typedef vector<vector<Map>> MMap;
typedef vector<MMap> BlockValueList ;

typedef pair<int,int> Pos;
typedef vector<vector<Pos>>PosMap;
typedef vector<PosMap> BlockPosList ;


namespace Data
{
    static bool isContinued;    // 是否断线重回
    static int curr_level;      // 当前题目
    static int end_level;       // 最大题目
    static int mod;
    static int threadNum;       // 当前正在跑的线程数
    static vector<thread*> vecThread;
	static atomic<int> cpuMask;		// 绑定线程到指定CPU
    static bool saving;         // 是否保存进度
    static long saveInterval;   // 两次保存的时间间隔 second
    static long saveNums;       // 备份数
    static atomic<int> threadStopCnt;   // 如果线程暂停,等待保存进度，则+1
    static unsigned long long possibility;  // 如果使用穷举的话，需要尝试的次数
    static unsigned long long tryTimes; // 为了性能，不使用原子类型。容忍出错
    static time_t task_beginDate;
    static clock_t task_beginTime;
    
    static bool foundResult;            // 找到结果则为 true
    static Task resultTask;             // 保存最终的结果
    static string strResult;
    static Room room;                   // 原始 Room
    static vector<Block> BlockList;     // 原始 Block
    static vector<Task> queue;          // 全局任务队列
    static vector<vector<int>> taskDetail;  // 任务阶数详情
    static std::mutex mtx;
    
    
    
    // blockList[i][y][x][y1][x1]，blockList[i]在 (x,y)处时在(x1,y1)位置的值(0 or 1)
    static BlockValueList blockValueList;
    // blockList[i][y][x]，blockList[i] 在 (x,y) 初时的下一个位置，不存在时为(-1,-1)
    static BlockPosList blockPosList;
    
    void reset()
    {
//        saveInterval = 300;
        //threadStopCnt = 0;
        isContinued = false;
        saving = false;
        //curr_level = 1;
        end_level = 59;
		cpuMask = 1;
        mod = 0;
        vecThread.clear();
        possibility = 1;
        tryTimes = 0;
        task_beginTime = 0;
        task_beginDate = 0;
        foundResult = false;
        
        resultTask = Task();
        strResult.clear();
        
        room = Room();
        BlockList.clear();
        queue.clear();
        blockValueList.clear();
        blockPosList.clear();
        
        
    }
    
};



#endif /* Data_h */
