// Engine.h
// header file for Engine object class
// handles all engine-related functions
// learned from working with a game engine on my team

#include "Engine.h"

#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

// initialize static pointers
Camera* Engine::mCamera = new Camera();
Light* Engine::mLighting = new Light();
IKBoneAnimation* Engine::mBoneAnim = new IKBoneAnimation();
bool Engine::keys[1024];

// additional vars
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

bool mousePressed = false;

Engine::Engine() { }

Engine::~Engine() { }

void Engine::Init() {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    mCamera->Position = glm::vec3(3.0f, 20.0f, -5.0f);

    // glfw window creation
    mWindow = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "560 Project", NULL, NULL);
    if (mWindow == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(mWindow);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
    // ImGui_ImplOpenGL3_Init(GLSL);
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // initialize other values
    mLighting->InitLight();
    mBoneAnim->Init();
}

void Engine::Display(GLFWwindow* window) {
    Shader mShader("Getting Started/Components/shader/shader.vert", "Getting Started/Components/shader/shader.frag");

    // main update loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        if (isSceneReset) {
            SceneReset();
            isSceneReset = false;
        }

        // handle camera
        OnCameraMove();

        // render functions
        mShader.use();
        SetupUniformValues(mShader);
        DrawScene(mShader);

        // feed inputs to dear imgui, start new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // render your GUI
        ImGui::Begin("560 Project 3");
        if (ImGui::Button("IK")) {
            mBoneAnim->didBoneMove = true;
        }
        ImGui::SliderFloat("Target X: ", &mBoneAnim->target.x, -10.0f, 10.0f);
        ImGui::SliderFloat("Target Y: ", &mBoneAnim->target.y, -10.0f, 10.0f);
        ImGui::SliderFloat("Target Z: ", &mBoneAnim->target.z, -10.0f, 10.0f);
        ImGui::End();

        // Render dear imgui into screen
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        // Swap screen buffers
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return;
}

void Engine::Run() {
    Init();
    Display(mWindow);
}

void Engine::LoadModels() {
    // make sure object list is empty
    objList.clear();
    // create objects from files
    // cube
    Object cubeObj("Getting Started/Components/objs/cube.obj");
    cubeObj.objColor = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
    cubeObj.objName = "cube";

    // plane
    Object planeObj("Getting Started/Components/objs/plane.obj");
    planeObj.objColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    planeObj.objName = "plane";

    // arrow for axes
    Object arrowObj("Getting Started/Components/objs/arrow.obj");
    arrowObj.objName = "arrow";

    // bind objects to VAO and VBO
    BindVAOVBO(cubeObj);
    BindVAOVBO(planeObj);
    BindVAOVBO(arrowObj);

    // push back into our object list
    objList.push_back(cubeObj);
    objList.push_back(planeObj);
    objList.push_back(arrowObj);
}

void Engine::DrawScene(Shader& shader) {
    // set up basic params
    glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CW);

    // now draw the objects in the scene
    glm::mat4 worldIdentityObjMat = glm::mat4(1.0f);
    DrawAxis(shader, worldIdentityObjMat);
    DrawPlane(shader);
    DrawBones(shader, mBoneAnim);
}

void Engine::OnCameraMove() {
    // handle change over time
    float currFrame = glfwGetTime();
    deltaTime = currFrame - lastFrame;
    lastFrame = currFrame;

    // handle camera controls
    if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(mWindow, true);

    if (glfwGetKey(mWindow, GLFW_KEY_W) == GLFW_PRESS)
        mCamera->ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(mWindow, GLFW_KEY_S) == GLFW_PRESS)
        mCamera->ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(mWindow, GLFW_KEY_A) == GLFW_PRESS)
        mCamera->ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(mWindow, GLFW_KEY_D) == GLFW_PRESS)
        mCamera->ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(mWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
        mCamera->ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(mWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        mCamera->ProcessKeyboard(DOWN, deltaTime);
    if (glfwGetMouseButton(mWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        mousePressed = true;
    }
    if (glfwGetMouseButton(mWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
        mousePressed = false;
    }
}

void Engine::DrawObject(Shader& shader, Object& object) {
    // bind VAO
    glBindVertexArray(object.VAO);

    // set shader values
    shader.setVec3("mObject.objColor", object.objColor.x, object.objColor.y, object.objColor.z);
    shader.setFloat("mObject.shininess", object.shininess);

    // check for render and object types
    if (object.mRenderType == RENDER_TRIANGLES) { // triangle render type
        if (object.mObjType == OBJ_POINTS) {
            std::cout << "Error: Cannot render triangles if input object type is point" << std::endl;
        }
        if (object.mObjType == OBJ_TRIANGLES) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glDrawArrays(GL_TRIANGLES, 0, object.VAOVertices.size());
        }
    }

    if (object.mRenderType == RENDER_LINES) { // line render type
        glLineWidth(10);
        if (object.mObjType == OBJ_POINTS) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawArrays(GL_LINE_LOOP, 0, object.VAOVertices.size());
        }
        if (object.mObjType == OBJ_TRIANGLES) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawArrays(GL_TRIANGLES, 0, object.VAOVertices.size());
        }
    }

    if (object.mRenderType == RENDER_POINTS) { // point render type
        if (object.mObjType == OBJ_POINTS) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINTS);
            glDrawArrays(GL_TRIANGLES, 0, object.VAOVertices.size());
        }
    }
    
    // reset bindings
    glBindVertexArray(0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Engine::DrawAxis(Shader& shader, const glm::mat4 axisObjMat) {
    // always see the arrows
    glDepthFunc(GL_ALWAYS);
    
    // get arrow object
    Object* arrowObj = nullptr;
    for (unsigned int i = 0; i < objList.size(); i++) {
        if (objList[i].objName == "arrow") {
            arrowObj = &objList[i];
        }
    }

    // arrow object not found, return
    if (arrowObj == nullptr)
        return;

    // Draw main axis
    // red X arrow
    arrowObj->objMatrix = axisObjMat;
    shader.setMat4("model", arrowObj->objMatrix);
    arrowObj->objColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    DrawObject(shader, *arrowObj);

    // green Y arrow
    arrowObj->objMatrix = axisObjMat;
    arrowObj->objMatrix = glm::rotate(arrowObj->objMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    shader.setMat4("model", arrowObj->objMatrix);
    arrowObj->objColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    DrawObject(shader, *arrowObj);

    // blue Z arrow
    arrowObj->objMatrix = axisObjMat;
    arrowObj->objMatrix = glm::rotate(arrowObj->objMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    shader.setMat4("model", arrowObj->objMatrix);
    arrowObj->objColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    DrawObject(shader, *arrowObj);

    // reset depth function back to default
    glDepthFunc(GL_LESS);
}

void Engine::DrawPlane(Shader& shader) {
    // get plane object
    Object* planeObj = nullptr;
    for (unsigned int i = 0; i < objList.size(); i++) {
        if (objList[i].objName == "plane") {
            planeObj = &objList[i];
        }
    }
    // couldn't find plane object
    if (planeObj == nullptr)
        return;
    
    // draw the plane 
    planeObj->objMatrix = glm::mat4(1.0f);
    planeObj->objMatrix = glm::scale(planeObj->objMatrix, glm::vec3(10.0f, 10.0f, 10.0f));
    shader.setMat4("model", planeObj->objMatrix);
    DrawObject(shader, *planeObj);
}

void Engine::DrawBones(Shader& shader, IKBoneAnimation* mBoneAnimation) {
    // find bone object
    Object* boneObj = nullptr;
    for (unsigned int i = 0; i < objList.size(); i++) {
        if (objList[i].objName == "cube") {
            boneObj = &objList[i];
        }
    }
    // bone object not found
    if (boneObj == nullptr)
        return;

    mBoneAnimation->Update(deltaTime);

    // Draw root bone
    glm::mat4 rootBoneMat = glm::mat4(1.0f);
    rootBoneMat = glm::translate(rootBoneMat, mBoneAnimation->rootPosition);
    rootBoneMat = glm::scale(rootBoneMat, mBoneAnimation->scaleVector[0]);
    shader.setMat4("model", rootBoneMat);
    boneObj->objColor = mBoneAnimation->colors[0];
    DrawObject(shader, *boneObj);

    // Draw yellow bone
    glm::mat4 yellowBoneMat = mBoneAnimation->GetYellowMat();
    shader.setMat4("model", yellowBoneMat);
    boneObj->objColor = mBoneAnimation->colors[1];
    DrawObject(shader, *boneObj);

    // Draw purple bone
    glm::mat4 purpleBoneMat = mBoneAnimation->GetPurpleMat();
    shader.setMat4("model", purpleBoneMat);
    boneObj->objColor = mBoneAnimation->colors[2];
    DrawObject(shader, *boneObj);

    // Draw blue bone
    glm::mat4 blueBoneMat = mBoneAnimation->GetBlueMat();
    shader.setMat4("model", blueBoneMat);
    boneObj->objColor = mBoneAnimation->colors[3];
    DrawObject(shader, *boneObj);

    // Draw target object
    glm::mat4 targetObjMat = mBoneAnimation->GetTargetMat();
    shader.setMat4("model", targetObjMat);
    boneObj->objColor = mBoneAnimation->colors[4];
    DrawObject(shader, *boneObj);


}

void Engine::BindVAOVBO(Object& currObj) {
    // generate and bind VAO and VBO
    glGenVertexArrays(1, &currObj.VAO);
    glGenBuffers(1, &currObj.VBO);

    glBindVertexArray(currObj.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, currObj.VBO);
    glBufferData(GL_ARRAY_BUFFER, currObj.VAOVertices.size() * sizeof(Object::Vertex), &currObj.VAOVertices[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Object::Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Object::Vertex), (void*)offsetof(Object::Vertex, Object::Vertex::normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Object::Vertex), (void*)offsetof(Object::Vertex, Object::Vertex::texCoords));

    glBindVertexArray(0);
}

void Engine::SetupUniformValues(Shader& shader) {
    // camera uniforms
    shader.setVec3("cameraPos", mCamera->Position.x, mCamera->Position.y, mCamera->Position.z);

    glm::mat4 camProjection = mCamera->GetProjectionMatrix();
    shader.setMat4("projection", camProjection);
    glm::mat4 camView = mCamera->GetViewMatrix();
    shader.setMat4("view", camView);

    // light uniform values
    shader.setInt("dirLight.status", mLighting->dLight.status);
    shader.setVec3("dirLight.direction", mLighting->dLight.direction.x, mLighting->dLight.direction.y, mLighting->dLight.direction.z);
    shader.setVec3("dirLight.ambient", mLighting->dLight.ambient.x, mLighting->dLight.ambient.y, mLighting->dLight.ambient.z);
    shader.setVec3("dirLight.diffuse", mLighting->dLight.diffuse.x, mLighting->dLight.diffuse.y, mLighting->dLight.diffuse.z);
    shader.setVec3("dirLight.specular", mLighting->dLight.specular.x, mLighting->dLight.specular.y, mLighting->dLight.specular.z);

    // set current point light at camera position
    mLighting->pLight.position = mCamera->Position;
    shader.setInt("pointLight.status", mLighting->pLight.status);
    shader.setVec3("pointLight.direction", mLighting->pLight.position.x, mLighting->pLight.position.y, mLighting->pLight.position.z);
    shader.setVec3("pointLight.ambient", mLighting->pLight.ambient.x, mLighting->pLight.ambient.y, mLighting->pLight.ambient.z);
    shader.setVec3("pointLight.diffuse", mLighting->pLight.diffuse.x, mLighting->pLight.diffuse.y, mLighting->pLight.diffuse.z);
    shader.setVec3("pointLight.specular", mLighting->pLight.specular.x, mLighting->pLight.specular.y, mLighting->pLight.specular.z);
    shader.setFloat("pointLight.constant", mLighting->pLight.constant);
    shader.setFloat("pointLight.linear", mLighting->pLight.linear);
    shader.setFloat("pointLight.quadratic", mLighting->pLight.quadratic);

}

void Engine::SceneReset() {
    LoadModels();
    mCamera = new Camera();
}