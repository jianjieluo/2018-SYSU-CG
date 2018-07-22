#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"

#include "imgui/imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include <iostream>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

// settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 800;

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
    Shader basic_shader = Shader(".\\Shader\\basic.vs", ".\\Shader\\basic.fs");
    Shader bonus_shader = Shader(".\\Shader\\bonus.vs", ".\\Shader\\bonus.fs");

    // 所有任务的VAOs构建
    GLuint VAO[4];
    glGenVertexArrays(4, VAO);
    GLuint VBO[4];
    glGenBuffers(4, VBO);
    GLuint EBO[2];
    glGenBuffers(2, EBO);

    // 三角形模式 mode = 0
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    float v0[] = {
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.0f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(v0), v0, GL_STATIC_DRAW);
    // 链接顶点属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // 画点模式 mode = 1
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    float v1[] = {
        0.0f, 0.0f, 0.0f
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(v1), v1, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    // 画线模式 mode = 2
    glBindVertexArray(VAO[2]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    float v2[] = {
        -0.4f, 0.4f, 0.0f,
        -0.4f, -0.4f, 0.0f,
        0.4f, -0.4f, 0.0f,
        0.4f, 0.4f, 0.0f
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(v2), v2, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
    GLuint i0[] = {
        0, 2,
        1, 3
    };
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(i0), i0, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    
    // 画多个(两个)三角形模式 mode = 3
    glBindVertexArray(VAO[3]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
    float v3[] = {
        -0.3f, 0.3f, 0.0f,
        -0.5f, 0.0f, 0.0f,
        -0.3f, -0.3f, 0.0f,
        0.0f, -0.5f, 0.0f,
        0.3f, -0.3f, 0.0f,
        0.5f, 0.0f, 0.0f,
        0.3f, 0.3f, 0.0f,
        0.0f, 0.5f, 0.0f
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(v3), v3, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
    GLuint i1[] = {
        0, 1, 2,
        2, 3, 4,
        4, 5, 6,
        0, 6, 7
    };
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(i1), i1, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
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
    bool show_demo_window = true;
    ImVec4 clear_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    ImVec4 tri_color = ImVec4(0.5f, 0.0f, 0.0f, 1.0f);

    bool can_change = false; 
    int mode = 0;

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
            if (ImGui::Button("Point"))
                mode = 1;

            ImGui::SameLine();
            if (ImGui::Button("Line"))
                mode = 2;

            ImGui::SameLine();
            if (ImGui::Button("Multi Triangles"))
                mode = 3;

            switch (mode)
            {
            case 0:
                // 单三角形模式下的选择框
                ImGui::Text("If you want to chang the triangle's color,");
                ImGui::Text("select the box below.");
                if (can_change) {
                    ImGui::Checkbox("Unlock!", &can_change);
                }
                else {
                    ImGui::Checkbox("Default Mode. Lock!", &can_change);
                }
                ImGui::ColorEdit3("triangle color", (float*)&tri_color); // Edit 3 floats representing a color
                break;
            case 1:
                ImGui::Text("Draw a point in the middle,");
                ImGui::Text("change the point size in the same time.");
                break;
            case 2:
                ImGui::Text("Draw 2 lines below.");
                break;
            case 3:
                ImGui::Text("Draw 4 triangle below.");
                break;
            default:
                break;
            }
            ImGui::End();
        }

        // Rendering
        //int display_w, display_h;
        //glfwGetFramebufferSize(window, &display_w, &display_h);
        //glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(0);
        switch (mode) {
        // 单纯画一个三角形，可以改变颜色
        case 0:
            basic_shader.use();
            basic_shader.setBool("isdefault", !can_change);

            if (can_change) {
                float curr_tri_color[4];
                curr_tri_color[0] = tri_color.x;
                curr_tri_color[1] = tri_color.y;
                curr_tri_color[2] = tri_color.z;
                curr_tri_color[3] = tri_color.w;
                basic_shader.setFloat4("uni_color", curr_tri_color);
            }

            glBindVertexArray(VAO[mode]);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            break;
        // 画点
        case 1:
            bonus_shader.use();
            glPointSize(5.0f);
            glBindVertexArray(VAO[mode]);
            glDrawArrays(GL_POINTS, 0, 1);
            break;
        // 画线
        case 2:
            bonus_shader.use();
            glBindVertexArray(VAO[mode]);
            glDrawElements(GL_LINES, 4, GL_UNSIGNED_INT, 0);
            break;
        // 画多个三角形
        case 3:
            bonus_shader.use();
            glBindVertexArray(VAO[mode]);
            //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawElements(GL_TRIANGLES, 16, GL_UNSIGNED_INT, 0);
            break;
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
