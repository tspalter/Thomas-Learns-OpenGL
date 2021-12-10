#include "Components/Engine.h"

int main()
{
    Engine engine;
    engine.Run();
    return 0;
}

//// glfw: whenever the window size changed (by OS or user resize) this callback function executes
//void framebuffer_size_callback(GLFWwindow* window, int width, int height)
//{
//    // make sure the viewport matches the new window dimensions; note that width and 
//    // height will be significantly larger than specified on retina displays.
//    glViewport(0, 0, width, height);
//}
//
//// glfw: whenever the mouse moves, this callback is called
//void mouse_callback(GLFWwindow* window, double xpos, double ypos)
//{
//    if (mousePressed) {
//        if (firstMouse)
//        {
//            lastX = xpos;
//            lastY = ypos;
//            firstMouse = false;
//        }
//
//        float xoffset = xpos - lastX;
//        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
//
//        lastX = xpos;
//        lastY = ypos;
//
//        camera.ProcessMouseMovement(xoffset, yoffset);
//    }
//}
//
//// glfw: whenever the mouse scroll wheel scrolls, this callback is called
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
//{
//    // camera.ProcessMouseScroll(yoffset);
//}
//
//// glfw: whenever a specific key is pressed, this callback is called
//void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
//    if (key == GLFW_KEY_B && action == GLFW_PRESS) {
//        drawBones = !drawBones;
//    }
//    if (key == GLFW_KEY_M && action == GLFW_PRESS) {
//        drawMesh = !drawMesh;
//    }
//    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
//        drawPath = !drawPath;
//    }
//    if (key == GLFW_KEY_L && action == GLFW_PRESS) {
//        drawSegments = !drawSegments;
//    }
//    if (key == GLFW_KEY_COMMA && action == GLFW_PRESS) {
//        drawModel = !drawModel;
//    }
//    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
//        drawFloor = !drawFloor;
//    }
//    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
//        iValue++;
//    }
//    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
//        iValue--;
//        if (iValue < 1) {
//            iValue = 1;
//        }
//    }
//}