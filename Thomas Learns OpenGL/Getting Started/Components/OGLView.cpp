// OGLView.cpp : source file
// Implements the OpenGL Window and Inverse Kinematic System
// Learned through Jeff Lander's tutorial and 
// demonstration of inverse kinematics on darwin3d.com

#include <math.h>
#include "MathFuncs.h"
#include "OGLView.h"
#include "Quaternion.h"
#include "SnakeModel.h"  // SoftImage Model Data
#include "Bitmap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Application Definitions
#define OGL_AXIS_DLIST	99		// OpenGL display list ID
#define ROTATE_SPEED	1.0		// Speed of rotation
#define EFFECTOR_POS	5		// This chain has 5 links
#define MAX_IK_TRIES	100		// Times through the CCD loop (Tries = # / Links)
#define IK_POS_THRESH	1.0f	// Threshold for success

OGLView::OGLView() {
	// Initialize the mode keys
	mDrawGeometry = TRUE;

	// Initialize some of the skeleton vars
	ResetBone(&mLink[0], NULL);
	mLink[0].id = 1;
	mLink[0].trans.x = 4.8f;
	mLink[0].trans.y = 6.0f;
	strcpy(mLink[0].name, "Base");
	mLink[0].childCount = 1;
	mLink[0].children = &mLink[1];

	ResetBone(&mLink[1], NULL);
	mLink[1].id = 2;
	mLink[1].trans.y = -1.0f;
	strcpy(mLink[1].name, "Link1");
	mLink[1].childCount = 1;
	mLink[1].children = &mLink[2];

	ResetBone(&mLink[2], NULL);
	mLink[2].id = 3;
	mLink[2].trans.y = -1.0f;
	strcpy(mLink[2].name, "Link2");
	mLink[2].childCount = 1;
	mLink[2].children = &mLink[3];

	ResetBone(&mLink[3], NULL);
	mLink[3].id = 4;
	mLink[3].trans.y = -1.0f;
	strcpy(mLink[3].name, "Link3");
	mLink[3].childCount = 1;
	mLink[3].children = &mLink[4];

	ResetBone(&mLink[4], NULL);
	mLink[4].id = 5;
	mLink[4].trans.y = -1.0f;
	strcpy(mLink[4].name, "Link4");
	mLink[4].childCount = 1;
	mLink[4].children = &mLink[5];

	// Set up end effector
	ResetBone(&mLink[5], NULL);
	mLink[5].id = 6;
	mLink[5].trans.y = -1.0f;
	strcpy(mLink[5].name, "Effector");

	// Set up default settings for the damping for six joints
	mLink[0].dampWidth = 10.0f;
	mLink[1].dampWidth = 10.0f;
	mLink[2].dampWidth = 10.0f;
	mLink[3].dampWidth = 10.0f;
	mLink[4].dampWidth = 10.0f;
	mLink[5].dampWidth = 10.0f;  // End effector, so it isn't really used

	// Set up default setting for the depth of field restrictions
	mLink[0].minRz = -30;
	mLink[1].minRz = -30;
	mLink[2].minRz = -30;
	mLink[3].minRz = -30;
	mLink[4].minRz = -30;
	mLink[5].minRz = -30;  // End effector, so it isn't really used

	mLink[0].maxRz = 30;
	mLink[1].maxRz = 30;
	mLink[2].maxRz = 30;
	mLink[3].maxRz = 30;
	mLink[4].maxRz = 30;
	mLink[5].maxRz = 30;  // End effector, so it isn't really used

	// by default, we don't want any damping or depth of field restriction
	mDamping = FALSE;
	mDOFRestrict = FALSE;
}

OGLView::~OGLView() {

}

// UpdateStatus()
// Update the status bar with orientation info
void OGLView::UpdateStatus() {
	// local vars
	char message[80];

	// write the orientations of the bones in the window status bar
	/*printf(message, "Joint Rot Values (%.2f,%.2f,%.2f,%.2f,%.2f)",
		mLink[0].rot.z, mLink[1].rot.z, mLink[2].rot.z, mLink[3].rot.z,
		mLink[4].rot.z);*/

}


// OGL message handlers

// function that acts on creation of window
int OGLView::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	// local vars
	RECT rect;

	// Create the display list for an axis with arrows pointing in
	// the positive direction (Red = X, Green = Y, Blue = Z)
	glNewList(OGL_AXIS_DLIST, GL_COMPILE);
	glPushMatrix();
	glScalef(2.0f, 2.0f, 2.0f);
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f);	// X axis start (Red)
	glVertex3f(-0.2f, 0.0f, 0.0f);
	glVertex3f(0.2f, 0.0f, 0.0f);
	glVertex3f(0.2f, 0.0f, 0.0f);	// Top of arrowhead
	glVertex3f(0.15f, 0.04f, 0.0f);
	glVertex3f(0.2f, 0.0f, 0.0f);	// Bottom of arrowhead
	glVertex3f(0.15f, -0.04f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);	// Y axis start (Green)
	glVertex3f(0.0f, 0.2f, 0.0f);
	glVertex3f(0.0f, -0.2f, 0.0f);
	glVertex3f(0.0f, 0.2f, 0.0f);	// Top of arrowhead
	glVertex3f(0.04f, 0.15f, 0.0f);
	glVertex3f(0.0f, 0.2f, 0.0f);	// Bottom of arrowhead
	glVertex3f(-0.04f, 0.15f, 0.0f);
	glColor3f(0.0f, 0.0f, 1.0f);	// Z axis start (Blue)
	glVertex3f(0.0f, 0.0f, 0.2f);
	glVertex3f(0.0f, 0.0f, -0.2f);
	glVertex3f(0.0f, 0.0f, 0.2f);	// Top of arrowhead
	glVertex3f(0.0f, 0.04f, 0.15f);
	glVertex3f(0.0f, 0.0f, 0.2f);	// Bottom of arrowhead
	glVertex3f(0.0f, -0.04f, 0.15f);
	glEnd();
	glPopMatrix();
	glEndList();

	CreateBoneDLists(&mLink[0]);

	// Load the texture maps for the object
	LoadBoneTexture(&mLink[0], "Snake.bmp");
	mLink[1].visuals = mLink[0].visuals; // Bones 1 - 3 use instanced textures
	mLink[2].visuals = mLink[0].visuals;
	mLink[3].visuals = mLink[0].visuals;
	LoadBoneTexture(&mLink[4], "Head.bmp");
	// no texture for the end effector as there's no visual for it

	// finally, draw the scene
	// DrawScene(TRUE);
	return 0;

}

// OpenGL code
GLvoid OGLView::Resize(GLsizei width, GLsizei height) {
	// local vars
	GLfloat aspect;

	// set class values to new width/height and set the viewport
	mWidth = width;
	mHeight = height;

	glViewport(0, 0, width, height);

	// set the aspect ratio
	aspect = (GLfloat)width / (GLfloat)height;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// For this application, we want to use a 2D orthographic view
	// gluOrtho2D(0.0f, (GLfloat)width, 0.0f, (GLfloat)height);
	// gluPerspective(10.0, aspect, 1.0, 2000.0;
	glMatrixMode(GL_MODELVIEW);
	mModelScale = (float)height / 6.0f;
	glScalef(mModelScale, mModelScale, 0.0f);

	mLink[0].trans.x = ((float)width / 2.0f) / mModelScale;
}

// LoadBoneTexture()
// Loads texture images for the bone
GLvoid OGLView::LoadBoneTexture(tBone* curBone, const char* name) {
	if (mDrawGeometry) {
		BITMAPINFO* info; // bitmap info
		void* bits; // bitmap pixel bits
		GLubyte* glbits; // Bitmap RGB pixels

		// Generate the OpenGL texture ID
		glGenTextures(1, (unsigned int*)&curBone->visuals);
		curBone->visualCount++;

		// load the bitmap
		bits = LoadDIBitmap(name, &info);
		// file couldn't be found
		if (bits == NULL) {
			::MessageBox(NULL, (LPCWSTR)"Unable to Open File...", (LPCWSTR)name, MB_OK);
			curBone->visuals = 0;
			return;
		}

		// convert it to an RGB texture
		glbits = ConvertBitsToGL(info, bits);
		// couldn't convert correctly, free memory before returning
		if (glbits == NULL) {
			free(info);
			free(bits);

			return;
		}

		// bind texture and set parameters
		glBindTexture(GL_TEXTURE_2D, (unsigned int)curBone->visuals);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		// define the 2D texture image
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // Force 4-byte alignment
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
		glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);

		glTexImage2D(GL_TEXTURE_2D, 0, 3, info->bmiHeader.biWidth, info->bmiHeader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, glbits);

		// free the bitmap and RGB images, then return 0
		free(glbits);
		free(info);
		free(bits);
	}
}

// init function for OpenGL
GLvoid OGLView::InitializeGL(GLsizei width, GLsizei height) {
	// local vars
	GLfloat aspect;

	// basic OpenGL init functions
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	aspect = (GLfloat)width / (GLfloat)height;

	// Establish viewing volume
	// For this app, we want to use a 2D orthographic view
	// gluOrtho2D(0.0f, (GLfloat)width, 0.0f, (GLfloat)height);
	// gluPerspective(10.0, aspect, 1, 2000);
	glMatrixMode(GL_MODELVIEW);

	// Set some OpenGL initial states so they are not done in the draw loop
	glPolygonMode(GL_FRONT, GL_FILL);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable(GL_TEXTURE_2D);

	glShadeModel(GL_SMOOTH);
	glDisable(GL_LIGHTING);
}

// Get the info on the OpenGL version that's running
void OGLView::GetGLInfo() {
	// local vars
	char* who, * which, * ver, * ext, * message;
	int len;

	who = (char*)::glGetString(GL_VENDOR);
	which = (char*)::glGetString(GL_RENDERER);
	ver = (char*)::glGetString(GL_VERSION);
	ext = (char*)::glGetString(GL_EXTENSIONS);

	len = 200 + strlen(who) + strlen(which) + strlen(ver) + strlen(ext);

	message = (char*)malloc(len);
	/*sprintf(message, "Who:\t%s\nWhich:\t%s\nVersion:\t%s\nExtensions:\t%s",
		who, which, ver, ext);*/

	::MessageBox(NULL, (LPCWSTR)message, (LPCWSTR)"GL Info", MB_OK);

	free(message);
}

// CreateBoneDLists()
// Creates the Drawlists for the bones in a skeleton
void OGLView::CreateBoneDLists(tBone* bone) {
	// Only make a bone if a child exists
	if (bone->childCount > 0) {
		// create the display list for a bone
		glNewList(bone->id, GL_COMPILE);
		glBegin(GL_LINE_STRIP);
		glColor3f(1.0f, 1.0f, 0.0f);	// Yellow
		glVertex3f(0.0f, 0.4f, 0.0f);		// 0
		glVertex3f(-0.4f, 0.0f, -0.4f);		// 1
		glVertex3f(0.4f, 0.0f, -0.4f);		// 2
		glVertex3f(0.0f, bone->children->trans.y, 0.0f);		// Base
		glVertex3f(-0.4f, 0.0f, -0.4f);		// 1
		glVertex3f(-0.4f, 0.0f, 0.4f);		// 4
		glVertex3f(0.0f, 0.4f, 0.0f);		// 0
		glVertex3f(0.4f, 0.0f, -0.4f);		// 2
		glVertex3f(0.4f, 0.0f, 0.4f);		// 3
		glVertex3f(0.0f, 0.4f, 0.0f);		// 0
		glVertex3f(-0.4f, 0.0f, 0.4f);		// 4
		glVertex3f(0.0f, bone->children->trans.y, 0.0f);		// Base
		glVertex3f(0.4f, 0.0f, 0.4f);		// 3
		glVertex3f(-0.4f, 0.0f, 0.4f);		// 4
		glEnd();
		glEndList();

		// recursively call function if the bone has children
		if (bone->childCount > 0)
			CreateBoneDLists(bone->children);
	}
}

// draws the model with textures (if available)
GLvoid OGLView::DrawModel(tBone* curBone) {
	// draw the texture if the bone has one
	if (curBone->visuals > 0) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, (unsigned int)curBone->visuals);
	}
	// interleave arrays using the SoftImage data from Model.h
	glInterleavedArrays(GL_T2F_C3F_V3F, 0, (GLvoid*)SNAKE);

	glDrawArrays(GL_TRIANGLES, 0, SNAKEPOLYCNT * 3);
	glDisable(GL_TEXTURE_2D);
}

// DrawSkeleton()
// Draws the skeleton recursively
GLvoid OGLView::DrawSkeleton(tBone* rootBone, bool actuallyDraw) {
	// local vars
	int loop;
	tBone* curBone;

	curBone = rootBone->children;
	for (loop = 0; loop < rootBone->childCount; loop++) {
		glPushMatrix();

		// Set base orientation and position
		glTranslatef(curBone->trans.x, curBone->trans.y, curBone->trans.z);

		glRotatef(curBone->rot.z, 0.0f, 0.0f, 1.0f);
		glRotatef(curBone->rot.y, 0.0f, 1.0f, 0.0f);
		glRotatef(curBone->rot.x, 1.0f, 0.0f, 0.0f);

		// since scale is local, we must push and pop
		glPushMatrix();
		glScalef(curBone->scale.x, curBone->scale.y, curBone->scale.z);

		if (actuallyDraw) {
			if (mDrawGeometry) {
				if (curBone->childCount > 0) {
					DrawModel(curBone);
				}
			}
			else {
				// Draw the axis OpenGL object
				glCallList(OGL_AXIS_DLIST);

				// Draw the actual bone structure
				// only make a bone if there is a child
				if (curBone->childCount > 0) {
					glColor3f(1.0f, 1.0f, 0.0f);  // Selected bone color is yellow
					// draw the bone structure
					glCallList(curBone->id);
				}
			}
		}

		// Grab the matrix at this point, so it can be used for the deformation
		glGetFloatv(GL_MODELVIEW_MATRIX, curBone->matrix.m);

		glPopMatrix(); // For the scale

		// Check if this bone has children, and recursively call if so
		if (curBone->childCount > 0) {
			DrawSkeleton(curBone, actuallyDraw);
		}

		glPopMatrix(); // For the whole matrix
		curBone++;
	}
}

// on paint function
void OGLView::OnPaint() {
	// DrawScene(TRUE);
}

// function to call on resize
void OGLView::OnSize(UINT nType, int cx, int cy) {
	// Resize the original window
	Resize(cx, cy);
}

// OnLButtonDown()
// Left mouse button grabs and continually updates the IK function while held down
void OGLView::OnLButtonDown(UINT nFlags, Point point) {
	// local vars
	Point joint1, joint2, effector;

	mMousePos = point;
	point.y = mHeight - point.y - 1;

	// If you click anywhere, solve the IK system
	if ((nFlags & MK_CONTROL) == 0) {
		ComputeCCDLink(point);
		// DrawScene(TRUE);
	}
	else {
		ComputeOneCCDLink(point, 2);
		// DrawScene(TRUE);
	}
	mGrabRotX = mLink[2].rot.x;
	mGrabRotY = mLink[2].rot.y;
	mGrabRotZ = mLink[2].rot.z;

	mGrabTransX = mLink[2].trans.x;
	mGrabTransY = mLink[2].trans.y;
	mGrabTransZ = mLink[2].trans.z;
}

// OnRButtonDown
// Right mouse button grabs the current point to be used for the CCD function
void OGLView::OnRButtonDown(UINT nFlags, Point point) {
	mMousePos = point;
	point.y = mHeight - point.y - 1;

	// If you click anywhere, solve the IK system
	if ((nFlags & MK_CONTROL) == 0) {
		ComputeCCDLink(point);
		// DrawScene(TRUE);
	}
	else {
		ComputeOneCCDLink(point, 4);
		// DrawScene(TRUE);
	}
	mGrabRotX = mLink[4].rot.x;
	mGrabRotY = mLink[4].rot.y;
	mGrabRotZ = mLink[4].rot.z;

	mGrabTransX = mLink[4].trans.x;
	mGrabTransY = mLink[4].trans.y;
	mGrabTransZ = mLink[4].trans.z;
}

// OnMButtonDown()
// Has relatively similar functionality to OnRButtonDown, but starts on the bone before it
void OGLView::OnMButtonDown(UINT nFlags, Point point) {
	mMousePos = point;
	point.y = mHeight - point.y - 1;

	// If you click anywhere, solve the IK system
	if ((nFlags & MK_CONTROL) == 0) {
		ComputeCCDLink(point);
		// DrawScene(TRUE);
	}
	else {
		ComputeOneCCDLink(point, 3);
		// DrawScene(TRUE);
	}
	mGrabRotX = mLink[3].rot.x;
	mGrabRotY = mLink[3].rot.y;
	mGrabRotZ = mLink[3].rot.z;

	mGrabTransX = mLink[3].trans.x;
	mGrabTransY = mLink[3].trans.y;
	mGrabTransZ = mLink[3].trans.z;
}

void OGLView::HandleKeyDown(UINT nChar) {

}

// function that handles KeyUp cases
void OGLView::HandleKeyUp(UINT nChar) {
	switch (nChar) {
	case 'g':
		mDrawGeometry = !mDrawGeometry;
		break;
	}
}

// OnMouseMove()
// Handles mouse moving while pressed
void OGLView::OnMouseMove(UINT nFlags, Point point) {
	if ((nFlags & MK_LBUTTON) > 0) {
		if ((nFlags & MK_CONTROL) > 0) {
			if ((point.x - mMousePos.x) != 0) {
				// mLink[3].rot.z = mGrabRotZ + ((float)ROTATE_SPEED * (point.x - mMousePos.x));
				// DrawScene(TRUE);
			}
		}
		else {
			point.y = mHeight - point.y - 1;
			ComputeCCDLink(point);
			// DrawScene(TRUE);
		}
	}
	else if ((nFlags & MK_RBUTTON) == MK_RBUTTON) {
		if ((nFlags & MK_CONTROL) > 0) {
			if ((point.x - mMousePos.x) != 0) {
				// mLink[4].rot.z = mGrabRotZ + ((float)ROTATE_SPEED * (point.x - mMousePos.x));
				// DrawScene(TRUE);
			}
		}
	}
	else if ((nFlags & MK_MBUTTON) == MK_MBUTTON) {
		if ((nFlags & MK_CONTROL) > 0) {
			if ((point.x - mMousePos.x) != 0) {
				// mLink[2].rot.z = mGrabRotZ + ((float)ROTATE_SPEED * (point.x - mMousePos.x));
				// DrawScene(TRUE);
			}
		}
	}
}

// OnLButtonDblClk()
// Left Double Click to get dialog for Orientation
void OGLView::OnLButtonDblClk(UINT nFlags, Point point) {

}

// ComputeOneCCDLink
// Computes the IK Solution to an end effector
bool OGLView::ComputeOneCCDLink(Point endPos, int link) {
	// local vars
	tVector rootPos, curEnd, desiredEnd, targetVector, curVector, crossResult;
	double cosAngle, turnAngle, turnDeg;

	// set root position
	rootPos.x = mLink[link].matrix.m[12];
	rootPos.y = mLink[link].matrix.m[13];
	rootPos.z = mLink[link].matrix.m[14];

	// set current end effector position
	curEnd.x = mLink[EFFECTOR_POS].matrix.m[12];
	curEnd.y = mLink[EFFECTOR_POS].matrix.m[13];
	curEnd.z = mLink[EFFECTOR_POS].matrix.m[14];

	// set the desired end point
	desiredEnd.x = (float)endPos.x;
	desiredEnd.y = (float)endPos.y;
	desiredEnd.z = 0.0f; // displaying in 2D for now

	// Check to see if we're close enough
	if (VectorSquaredDistance(&curEnd, &desiredEnd) > 1.0f) {
		curVector.x = curEnd.x - rootPos.x;
		curVector.y = curEnd.y - rootPos.y;
		curVector.z = curEnd.z - rootPos.z;

		targetVector.x = endPos.x - rootPos.x;
		targetVector.y = endPos.y - rootPos.y;
		targetVector.z = 0.0f; // displaying in 2D for now

		// normalize both vectors
		NormalizeVector(&curVector);
		NormalizeVector(&targetVector);

		// set the angle based on the dot product of our two vector
		cosAngle = DotProduct(&targetVector, &curVector);

		// check if our angle causes a rotation
		if (cosAngle < 0.99999) {
			// take the cross product of our vectors
			CrossProduct(&targetVector, &curVector, &crossResult);

			// rotate clockwise
			if (crossResult.z > 0.0f) {
				turnAngle = acos((float)cosAngle);
				turnDeg = RADTODEG(turnAngle);
				mLink[link].rot.z -= (float)turnDeg;
			}
			// rotate counter-clockwise
			else if (crossResult.z < 0.0f) {
				turnAngle = acos((float)cosAngle);
				turnDeg = RADTODEG(turnAngle);
				mLink[link].rot.z += (float)turnDeg;
			}
			// DrawScene(FALSE); // Change to true to view the iteration
		}
	}

	return TRUE;
}

// ComputeCCDLink()
// Compute an IK Solution to an end effector, handles all links in the chain
bool OGLView::ComputeCCDLink(Point endPos) {
	// local vars
	tVector		rootPos, curEnd, desiredEnd, targetVector, curVector, crossResult;
	double		cosAngle, turnAngle, turnDeg;
	int			link, tries;

	// Start at the last link in the chain
	link = EFFECTOR_POS - 1;
	tries = 0; // loop counter so the program knows when to quit
	do {
		// The coords of the x, y, z position of the root of this bone is in the matrix
		// translation part which is in the 12, 13, 14 position of the matrix
		rootPos.x = mLink[link].matrix.m[12];
		rootPos.y = mLink[link].matrix.m[13];
		rootPos.z = mLink[link].matrix.m[14];

		// Position of the end effector
		curEnd.x = mLink[EFFECTOR_POS].matrix.m[12];
		curEnd.y = mLink[EFFECTOR_POS].matrix.m[13];
		curEnd.z = mLink[EFFECTOR_POS].matrix.m[14];

		// Desired end effector position
		desiredEnd.x = (float)endPos.x;
		desiredEnd.y = (float)endPos.y;
		desiredEnd.z = 0.0f; // displaying in 2D for now

		// Check to see if we're close enough
		if (VectorSquaredDistance(&curEnd, &desiredEnd) > 1.0f) {
			// create the vector to the current effector position
			curVector.x = curEnd.x - rootPos.x;
			curVector.y = curEnd.y - rootPos.y;
			curVector.z = curEnd.z - rootPos.z;

			// create the desired effector position vector
			targetVector.x = endPos.x - rootPos.x;
			targetVector.y = endPos.y - rootPos.y;
			targetVector.z = 0.0f; // displaying in 2D for now

			// normalize both vectors
			NormalizeVector(&curVector);
			NormalizeVector(&targetVector);

			// set the cosine angle based on the dot product of our two vectors
			cosAngle = DotProduct(&targetVector, &curVector);

			// check if our angle causes a rotation (1 means no rotation)
			if (cosAngle < 0.99999) {
				// take the cross product of our vectors to determine rotation direction
				CrossProduct(&targetVector, &curVector, &crossResult);

				// rotate clockwise
				if (crossResult.z > 0.0f) {
					turnAngle = acos((float)cosAngle);
					turnDeg = RADTODEG(turnAngle);
					// damping
					if (mDamping && turnDeg > mLink[link].dampWidth)
						turnDeg = mLink[link].dampWidth;
					mLink[link].rot.z -= (float)turnDeg;  // turn the link
					// depth of field restrictions
					if (mDOFRestrict && mLink[link].rot.z < (float)mLink[link].minRz)
						mLink[link].rot.z = (float)mLink[link].minRz;
				}
				// rotate counter-clockwise
				else if (crossResult.z < 0.0f) {
					turnAngle = acos((float)cosAngle);
					turnDeg = RADTODEG(turnAngle);
					// damping
					if (mDamping && turnDeg > mLink[link].dampWidth)
						turnDeg = mLink[link].dampWidth;
					mLink[link].rot.z += (float)turnDeg;
					// depth of field restrictions
					if (mDOFRestrict && mLink[link].rot.z > (float)mLink[link].maxRz)
						mLink[link].rot.z = (float)mLink[link].maxRz;
				}
				// Make false to recalculate all matrices without drawing
				// DrawScene(FALSE); // Change to true to view the iteration
			}
			if (--link < 0) link = EFFECTOR_POS - 1; // start of the chain, so restart
		}
	} while (tries++ < MAX_IK_TRIES && VectorSquaredDistance(&curEnd, &desiredEnd) > IK_POS_THRESH);
	// Quit once we're close enough or the loop has run long enough

	return TRUE;
}