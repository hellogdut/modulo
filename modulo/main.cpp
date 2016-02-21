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
        double currPercent = (double)tryTimes * 100 / possibility;
        if(currPercent >= percent +  1.0)
        {
            cout << (int)currPercent << "%," << tryTimes << " / " << possibility << endl;
            percent = currPercent;
        }
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
    while(nextVal > 0)
    {
        if((nextVal% mod) == 0)
        {
            nextVal -= mod;
        }
        else
        {
            int times = nextVal / mod;
            nextVal = times * mod;
        }
        int offset = val - nextVal;
        if(offset > 0)
        {
            if(n >= offset)
            {
                vec.push_back(offset);
            }
            else
            {
                break;
            }
        }
    }
    
    return vec;
}
bool move(Room room,vector<Block>& blockList,int x,int y)
{
    if(x == 0 && y == 0)
    {
        for(int i = 0;i < blockList.size();++i)
        {
            room.add(blockList[i]);
        }
    }
    if(room.isZeroAt(x,y) && room.isZero())
    {
        return true;
    }
    // 是最后一个位置
    if(x == room.m - 1 && y == room.n - 1)
    {
        return false;
    }
    // 找出在该位置上得所有方块
    vector<int> vec1 = getBlockAtPos(blockList,x,y);
    if(vec1.size() == 0)
        return false;
    
    // 找出这些方块中，在 (x,y) 上值不为0的方块
    vec1 = getNonZeroBlock(blockList,vec1,x,y);
    if(vec1.size() == 0)
        return false;
    
    // 找出能移动的方块，既能改变(x,y)值的方块
    vec1 = getMoveAbleBlock(room,blockList,vec1,x,y);
    if(vec1.size() == 0)
        return false;
    
    int n = vec1.size();
    
    // 对该位置，根据该位置的值和能移动的方块数，计算所有可以移动哪些个数
    vector<int> moveNums = getMoveNums(room,x,y,n);
    
    if(moveNums.size() == 0)
        return false;
    
    
    for(int i = 0;i < moveNums.size();++i)
    {
        int m = moveNums[i];
        // 从 vec1 的 n 个中挑 m 个出来。
        vector<vector<int>> combs = getCombis(vec1,m);
        
        // 对于每种组合里面的方块，移动到下个位置。
        for(int j = 0;j < combs.size();++j)
        {
            // 记下这个组合旧的位置
            map<int,pair<int,int>> oldPos;
            
            for(int k = 0;k < combs[j].size();++k)
            {
                // 每个block 移动到新的位置
                Block& block = blockList[combs[j][k]];
                oldPos[combs[j][k]] = make_pair(block.x,block.y);
            }
            
            // 方块移到下个位置
            for(int k = 0;k < combs[j].size();++k)
            {
                Block& block = blockList[combs[j][k]];
                int posX,posY;
                if(!hasNextPos(room,block,posX,posY))
                {
                    cout << "error" << endl;
                }
                
                room.remove(block);
                block.moveTo(posX,posY);
                room.add(block);
                
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
            
            if(!move(room,blockList,x1,y1))
            {
                // 如果没找到，恢复方块的位置
                for(auto it = oldPos.begin(); it != oldPos.end();++it)
                {
                    Block& block = blockList[(*it).first];
                    pair<int,int> pos = (*it).second;
                    room.remove(block);
                    block.moveTo(pos.first,pos.second);
                    room.add(block);
                }
            }
            else
            {
                return true;
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
    
    int level = 0;
    int modu = 0;
    string str;
    string output;
    Room room;
    vector<Block> blockList;
    str = string("{\"level\":8,\"modu\":\"2\",\"map\":[\"110\",\"100\",\"111\"],\"pieces\":[\".X,XX,X.\",\"XXX,..X,..X\",\"X.,XX,.X\",\"X\",\".X,XX\",\"X,X,X\"]}");
    //    str = string("{\"level\":25,\"modu\":\"3\",\"map\":[\"1222\",\"0200\",\"1012\",\"2222\",\"2121\"],\"pieces\":[\".X.,.XX,XXX\",\"X.,XX,X.,X.\",\"XXX,..X\",\".X,XX,X.,XX\",\"XXXX\",\"X.,X.,XX,X.,X.\",\"X..,XXX\",\".X,XX\",\".X,XX,X.\",\"X.,X.,X.,XX,.X\",\"X..,X..,XXX\",\".X,XX,.X\"]}");
    processInput(str,level,modu,room,blockList);
    
    lastTime = clock();
    tryTimes = 0;
    
    
    /// 对 Block 进行排序，面积大的在前面
    
//    std::sort(blockList.begin(),blockList.end(),[](const Block& a,const Block& b){
//        return a.w * a.h > b.w * b.h;
//        //return a.w > b.w;
//    });
    
    calPossibility(room,blockList,possibility);
    cout << "Total: " << possibility << endl;
    
    // 方法1
    //bool suss = calculate(room,blockList);
    // 方法2
    bool suss = move(room,blockList,0,0);
    if(suss)
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
        room.print();
        
        
        for(int i = 0;i < blockList.size();++i)
        {
            blockList[i].print();
            cout << endl;
        }
        
        
        /// 根据 id，排序回来
        std::sort(blockList.begin(),blockList.end(),[](const Block& a,const Block& b){
            return a.id < b.id;
        });
        for(int i = 0;i < blockList.size();++i)
        {
            output += blockList[i].y + '0';
            output += blockList[i].x + '0';
        }
        
        
        cout << "no answer" << endl;
        cout << "Result:    " << output << endl;
        cout << "tryTimes:  " << tryTimes << endl;
        cout << "Time(s):   " << (clock() - lastTime)/CLOCKS_PER_SEC << endl;
    }
    
    return 0;
}
