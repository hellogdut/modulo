//
//  Task.h
//  modulo
//
//  Created by mario on 16/2/24.
//  Copyright © 2016年 mario. All rights reserved.
//

#ifndef Task_h
#define Task_h

#include "Block.h"
#define MAX_ROOM_WIDTH  9   //8
#define MAX_ROOM_HEIGHT 12  //10
struct Task
{
    int level[MAX_ROOM_WIDTH*MAX_ROOM_HEIGHT];
    char blocksX[MAX_BLOCK_NUMS];
    char blocksY[MAX_BLOCK_NUMS];
    char vecLock[MAX_BLOCK_NUMS];
    char x;
    char y;
    char number;

    
    
};


#endif /* Task_h */
