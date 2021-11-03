#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb-master/stb_image.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include "Components/FileSystem.h"
#include "Components/Shader.h"
#include "Components/Camera.h"
#include "Components/Animator.h"
#include "Components/Pathfinding.h"

#include <iostream>
#include <chrono>
#include <thread>

using std::chrono::system_clock;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow* window);

// function to draw floor
void DrawFloor(Shader& shader);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(2.0f, 1.0f, 8.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// global bools
bool drawBones = false;
bool drawMesh = false;
bool drawPath = true;

// movement speed
int iValue = 1;

int main()
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "560 Project", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader ourShader("Getting Started/model.vert", "Getting Started/model.frag");
    Shader lineShader("Getting Started/model.vert", "Getting Started/debug.frag");
    Shader floorShader("Getting Started/floor.vert", "Getting Started/floor.frag");

    // load models
    Model ourModel("Getting Started/Resources/cowboy/model.dae");
    Animation danceAnimation("Getting Started/Resources/cowboy/model.dae", &ourModel);
    Animator animator(&danceAnimation);

    std::vector<Point3D> points;
    Point3D p0, p1, p2, p3, p4, p5, p6, p7;
    p0.x = 4.0f; p0.y = 0.0f; p0.z = 3.0f;
    p1.x = 2.4f; p1.y = 0.0f; p1.z = 3.6f;
    p2.x = -0.4f; p2.y = 0.0f; p2.z = 2.4f;
    p3.x = -1.0f; p3.y = 0.0f; p3.z = 0.6f;
    p4.x = -2.0f; p4.y = 0.0f; p4.z = -1.2f;
    p5.x = -1.6f; p5.y = 0.0f; p5.z = -2.0f;
    p6.x = 0.6f; p6.y = 0.0f; p6.z = -0.4f;
    p7.x = 2.8f; p7.y = 0.0f; p7.z = -0.2f;
    points.push_back(p0);
    points.push_back(p1);
    points.push_back(p2);
    points.push_back(p3);
    points.push_back(p4);
    points.push_back(p5);
    points.push_back(p6);
    points.push_back(p7);

    std::vector<Point3D> splinePoints;
    int TOTAL_SPLINE_POINTS = 2000;
    for (int i = 0; i < TOTAL_SPLINE_POINTS; i++) {
        float t = ((float)i / (float)TOTAL_SPLINE_POINTS) * points.size();
        Point3D pt = GetSpline(points, t);
        splinePoints.push_back(pt);
    }
    // change points to vector of floats, for drawing purposes
    std::vector<float> pointCoords = ConvertPointsToFloats(points);
    std::vector<float> splinePointCoords = ConvertPointsToFloats(splinePoints);

    // arc length table
    std::vector<float> arcLengthTable = GetArcLengths(splinePoints);
    float curveLength = arcLengthTable[arcLengthTable.size() - 1];

    // finally, divide points by the curveLength value and create the final values
    tArcValues tArcPairs = GetFinalPairValues(points, arcLengthTable, curveLength);

    std::cout << curveLength << std::endl;

    // iterate through spline points
    int splineIter = 0;
    float dT = 1.0f / (float)TOTAL_SPLINE_POINTS;
    
    // find an arc index based off of the arc length
    int i = FindArcIndex(tArcPairs, 0, tArcPairs.size(), 6.2f);

    // from a given arc length at an index, find the u-value [0, 1]
    float u = FindParametricValue(tArcPairs, arcLengthTable[i], dT);

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        if (drawMesh) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        // per-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        // iterate for the path
        splineIter = splineIter % TOTAL_SPLINE_POINTS;
        glm::vec3 currPoint = glm::vec3(splinePoints[splineIter].x, splinePoints[splineIter].y, splinePoints[splineIter].z);

        // forward point for rotation
        float t = tArcPairs[splineIter].first;
        Point3D forward;
        if (splineIter < TOTAL_SPLINE_POINTS - 1) {
            forward = splinePoints[splineIter + 1];
        }
        else {
            forward = splinePoints[0];
        }
        glm::vec3 fwdPt = glm::vec3(forward.x, forward.y, forward.z);

        // update the animation based on the arc length differential
        int prev = splineIter - 1;
        if (splineIter == 0) {
            prev = TOTAL_SPLINE_POINTS - 1;
        }
        // use the arc speed 
        float arcSpeed = (tArcPairs[splineIter].second - tArcPairs[prev].second) / (dT * 4);
        if (splineIter == 0) {
            arcSpeed = arcSpeed * 0.0f;
        }
        if (splineIter < 100) {
            arcSpeed = arcSpeed * 0.01f * splineIter;
        }
        if (splineIter > TOTAL_SPLINE_POINTS - 100) {
            arcSpeed = arcSpeed * 0.01f * (TOTAL_SPLINE_POINTS - splineIter);
        }
        animator.UpdateAnimation(deltaTime * (iValue * 0.7f) * arcSpeed);


        splineIter = splineIter + iValue;
        if (splineIter >= TOTAL_SPLINE_POINTS) {
            this_thread::sleep_until(system_clock::now() + 1s);
        }

        // render
        glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        ourShader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        auto transforms = animator.GetFinalBoneMatrices();
        for (int i = 0; i < transforms.size(); i++)
            ourShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);

        glm::vec2 curr = glm::vec2(currPoint.x, currPoint.z);
        glm::vec2 fwd = glm::vec2(fwdPt.x, fwdPt.z);

        float angle = FindArcLength(tArcPairs, t + dT) - FindArcLength(tArcPairs, t);
        
        model = glm::translate(model, currPoint); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.1f));	// it's a bit too big for our scene, so scale it down
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);
       
        // reset the model matrix
        model = glm::mat4(1.0f);

        lineShader.use();
        lineShader.setMat4("projection", projection);
        lineShader.setMat4("view", view);
        lineShader.setMat4("model", model);
        // draw the floor
        DrawFloor(lineShader);
        
        if (drawPath) {
            lineShader.setBool("controlColor", true);
            DrawPath(lineShader, pointCoords);
            lineShader.setBool("controlColor", false);
            lineShader.setBool("pathColor", true);
            DrawSplinePath(lineShader, splinePointCoords);
            lineShader.setBool("pathColor", false);
        }
        if (drawBones) {
            glClear(GL_DEPTH_BUFFER_BIT);
            for (int i = 0; i < transforms.size(); i++)
                lineShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
            model = glm::translate(model, currPoint);
            model = glm::scale(model, glm::vec3(0.1f));
            model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
            lineShader.setMat4("model", model);
            animator.DebugDraw();
        }

        


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

// glfw: whenever a specific key is pressed, this callback is called
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_B && action == GLFW_PRESS) {
        drawBones = !drawBones;
    }
    if (key == GLFW_KEY_M && action == GLFW_PRESS) {
        drawMesh = !drawMesh;
    }
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        drawPath = !drawPath;
    }
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
        iValue++;
    }
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
        iValue--;
        if (iValue < 1) {
            iValue = 1;
        }
    }
}

void DrawFloor(Shader& shader)
{
    std::vector<float> floorVerts;
    std::vector<GLuint> floorInds;

    float floorL = 5.0f;
    float floorW = 5.0f;

    int linesL = 5;
    int linesW = 5;


    for (float i = 0.0f; i < linesL; i++) {

        floorVerts.push_back(-floorL);
        floorVerts.push_back(0.0f);
        floorVerts.push_back(i / (linesL - 1) * (2.f * floorW) - floorW);

        floorVerts.push_back(floorL);
        floorVerts.push_back(0.0f);
        floorVerts.push_back(i / (linesL - 1) * (2.f * floorW) - floorW);

        floorInds.push_back(i * 2);
        floorInds.push_back(i * 2 + 1);

    }

    for (float i = 0.0f; i < linesW; i++) {

        floorVerts.push_back(i / (linesW - 1) * (2.f * floorL) - floorL);
        floorVerts.push_back(0.0f);
        floorVerts.push_back(-floorW);

        floorVerts.push_back(i / (linesW - 1) * (2.f * floorL) - floorL);
        floorVerts.push_back(0.0f);
        floorVerts.push_back(floorW);


        floorInds.push_back(i * 2 + 2 * linesL);
        floorInds.push_back(i * 2 + 1 + 2 * linesL);

    }

    GLuint floorVAO, floorEBO, floorVBO;

    shader.use();
    glGenBuffers(1, &floorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, floorVerts.size() * sizeof(float), &floorVerts[0], GL_STATIC_DRAW);

    glGenBuffers(1, &floorEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, floorInds.size() * sizeof(GLuint), &floorInds[0], GL_STATIC_DRAW);

    glGenVertexArrays(1, &floorVAO);
    glBindVertexArray(floorVAO);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindVertexArray(0);

    glBindVertexArray(floorVAO);
    glDrawArrays(GL_LINES, 0, floorVerts.size());
    glBindVertexArray(0);
}