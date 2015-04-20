//
//  main.cpp
//  Robot_Path
//
//  Created by raymond ferranti on 10/30/14.
//  Copyright (c) 2014 raymond ferranti. All rights reserved.
//

/* compiles with command line  gcc xlibdemo.c -lX11 -lm -L/usr/X11R6/lib */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <deque>
#include "Rectangle.h"
#include "quadTree.h"

using namespace std;
Display *display_ptr;
Screen *screen_ptr;
int screen_num;
char *display_name = NULL;
unsigned int display_width, display_height;

Window win;
int border_width;
unsigned int win_width, win_height;
int win_x, win_y;

XWMHints *wm_hints;
XClassHint *class_hints;
XSizeHints *size_hints;
XTextProperty win_name, icon_name;
char const *win_name_string = "Example Window";
char const *icon_name_string = "Icon for Example Window";

XEvent report;

GC gc, gc_yellow, gc_red, gc_grey,gc_black, gc_green;
unsigned long valuemask = 0;
XGCValues gc_values, gc_yellow_values, gc_red_values, gc_grey_values, gc_black_values, gc_green_values;
Colormap color_map;
XColor tmp_color1, tmp_color2;


int timesClicked = 0;
int firstClickX, firstClickY, secondClickX, secondClickY,thirdClickX, thirdClickY, radius;
bool flag = true;
int distanceFormula(int x1 , int y1 , int x2 , int y2);
int robotArea;
int orientation(int x1, int y1, int x2, int y2, int x3, int y3);
bool oneSegment(int x1, int y1, int x2, int y2, int x3, int y3);
bool doIntersect(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
void computePathMatrix(vector<quadTree> nodes);
void findPath();

quadTree *startNode = nullptr;
quadTree *endNode = nullptr;
vector<Rectangle> obstacles;
vector<quadTree> stack;
vector<quadTree> redNodes;
vector<quadTree> greenNodes;
vector<quadTree> yellowNodes;
vector<vector<int> > distanceMatrix;
int startNodePosition;
int endNodePosition;





int main(int argc, char **argv)
{
    /* opening display: basic connection to X Server */
    if( (display_ptr = XOpenDisplay(display_name)) == NULL )
    { printf("Could not open display. \n"); exit(-1);}
    printf("Connected to X server  %s\n", XDisplayName(display_name) );
    screen_num = DefaultScreen( display_ptr );
    screen_ptr = DefaultScreenOfDisplay( display_ptr );
    color_map  = XDefaultColormap( display_ptr, screen_num );
    display_width  = 900;
    display_height = 720;
    
    printf("Width %d, Height %d, Screen Number %d\n",
           display_width, display_height, screen_num);
    /* creating the window */
    border_width = 10;
    win_x = 0; win_y = 0;
    win_width = 900;
    win_height = 720;
    
    win= XCreateSimpleWindow( display_ptr, RootWindow( display_ptr, screen_num),
                             win_x, win_y, win_width, win_height, border_width,
                             BlackPixel(display_ptr, screen_num),
                             WhitePixel(display_ptr, screen_num) );
    /* now try to put it on screen, this needs cooperation of window manager */
    size_hints = XAllocSizeHints();
    wm_hints = XAllocWMHints();
    class_hints = XAllocClassHint();
    if( size_hints == NULL || wm_hints == NULL || class_hints == NULL )
    { printf("Error allocating memory for hints. \n"); exit(-1);}
    
    size_hints -> flags = PPosition | PSize | PMinSize  ;
    size_hints -> min_width = 60;
    size_hints -> min_height = 60;
    
    XStringListToTextProperty( (char**)&win_name_string,1,&win_name);
    XStringListToTextProperty( (char**)&icon_name_string,1,&icon_name);
    
    wm_hints -> flags = StateHint | InputHint ;
    wm_hints -> initial_state = NormalState;
    wm_hints -> input = False;
    
    class_hints -> res_name = (char*)"x_use_example";
    class_hints -> res_class =(char*)"examples";
    
    XSetWMProperties( display_ptr, win, &win_name, &icon_name, argv, argc,
                     size_hints, wm_hints, class_hints );
    
    /* what events do we want to receive */
    XSelectInput( display_ptr, win,
                 ExposureMask | StructureNotifyMask | ButtonPressMask );
    
    /* finally: put window on screen */
    XMapWindow( display_ptr, win );
    
    XFlush(display_ptr);
    
    /* create graphics context, so that we may draw in this window */
    gc = XCreateGC( display_ptr, win, valuemask, &gc_values);
    XSetForeground( display_ptr, gc, BlackPixel( display_ptr, screen_num ) );
    XSetLineAttributes( display_ptr, gc, 5, LineSolid, CapRound, JoinRound);
    /* and five other graphics contexts, to draw in yellow, red, grey, black, and green*/
    gc_yellow = XCreateGC( display_ptr, win, valuemask, &gc_yellow_values);
    XSetLineAttributes(display_ptr, gc_yellow, 5, LineSolid,CapRound, JoinRound);
    if( XAllocNamedColor( display_ptr, color_map, "yellow",
                         &tmp_color1, &tmp_color2 ) == 0 )
    {printf("failed to get color yellow\n"); exit(-1);}
    else
        XSetForeground( display_ptr, gc_yellow, tmp_color1.pixel );
    gc_red = XCreateGC( display_ptr, win, valuemask, &gc_red_values);
    XSetLineAttributes( display_ptr, gc_red, 1, LineSolid, CapRound, JoinRound);
    if( XAllocNamedColor( display_ptr, color_map, "red",
                         &tmp_color1, &tmp_color2 ) == 0 )
    {printf("failed to get color red\n"); exit(-1);}
    else
        XSetForeground( display_ptr, gc_red, tmp_color1.pixel );
    gc_grey = XCreateGC( display_ptr, win, valuemask, &gc_grey_values);
    if( XAllocNamedColor( display_ptr, color_map, "light grey",
                         &tmp_color1, &tmp_color2 ) == 0 )
    {printf("failed to get color grey\n"); exit(-1);}
    else
        XSetForeground( display_ptr, gc_grey, tmp_color1.pixel );
    gc_black = XCreateGC( display_ptr, win, valuemask, &gc_black_values);
    XSetLineAttributes(display_ptr, gc_black, 5, LineSolid,CapRound, JoinRound);
    if( XAllocNamedColor( display_ptr, color_map, "black",
                         &tmp_color1, &tmp_color2 ) == 0 )
    {printf("failed to get color black\n"); exit(-1);}
    else
        XSetForeground( display_ptr, gc_black, tmp_color1.pixel );
    gc_green = XCreateGC( display_ptr, win, valuemask, &gc_green_values);
    XSetLineAttributes(display_ptr, gc_green, 5, LineSolid,CapRound, JoinRound);
    if( XAllocNamedColor( display_ptr, color_map, "green",
                         &tmp_color1, &tmp_color2 ) == 0 )
    {printf("failed to get color green\n"); exit(-1);}
    else
        XSetForeground( display_ptr, gc_green, tmp_color1.pixel );
    
    /* and now it starts: the event loop */
    while(1)
    { XNextEvent( display_ptr, &report );
        switch( report.type )
        {
            case Expose:{
                if (flag == true)
                {

                    //(a,b) = bottom left point   (c,d) = top right point
                    // Then (a,d) = top left point   width = c-a    height = b-d
                    int a,b,c,d;
                    int width , height;
                    FILE *fp=fopen("/Users/raymondferranti/Desktop/obstacles_copy","r");
                    char linebuff[32];
                    
                    while(fgets(linebuff, sizeof(linebuff), fp)){
                        
                        sscanf(linebuff,"R (%d,%d) (%d,%d)",&a,&b,&c,&d);
                        if (d>=b) {
                            height = d-b;
                        }
                        else height = b-d;
                        width = c-a;
                        
                        XDrawRectangle(display_ptr, win, gc_black, a, 700-d, width, height);
                        cout<<"("<<a<<","<<700-d<<") "<<width<<" "<<height<<endl;
                        Rectangle tmp (a,700-d,width,height);
                        obstacles.push_back(tmp);
        
                    }
                    flag = false;
                    
                    fclose(fp);

                    }
                }
                    
            
            
                
                break;
            case ConfigureNotify:
                /* This event happens when the user changes the size of the window*/
                win_width = report.xconfigure.width;
                win_height = report.xconfigure.height;
                for (int i = 0; i<obstacles.size(); i++) {
                   XDrawRectangle(display_ptr, win, gc_black, obstacles[i].getX(), obstacles[i].getY(), obstacles[i].getWidth(), obstacles[i].getHeight());

                }
                break;
            case ButtonPress:
                /* This event happens when the user pushes a mouse button. I draw
                 a circle to show the point where it happened, but do not save
                 the position; so when the next redraw event comes, these circles
                 disappear again. */
            {
                int x, y, area;
                if (timesClicked == 0 ) {
                    firstClickX = report.xbutton.x;
                    firstClickY = report.xbutton.y;
                    XDrawLine(display_ptr, win, gc_black, firstClickX, firstClickY,firstClickX, firstClickY);
                }
                if (timesClicked == 1) {
                    secondClickX = report.xbutton.x;
                    secondClickY = report.xbutton.y;
                    XDrawArc(display_ptr, win, gc_black, firstClickX - radius, firstClickY - radius, radius *2, radius*2, 0, 360*64);
                    radius = distanceFormula(firstClickX, firstClickY, secondClickX, secondClickY);
                    robotArea = (3.14) * ((radius) * (radius));
                }
                if (timesClicked == 2 ) {
                    thirdClickX = report.xbutton.x;
                    thirdClickY = report.xbutton.y;
                    

                    quadTree test (0,Rectangle(0+radius,0+radius,display_width-(2*radius),display_height-(2*radius)));
                    stack.push_back(test);
                    while (stack.empty() != true) {
                        quadTree currentNode = stack.back();
                        stack.pop_back();
                       
                        area = currentNode.bounds.area();
                        while (currentNode.bounds.getWidth() ==0 && stack.empty() == false) {
                            currentNode = stack.back();
                            stack.pop_back();
                            area = currentNode.bounds.area();
                        }
                        
                        //See if the current node contains any obstacles. If so, it needs to be subdivided.
                        currentNode.containsObstacle(obstacles, radius);
                        
                        int tmpX = currentNode.bounds.getX();
                        int tmpY = currentNode.bounds.getY();
                        
                        //Find the node that the first click was in. This is the start node.
                        if (firstClickX >= tmpX && firstClickX <= tmpX+currentNode.bounds.getWidth() && firstClickY >= tmpY && firstClickY <= tmpY + currentNode.bounds.getHeight()) {
                            quadTree tmp = currentNode;
                            startNode = &tmp;
                            startNodePosition = greenNodes.size();
                        }
                        //Find the node that the second click was in. This is the finish node.
                        if (thirdClickX >= tmpX && thirdClickX <= tmpX+currentNode.bounds.getWidth() && thirdClickY >= tmpY && thirdClickY <= tmpY + currentNode.bounds.getHeight()) {
                            quadTree tmp = currentNode;
                            endNode = &tmp;
                            endNodePosition = greenNodes.size();
                        }
                        if (currentNode.collision == 'Y' ) {

                            yellowNodes.push_back(currentNode);
                        
                            currentNode.split();
                            stack.push_back(*currentNode.nodes[0]);
                            stack.push_back(*currentNode.nodes[1]);
                            stack.push_back(*currentNode.nodes[2]);
                            stack.push_back(*currentNode.nodes[3]);
                                
                            
                        }
                        else if(currentNode.collision == 'R'){

                             XFillRectangle(display_ptr, win, gc_red,currentNode.bounds.getX(), currentNode.bounds.getY(), currentNode.bounds.getWidth(),                                                                 currentNode.bounds.getHeight());
                            XFlush(display_ptr);
                            redNodes.push_back(currentNode);
                            currentNode.split();
                            
                            
                        }
                        
                        else{
                            greenNodes.push_back(currentNode);
                            XFillRectangle(display_ptr, win, gc_green, currentNode.bounds.getX(), currentNode.bounds.getY(), currentNode.bounds.getWidth(),currentNode.bounds.getHeight());
                            XFlush(display_ptr);
                            
                        }
                    }
                    if(startNode == NULL || endNode == NULL){
                        cout<<"Not a valid start position. Robot starts within obstacle or Robot start partially off grid"<<endl;
                        exit(0);
                    }
                    if ((startNode->collision == 'R' || endNode->collision == 'R')) {
                        cout<<"No Path"<<endl;
                        exit(0);
                    }

                    int n =greenNodes.size()-1;
                    XDrawArc(display_ptr, win, gc_black, firstClickX - radius, firstClickY - radius, radius *2, radius*2, 0, 360*64);
                    XDrawLine(display_ptr, win, gc_black, firstClickX, firstClickY,firstClickX, firstClickY);
                    XDrawLine(display_ptr, win, gc_black, thirdClickX, thirdClickY,thirdClickX, thirdClickY);
                    for (int i = 0; i<obstacles.size(); i++) {
                        XDrawRectangle(display_ptr, win, gc_black, obstacles[i].getX(), obstacles[i].getY(), obstacles[i].getWidth(), obstacles[i].getHeight());
                    }
                    
                    std::swap(greenNodes[0], greenNodes[startNodePosition]);
                    std::swap(greenNodes[greenNodes.size() - 1], greenNodes[endNodePosition]);
                    XDrawRectangle(display_ptr, win, gc_black, greenNodes[0].bounds.getX(), greenNodes[0].bounds.getY(), greenNodes[0].bounds.getWidth(), greenNodes[0].bounds.getHeight());
                    XDrawRectangle(display_ptr, win, gc_black, greenNodes[greenNodes.size() - 1].bounds.getX(), greenNodes[greenNodes.size() - 1].bounds.getY(), greenNodes[greenNodes.size() - 1].bounds.getWidth(), greenNodes[greenNodes.size() - 1].bounds.getHeight());
                    computePathMatrix(greenNodes);
                    findPath();
                    cout<<greenNodes.size()<<endl;
                    

                }
                timesClicked++;
                break;
            default:
                /* this is a catch-all for other events; it does not do anything.
                 One could look at the report type to see what the event was */
                break;
            }
        
        }

    }
}

int distanceFormula(int x1 , int y1 , int x2 , int y2){
    int distance = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
    return distance;
}

int orientation(int x1, int y1, int x2, int y2, int x3, int y3){
    int val = ( (y2 - y1) * (x3 - x2) - (x2 - x1) * (y3 - y2));
    if( val == 0)
        return 0; // If val == 0 then the three points are colinnear.
    
    if ( val > 0){
        return 1;
    }
    else
        return 2;
}

bool onSegment(int x1, int y1, int x2, int y2, int x3, int y3)
{
    if (x2 <= max(x1, x3) && x2 >= min(x1, x3) &&
        y2 <= max(y1, y3) && y2 >= min(y1, y3))
        return true;
    
    return false;
}

bool doIntersect(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
{
    // Find the four orientations needed for general and
    // special cases
    int o1 = orientation(x1, y1, x2, y2, x3, y3);
    int o2 = orientation(x1, y1, x2, y2, x4, y4);
    int o3 = orientation(x3, y3, x4, y4, x1, y1);
    int o4 = orientation(x3, y3, x4, y4, x2, y2);
    
    // General case
    if (o1 != o2 && o3 != o4)
        return true;
    
    // Special Cases
    // p1, q1 and p2 are colinear and p2 lies on segment p1q1
    if (o1 == 0 && onSegment(x1, y1, x3, y3, x2, y2)) return true;
    
    // p1, q1 and p2 are colinear and q2 lies on segment p1q1
    if (o2 == 0 && onSegment(x1, y1, x4, y4, x2, y2)) return true;
    
    // p2, q2 and p1 are colinear and p1 lies on segment p2q2
    if (o3 == 0 && onSegment(x3, y3, x1, y1, x4, y4)) return true;
    
    // p2, q2 and q1 are colinear and q1 lies on segment p2q2
    if (o4 == 0 && onSegment(x3, y3, x2, y2, x4, y4)) return true;
    
    return false; // Doesn't fall in any of the above cases
}

void computePathMatrix(vector<quadTree> nodes){  //If there is a path from node i to node j, push node j onto distanceMatrix[i]
    int i;
    int j;
    int k;
    vector<int> tmp;
    bool flag1 = false;
    bool flag2 = false;
    bool intersects1, intersects2, intersects3, intersects4 ;
    for (i = 0 ; i<nodes.size() ; i++) {
        for (j = 0 ; j<nodes.size() ; j++) {
            if ( i == j ){
                
                
                if (j == nodes.size()-1) {
                    flag1 = true;
                    break;
                }
                else
                    j++;
            }
            intersects1 = false;
            intersects2 = false;
            intersects3 = false;
            intersects4 = false;
            for (k = 0 ; k<obstacles.size(); k++) {
                // The western side of the obstacle.
                intersects1 = doIntersect(nodes[i].bounds.getX(), nodes[i].bounds.getY(),nodes[j].bounds.getX(), nodes[j].bounds.getY(),   obstacles[k].getX()-radius, obstacles[k].getY()-radius, obstacles[k].getX(), obstacles[k].getY()+obstacles[k].getHeight()+radius);
                // The southern side of the obstacle.
                intersects2 = doIntersect(nodes[i].bounds.getX(), nodes[i].bounds.getY(),nodes[j].bounds.getX(), nodes[j].bounds.getY(),   obstacles[k].getX()-radius, obstacles[k].getY()+obstacles[k].getHeight()+radius, obstacles[k].getX()+obstacles[k].getWidth()+radius, obstacles[k].getY()+obstacles[k].getHeight()+radius);
                // The eastern side of the obstacle.
                intersects3 = doIntersect(nodes[i].bounds.getX(), nodes[i].bounds.getY(),nodes[j].bounds.getX(), nodes[j].bounds.getY(),   obstacles[k].getX()+obstacles[k].getWidth()+radius, obstacles[k].getY()-radius, obstacles[k].getX()+obstacles[k].getWidth()+radius, obstacles[k].getY()+obstacles[k].getHeight()+radius);
                // The northern side of the obstacle
                intersects4 = doIntersect(nodes[i].bounds.getX(), nodes[i].bounds.getY(), nodes[j].bounds.getX(), nodes[j].bounds.getY(),   obstacles[k].getX()-radius, obstacles[k].getY()-radius, obstacles[k].getX()+obstacles[k].getWidth()+radius, obstacles[k].getY()-radius);
                
                if (intersects1 == true || intersects2 == true || intersects3 == true || intersects4 == true) {
                    break;
                }
            }
            if (intersects1 == false && intersects2 == false && intersects3 == false && intersects4 == false) {
                tmp.push_back(j); //Put the index of the node that forms a path from i
            }
            
            flag2 = false;
        }
        
        distanceMatrix.push_back(tmp);
        tmp.clear();
        intersects1 = false;
        intersects2 = false;
        intersects3 = false;
        intersects4 = false;
        if (flag1 == true) break;
        
    }
    
}

void findPath(){
    int n = greenNodes.size()-1;
    //Check to see if there is a direct path from the start node to the finish node
    if (distanceMatrix[n][0] == 0) {
        XDrawLine(display_ptr, win, gc_yellow, greenNodes[0].bounds.getX(), greenNodes[0].bounds.getY(), greenNodes[n].bounds.getX(), greenNodes[n].bounds.getY());
        XDrawLine(display_ptr, win, gc_yellow, firstClickX, firstClickY, greenNodes[0].bounds.getX(), greenNodes[0].bounds.getY());
        XDrawLine(display_ptr, win, gc_yellow, thirdClickX, thirdClickY, greenNodes[n].bounds.getX(), greenNodes[n].bounds.getY());

        return;
    }
    bool foundPath = false;
    int position1 = 0;
    int position2 = n;
    bool continueSearching1 = true;
    bool continueSearching2 = true;

    deque<int> path;
    vector<int> node1Neighbors = distanceMatrix[0];
    vector<int> node2Neighbors= distanceMatrix[n];
    vector<int> node1Parents;
    vector<int> node2Parents;
    node1Parents.push_back(position1);
    node2Parents.push_back(position2);

    while (true && (node1Parents.size()!=0 && node2Parents.size() != 0)) {
        for (int i =0; i<node1Neighbors.size(); i++) {
            for (int j = 0; j<node2Neighbors.size(); j++) {
                if (node1Neighbors[i] == node2Neighbors[j]) {
                    if (position1 != 0 && position2 != n) {
                        node1Parents.push_back(position1);
                        node2Parents.push_back(position2);
                    }
                    node1Parents.push_back(node1Neighbors[i]);
                    foundPath = true;
                    goto pathFound;
                    return;
                }
            }
            
        }
        if (foundPath == false) {
            while(continueSearching1 == true && node1Parents.size() != 0){
                    if(position1 != node1Parents.back())
                        node1Parents.push_back(position1);
                    greenNodes[position1].explored = true;
                    for (int i =0; i<node1Neighbors.size(); i++) {
                        if (greenNodes[node1Neighbors[i]].explored != true) {
                            position1 = node1Neighbors[i];
                            node1Neighbors = distanceMatrix[node1Neighbors[i]];
                            continueSearching1 = false;
                            break;
                        }
                    }
                    //If the value of positon1 has not changed, then this nodes children have all been explored, so backtrack;
                    if (node1Parents.size() != 0 && position1 == node1Parents.back()) {
                        cout<<node2Parents[node2Parents.size()-1]<<node2Parents[node2Parents.size()-2]<<flush;
                        node1Parents.pop_back();
                        node1Neighbors = distanceMatrix[node1Parents.back()];
                        position1 = node1Parents.back();
                        
                    }
                
            }
            continueSearching1 = true;
            while(continueSearching2 == true && node2Parents.size() != 0) {
                if(position2 != node2Parents.back())
                    node2Parents.push_back(position2);
                greenNodes[position2].explored = true;
                for (int i =0; i<node2Neighbors.size(); i++) {
                    if (greenNodes[node2Neighbors[i]].explored != true) {
                        position2 = node2Neighbors[i];
                        node2Neighbors = distanceMatrix[node2Neighbors[i]];
                        continueSearching2 = false;
                        break;
                    }
                }
                if (node2Parents.size() != 0 && position2 == node2Parents.back()){
                    node2Parents.pop_back();
                    node2Neighbors = distanceMatrix[node2Parents.back()];
                    position2 = node2Parents.back();
                    
                }
            }
            continueSearching2 = true;
        }
        if (node1Parents.size() == 0 || node2Parents.size() == 0) {
            //No path has been found
            cout<<"No Path"<<endl;
            exit(0);
        }
    }
pathFound:
    for (int i = 0; i<node1Parents.size(); i++) {
        path.push_back(node1Parents[i]);
    }
    for (int i = node2Parents.size()-1; i>=0; i--) {
        path.push_back(node2Parents[i]);
    }
    for (int i = 0; path.size()>1; i++) {
        XDrawLine(display_ptr, win, gc_yellow, greenNodes[path.front()].bounds.getX(), greenNodes[path.front()].bounds.getY(),
                  greenNodes[path[1]].bounds.getX(), greenNodes[path[1]].bounds.getY());
        path.pop_front();
    }
    XDrawLine(display_ptr, win, gc_yellow, firstClickX, firstClickY, greenNodes[0].bounds.getX(), greenNodes[0].bounds.getY());
    XDrawLine(display_ptr, win, gc_yellow, thirdClickX, thirdClickY, greenNodes[n].bounds.getX(), greenNodes[n].bounds.getY());
}

















