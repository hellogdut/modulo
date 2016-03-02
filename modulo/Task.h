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

struct Task
{
    char x;
    char y;
    char number;
    
    char blocksX[MAX_BLOCK_NUMS];
    char blocksY[MAX_BLOCK_NUMS];
    bool vecLock[MAX_BLOCK_NUMS];
};


#endif /* Task_h */
