#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Shader.h"

#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

#include "imgui/imgui.h"
#include "imgui_impl_glfw_gl3.h"

#define IMGUI
//#define DEBUG

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
bool isNeedControlPoints();
vector<glm::vec3>::iterator findPointCanControlled(const float xpos, const float ypos, const float threshold);

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Global value
vector<glm::vec3> p;
vector<glm::vec3>::iterator currPointIter;
bool isLeftButtonPressed = false;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwSetErrorCallback(glfw_error_callback);
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

                                                         // glfw window creation
                                                         // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Bezier Curve", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);


    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 创造着色器程序
    Shader curveShader(".\\Shader\\curveShader.vs", ".\\Shader\\curveShader.fs");
    Shader pointShader(".\\Shader\\pointShader.vs", ".\\Shader\\pointShader.fs");

    // 生成顶点数据 t
    float step = 0.001;
    vector<float> data;
    data.resize(int(1 / step));
    for (int i = 0; i < data.size(); ++i) {
        data[i] = i * step;
    }

    // 所有任务的VBO构建
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // 设置 4 control points
    GLfloat points[] = {
        -0.5f, -0.5f, 0.0f,
        -0.2f, 0.1f, 0.0f,
        0.2f, 0.4f, 0.0f,
        0.5f, -0.2f, 0.0f
    };
    p.clear();
    p.resize(4);
    currPointIter = p.end();

    for (int i = 0; i < 4; ++i) {
        p[i] = glm::vec3(-100.0f, -100.0f, -100.0f);
    }

#ifdef IMGUI
    // Imgui 的设置
    // Setup ImGui binding
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfwGL3_Init(window, true);

    // Setup style
    //ImGui::StyleColorsDark();
    ImGui::StyleColorsClassic();
#endif // IMGUI

    // render loop 控制变量
    bool show_demo_window = false;
    float col1[3] = { 1.0f, 0.5f, 0.2f };

    GLuint pVAO, pVBO;
    glGenVertexArrays(1, &pVAO);
    glGenBuffers(1, &pVBO);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // ------------------------------------------------------------------------------- 
        glfwPollEvents();

        int scr_width, scr_height;
        glfwGetFramebufferSize(window, &scr_width, &scr_height);
        glViewport(0, 0, scr_width, scr_height);

#ifdef IMGUI
        // set up imgui
        ImGui_ImplGlfwGL3_NewFrame();

        // 1. Show a simple window.
        // This is the menu of gui
        // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
        {
            ImGui::Begin("Menu");
            ImGui::Text("Use left mouse button to select or move control points");
            ImGui::Text("Use right mouse button to remove the control points");

            ImGui::ColorEdit3("Bezier Curve Color", col1);
#ifdef DEBUG
            ImGui::Checkbox("Debug", &show_demo_window);
#endif // DEBUG
            ImGui::End();
        }
#endif // IMGUI

        // Rendering
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render Control Points
        if (isLeftButtonPressed) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            if (!isNeedControlPoints()) {
                // record the selected point index
                // 尝试进入控制模式
                currPointIter = findPointCanControlled(xpos, ypos, 180);
                if (currPointIter != p.end())
                    *currPointIter = glm::vec3(xpos, ypos, 0.0f);
            }
        }
        glBindVertexArray(pVAO);
        glBindBuffer(GL_ARRAY_BUFFER, pVBO);
        auto controlPoints2dataVector = []() -> vector<GLfloat> {
            vector<GLfloat> res;
            res.clear();
            for (int i = 0; i < 4; ++i) {
                if (p[i] != glm::vec3(-100.0f, -100.0f, -100.0f)) {
                    res.push_back(p[i].x);
                    res.push_back(p[i].y);
                    res.push_back(p[i].z);
                }
            }
            // 将数据归一化到[-1, 1]
            // 归一化还有问题
            for (int i = 0; i < res.size(); i = i + 3) {
                auto norx = (2 * res[i]) / SCR_WIDTH - 1;
                auto nory = 1 - (2 * res[i + 1]) / SCR_HEIGHT;
                res[i] = norx;
                res[i + 1] = nory;
            }
            return res;
        };
        auto pointData = controlPoints2dataVector();
        glBufferData(GL_ARRAY_BUFFER, pointData.size() * sizeof(GLfloat), pointData.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        pointShader.use();
        glPointSize(5.0f);
        glDrawArrays(GL_POINTS, 0, pointData.size() / 3);
        glBindVertexArray(0);

        auto glfwPos2nocPos = [](const glm::vec3 p) -> glm::vec3 {
            glm::vec3 res;
            res.x = (2 * p.x) / SCR_WIDTH - 1;
            res.y = 1 - (2 * p.y) / SCR_HEIGHT;
            res.z = 0.0f;
            return res;
        };

        // Render Bezier Curve
        if (!isNeedControlPoints()) {
            curveShader.use();
            curveShader.setVec3("p0", glfwPos2nocPos(p[0]));
            curveShader.setVec3("p1", glfwPos2nocPos(p[1]));
            curveShader.setVec3("p2", glfwPos2nocPos(p[2]));
            curveShader.setVec3("p3", glfwPos2nocPos(p[3]));
            curveShader.setFloat3("curveColor", col1);
            glBindVertexArray(VAO);
            glPointSize(1.0f);
            glDrawArrays(GL_POINTS, 0, data.size());
            glBindVertexArray(0);
        }

#ifdef IMGUI
        if (show_demo_window)
        {
            ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
            ImGui::ShowDemoWindow(&show_demo_window);
        }

        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
#endif // IMGUI

        glfwSwapBuffers(window);
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    // Cleanup
#ifdef IMGUI
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();
#endif // IMGUI
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

bool isNeedControlPoints() {
    return find(p.begin(), p.end(), glm::vec3(-100.0f, -100.0f, -100.0f)) != p.end();
}

bool isPointInVector(const float xpos, const float ypos) {
    return find(p.begin(), p.end(), glm::vec3(xpos, ypos, 0.0f)) != p.end();
}

void addPoint(const float xpos, const float ypos) {
    auto emptyItem = find(p.begin(), p.end(), glm::vec3(-100.0f, -100.0f, -100.0f));
    *emptyItem = glm::vec3(xpos, ypos, 0.0f);
}

vector<glm::vec3>::iterator findPointCanControlled(const float xpos, const float ypos, const float threshold) {
    // 粗略查找点击范围中是否有可以控制的点
    vector<glm::vec3>::iterator  res = p.end();
    auto dist = [&xpos, &ypos](const vector<glm::vec3>::iterator iter) -> float {
        return pow((xpos - iter->x), 2) + pow((ypos - iter->y), 2);
    };
    for (auto iter = p.begin(); iter != p.end(); ++iter) {
        auto dis = dist(iter);
        if (dis < threshold) {
            if (res == p.end()) { res = iter; }
            else { res = (dist(res) < dis) ? res : iter; }
        }
    }

    return res;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        // add one point on the canvas  && move the selected points
        if (action == GLFW_PRESS) {
            isLeftButtonPressed = true;
            if (isNeedControlPoints() && !isPointInVector(xpos, ypos)) {
                // add the selected point
                addPoint(xpos, ypos);
#ifdef DEBUG
                cout << "add point" << xpos << "  " << ypos << endl;
#endif // DEBUG
            }
        }

        if (action == GLFW_RELEASE) {
            currPointIter = p.end();
            isLeftButtonPressed = false;
        }
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        // delete one point on the canvas
        auto tempIter = findPointCanControlled(xpos, ypos, 80);
        if (tempIter != p.end()) {
            *tempIter = glm::vec3(-100.0f, -100.0f, -100.0f);
        }
    }
}