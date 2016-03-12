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
#include "../rapidjson/document.h"
#include "../rapidjson/stringbuffer.h"
#include "../rapidjson/writer.h"

using namespace Data;
using namespace rapidjson;

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
void calMinValue(Room& room)
{
    int mod = room.mod;
    
    Data::minValueFromPos.clear();
    for(int y = 0;y < room.h;y++)
    {
        vector<int> row(room.w,0);
        Data::minValueFromPos.push_back(row);
    }
    
    // 迭代计算
    for(int y = room.h - 1;y >= 0;--y)
    {
        for(int x = room.w - 1;x >= 0;--x)
        {
            // 到最近 mod 值的最小增量
            int val = 0;
            if(room.room[y][x] % mod)
            {
                int time = room.room[y][x] / mod;
                val = (time + 1) * mod - room.room[y][x];
            }
            // 加上下一个位置的增量
            int x1,y1;
            bool hasNextPos = room.getNextPos(x,y,x1,y1);
            if(hasNextPos)
            {
                val += Data::minValueFromPos[y1][x1];
            }
            Data::minValueFromPos[y][x] = val;
        }
    }
}
void calNoneZeroPos(Room& room)
{
    Data::noneZeroPosOfRoom.clear();
    for(int y = room.h - 1;y >= 0;--y)
    {
        for(int x = room.w - 1;x >= 0;--x)
        {
            if((room.room[y][x] % room.mod) != 0)
            {
                Data::noneZeroPosOfRoom.push_back(make_pair(x,y));
            }
        }
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
    if(root)
    {
        cJSON_Delete(root);
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
    cout << "skipCnt : " << Data::skipCnt << endl;
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
    string fileName = std::to_string(Data::curr_level) + ".txt";
    outdata.open(fileName,ios::app);//ios::app是尾部追加的意思
    
    time_t endTime;
    time(&endTime);
    long second = (clock() - Data::task_beginTime)/CLOCKS_PER_SEC;
    
    outdata << "Level : " << Data::curr_level << endl;
    outdata << "Total: " << Data::possibility << endl;
    outdata << "Result:    " << Data::strResult << endl;
    outdata << "tryTimes:  " << Data::tryTimes << endl;
    cout << "skipCnt : " << Data::skipCnt << endl;
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
        cout << "tryTimes:" << Data::tryTimes << " ,TrySpeed: " << Data::tryTimes/speedTime << "skipCnt :" << Data::skipCnt << " ,SkipSpeed: " << Data::skipCnt/speedTime << " ,time: " << speedTime << endl;
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
    for(int i = 0;i < Data::room.h * Data::room.w;++i)
    {
        task.level[i] = 0;
    }
    Data::queue.push_back(task);
}
int getUnlockBlockCnt(const Task& a)
{
    int cnt = 0;
    for(int i = 0; i < Data::BlockList.size();i++)
    {
        if(!a.vecLock[i])
            cnt++;
    }
    return cnt;
}
void calTaskDetail()
{
    // 计算当前任务的阶数情况
    
    vector<int> row(room.w,0);
    vector<vector<int>> detail(room.h,row);
    detail.swap(Data::taskDetail);
    
    std::sort(Data::queue.begin(),Data::queue.end(),[](const Task& a,const Task& b){
//        return a.number < b.number;
        // 从优先级低到高排序,如 0,6,2 < 0,6,1
        int minNum = min(a.number,b.number);
        for(int i = 0;i <= minNum;++i)
        {
            if(a.level[i] > b.level[i])
                return true;
        }
        
        return a.number < b.number;
    });
    
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
    
    // 尝试优化思路：放开更多的block流入到下一级
    std::sort(Data::queue.begin(),Data::queue.end(),[](const Task& a,const Task& b){
        return a.number < b.number || (a.number == b.number && getUnlockBlockCnt(a) < getUnlockBlockCnt(b));
    });
    
    
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
    char* strJson = cJSON_Print(root);
    string str(strJson);
    if(strJson)
    {
        free(strJson);
    }
    if(root)
    {
        cJSON_Delete(root);
    }
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

void saveTaskToDisk_rapid(string filePath)
{
    Data::mtx.lock();
    time_t currTime;
    time(&currTime);
    char* cStrTime = ctime(&currTime);
    cout << "=======Begin Save===== " << cStrTime << endl;
    
    rapidjson::Document document;
    Document::AllocatorType& allocator = document.GetAllocator();
    std::string str = "{}"; // 这个是必须的，且不能为 ""，否则 Parse 出错
    document.Parse(str.c_str());
    
    Value jsonTime;
    jsonTime.SetString(cStrTime,strlen(cStrTime),document.GetAllocator());
    document.AddMember("time", jsonTime, document.GetAllocator());
    document.AddMember("level", Data::curr_level, document.GetAllocator());
    document.AddMember("threadNum", Data::threadNum, document.GetAllocator());
    
    Value jsonTryTime;
    string strTryTime = to_string(Data::tryTimes);
    jsonTryTime.SetString(strTryTime.c_str(),strTryTime.size(),document.GetAllocator());
    
    int blockNums = Data::BlockList.size();
    int taskNums = Data::queue.size();
    document.AddMember("tryTimes", jsonTryTime, document.GetAllocator());
    document.AddMember("blockNums", blockNums, document.GetAllocator());
    document.AddMember("taskNums", taskNums, document.GetAllocator());
    
    Value taskList(kArrayType);
    for(int i = 0;i < Data::queue.size();++i)
    {
        Task& task = Data::queue[i];
        Value jsonTask(kObjectType);
        
        jsonTask.AddMember("x",task.x,document.GetAllocator());
        jsonTask.AddMember("y",task.y,document.GetAllocator());
        jsonTask.AddMember("number",task.number,document.GetAllocator());
        
        Value blocksX(kArrayType);
        Value blocksY(kArrayType);
        Value vecLock(kArrayType);
        Value vecLevel(kArrayType);
        
        for(int j = 0;j < Data::BlockList.size();++j)
        {
            blocksX.PushBack(task.blocksX[j], allocator);
            blocksY.PushBack(task.blocksY[j], allocator);
            vecLock.PushBack(task.vecLock[j], allocator);
        }
        for(int j = 0;j < MAX_ROOM_HEIGHT * MAX_ROOM_WIDTH;++j)
        {
            vecLevel.PushBack(task.level[j], allocator);
        }
        jsonTask.AddMember("blocksX",blocksX,allocator);
        jsonTask.AddMember("blocksY",blocksY,allocator);
        jsonTask.AddMember("vecLock",vecLock,allocator);
         jsonTask.AddMember("vecLevel",vecLevel,allocator);
        taskList.PushBack(jsonTask,allocator);
    }
    
    
    document.AddMember("Tasks", taskList, document.GetAllocator());
    
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<StringBuffer> writer(buffer);
    document.Accept(writer);
    
    const char* output = buffer.GetString();
    string strOut(output);
    
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
        out << strOut;
    }
    if(sufix < Data::saveNums)
    {
        sufix++;
    }
    else
    {
        sufix = 1;
    }
    
    time_t currTime2;
    time(&currTime2);
    char* cStrTime2 = ctime(&currTime2);
    cout << "=======End Save===== " << cStrTime2 << endl;

    
    calTaskDetail();
    Data::mtx.unlock();
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
    if(root)
    {
        cJSON_Delete(root);
    }
    
}
void readFromDisk_rapid(string filePath)
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
    
    Document document;
    document.Parse(strContent.c_str());
    Data::curr_level = document["level"].GetInt();
    Data::threadNum = document["threadNum"].GetInt();
#ifdef WIN32
    Data::tryTimes = _atoi64(document["tryTimes"].GetString());
#else
    Data::tryTimes = std::atoll(document["tryTimes"].GetString());
#endif
    
    int blockNums = document["blockNums"].GetInt();
    int taskNums = document["taskNums"].GetInt();
    
    vector<Task> vecTask;
    
    const Value& taskList = document["Tasks"];
    for (SizeType i = 0; i < taskList.Size(); i++) // 使用 SizeType 而不是 size_t
    {
        Task task;
        
        const Value& dictTask = taskList[i];
        task.x = dictTask["x"].GetInt();
        task.y = dictTask["y"].GetInt();;
        task.number = dictTask["number"].GetInt();
        
        const Value& vecBlockX = dictTask["blocksX"];
        const Value& vecBlockY = dictTask["blocksY"];
        const Value& vecLock = dictTask["vecLock"];
        const Value& vecLevel = dictTask["vecLevel"];
        
        for(int j = 0;j < blockNums;++j)
        {
            task.blocksX[j] = vecBlockX[j].GetInt();
            task.blocksY[j] = vecBlockY[j].GetInt();
            task.vecLock[j] = vecLock[j].GetInt();
        }
        for(int j = 0;j < MAX_ROOM_WIDTH * MAX_ROOM_HEIGHT;++j)
        {
            task.level[j] = vecLevel[j].GetInt();
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
        //saveTaskToDisk(getSavePath());
        saveTaskToDisk_rapid(getSavePath());
        Data::saving = false;
        
    }
}

#endif /* Utility_h */
