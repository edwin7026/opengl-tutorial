/**
 *@file holy_triangle.cpp
 *@name Edwin Joy
 *@email edwin7026@gmail.com
*/

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

/**
 * 
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

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Initialize GLEW */
    if (glewInit() != GLEW_OK)
        std::cout << "GLEW is not OK!" << std::endl;
    else
        std::cout << "GLEW is OK!" << std::endl;

    std::cout << "Current OpenGL version is: " << glewGetString(GLEW_VERSION);


    /* Positions of the vertices of the triangle to push to the buffer */
    float positions[6] = {
        -0.5f, -0.5f,
		 0.0f,  0.5f,
    	 0.5f, -0.5f
    };

	unsigned int buffer;                        // Buffer ID
    // glGenBuffer takes buffer and updates buffer with a unique ID
	glGenBuffers(1, &buffer);                   // Create buffer
    glBindBuffer(GL_ARRAY_BUFFER, buffer);      // Selecting type of buffer and buffer

    // Creates and initializes buffer object
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

    // Enable the vertex attribute array. Remember: OpenGL specification acts like a state machine
    glEnableVertexAttribArray(0);

    // Give OpenGL the description of the buffer
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    /* The shader code goes here*/

    // Vertex shader describes where to render the triangle
    std::string vertexShader =
        "#version 330 core\n"
        "\n"
        "layout(location = 0) in vec4 position;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = position;\n"
        "}\n";

    // Fragment shader basically descries the coloring and rendering of
    // individual pixels in the triangle
    std::string fragmentShader =
        "#version 330 core\n"
        "\n"
        "layout(location = 0) out vec4 color;\n"
        "void main()\n"
        "{\n"
        "   color = vec4(1.0, 0.0, 0.0, 1.0);\n"
        "}\n";

    // Create shader and get shader ID
    unsigned int shader = CreateShader(vertexShader, fragmentShader);
    glUseProgram(shader);               // Tell OpenGL specification to use that particular shader


	/* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);           // Draw call


        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    // Delete the shader program after use
    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}