//
//  Utility.h
//  modulo
//
//  Created by mario on 16/3/2.
//  Copyright © 2016年 mario. All rights reserved.
//

#ifndef Utility_h
#define Utility_h

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#endif
#include "Data.h"
#include "baseMethod.h"
#include "Strategy.h"
#include "Utility.h"
#include <stdlib.h>
using namespace Data;

void mySleep(int millSecond)
{
#ifdef _WIN32
    Sleep(millSecond);
#else
    sleep(millSecond);
#endif
}



void sendMail(int level,string result,long tryTimes,int second)
{
    
}
void calPossibility(Room& room,vector<Block>& blockList,unsigned long long& possibility)
{
    for(int i = 0;i < blockList.size();++i)
    {
        const Block& block = blockList[i];
        long rightStep = room.w - block.w + 1;
        long downStep = room.h - block.h + 1;
        
        possibility *= rightStep * downStep;
    }
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
void reportTaskBegin()
{
    time(&Data::task_beginDate);
    
    cout << "Level : " << Data::curr_level << endl;
    cout << "Total: " << Data::possibility << endl;
    cout << "Begin Time :" << ctime(&Data::task_beginDate);
}
void reportTaskFinish()
{
    if(Data::foundResult)
    {
        for(int i = 0;i < Data::BlockList.size();++i)
        {
            Data::strResult += Data::resultTask.blocksY[i] + '0';
            Data::strResult += Data::resultTask.blocksX[i] + '0';
        }
    }
    else
    {
        Data::strResult = "XXXXXXXXXXXXXXXXXXXXXX";
    }
    
    time_t endTime;
    time(&endTime);
    
    long second = (clock() - Data::task_beginTime)/CLOCKS_PER_SEC;
    
    cout << "Result:    " << Data::strResult << endl;
    cout << "tryTimes:  " << Data::tryTimes << endl;
    cout << "Time(s):   " << second << endl;
    cout << "Speed:     " << Data::tryTimes / max((long)1,second) << endl;
    cout << "Begin Date :" << ctime(&Data::task_beginDate);
    cout << "End Date :" << ctime(&endTime);
    cout << endl;
    
}
void saveResult()
{
    // 写入文件
    ofstream outdata;
    string fileName = std::to_string(Data::curr_level + 1) + ".txt";
    outdata.open(fileName,ios::app);//ios::app是尾部追加的意思
    
    time_t endTime;
    time(&endTime);
    long second = (clock() - Data::task_beginTime)/CLOCKS_PER_SEC;
    
    outdata << "Level : " << Data::curr_level << endl;
    outdata << "Total: " << Data::possibility << endl;
    outdata << "Result:    " << Data::strResult << endl;
    outdata << "tryTimes:  " << Data::tryTimes << endl;
    outdata << "Time(s):   " << (clock() - Data::task_beginTime)/CLOCKS_PER_SEC << endl;
    outdata << "Speed:     " << Data::tryTimes / max((long)1,second) << endl;
    outdata << "Begin Date :" << ctime(&Data::task_beginDate);
    outdata << "End Date :" << ctime(&endTime);
    outdata.close();
    
}
void reportProgress()
{
    static clock_t lastReportTime = clock();
    
    mySleep(1);
    long currReportTime = clock();
    long interval = (currReportTime - lastReportTime)/CLOCKS_PER_SEC;
    
    // 60秒报告一次
    if(interval > 60)
    {
        lastReportTime = currReportTime;
        long speedTime = (clock() - Data::task_beginTime)/CLOCKS_PER_SEC;
        cout << "tryTimes:" << Data::tryTimes << " ,speed: " << Data::tryTimes/speedTime << " ,time: " << speedTime << endl;
    }
}
void initTask()
{
    Task task;
    for(int i = 0;i < Data::BlockList.size();++i)
    {
        task.x = 0;
        task.y = 0;
        task.number = 1;
        task.blocksX[i] = 0;
        task.blocksY[i] = 0;
        task.vecLock[i] = false;
    }
    Data::queue.push_back(task);
}
void calTaskDetail()
{
    // 计算当前任务的阶数情况
    
    vector<int> row(room.w,0);
    vector<vector<int>> detail(room.h,row);
    detail.swap(Data::taskDetail);
    
    std::sort(Data::queue.begin(),Data::queue.end(),[](const Task& a,const Task& b){return a.number < b.number;});
    
    std::map<int,int> mapCnt;
    cout << "==========================" << endl;
    for(int i = 0;i < queue.size();++i)
    {
        if(mapCnt.find(queue[i].number) == mapCnt.end())
        {
            mapCnt[queue[i].number] = 0;
        }
        else
        {
            mapCnt[queue[i].number]++;
        }
    }
    cout << "==========================" << endl;
    for(auto it = mapCnt.begin();it != mapCnt.end();++it)
    {
        int y = (it->first - 1) / room.w;
        int x = (it->first - room.w * y) - 1;
        taskDetail[y][x] = it->second;
    }
    
    for(int y = 0;y < taskDetail.size();++y)
    {
        for(int x = 0;x < taskDetail[0].size();++x)
        {
            cout << taskDetail[y][x] << " ";
        }
        cout << endl;
    }
    
}

void saveTaskToDisk(string filePath)
{
    Data::mtx.lock();
    time_t currTime;
    time(&currTime);
    char* time = ctime(&currTime);
    cout << "=======Save===== " << time << endl;
    
    cJSON *root;
    root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "time",cJSON_CreateString(time));
    cJSON_AddItemToObject(root, "level",cJSON_CreateNumber(Data::curr_level));
    cJSON_AddItemToObject(root, "threadNum",cJSON_CreateNumber(Data::threadNum));
	cJSON_AddItemToObject(root, "tryTimes", cJSON_CreateString(to_string(Data::tryTimes).c_str()));
    cJSON_AddItemToObject(root, "blockNums",cJSON_CreateNumber(Data::BlockList.size()));
    cJSON_AddItemToObject(root, "taskNums",cJSON_CreateNumber(Data::queue.size()));
    cJSON *taskList = cJSON_CreateArray();
    cJSON_AddItemToObject(root, "Tasks",taskList);
    
    for(int i = 0;i < Data::queue.size();++i)
    {
        cJSON *jsonTask = cJSON_CreateObject();
        
        Task task = Data::queue[i];
        
        cJSON_AddItemToObject(jsonTask, "x",cJSON_CreateNumber(task.x));
        cJSON_AddItemToObject(jsonTask, "y",cJSON_CreateNumber(task.y));
        cJSON_AddItemToObject(jsonTask, "number",cJSON_CreateNumber(task.number));
        
        
        
        cJSON *vecX,*vecY,*vecLock;
        int arrX[MAX_BLOCK_NUMS];
        int arrY[MAX_BLOCK_NUMS];
        int arrLock[MAX_BLOCK_NUMS];
        for(int j = 0;j < BlockList.size();++j)
        {
            arrX[j] = (int)task.blocksX[j];
            arrY[j] = (int)task.blocksY[j];
            arrLock[j] = task.vecLock[j];
        }
        
        vecX = cJSON_CreateIntArray(arrX,BlockList.size());
        vecY = cJSON_CreateIntArray(arrY,BlockList.size());
        vecLock = cJSON_CreateIntArray(arrLock,BlockList.size());
        
        cJSON_AddItemToObject(jsonTask, "blocksX",vecX);
        cJSON_AddItemToObject(jsonTask, "blocksY",vecY);
        cJSON_AddItemToObject(jsonTask, "vecLock",vecLock);
        
        cJSON_AddItemToArray(taskList,jsonTask);
    }
    Data::mtx.unlock();
    string str = cJSON_Print(root);
    
    static long sufix = 1;
    string tmp = filePath + "_" + to_string(sufix);
    ofstream out(tmp,std::ofstream::out);
    
    if(!out.is_open())
    {
        cout << "saveTaskToDisk write error!" << endl;
        return;
    }
    else
    {
        out << str;
    }
    if(sufix < Data::saveNums)
    {
        sufix++;
    }
    else
    {
        sufix = 1;
    }
    
    calTaskDetail();
}
void readFromDisk(string filePath)
{
    ifstream in(filePath, ios::in);
    if(!in.is_open())
    {
        cout << "readFromDisk fail" << endl;
        return;
    }
    istreambuf_iterator<char> beg(in), end;
    string strContent(beg, end);
    in.close();
    
    const char* my_json_string = strContent.c_str();
    cJSON *root = cJSON_Parse(my_json_string);
    
    Data::curr_level = cJSON_GetObjectItem(root,"level")->valueint;
    Data::threadNum = cJSON_GetObjectItem(root,"threadNum")->valueint;
#ifdef WIN32
	Data::tryTimes = _atoi64(cJSON_GetObjectItem(root, "tryTimes")->valuestring);
#else
    Data::tryTimes = std::atoll(cJSON_GetObjectItem(root, "tryTimes")->valuestring);
#endif
    int blockNums = cJSON_GetObjectItem(root,"blockNums")->valueint;
   
    vector<Task> vecTask;

    
    cJSON * taskList = cJSON_GetObjectItem(root,"Tasks");
    int size = cJSON_GetArraySize(taskList);
    
    for(int i =0;i < size;++i)
    {
        Task task;
        
        cJSON* item = cJSON_GetArrayItem(taskList, i);
        task.x = cJSON_GetObjectItem(item,"x")->valueint;
        task.y = cJSON_GetObjectItem(item,"y")->valueint;
        task.number = cJSON_GetObjectItem(item,"number")->valueint;
        
        cJSON* blockX = cJSON_GetObjectItem(item,"blocksX");
        cJSON* blockY = cJSON_GetObjectItem(item,"blocksY");
        cJSON* vecLock = cJSON_GetObjectItem(item,"vecLock");
        for(int j = 0;j < blockNums;++j)
        {
            task.blocksX[j] = cJSON_GetArrayItem(blockX, j)->valueint;
            task.blocksY[j] = cJSON_GetArrayItem(blockY, j)->valueint;
            task.vecLock[j] = cJSON_GetArrayItem(vecLock, j)->valueint;
        }
        vecTask.push_back(task);
    }
    // 阶数小的排在前面
    std::sort(vecTask.begin(),vecTask.end(),[](const Task& a,const Task& b){return a.number < b.number;});
    Data::queue.swap(vecTask);
    
}

string getSavePath()
{
    char buffer[256];
#ifdef _WIN32
	_getcwd(buffer, 256);
#else
	getcwd(buffer, 256);
#endif
    string path = buffer;
    if(path.back() != '/')
    {
        path.push_back('/');
    }
    path += "modolu.json";
    
    return path;
}
void checkSaveProgress()
{
    static clock_t lastSaveTime = clock();
    mySleep(1);
    long currReportTime = clock();
    long interval = (currReportTime - lastSaveTime)/CLOCKS_PER_SEC;
    
    // 60秒报告一次
    if(interval > Data::saveInterval)
    {
        lastSaveTime = currReportTime;
        Data::saving = true;
        Data::threadStopCnt = 0;
        while(Data::threadStopCnt < Data::threadNum)
        {
            // 等待线程暂停
            mySleep(1);
        }
        saveTaskToDisk(getSavePath());
        Data::saving = false;
        
    }
}

#endif /* Utility_h */
