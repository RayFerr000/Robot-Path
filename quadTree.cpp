//
//  quadTree.cpp
//  Robot_Path
//
//  Created by raymond ferranti on 11/4/14.
//  Copyright (c) 2014 raymond ferranti. All rights reserved.
//

#include "quadTree.h"

void quadTree:: split(){
    int childWidth  = (int)( bounds.getWidth() / 2);
    int childHeight = (int)(bounds.getHeight() / 2);
    //Upper right child
    nodes[0] = new quadTree(level + 1 , Rectangle(bounds.getX()+childWidth ,
                                                                bounds.getY() ,childWidth , childHeight));
    
    //Lower left child
    nodes[1] = new quadTree(level + 1 , Rectangle(bounds.getX() , bounds.getY()+     childHeight ,childWidth , childHeight));
    
    //Lower right child
    nodes[2] = new quadTree(level + 1 , Rectangle(bounds.getX()+childWidth , bounds.getY()+childHeight ,childWidth , childHeight));
    
    // Upper left child
    nodes[3] = new quadTree(level + 1, Rectangle(bounds.getX() , bounds.getY() , childWidth, childHeight));
}

quadTree::quadTree(int level , Rectangle bounds){
    this -> level = level;
    this -> bounds = bounds;
}

void quadTree::setCollision(char h) {
    this -> collision = h;
}
void quadTree::setExplored(bool b) {
    this -> explored = b;
}

void quadTree::containsObstacle(vector<Rectangle> obstacles, int radius){
    int Ax1 , Ax2,  Ay1 , Ay2 , Bx1 , Bx2 , By1 , By2, i;
    Ax1 = bounds.getX(); // top left x
    Ax2 = bounds.getX() + bounds.getWidth(); // bottom right x
    Ay1 = bounds.getY(); // top left y
    Ay2 = bounds.getY() + bounds.getHeight(); // bottom right y

    
    for (i = 0; i<obstacles.size(); i++) {
        Bx1 = obstacles[i].getX() - radius;
        Bx2 = obstacles[i].getX() + obstacles[i].getWidth() + radius;
        By1 = obstacles[i].getY() - radius;
        By2 = obstacles[i].getY() + obstacles[i].getHeight() + radius;
        
        if ((Ax1 >= Bx1) && (Ax1 <= Bx2) && (Ax2 >= Bx1) && (Ax2 <= Bx2)
        &&  (Ay1 >= By1) && (Ay1 <= By2) && (Ay2 >= By1) && (Ay2 <= By2)) {
            this->collision = 'R';
            break;
        }

        else if ((Ax1 < Bx2) && (Ax2 > Bx1) && (Ay1 < By2) && (Ay2 > By1)) {
            this->collision = 'Y';
            break; //You only need to detect the presence of one obstacle in order to possibly subdivide the area.
        }
        else
            this->collision = 'G';
        
    }
}




