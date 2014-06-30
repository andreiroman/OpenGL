#ifndef MMATRIX4
#define MMATRIX4
#include <math.h>
#include "Vector3d.h"

#define PI 3.1415926535897932384626433832795f
template <class T>
class cMatrix4
{
public:

	T   m[4][4];

public:
	cMatrix4()
	{
		m[0][0]=0;m[0][1]=0;m[0][2]=0;m[0][3]=0;
		m[1][0]=0;m[1][1]=0;m[1][2]=0;m[1][3]=0;
		m[2][0]=0;m[2][1]=0;m[2][2]=0;m[2][3]=0;
		m[3][0]=0;m[3][1]=0;m[3][2]=0;m[3][3]=0;
	}
	
	cMatrix4(cMatrix4 &ct)
	{
		m[0][0]=ct.m[0][0];m[0][1]=ct.m[0][1];m[0][2]=ct.m[0][2];m[0][3]=ct.m[0][3];
		m[1][0]=ct.m[1][0];m[1][1]=ct.m[1][1];m[1][2]=ct.m[1][2];m[1][3]=ct.m[1][3];
		m[2][0]=ct.m[2][0];m[2][1]=ct.m[2][1];m[2][2]=ct.m[2][2];m[2][3]=ct.m[2][3];
		m[3][0]=ct.m[3][0];m[3][1]=ct.m[3][1];m[3][2]=ct.m[3][2];m[3][3]=ct.m[3][3];
	}

	void Scale(float sx, float sy, float sz)
	{
		m[0][0] *= sx;m[0][1] *= sx;m[0][2] *= sx;m[0][3] *= sx;
		m[1][0] *= sy;m[1][1] *= sy;m[1][2] *= sy;m[1][3] *= sy;
		m[2][0] *= sz;m[2][1] *= sz;m[2][2] *= sz;m[2][3] *= sz;
	}
	
	void Translate(float tx, float ty, float tz)
	{
		m[3][0] += (m[0][0] * tx + m[1][0] * ty + m[2][0] * tz);
		m[3][1] += (m[0][1] * tx + m[1][1] * ty + m[2][1] * tz);
		m[3][2] += (m[0][2] * tx + m[1][2] * ty + m[2][2] * tz);
		m[3][3] += (m[0][3] * tx + m[1][3] * ty + m[2][3] * tz);
	}

	void Rotate(float angle, float x, float y, float z)
	{
		GLfloat sinAngle, cosAngle;
		GLfloat mag = sqrtf(x * x + y * y + z * z);

		sinAngle = sinf ( angle * PI / 180.0f );
		cosAngle = cosf ( angle * PI / 180.0f );
		if ( mag > 0.0f )
		{
			GLfloat xx, yy, zz, xy, yz, zx, xs, ys, zs;
			GLfloat oneMinusCos;
			cMatrix4<T> rotMat;

			x /= mag;
			y /= mag;
			z /= mag;

			xx = x * x;
			yy = y * y;
			zz = z * z;
			xy = x * y;
			yz = y * z;
			zx = z * x;
			xs = x * sinAngle;
			ys = y * sinAngle;
			zs = z * sinAngle;
			oneMinusCos = 1.0f - cosAngle;

			rotMat.m[0][0] = (oneMinusCos * xx) + cosAngle;
			rotMat.m[0][1] = (oneMinusCos * xy) - zs;
			rotMat.m[0][2] = (oneMinusCos * zx) + ys;
			rotMat.m[0][3] = 0.0F; 

			rotMat.m[1][0] = (oneMinusCos * xy) + zs;
			rotMat.m[1][1] = (oneMinusCos * yy) + cosAngle;
			rotMat.m[1][2] = (oneMinusCos * yz) - xs;
			rotMat.m[1][3] = 0.0F;

			rotMat.m[2][0] = (oneMinusCos * zx) - ys;
			rotMat.m[2][1] = (oneMinusCos * yz) + xs;
			rotMat.m[2][2] = (oneMinusCos * zz) + cosAngle;
			rotMat.m[2][3] = 0.0F; 

			rotMat.m[3][0] = 0.0F;
			rotMat.m[3][1] = 0.0F;
			rotMat.m[3][2] = 0.0F;
			rotMat.m[3][3] = 1.0F;
			cMatrix4<T> result=(*this);
			
			result=rotMat*result;
			(*this)=result;
		}
	}

	void InvRotate()
	{
		cMatrix4<T> temp(*this);
		for(int i=0 ; i<3; i++) 
		{
			for(int j=0; j<3; j++) 
			{
			  m[j][i] = temp.m[i][j];
			}
		}
	}

	void Inverse()
	{
		InvRotate();
		cVector3df vTmp, vTmp2;
		vTmp.x = -m[3][0];
		vTmp.y = -m[3][1];
		vTmp.z = -m[3][2];
		vTmp2=vTmp;
		vTmp2.Rotate( (*this));
		m[3][0] = vTmp2.x;
		m[3][1] = vTmp2.y;
		m[3][2] = vTmp2.z;
		m[0][3] = m[1][3] = m[2][3] = 0.0f;
		m[3][3] = 1.0f;
	}

	void Frustum(float left, float right, float bottom, float top, float nearZ, float farZ)
	{
		float       deltaX = right - left;
		float       deltaY = top - bottom;
		float       deltaZ = farZ - nearZ;
		cMatrix4<T>    frust;

		if ( (nearZ <= 0.0f) || (farZ <= 0.0f) ||
			(deltaX <= 0.0f) || (deltaY <= 0.0f) || (deltaZ <= 0.0f) )
			return;

		frust.m[0][0] = 2.0f * nearZ / deltaX;
		frust.m[0][1] = frust.m[0][2] = frust.m[0][3] = 0.0f;

		frust.m[1][1] = 2.0f * nearZ / deltaY;
		frust.m[1][0] = frust.m[1][2] = frust.m[1][3] = 0.0f;

		frust.m[2][0] = (right + left) / deltaX;
		frust.m[2][1] = (top + bottom) / deltaY;
		frust.m[2][2] = -(nearZ + farZ) / deltaZ;
		frust.m[2][3] = -1.0f;

		frust.m[3][2] = -2.0f * nearZ * farZ / deltaZ;
		frust.m[3][0] = frust.m[3][1] = frust.m[3][3] = 0.0f;

		(*this)= frust* (*this);
	}


	void Perspective( float fovy, float aspect, float nearZ, float farZ)
	{
		GLfloat frustumW, frustumH;

		frustumH = tanf( fovy / 360.0f * PI ) * nearZ;
		frustumW = frustumH * aspect;

		Frustum( -frustumW, frustumW, -frustumH, frustumH, nearZ, farZ );
	}

	void Ortho(float left, float right, float bottom, float top, float nearZ, float farZ)
	{
		float       deltaX = right - left;
		float       deltaY = top - bottom;
		float       deltaZ = farZ - nearZ;
		cMatrix4    ortho;

		if ( (deltaX == 0.0f) || (deltaY == 0.0f) || (deltaZ == 0.0f) )
			return;

		ortho.eye();
		ortho.m[0][0] = 2.0f / deltaX;
		ortho.m[3][0] = -(right + left) / deltaX;
		ortho.m[1][1] = 2.0f / deltaY;
		ortho.m[3][1] = -(top + bottom) / deltaY;
		ortho.m[2][2] = -2.0f / deltaZ;
		ortho.m[3][2] = -(nearZ + farZ) / deltaZ;
		(*this)= ortho*(*this);
	}

	friend cMatrix4<T> operator*(const cMatrix4<T>& srcA, const cMatrix4<T>& srcB)
	{
		cMatrix4<T>    tmp;
		int         i;

		for (i=0; i<4; i++)
		{
			tmp.m[i][0] =	(srcA.m[i][0] * srcB.m[0][0]) +
				(srcA.m[i][1] * srcB.m[1][0]) +
				(srcA.m[i][2] * srcB.m[2][0]) +
				(srcA.m[i][3] * srcB.m[3][0]) ;

			tmp.m[i][1] =	(srcA.m[i][0] * srcB.m[0][1]) + 
				(srcA.m[i][1] * srcB.m[1][1]) +
				(srcA.m[i][2] * srcB.m[2][1]) +
				(srcA.m[i][3] * srcB.m[3][1]) ;

			tmp.m[i][2] =	(srcA.m[i][0] * srcB.m[0][2]) + 
				(srcA.m[i][1] * srcB.m[1][2]) +
				(srcA.m[i][2] * srcB.m[2][2]) +
				(srcA.m[i][3] * srcB.m[3][2]) ;

			tmp.m[i][3] =	(srcA.m[i][0] * srcB.m[0][3]) + 
				(srcA.m[i][1] * srcB.m[1][3]) +
				(srcA.m[i][2] * srcB.m[2][3]) +
				(srcA.m[i][3] * srcB.m[3][3]) ;
		}
		return tmp;
	}
	
	void operator= (const cMatrix4<T> &ct)
	{
		m[0][0]=ct.m[0][0];m[0][1]=ct.m[0][1];m[0][2]=ct.m[0][2];m[0][3]=ct.m[0][3];
		m[1][0]=ct.m[1][0];m[1][1]=ct.m[1][1];m[1][2]=ct.m[1][2];m[1][3]=ct.m[1][3];
		m[2][0]=ct.m[2][0];m[2][1]=ct.m[2][1];m[2][2]=ct.m[2][2];m[2][3]=ct.m[2][3];
		m[3][0]=ct.m[3][0];m[3][1]=ct.m[3][1];m[3][2]=ct.m[3][2];m[3][3]=ct.m[3][3];
	}

	void eye()
	{
		m[0][0]=1;m[0][1]=0;m[0][2]=0;m[0][3]=0;
		m[1][0]=0;m[1][1]=1;m[1][2]=0;m[1][3]=0;
		m[2][0]=0;m[2][1]=0;m[2][2]=1;m[2][3]=0;
		m[3][0]=0;m[3][1]=0;m[3][2]=0;m[3][3]=1;
	}
};
typedef cMatrix4<float> cMatrix4f;
#endif