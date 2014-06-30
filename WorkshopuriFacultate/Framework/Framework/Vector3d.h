#ifndef CVECTOR3D
#define CVECTOR3D
#include <assert.h>

#include "Matrix4.h"

template <class T>
class cVector3d
{
public:
	union 
	{
		T v[3];
		struct 
		{
			T x,y,z;
		};
	};
	cVector3d():x(0),y(0),z(0){}
	cVector3d(T v[3]):x(v[0]),y(v[1]),z(v[3]){}
	
	cVector3d(cVector3d& f)
	{
		x = f.x; y = f.y; z = f.z;
	}

	cVector3d(cVector3d& f,cVector3d& a)
	{
		x=f.x+a.x;y=f.y+a.y;z=f.z+a.z;
	}

	cVector3d(T x1,T y1 ,T z1):x(x1),y(y1),z(z1){}

	friend bool operator==(const cVector3d& other, const cVector3d& other1)
	{
		return other.x == other1.x && other.y==other1.y && other.z==other1.z;
	}
	
	friend cVector3d operator+(const cVector3d& other, const cVector3d& other1)
	{
		cVector3d t;
		t.x=other1.x+other.x;t.y=other1.y+other.y;t.z=other1.z+other.z;
		return t;
	}

	
	/*void Rotate(cMatrix4<T> mtx)
	{
		x = x*mtx.m[0][0] + y*mtx.m[1][0] + z*mtx.m[2][0];
		y = x*mtx.m[0][1] + y*mtx.m[1][1] + z*mtx.m[2][1];
		z = x*mtx.m[0][2] + y*mtx.m[1][2] + z*mtx.m[2][2];
	}*/

	friend cVector3d operator*(const cVector3d& other, T mul)
	{
		cVector3d t;
		t.x=other.x*mul;t.y=other.y*mul;t.z=other.z*mul;
		return t;
	}
	friend cVector3d operator-(const cVector3d& other, const cVector3d& other1)
	{
		cVector3d t;
		t.x=other.x-other1.x;t.y=other.y-other1.y;t.z=other.z-other1.z;
		return t;
	}

	friend bool operator!=(const cVector3d& other, const cVector3d& other1)
	{
		return !(other.x == other1.x && other.y==other1.y && other.z==other1.z);
	}
	T operator [](unsigned int i)
	{
		assert(i==0||i==1||i==2);
		return v[i];
	}
	void Display()
	{
		std::cout<<x<<" "<<y<<" "<<z<<" \n";
	}
	float Dot(cVector3d<T> yV)
	{
		return x*yV.x+y*yV.y+z*yV.z;
	}
	void Cross(cVector3d<T> &in2)
	{
		cVector3d<T> t(*this);

		x=t.y*in2.z-in2.y*t.z;
		y=t.z*in2.x-in2.z*t.x;
		z=t.x*in2.y-t.x*in2.y;
	}
	void Normalize()
	{
		float len=sqrt(Dot(*this));
		if(len!=0)
		{
			x=x/len;
			y=y/len;
			z=z/len;
		}
	}
	~cVector3d(){}
};
typedef   cVector3d<int> cVector3di;
typedef   cVector3d<float> cVector3df ;
typedef   cVector3d<double> cVector3dd ;
#endif