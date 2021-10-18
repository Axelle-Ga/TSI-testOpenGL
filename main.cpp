#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define ASSERT(x) if (!(x)) __builtin_trap();
#define GlCall(x) GlClearError();\
    x;\
    ASSERT(GlLogCall(#x, __FILE__, __LINE__ ))

static void GlClearError()
{
    while (glGetError() != GL_NO_ERROR);    
}

static bool GlLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "OpenGl_Error : ("<< error << ") :"<< function << " " 
        << file << " : " << line << std::endl;
        return false;
    }  
    return true;  
}



struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

/**
 * @brief Parser of shader file
 * 
 * @param filepath of the shader file
 * @return ShaderProgramSource 
 */
static ShaderProgramSource ParseShader(const std::string filepath)
{   
    std::ifstream stream(filepath);

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while (std::getline(stream, line))
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
            ss[(int)type] << line << '\n';
        }  
        
    }
    
    return { ss[0].str(), ss[1].str()};
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str(); //pointer to the source data
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << 
        (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
        << "shader" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string fragmentShader)
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
    GlCall(glfwMakeContextCurrent(window));

    /* Init Glew */
    if(glewInit()!= GLEW_OK){
        std::cout<<"Glew init not ok"<<std::endl;
    }

    /*Print GL version*/
    std::cout<<glGetString(GL_VERSION)<<std::endl;

    //Vertex position
    float positions[] = {
        -0.5f, -0.5,
         0.5f, -0.5f,
         0.5f, 0.5f,
         -0.5f, 0.5f
    };

    unsigned int indices[] = {
        0,1,2,
        2,3,0
    };
    
    //Definition of the buffer
    unsigned int buffer;
    GlCall(glGenBuffers(1, &buffer ));
    GlCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
    GlCall(glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), positions, GL_STATIC_DRAW));

    GlCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*2, 0));
    GlCall(glEnableVertexAttribArray(0));

    //Index buffer
    unsigned int ibo;
    GlCall(glGenBuffers(1, &ibo ));
    GlCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    GlCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW));


    ShaderProgramSource source = ParseShader("../res/shaders/Basic.shader");

    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    GlCall(glUseProgram(shader));

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        GlCall(glClear(GL_COLOR_BUFFER_BIT));

        //Draw the triangle
        GlCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr ));


        /* Swap front and back buffers */
        GlCall(glfwSwapBuffers(window));

        /* Poll for and process events */
        GlCall(glfwPollEvents());

    }

    GlCall(glDeleteProgram(shader));

    glfwTerminate();
    return 0;
}
