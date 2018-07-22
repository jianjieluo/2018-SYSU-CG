#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"

#include <iostream>
#include <cmath>
#include <vector>

#include "imgui/imgui.h"
#include "imgui_impl_glfw_gl3.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// settings
const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool isFirstInFPS = false;
int mode = 3;


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
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

                                                         // glfw window creation
                                                         // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "My Window", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

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

    // 创造着色器程序
    Shader my_shader = Shader(".\\Shader\\shader.vs", ".\\Shader\\shader.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // 背面
        -0.2f, -0.2f, -0.2f,  0.8f, 0.2f, 0.2f,
        0.2f, -0.2f, -0.2f,   0.8f, 0.2f, 0.2f,
        0.2f,  0.2f, -0.2f,   0.8f, 0.2f, 0.2f,
        0.2f,  0.2f, -0.2f,   0.8f, 0.2f, 0.2f,
        -0.2f,  0.2f, -0.2f,  0.8f, 0.2f, 0.2f,
        -0.2f, -0.2f, -0.2f,  0.8f, 0.2f, 0.2f,
        // 左
        -0.2f,  0.2f,  0.2f,  0.2f, 0.8f, 0.2f,
        -0.2f,  0.2f, -0.2f,  0.2f, 0.8f, 0.2f,
        -0.2f, -0.2f, -0.2f,  0.2f, 0.8f, 0.2f,
        -0.2f, -0.2f, -0.2f,  0.2f, 0.8f, 0.2f,
        -0.2f, -0.2f,  0.2f,  0.2f, 0.8f, 0.2f,
        -0.2f,  0.2f,  0.2f,  0.2f, 0.8f, 0.2f,
        // 正面
        -0.2f, -0.2f,  0.2f,  0.2f, 0.2f, 0.8f,
        0.2f, -0.2f,  0.2f,   0.2f, 0.2f, 0.8f,
        0.2f,  0.2f,  0.2f,   0.2f, 0.2f, 0.8f,
        0.2f,  0.2f,  0.2f,   0.2f, 0.2f, 0.8f,
        -0.2f,  0.2f,  0.2f,  0.2f, 0.2f, 0.8f,
        -0.2f, -0.2f,  0.2f,  0.2f, 0.2f, 0.8f,
        // 右
        0.2f,  0.2f,  0.2f,   0.2f, 0.8f, 0.8f,
        0.2f,  0.2f, -0.2f,   0.2f, 0.8f, 0.8f,
        0.2f, -0.2f, -0.2f,   0.2f, 0.8f, 0.8f,
        0.2f, -0.2f, -0.2f,   0.2f, 0.8f, 0.8f,
        0.2f, -0.2f,  0.2f,   0.2f, 0.8f, 0.8f,
        0.2f,  0.2f,  0.2f,   0.2f, 0.8f, 0.8f,
        // 下
        -0.2f, -0.2f, -0.2f,  0.8f, 0.2f, 0.8f,
        0.2f, -0.2f, -0.2f,   0.8f, 0.2f, 0.8f,
        0.2f, -0.2f,  0.2f,   0.8f, 0.2f, 0.8f,
        0.2f, -0.2f,  0.2f,   0.8f, 0.2f, 0.8f,
        -0.2f, -0.2f,  0.2f,  0.8f, 0.2f, 0.8f,
        -0.2f, -0.2f, -0.2f,  0.8f, 0.2f, 0.8f,
        // 上
        -0.2f,  0.2f, -0.2f,  0.8f, 0.8f, 0.2f,
        0.2f,  0.2f, -0.2f,   0.8f, 0.8f, 0.2f,
        0.2f,  0.2f,  0.2f,   0.8f, 0.8f, 0.2f,
        0.2f,  0.2f,  0.2f,   0.8f, 0.8f, 0.2f,
        -0.2f,  0.2f,  0.2f,  0.8f, 0.8f, 0.2f,
        -0.2f,  0.2f, -0.2f,  0.8f, 0.8f, 0.2f
    };
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // Imgui 的设置
    // Setup ImGui binding
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfwGL3_Init(window, true);

    // Setup style
    //ImGui::StyleColorsDark();
    ImGui::StyleColorsClassic();

    // render loop 控制变量
    bool show_demo_window = false;

    float camPos[3] = { 0.0f, 0.0f, 3.0f };
    float cubeCenter[3] = { -1.5f, 0.5f, -1.5f };
    float lookAtCenter[3] = {0.0f, 0.0f, 0.0f};
    float ortho[4] = { -3.0f, 3.0f, -3.0f, 3.0f };
    float perspect[3] = {45.0f, 1.0f, 1.0f};
    float near_far[2] = { 0.1f, 100.0f };

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // set up imgui 
        ImGui_ImplGlfwGL3_NewFrame();

        // 1. Show a simple window.
        // This is the menu of gui
        // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".

        ImGui::Begin("Menu");

        //// 模式选择框
        //ImGui::Checkbox("Debug", &show_demo_window);
        ImGui::RadioButton("Ortho Projection", &mode, 0);
        ImGui::RadioButton("Perspective Projection", &mode, 1);
        ImGui::RadioButton("View changing", &mode, 2);
        ImGui::RadioButton("Bonus", &mode, 3);

        switch (mode)
        {
        case 0:
            ImGui::Text("orthographic projection");
            break;
        case 1:
            ImGui::Text("perspective projection");
            break;
        case 2:
            ImGui::Text("View Changing");
            break;
        case 3:
            isFirstInFPS = true;
            break;
        default:
            break;
        }

        if (mode != 3) {
            ImGui::InputFloat3("Cube center translate", cubeCenter, 2);
            ImGui::InputFloat3("Camera Position", camPos, 2);
            ImGui::InputFloat3("Look at center", lookAtCenter, 2);
            ImGui::InputFloat4("Othro paras", ortho, 2);
            ImGui::InputFloat3("Perspect paras", perspect, 2);
            ImGui::InputFloat2("Near and Far", near_far, 2);
        }

        ImGui::End();


        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;

        float Radius = 5.0f;
        float camPosX = sin(glfwGetTime()) * Radius;
        float camPosZ = cos(glfwGetTime()) * Radius;

        switch (mode) {
        case 0:
            model = glm::translate(model, glm::vec3(cubeCenter[0], cubeCenter[1], cubeCenter[2]));
            view = glm::lookAt(
                glm::vec3(camPos[0], camPos[1], camPos[2]), 
                glm::vec3(lookAtCenter[0], lookAtCenter[1], lookAtCenter[2]), 
                glm::vec3(0.0f, 1.0f, 0.0f)
            );
            projection = glm::ortho(ortho[0], ortho[1], ortho[2], ortho[3], near_far[0], near_far[1]);
            break;
        case 1:
            model = glm::translate(model, glm::vec3(cubeCenter[0], cubeCenter[1], cubeCenter[2]));
            view = glm::lookAt(
                glm::vec3(camPos[0], camPos[1], camPos[2]),
                glm::vec3(lookAtCenter[0], lookAtCenter[1], lookAtCenter[2]),
                glm::vec3(0.0f, 1.0f, 0.0f)
            );
            projection = glm::perspective(perspect[0], perspect[1] / perspect[2], near_far[0], near_far[1]);
            break;
        case 2:
            view = glm::lookAt(glm::vec3(camPosX, 0.0f, camPosZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            projection = glm::perspective(45.0f, (float)SCR_WIDTH / (float)SCR_WIDTH, 0.1f, 100.0f);
            break;
        case 3:
            if (isFirstInFPS) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
            view = camera.GetViewMatrix();
            projection = glm::perspective(45.0f, (float)SCR_WIDTH / (float)SCR_WIDTH, 0.1f, 100.0f);
        default:
            break;
        }

        my_shader.use();
        my_shader.setMat4("model", glm::value_ptr(model));
        my_shader.setMat4("view", glm::value_ptr(view));
        my_shader.setMat4("projection", glm::value_ptr(projection));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        // Imgui render
        if (show_demo_window)
        {
            ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
            ImGui::ShowDemoWindow(&show_demo_window);
        }


        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.moveForward(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.moveBack(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.moveLeft(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.moveRight(deltaTime);

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        isFirstInFPS = false;
        mode = 0;
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


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
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