#include "glad/glad.h"
#include <GLFW/glfw3.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define  STB_IMAGE_IMPLEMENTATION
#include <C:\Users\DELL\source\repos\first_glfw\first_glfw\stb_image.h>
#include<C:\Users\DELL\source\repos\first_glfw\first_glfw\shader_class_m.h>
#include<C:\Users\DELL\source\repos\first_glfw\first_glfw\chalni_cam.h>
#include<C:\Users\DELL\source\repos\first_glfw\first_glfw\chalni_model.h>

#include <iostream>
#include <cmath>
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadCubemap(vector<std::string> faces);

//Processes quadratic curve 
glm::vec3 calculateBezierPoint(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 D, float t);



// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera

Camera camera(glm::vec3(0.88f, 0.0f, 28.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
unsigned int num_spaces = 0;
bool isSpacePressed = false;
bool leftAltPressed = false;
bool isMouseDragging = false;


//Animation stuff
struct TowerInfo
{
    int tower_no;
    std::vector<int> discs;
};

struct Movement
{
    int disc_no;
    int src_tower;
    int dst_tower;
};

void solveTOH(TowerInfo& src, TowerInfo& dst, TowerInfo& aux, int count, std::vector<Movement>& cmd_buffer)
{

    if (src.discs.empty() || count <= 0)
        return;

    if ((src.discs.size() == 1) || (count == 1))
    {
        int top = src.discs.back();
        dst.discs.push_back(top);
        src.discs.pop_back();
        Movement move;
        move.disc_no = top;
        move.dst_tower = dst.tower_no;
        move.src_tower = src.tower_no;
        cmd_buffer.push_back(move);
        return;

    }

    solveTOH(src, aux, dst, count - 1, cmd_buffer);
    solveTOH(src, dst, aux, 1, cmd_buffer);
    solveTOH(aux, dst, src, count - 1, cmd_buffer);

}

bool anime = false;
float anim_speed = 0.9f;
int destination_pole = 2;
int selected_disc = 0;

//Animation bezier points, {needs to be constant for the whole animation}
glm::vec3 bezier_points[4];


// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);



    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
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
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("C:\\Users\\DELL\\source\\repos\\first_glfw\\first_glfw\\new_models.vs", "C:\\Users\\DELL\\source\\repos\\first_glfw\\first_glfw\\new_models.fs");
    Shader skyboxShader("C:\\Users\\DELL\\source\\repos\\first_glfw\\first_glfw\\skybox.vs", "C:\\Users\\DELL\\source\\repos\\first_glfw\\first_glfw\\skybox.fs");;
    ourShader.use();
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    //skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    vector<std::string> faces
    {
        ("right.jpg"),
         ("left.jpg"),
         ("top.jpg"),
         ("bottom.jpg"),
         ("front.jpg"),
         ("back.jpg")
    };
    unsigned int cubemapTexture = loadCubemap(faces);
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    Model base("base.stl");

    Model tower1("tower1.stl");
    Model tower2("tower2.stl");
    Model tower3("tower3.stl");

    Model disc0("disc.stl");
    Model disc1("disc.stl");
    Model disc2("disc1.stl");
    Model disc3("disc1.stl");
    Model disc4("disc2.stl");

    glm::vec3 tower1_coord = tower1.CalculateModelPos();
    glm::vec3 tower2_coord= tower2.CalculateModelPos();
    
    //to get the discs in source tower
    disc0.model_translate.x -= tower2_coord.x - tower1_coord.x;
    disc1.model_translate.x -= tower2_coord.x - tower1_coord.x;
    disc2.model_translate.x -= tower2_coord.x - tower1_coord.x;
    disc3.model_translate.x -= tower2_coord.x - tower1_coord.x;
    disc4.model_translate.x -= tower2_coord.x - tower1_coord.x;

    disc1.model_translate.z += disc1.getSize().z;
    disc2.model_translate.z += disc2.getSize().z;
    disc3.model_translate.z += disc3.getSize().z * 2;
    disc4.model_translate.z += disc4.getSize().z * 2;

    //Storing towers and discs in an array, kind of, loop can be used
    Model* discs[] = { &disc0,&disc1,&disc2, &disc3, &disc4 };
    Model* towers[] = { &tower1, &tower2, &tower3 };
    TowerInfo tower_info[3];
    tower_info[0].tower_no = 0;
    tower_info[1].tower_no = 1;
    tower_info[2].tower_no = 2;

    //All discs are in tower 0
    tower_info[0].discs.push_back(0);
    tower_info[0].discs.push_back(1);
    tower_info[0].discs.push_back(2);
    tower_info[0].discs.push_back(3);
    tower_info[0].discs.push_back(4);

    std::vector<Movement> cmd_buffer;
    int curr_cmd = 0;


    // render loop
    // -----------

    float t = 0.0f;
    while (!glfwWindowShouldClose(window)) {
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

        glm::mat4 uprightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        // render the loaded models

        //Draw base

        base.rotation_matrix = uprightRotation*sceneRotation; // Apply scene rotation to the model matrix
        base.scale_matrix = glm::mat4(1.0f);
        base.Draw(ourShader);

        //Draw tower1,2,3
        for (auto& tower : towers) {
            tower->rotation_matrix = uprightRotation * sceneRotation;
            tower->scale_matrix = glm::mat4(1.0f);
            tower->Draw(ourShader);

        }

        //Draw disc1,2,3
        //disc disc1 disc2 and disc3 are three seperate model objects

        for (auto& disc : discs) {
            disc->rotation_matrix = uprightRotation * sceneRotation;
            disc->scale_matrix = glm::mat4(1.0f);
            disc->Draw(ourShader);

        }

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {


            if (cmd_buffer.empty() || (curr_cmd == cmd_buffer.size())) {

                cmd_buffer.clear();
                curr_cmd = 0;

                destination_pole = (destination_pole + 1) % 3;

                TowerInfo tow0 = tower_info[(destination_pole + 0) % 3];
                TowerInfo tow1 = tower_info[(destination_pole + 1) % 3];
                TowerInfo tow2 = tower_info[(destination_pole + 2) % 3];

                solveTOH(tow1, tow0, tow2, std::size(discs), cmd_buffer);


            }
        }
        if (!cmd_buffer.empty() & (curr_cmd != cmd_buffer.size()))

        {
            //Only start an animation if not started already
            if (!anime) {
                anime = true;
                t = 0.0f;

                //Need to setup these points at the start of animation only
                selected_disc = cmd_buffer[curr_cmd].disc_no;
                destination_pole = cmd_buffer[curr_cmd].dst_tower;

                //Set initial world position of disc
                bezier_points[0] =
                    discs[selected_disc]->model_coord +
                    discs[selected_disc]->model_translate;

                //Set destination world position of disc
                bezier_points[3] =
                    towers[destination_pole]->model_coord +
                    towers[destination_pole]->model_translate -
                    glm::vec3(0.f, 0.f, 0.5f * towers[destination_pole]->getSize().z - (tower_info[destination_pole].discs.size() + 0.5f) * discs[0]->getSize().z);

                //Control points
                float k = towers[destination_pole]->getSize().z * 2.0f;
                bezier_points[1] = bezier_points[0] + glm::vec3(0.f, 0.f, k);
                bezier_points[2] = bezier_points[3] + glm::vec3(0.f, 0.f, k);

            }
            else {
                //If not just starting animation, then increase t

                t += deltaTime * anim_speed;
            }

            glm::vec3 interpolatedPosition = calculateBezierPoint(bezier_points[0], bezier_points[1], bezier_points[2], bezier_points[3], min(1.0f, t));

            //Set the translation vector of selected disc
            discs[selected_disc]->model_translate = interpolatedPosition - discs[selected_disc]->model_coord;

            if (t >= 1.0f) {
                //Stopping animation case
                anime = false;

                //Actually removing the disc from source tower
                tower_info[cmd_buffer[curr_cmd].src_tower].discs.pop_back();

                //Actually setting disc to dst tower
                tower_info[cmd_buffer[curr_cmd].dst_tower].discs.push_back(selected_disc);

                curr_cmd++;
            }

        }

        // -------------------------------------------------------------------------------
        // 
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

            // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

glm::vec3 calculateBezierPoint(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 D, float t) {
    float u = 1 - t;
    glm::vec3 point =
        u * u * u * A + 3 * u * u * t * B + 3 * u * t * t * C + t * t * t * D;
    return point;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
        leftAltPressed = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    /* if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_RELEASE && leftAltPressed)
     {
         leftAltPressed = false;
         glfwSetCursorEnterCallback(window, cursorEnter_callback);
     }*/

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
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

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        isSpacePressed = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
        isSpacePressed = false;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
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
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    if (isMouseDragging) {
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
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height,
            &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
                width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap failed to load at path: " << faces[i]
                << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,
        GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,
        GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,
        GL_CLAMP_TO_EDGE);
    return textureID;
}
