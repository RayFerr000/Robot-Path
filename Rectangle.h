//
//  Rectangle.h
//  Robot_Path
//
//  Created by raymond ferranti on 11/4/14.
//  Copyright (c) 2014 raymond ferranti. All rights reserved.
//

#ifndef __Robot_Path__Rectangle__
#define __Robot_Path__Rectangle__

#include <stdio.h>

class Rectangle{
public:
    
    //Stores the values of the left-upper point of a Rectangle
    int x, y, width, height;
   
    int getX();
    
    int getY();
    
    
    Rectangle(int x, int y , int width , int height);
    
    Rectangle();
    
    int getWidth();
    
    int getHeight();
    
    int area();
    
};

#endif /* defined(__Robot_Path__Rectangle__) */
