/**
 *@file square.cpp
 *@name Edwin Joy
 *@email edwin7026@gmail.com
**/

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string> 
#include <sstream>

#include <Renderer.h>
#include <VertexBuffer.h>
#include <IndexBuffer.h>

// Pretty cool stuff
// Assertion


struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    enum class ShaderType
    {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1
    };

    ShaderType type = ShaderType::NONE;
    std::string line;
    std::stringstream ss[2];
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }
        else
        {
            ss[(int)type] << line << "\n";
        }
    }

    return { ss[0].str(), ss[1].str() };
}

/**
 * Function to compile shaders
 * Also handles errors if any error is found during compilation
 */
static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    // Error handling
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " <<
            (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << message << std::endl;

        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    // Set OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Set OpenGL profile to be Core
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // Control Frame rate
    glfwSwapInterval(1);


    /* Initialize GLEW */
    if (glewInit() != GLEW_OK)
        std::cout << "GLEW is not OK!" << std::endl;
    else
        std::cout << "GLEW is OK!" << std::endl;

    std::cout << "Current OpenGL version is: " << glewGetString(GLEW_VERSION) << std::endl;

    // Because the OpenL context terminates before unbinding the index and vertex buffers
    {
        /* Positions of the vertices of the square to push to the buffer */
        float positions[] = {
            -0.5f, -0.5f,
             0.5f, -0.5f,
             0.5f,  0.5f,
            -0.5f,  0.5,
        };

        // Index buffer
        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        unsigned int vao;
        GLCall(glGenVertexArrays(1, &vao));
        GLCall(glBindVertexArray(vao));

        VertexBuffer vb(positions, 4 * 2 * sizeof(float));

        // Link buffer with the Vertex Array Object vao
        GLCall(glEnableVertexAttribArray(0));
        GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));

        IndexBuffer ib(indices, 6);

        /* The shader code goes here*/
        ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
        unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
        GLCall(glUseProgram(shader));

        // Use of uniforms
        GLCall(int location = glGetUniformLocation(shader, "u_Color"));
        ASSERT(location != -1);
        GLCall(glUniform4f(location, 0.2f, 0.3f, 0.8f, 1.0f));

        GLCall(glBindVertexArray(0));
        GLCall(glUseProgram(0));
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

        float r = 0.0f;
        float increment = 0.05f;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            GLCall(glClear(GL_COLOR_BUFFER_BIT));

            GLCall(glUseProgram(shader));
            GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f))


            GLCall(glBindVertexArray(vao));
            ib.Bind();

            GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));           // Draw call

            if (r > 1.0f)
                increment = -0.05f;
            else if (r < 0.0f)
                increment = 0.05f;

            r = r + increment;

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }

        // Delete the shader program after use
        GLCall(glDeleteProgram(shader));
    }
    glfwTerminate();
    return 0;
}