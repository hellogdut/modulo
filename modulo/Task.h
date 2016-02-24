//
//  Task.h
//  modulo
//
//  Created by mario on 16/2/24.
//  Copyright © 2016年 mario. All rights reserved.
//

#ifndef Task_h
#define Task_h

#define MAX_BLOCK_NUMS 20
struct Task
{
    int x;
    int y;
    int number;
    
    int blocksX[MAX_BLOCK_NUMS];
    int blocksY[MAX_BLOCK_NUMS];
    int vecLock[MAX_BLOCK_NUMS];
    //vector<int> blocksX;
    //    vector<int> blocksY;
    //    vector<bool> vecLock;
};


#endif /* Task_h */
