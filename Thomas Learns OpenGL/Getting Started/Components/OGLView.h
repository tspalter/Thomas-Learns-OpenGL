// OGLView.h : header file
// Implements the OpenGL Window and Inverse Kinematic System
// Learned through Jeff Lander's tutorial and 
// demonstration of inverse kinematics on darwin3d.com

#ifndef OGLVIEW_H
#define OGLVIEW_H

#include <Windows.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>


#include "Skeleton.h"
#include "Point.h"

// OGLView window
class OGLView {
	// ctor
public:
	OGLView();

	// attributes
public:
	Point mMousePos;
	float mGrabRotX, mGrabRotY, mGrabRotZ;
	float mGrabTransX, mGrabTransY, mGrabTransZ;
	bool mDrawGeometry;
	bool mUseQuat;
	bool mDamping;
	bool mDOFRestrict;

	// operations
public:
	void DrawScene(bool actuallyDraw);
	void CreateBoneDLists(tBone* bone);
	void LoadBoneTexture(tBone* curBone, const char* name);
	void DrawModel(tBone* curBone);
	void DrawSkeleton(tBone* rootBone, bool actuallyDraw);
	void InitializeGL(GLsizei width, GLsizei height);
	void Resize(GLsizei width, GLsizei height);
	void SetRestrictions();
	bool ComputeOneCCDLink(Point endPos, int link);
	bool ComputeCCDLink(Point endPos);
	void GetGLInfo();
	void UpdateStatus();
	void HandleKeyUp(unsigned int nChar);
	void HandleKeyDown(unsigned int nChar);

public:
	// virtual bool Create(const char* lpszClassName, const char* lpszWindowName, DWORD dwStyle, const RECT& rect, GLFWwindow* pParentWnd, UINT nID, GLFWwindow* pContext = NULL);
	//}}AFX_VIRTUAL

	// implementation
public:
	virtual ~OGLView();

	// Generated message map functions
protected:
	tBone mLink[6];
	float mModelScale;
	int mWidth, mHeight;

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnDestroy();
	void OnPaint();
	void OnSize(unsigned int nType, int cx, int cy);
	void OnLButtonDown(unsigned int nFlags, Point point);
	void OnRButtonDown(unsigned int nFlags, Point point);
	void OnMouseMove(unsigned int nFlags, Point point);
	void OnLButtonDblClk(unsigned int nFlags, Point point);
	void OnMButtonDown(unsigned int nFlags, Point point);
};

#endif