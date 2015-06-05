/*
 *  PolyModel.cpp
 *
 */

#include "PolyModel.h"


#include "Vec3.h"

#include <vector>
#include <iostream>
#include <sstream>
#include <OpenGL/gl.h>
#include <GLUT/glut.h>

using namespace std;

void PolyModel::moveTo(float a, float b, float c){
    
    
    for(int i=0; i<m_verts.size(); i++){
        m_verts[i].x()+=a-m_center.x();
        m_verts[i].y()+=b-m_center.y();
        m_verts[i].z()+=c-m_center.z();
       
    }
    m_max_bounding.x()+=a-m_center.x();
    m_max_bounding.y()+=b-m_center.y();
    m_max_bounding.z()+=c-m_center.z();
    
    m_min_bounding.x()+=a-m_center.x();
    m_min_bounding.y()+=b-m_center.y();
    m_min_bounding.z()+=c-m_center.z();
    
    m_center=(m_max_bounding+m_min_bounding)/2;
    
    
}

void PolyModel::scale(float f){
    for(int i=0; i<m_verts.size();i++){
        m_verts[i].x()-=m_center.x();
        m_verts[i].y()-=m_center.y();
        m_verts[i].z()-=m_center.z();
        
        m_verts[i].x()+=m_verts[i].x()*f;
        m_verts[i].y()+=m_verts[i].y()*f;
        m_verts[i].z()+=m_verts[i].z()*f;
        
        m_verts[i].x()+=m_center.x();
        m_verts[i].y()+=m_center.y();
        m_verts[i].z()+=m_center.z();
        
        
        
        
    }
    
    m_min_bounding.x()-=m_center.x();
    m_min_bounding.y()-=m_center.y();
    m_min_bounding.z()-=m_center.z();
    
    m_min_bounding.x()+=m_min_bounding.x()*f;
    m_min_bounding.y()+=m_min_bounding.y()*f;
    m_min_bounding.z()+=m_min_bounding.z()*f;
    
    m_min_bounding.x()+=m_center.x();
    m_min_bounding.y()+=m_center.y();
    m_min_bounding.z()+=m_center.z();
    
    
    
    m_max_bounding.x()-=m_center.x();
    m_max_bounding.y()-=m_center.y();
    m_max_bounding.z()-=m_center.z();
    
    m_max_bounding.x()+=m_max_bounding.x()*f;
    m_max_bounding.y()+=m_max_bounding.y()*f;
    m_max_bounding.z()+=m_max_bounding.z()*f;
    
    m_max_bounding.x()+=m_center.x();
    m_max_bounding.y()+=m_center.y();
    m_max_bounding.z()+=m_center.z();
    
    m_center=(m_max_bounding+m_min_bounding)/2;
    
}




void PolyModel::computeFaceNormals()
{
    for (int i=0; i<m_polys.size(); ++i)
    {
        Vec3f vtx0 = m_verts[m_polys[i][0]];
        Vec3f vtx1 = m_verts[m_polys[i][1]];
        Vec3f vtx2 = m_verts[m_polys[i][2]];
        
        Vec3f v1 = vtx1-vtx0;
        Vec3f v2 = vtx2-vtx1;
        Vec3f n = v1.cross(v2);
        
        n.normalize();
        m_face_norms[i] = n;
    }
}

void PolyModel::computeVertexNormals()
{
    for (int i=0; i<m_verts.size(); ++i)
    {
        Vec3f n(0.0f);
        
        for (int j=0; j<m_vertex_polys[i].size(); ++j)
        {
            n += m_face_norms[m_vertex_polys[i][j]];
        }
        
        n.normalize();
        m_norms[i] = n;
    }
}


void PolyModel::draw()
{
	Vec3f centroid(0.0);
	
	glBegin(GL_TRIANGLES);
	for (int i=0; i<m_polys.size(); ++i)
	{
		if (m_polys[i].size() == 3) 
		{
			for (int j=0; j<3; ++j)
			{
				int idx = m_polys[i][j];
				Vec3f v = m_verts[idx];
				glNormal3fv(m_norms[idx].getPtr());
				glVertex3fv(v.getPtr());
			}
		}
	}
	glEnd();
}

void PolyModel::drawTex()
{
    Vec3f centroid(0.0);
    
    glBegin(GL_TRIANGLES);
    glVertexAttrib2f(1, 1.0f, 0.0f);
    
    for (int i=0; i<m_polys.size(); ++i)
    {
        if (m_polys[i].size() == 3)
        {
            for (int j=0; j<3; ++j)
            {
                int idx = m_polys[i][j];
                Vec3f v = m_verts[idx];
                glNormal3fv(m_norms[idx].getPtr());
                glTexCoord2fv(m_tex_coords[idx].getPtr());
                glVertex3fv(v.getPtr());
            }
        }
    }
    glEnd();
}

void PolyModel::roll(float j){
    direction+=j;
    
    
    for(int i=0; i<m_verts.size();i++){
        
        m_verts[i].x()-=m_center.x();
        m_verts[i].y()-=m_center.y();
        m_verts[i].z()-=m_center.z();
        
        float t=m_verts[i].y();
        m_verts[i].y()=m_verts[i].y()*cos(j)+m_verts[i].z()*sin(j);
        m_verts[i].z()=m_verts[i].z()*cos(j)-t*sin(j);
        
        m_verts[i].x()+=m_center.x();
        m_verts[i].y()+=m_center.y();
        m_verts[i].z()+=m_center.z();
        
        
        
        
    }
    
    
    m_min_bounding-=m_center;
    float t=m_min_bounding.x();
    m_min_bounding.x()=m_min_bounding.x()*cos(j)+m_min_bounding.y()*sin(j);
    m_min_bounding.y()=m_min_bounding.y()*cos(j)-t*sin(j);
    m_min_bounding+=m_center;
    
    
    m_max_bounding-=m_center;
    t=m_max_bounding.x();
    m_max_bounding.x()=m_max_bounding.x()*cos(j)+m_max_bounding.y()*sin(j);
    m_max_bounding.y()=m_max_bounding.y()*cos(j)-t*sin(j);
    m_max_bounding+=m_center;
    
    
    
    m_center=(m_min_bounding+m_max_bounding)/2;
    
    
    
    
    
    
    
}

bool PolyModel::loadModel(std::istream& istr)
{	
	int vertex_count;
	int face_count;
	string data;

	
	if (!istr.good())
		return false;
	
    
    char line[1024];
    istr.getline(line, 1024);
    
    std::istringstream sstr(line);
    sstr >> data >> vertex_count >> face_count;
    
    std::string attrib_type;
    bool has_texcoords = false;
    while (!sstr.eof() && sstr.good()) {
        sstr >> attrib_type;
        if (attrib_type == "tex-coords1")
            has_texcoords = true;
    }
	
	m_verts.resize(vertex_count);
	m_norms.resize(vertex_count);
	m_polys.resize(face_count);
	m_face_norms.resize(face_count);
	m_vertex_polys.resize(vertex_count);
    
    m_center = 0.0f;
    m_max_bounding = -numeric_limits<float>::max();
    m_min_bounding = numeric_limits<float>::max();
	
	int count = 0;
	while (istr.good() && count < vertex_count)
	{
		Vec3f v;
		istr >> v.x() >> v.y() >> v.z();
		m_verts[count] = v;
        
        m_min_bounding.x() = std::min(m_min_bounding.x(), v.x());
        m_min_bounding.y() = std::min(m_min_bounding.y(), v.y());
        m_min_bounding.z() = std::min(m_min_bounding.z(), v.z());
        
        m_max_bounding.x() = std::max(m_max_bounding.x(), v.x());
        m_max_bounding.y() = std::max(m_max_bounding.y(), v.y());
        m_max_bounding.z() = std::max(m_max_bounding.z(), v.z());
        
        m_center += v;
		
		++count;
	}
    
    m_center /= (float)count;
	
	count=0;
	while (istr.good() && count < face_count)
	{
		int vert_count;
		istr >> vert_count;
		
		std::vector<int> verts;
		for (int j=0; j<vert_count; ++j)
		{
			int vert_idx;
			istr >> vert_idx;
            verts.push_back(vert_idx);
            m_vertex_polys[vert_idx].push_back(count);
		}
		
		m_polys[count] = verts;
		++count;
	}
    
    
    if (has_texcoords) {
        float numTex;
        m_tex_coords.clear();
    
        for (int k=0 ; k < vertex_count; k++ ) {
            Vec3f t;
        
            istr >> numTex >> t.x() >> t.y();
            m_tex_coords.push_back(t);
        }
    }
	
	computeFaceNormals();
	computeVertexNormals();

    return true;
}
