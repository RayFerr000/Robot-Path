//
//  Rectangle.cpp
//  Robot_Path
//
//  Created by raymond ferranti on 11/4/14.
//  Copyright (c) 2014 raymond ferranti. All rights reserved.
//

#include "Rectangle.h"

int Rectangle :: getX(){
    return x;
}
int Rectangle :: getY(){
    return y;
}


int Rectangle :: getWidth(){
    return width;
}

int Rectangle::getHeight(){
    return height;
}

int Rectangle::area(){
    return height * width;
}

Rectangle::Rectangle(int x, int y, int width , int height){
    this -> x = x;
    this -> y = y;
    this -> width = width;
    this -> height = height;
}
Rectangle::Rectangle(){}