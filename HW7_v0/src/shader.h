#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>; // ����glad����ȡ���еı���OpenGLͷ�ļ�
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


class Shader
{
public:
    // ����ID
    GLuint ID;

    Shader() {}
    Shader(const Shader& _shader) {
        ID = _shader.ID;
    }
    // ��������ȡ��������ɫ��
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
    // ʹ��/�������
    void use();
    // uniform���ߺ���
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;

    void setFloat3(const std::string &name, const float vec[]) const;
    void setFloat4(const std::string &name, const float vec[]) const;
    void setMat4(const std::string &name, const float vec[]) const;

    void setVec3(const std::string &name, const glm::vec3 &value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec3(const std::string &name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
};

#endif