//
//  Tile.cpp
//  FCA
//
//  Created by dayuwater on 14/11/14.
//
//

#include <stdio.h>
#include "PolyModel.h"


int Tile::getAttributes(){
    return attributes;
}

void Tile::setAttributes(int att){
    attributes=att;
}
int Tile::getNext(){
    return next;
}

void Tile::setNext(int n){
    next=n;
}

int Tile::getOccupy(){
    return occupy;
}
void Tile::setOccupy(int oc){
    occupy=oc;
}

int Tile::getOwner(){
    return owner;
}
void Tile::setOwner(int ow){
    owner=ow;
}
