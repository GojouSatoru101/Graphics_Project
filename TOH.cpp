#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define  STB_IMAGE_IMPLEMENTATION
#include <C:\Users\DELL\source\repos\first_glfw\first_glfw\stb_image.h>
#include<C:\Users\DELL\source\repos\first_glfw\first_glfw\shader_class_m.h>
#include<C:\Users\DELL\source\repos\first_glfw\first_glfw\new_cam.h>
#include<C:\Users\DELL\source\repos\first_glfw\first_glfw\new_model.h>

#include <iostream>
#include <cmath>
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
glm::vec3 calculateBezierPoint(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 D, glm::vec3 E, float t);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.88f, 8.8f, 28.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
unsigned int num_spaces = 0;
bool isSpacePressed = false;
bool leftAltPressed = false;
bool isMouseDragging = false;
bool anime = false;
float anim_speed = 0.001f;
// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    //glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);



    // tell GLFW to capture our mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("C:\\Users\\DELL\\source\\repos\\first_glfw\\first_glfw\\new_models.vs", "C:\\Users\\DELL\\source\\repos\\first_glfw\\first_glfw\\new_models.fs");

    ourShader.use();


    Model base("three_towers.obj");
    Model disc("disc.obj");
    Model disc1("disc1.obj");
    Model disc2("disc2.obj");
    // render loop
    // -----------
    glm::vec3 translate_vec = glm::vec3(0.0f, 0.0f, 0.0f);

    float t = 0.0f;
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);
        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // activate shader
        ourShader.use();

        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);

        glm::mat4 sceneRotation = camera.GetSceneRotationMatrix();


        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);

        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = sceneRotation * model; // Apply scene rotation to the model matrix
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	
        ourShader.setMat4("model", model);

        base.Draw(ourShader);
        //disc disc1 disc2 and disc3 are three seperate model objects
        disc.Draw(ourShader);
        disc1.Draw(ourShader);
        //disc2.Draw(ourShader);

        glm::mat4 translateModel = glm::mat4(1.0f);
        translateModel = glm::translate(translateModel, translate_vec); // translate it down so it's at the center of the scene
        translateModel = sceneRotation * translateModel; // Apply scene rotation to the model matrix
        translateModel = glm::scale(translateModel, glm::vec3(1.0f, 1.0f, 1.0f));	
        ourShader.setMat4("model", translateModel);
        disc2.Draw(ourShader);
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            if (!anime) {
                anime = true;
                t = 0.0f;
            }
            if(anime)
            {
                glm::vec3 bezierP0 = disc2.CalculatePos(translate_vec);/* initial position of the disc */
                glm::vec3 bezierP1 = glm::vec3(0.0, 0.0,0.0);
                glm::vec3 bezierP2 = glm::vec3(0, 15, 0.0);
                glm::vec3 bezierP3 = glm::vec3(7.5, -2.5, 0.0);
                glm::vec3 bezierP4 = glm::vec3(15.0, 15.0, 0.0);
                glm::vec3 bezierP5 = glm::vec3(15.0, -2.0, 0.0);
                /* final position of disc */
                //glm::vec3 bezierP3 = base.CalculatePos(glm::vec3(0.0f, 0.0f, 0.0f));
                t += 0.0001f;
 
                //cout << t << endl;
                t = glm::clamp(t, 0.0f, 1.0f);
                glm::vec3 interpolatedPosition = calculateBezierPoint(bezierP0, bezierP1, bezierP2, bezierP3, bezierP4,t);
                translate_vec = interpolatedPosition;
            }
        }
        else {
            anime = false;
        }
       
    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

glm::vec3 calculateBezierPoint(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 D,glm::vec3 E ,float t)
{
    float u=1-t;
    glm::vec3 point;
    point.x = pow(u, 4) * A.x + 4 * t * pow(u, 3) * B.x + 6 * pow(t, 2) * pow(u, 2) * C.x + 4 * pow(t, 3) * u * D.x + pow(t, 4) * E.x;

   
    point.x = pow(u, 4) * A.y + 4 * t * pow(u, 3) * B.y + 6 * pow(t, 2) * pow(u, 2) * C.y + 4 * pow(t, 3) * u * D.y + pow(t, 4) * E.y;
    point.z = A.z;
    //point.x = uuu * P0.x + 3.0f * uu * t * P1.x + 3 * u * tt * P2.x + ttt * P3.x;
    return point;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
    {
        leftAltPressed = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    /* if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_RELEASE && leftAltPressed)
     {
         leftAltPressed = false;
         glfwSetCursorEnterCallback(window, cursorEnter_callback);
     }*/

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(DEFAULT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.ProcessKeyboard(MATHI, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.ProcessKeyboard(TALA, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);

    if (glfwGetKey(window,GLFW_KEY_SPACE)==GLFW_PRESS) {
            isSpacePressed = true;
        }
    else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
            isSpacePressed = false;
        }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}



void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            isMouseDragging = true;
            glfwSetCursorPosCallback(window, mouse_callback);
            camera.lastYaw = camera.Yaw;
            camera.lastPitch = camera.Pitch;
        }
        else if (action == GLFW_RELEASE) {
            isMouseDragging = false;
            //camera.sceneYaw = 0.0f;    // Reset accumulated scene rotation
            //camera.scenePitch = 0.0f;  // Reset accumulated scene rotation
        }
    }
}
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    if (isMouseDragging)
    {
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;
        // Calculate the change in Yaw and Pitch since dragging started
        float yawChange = xoffset * camera.MouseSensitivity;
        float pitchChange = yoffset * camera.MouseSensitivity;

        // Apply the changes starting from the last known Yaw and Pitch values
        camera.Yaw = camera.lastYaw + yawChange;
        camera.Pitch = camera.lastPitch + pitchChange;
        camera.ProcessMouseMovement(xoffset, yoffset, isMouseDragging);
    }

}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
