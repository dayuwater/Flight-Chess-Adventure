/*
 *  PolyModel.h
 *
 */

#pragma once

#include "Vec3.h"

#include <vector>
#include <list>
#include <iostream>

#include <iostream>
// the general model class
class PolyModel
{
public:
	PolyModel(){}
	PolyModel(std::istream& istr);
    void moveTo(float a, float b, float c);
    void scale(float f);
	void draw();
    void drawTex();
    void roll(float r);
    
	void computeFaceNormals();
	void computeVertexNormals();
	
    bool loadModel(std::istream& istr);
    
    Vec3f getCenter() const { return m_center; }
    Vec3f getMinBounding() const { return m_min_bounding; }
    Vec3f getMaxBounding() const { return m_max_bounding; }

    
    std::vector<Vec3f>& getVertices() { return m_verts; }
	std::vector<std::vector<int> >& getPolygons() { return m_polys; }

protected:
	std::vector<Vec3f> m_verts;
	std::vector<Vec3f> m_norms;
	std::vector<Vec3f> m_face_norms;
    std::vector<Vec3f> m_tex_coords;
    
	std::vector<std::vector<int> > m_vertex_polys;
	std::vector<std::vector<int> > m_polys;  
    float direction=0.0f;
    Vec3f m_center;
    Vec3f m_min_bounding;
    Vec3f m_max_bounding;
};
// the tile class, inherited from the general model class
class Tile : public PolyModel{
public:
    int getOwner();
    void setOwner(int ow);
    
    int getOccupy();
    void setOccupy(int oc);
    
    int getNext();
    void setNext(int n);
    
    int getAttributes();
    void setAttributes(int att);
    
    
protected:
    int owner; // the color of the tile. 4=yellow 1=red 2=green 3=blue
    int occupy; // indicate who is standing on the tile -1=enemy 0=unoccupied 4=yellow 1=red 2=green 3=blue
    int next; // indicate which tile a chess piece should go in normal situation, used by the number of the tile
    int attributes; // 0=normal tile, 1=outsidetower entrance tile, 2=outsidetower tile, 3=flying path entrance tile 4=insidetower entrance tile 5=insidetower tile
    
};
// the chess piece class, inherited from the general model class
class Chess : public PolyModel{
public:
    int getIdentity(){return identity;}
    void setIdentity(int i){identity=i;}
    int getPosition(){return pos;}
    void setPosition(int p){pos=p;} // -6=dead , -1=not start yet
    void setOrder(){
       
        std::vector<int> nums;
        nums.resize(9);
        
        for(int i=0; i<nums.size(); i++){
            nums[i]=i;
        }
        for(int j=0; j<9; j++){
            int r=rand()%(9-j);
            order.push_back(nums[r]);
            for(int i=r; i<nums.size()-1; i++){
                nums[i]=nums[i+1];
            }
            nums.resize(9-j-1);
        
        }
        
        horv=rand()%2;
        
    
        
    }
    // only use for debug
    void printOrder(){
        std::cout<< std::endl;
        for(int i=0; i<9; i++){
            int t=order.front();
            std::cout << t <<" ,";
            
            order.pop_front();
            order.push_back(t);
        }
    }
    int getOrder(){
        int result=order.front();
        order.pop_front();
        order.push_back(result);
        return result;
        
        
    }
    int getHorv(){
        return horv;
    }
    void wo(){
        won=true;
    }
    bool getWon(){
        return won;
    }
protected:
    int identity; // -1=enemy 4=yellow 1=red 2=green 3=blue
    int pos; //position
    int horv; // 0 if move horizontally, 1 if move vertically
    bool won=false;
    std::list<int> order;
};

