//
//  quadTree.h
//  Robot_Path
//
//  Created by raymond ferranti on 11/4/14.
//  Copyright (c) 2014 raymond ferranti. All rights reserved.
//

#ifndef __Robot_Path__quadTree__
#define __Robot_Path__quadTree__

#include <stdio.h>
#include "Rectangle.h"
#include <vector>
using namespace std;

class quadTree{
    public :
    Rectangle bounds;
    quadTree *nodes[4]; 
    int level;
    char collision;
    bool explored;
    
    void split();
    void containsObstacle( vector<Rectangle> obstacles, int radius);
    void setCollision(char h);
    void setExplored(bool  b);

    
    quadTree(int level, Rectangle bounds);
};

#endif /* defined(__Robot_Path__quadTree__) */
