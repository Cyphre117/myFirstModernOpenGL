#include <iostream>
#include <fstream>
#include <sstream>
#include <SDL2/SDL.h>
#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

int main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_EVERYTHING);
    // Specify the version of OpenGL we want
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    // Create the window ready for OpenGL rendering
    SDL_Window* window = SDL_CreateWindow("OpenGL", 100, 100, 800, 600, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    
    // Load OpenGL functions
    glewExperimental = GL_TRUE;
    glewInit();

    float verts[] = {
        0.0f,  0.5f,
       -0.5f, -0.5f,
        0.5f, -0.5f
    };

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    {
        // Load the vertex shader from a file
        std::ifstream vs("vertex.glsl");
        if( vs )
            std::cout << "Loaded vertex shader" << std::endl;
        else
            std::cout << "Could not load vertex shader" << std::endl;
        std::stringstream buffer;
        buffer << vs.rdbuf();

        // Compile the vertex shader
        const char* c_buffer = buffer.str().c_str();
        glShaderSource(vertexShader, 1, &c_buffer, NULL);
        glCompileShader(vertexShader);
        
        // Check the shader was compiled correctly
        GLint status;
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
        if( status != GL_TRUE )
        {
            // There was an error compiling the shader
            char buffer[512];
            glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
            std::cout << "VS Shader Error: " << buffer << std::endl;
            return 1;
        }
        else std::cout << "Compiled vertex Shader" << std::endl;
    }
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    {
        // Load the fragment shader from a file
        std::ifstream fs("fragment.glsl");
        if( fs )
            std::cout << "Loaded fragment shader" << std::endl;
        else
            std::cout << "Could not load vertex shader" << std::endl;
        std::stringstream buffer;
        buffer << fs.rdbuf();

        // Compile the fragment shader
        const char* c_buffer = buffer.str().c_str();
        glShaderSource(fragmentShader, 1, &c_buffer, NULL);
        glCompileShader(fragmentShader);

        // Check the shader was compiled succesfully
        GLint status;
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
        if( status != GL_TRUE )
        {
            char buffer[512];
            glGetShaderInfoLog(fragmentShader, 512, NULL, buffer);
            std::cout << "FS Shader Error: " << buffer << std::endl;
            return 1;
        }
        else std::cout << "Compiled fragment Shader" << std::endl;
    }
    
    // Create the combined shader
    GLuint shaderProgram = glCreateProgram();
    glAttachShader( shaderProgram, vertexShader );
    glAttachShader( shaderProgram, fragmentShader );
        
    glBindFragDataLocation( shaderProgram, 0, "outColour" );

    glLinkProgram( shaderProgram );
    glUseProgram( shaderProgram );
    
    // Set the vertex attributes
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    GLint positionAttrib = glGetAttribLocation( shaderProgram, "position" );
    glVertexAttribPointer( positionAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0 );
    glEnableVertexAttribArray( positionAttrib );

    SDL_Event event;
    bool quit = false;
    while( !quit ) // START OF MAIN LOOP
    {
        // Handle window events
        while( SDL_PollEvent( &event ) ) {
            if( event.type == SDL_QUIT ) quit = true;
            if( event.type == SDL_KEYDOWN ) {
                if( event.key.keysym.scancode == SDL_SCANCODE_ESCAPE ) quit = true;
            }
        }
        glClear( GL_COLOR_BUFFER_BIT );
        
        glDrawArrays( GL_TRIANGLES, 0, 3 );
        
        // Show the graphics
        SDL_GL_SwapWindow( window );
    }
    
    // Cleanup
    SDL_GL_DeleteContext(context);
    SDL_Quit();
    return 0;
}
