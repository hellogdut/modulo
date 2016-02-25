//
//  Task.h
//  modulo
//
//  Created by mario on 16/2/24.
//  Copyright © 2016年 mario. All rights reserved.
//

#ifndef Task_h
#define Task_h

#define MAX_BLOCK_NUMS 40
struct Task
{
    char x;
    char y;
    char number;
    
    char blocksX[MAX_BLOCK_NUMS];
    char blocksY[MAX_BLOCK_NUMS];
    bool vecLock[MAX_BLOCK_NUMS];
    //vector<int> blocksX;
    //    vector<int> blocksY;
    //    vector<bool> vecLock;
};


#endif /* Task_h */
