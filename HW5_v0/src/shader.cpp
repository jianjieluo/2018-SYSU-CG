#include "shader.h"

Shader::Shader(const GLchar * vertexPath, const GLchar * fragmentPath)
{
    // 1. ���ļ�·���л�ȡ����/Ƭ����ɫ��
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // ��֤ifstream��������׳��쳣��
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // ���ļ�
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // ��ȡ�ļ��Ļ������ݵ���������
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // �ر��ļ�������
        vShaderFile.close();
        fShaderFile.close();
        // ת����������string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }

    const char* vertex_shader_src = vertexCode.c_str();
    const char* fragment_shader_src = fragmentCode.c_str();

    GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vshader, 1, &vertex_shader_src, NULL);
    glCompileShader(vshader);

    GLint vertex_compiled;
    glGetShaderiv(vshader, GL_COMPILE_STATUS, &vertex_compiled);
    if (vertex_compiled != GL_TRUE) {
        GLsizei log_length = 0;
        GLchar mes[1024];
        glGetShaderInfoLog(vshader, 1024, NULL, mes);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << mes << std::endl;
    }

    // ����Ƭ����ɫ��
    GLuint  fshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fshader, 1, &fragment_shader_src, NULL);
    glCompileShader(fshader);

    GLint fragment_compiled;
    glGetShaderiv(fshader, GL_COMPILE_STATUS, &fragment_compiled);
    if (fragment_compiled != GL_TRUE) {
        GLsizei log_length = 0;
        GLchar mes[1024];
        glGetShaderInfoLog(fshader, 1024, NULL, mes);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << mes << std::endl;
    }

    // ������ɫ������
    ID = glCreateProgram();
    glAttachShader(ID, vshader);
    glAttachShader(ID, fshader);
    glLinkProgram(ID);

    GLint program_linked;
    glGetProgramiv(ID, GL_LINK_STATUS, &program_linked);
    if (program_linked != GL_TRUE) {
        GLsizei log_length = 0;
        GLchar mes[1024];
        glGetProgramInfoLog(ID, 1024, &log_length, mes);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << mes << std::endl;
    }


    // ɾ��������Ҫʹ�õ���ɫ��
    glDeleteShader(vshader);
    glDeleteShader(fshader);
}

void Shader::use()
{
    glUseProgram(ID);
}

void Shader::setBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat4(const std::string & name, const float vec[]) const
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), vec[0], vec[1], vec[2], vec[3]);
}

void Shader::setMat4(const std::string & name, const float vec[]) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, vec);
}
