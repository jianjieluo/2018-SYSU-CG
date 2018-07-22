#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"

#include "imgui/imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include "OnlyPoints.h"
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

void pointData2vao(const GLuint& VAO, const GLuint& VBO, std::vector<GLfloat>& data) {
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

static void ShowHelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "MyWindow", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 创造着色器程序
    Shader my_shader = Shader(".\\Shader\\shader.vs", ".\\Shader\\shader.fs");

    // 所有任务的VAOs构建
    GLuint VAO[2];
    GLuint VBO[2];
    glGenVertexArrays(2, VAO);
    glGenBuffers(2, VBO);

    // Mode 1: Triangle
    GLfloat tri2dVex[] = {
        -150.0, -150.0,
        20.0, 100.0,
        200.0, -70.0
    };
    bool isFilled = false;
    auto triData = Bresenham::genTriangleData(
        Point(tri2dVex[0], tri2dVex[1]),
        Point(tri2dVex[2], tri2dVex[3]),
        Point(tri2dVex[4], tri2dVex[5]),
        isFilled
    );
    pointData2vao(VAO[0], VBO[0], Utils::scrCoor2glCoor(triData, SCR_WIDTH, SCR_HEIGHT));

    // Mode 2: Circle
    // input paras
    GLint radius = 100;
    Point origin = Point(0.0f, 0.0f);

    auto circleData = Bresenham::genCircleData(origin, radius);
    pointData2vao(VAO[1], VBO[1], Utils::scrCoor2glCoor(circleData, SCR_WIDTH, SCR_HEIGHT));

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
    ImVec4 clear_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

    int mode = 0;
    bool isChecked = isFilled;

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

        // set up imgui
        ImGui_ImplGlfwGL3_NewFrame();

        // 1. Show a simple window.
        // This is the menu of gui
        // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
        {
            ImGui::Begin("Menu");

            ImGui::Text("I want HIGH SCORE!! Thank you TAs~");
            ImGui::Text("Choose the following mode you want.");

            // 模式选择框
            if (ImGui::Button("Triangle"))
                mode = 0;

            ImGui::SameLine();
            if (ImGui::Button("Circle"))
                mode = 1;

            int curr_radius = radius;
            switch (mode)
            {
            case 0:
                // 单三角形模式下的选择框
                ImGui::Checkbox("Is Filled", &isChecked);
                if (isChecked != isFilled) {
                    triData.clear();
                    triData = Bresenham::genTriangleData(
                        Point(tri2dVex[0], tri2dVex[1]),
                        Point(tri2dVex[2], tri2dVex[3]),
                        Point(tri2dVex[4], tri2dVex[5]),
                        isChecked
                    );
                    pointData2vao(VAO[0], VBO[0], Utils::scrCoor2glCoor(triData, scr_width, scr_height));
                    isFilled = isChecked;
                }
                break;
            case 1:
                ImGui::Text("Draw a Circle");
                ImGui::InputInt("Radius", &curr_radius);
                ImGui::SameLine(); ShowHelpMarker("You can apply arithmetic operators +,*,/ on numerical values.\n  e.g. [ 100 ], input \'*2\', result becomes [ 200 ]\nUse +- to subtract.\n");

                if (curr_radius < 2) {
                    ImGui::Text("Circle's radius isn't less than 2");
                }
                else {
                    if (curr_radius != radius) {
                        circleData.clear();
                        circleData = Bresenham::genCircleData(origin, curr_radius);
                        radius = curr_radius;
                        pointData2vao(VAO[1], VBO[1], Utils::scrCoor2glCoor(circleData, scr_width, scr_height));
                    }
                }
                break;
            default:
                break;
            }
            ImGui::End();
        }

        // Rendering
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        my_shader.use();
        switch (mode) {
        case 0:
            glBindVertexArray(VAO[mode]);
            glDrawArrays(GL_POINTS, 0, triData.size() / 3);
            break;
        case 1:
            glBindVertexArray(VAO[mode]);
            glDrawArrays(GL_POINTS, 0, circleData.size() / 3);
            break;
        default:
            break;
        }

        if (show_demo_window)
        {
            ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
            ImGui::ShowDemoWindow(&show_demo_window);
        }

        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    // Cleanup
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
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
