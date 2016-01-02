#include <iostream>
#include <fstream>
#include <sstream>
#include <SDL2/SDL.h>
#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

GLuint compile_frag_shader( std::string filename );
GLuint compile_vert_shader( std::string filename );

int main(int argc, char* argv[])
{
    // Dont use cmd line ards yet
    for( int i = 0; i < argc; i++ )
        std::cout << argv[i];
    std::cout << std::endl;

    SDL_Init(SDL_INIT_EVERYTHING);
    // Specify the version of OpenGL we want
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    // Create the window ready for OpenGL rendering
    SDL_Window* window = SDL_CreateWindow("OpenGL", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    
    // Load OpenGL functions
    glewExperimental = GL_TRUE;
    glewInit();

    // Create vertex buffer object
    GLuint vbo;
    glGenBuffers(1, &vbo);
    std::cout << "Vertex array buffer: " << vbo << std::endl;

    GLfloat verts[] = {
       -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
        0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
       -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 1.0f, 1.0f, 1.0f
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    // Create element array
    GLuint ebo;
    glGenBuffers( 1, &ebo );
    std::cout << "Element array buffer: " << ebo << std::endl;

    GLushort elements[] = {
        0, 1, 2,
        2, 3, 1
    };

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW );

    GLuint vertexShader = compile_vert_shader("vertex.glsl");
    if( vertexShader == 0 ) return -1;
    GLuint fragmentShader = compile_frag_shader("fragment.glsl");
    if( fragmentShader == 0 ) return -1;
    
    // Create the shader program, attach the vertex and fragment shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader( shaderProgram, vertexShader );
    glAttachShader( shaderProgram, fragmentShader );
    glBindFragDataLocation( shaderProgram, 0, "outColour" );
    glLinkProgram( shaderProgram );
    glUseProgram( shaderProgram );
    
    // Set the vertex attributes, to align with the vertex array
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLint positionAttrib = glGetAttribLocation( shaderProgram, "position" );
    glEnableVertexAttribArray( positionAttrib );
    glVertexAttribPointer( positionAttrib, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), 0 );

    GLint colourAttrib = glGetAttribLocation( shaderProgram, "vColour" );
    glEnableVertexAttribArray( colourAttrib );
    glVertexAttribPointer( colourAttrib, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (void*)(2*sizeof(GLfloat)) );

    // Set uniforms
    GLint brightness = glGetUniformLocation( shaderProgram, "brightness" );
    glUniform1f( brightness, 1.0f );

    // Ensure the element array buffer is bound
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo );

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
        // Clear the window
        glClearColor( 0.2f, 0.1f, 0.2f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT );
        
        glUniform1f( brightness, sinf( SDL_GetTicks() / 1000.0f ) );

        //glDrawArrays( GL_TRIANGLES, 1, 3 );
        glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );
        
        // Show the graphics
        SDL_GL_SwapWindow( window );
    }
    
    // Cleanup
    glDeleteProgram( shaderProgram );
    glDeleteShader( vertexShader );
    glDeleteShader( fragmentShader );

    glDeleteBuffers( 1, &ebo );
    glDeleteBuffers( 1, &vbo );
    glDeleteVertexArrays( 1, &vao );

    SDL_GL_DeleteContext(context);
    SDL_Quit();
    return 0;
}

GLuint compile_vert_shader( std::string filename )
{
    GLuint vertexShader = glCreateShader( GL_VERTEX_SHADER );
    // Load the vertex shader from a file
    std::ifstream vs( filename );
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
        return 0;
    }
    else std::cout << "Compiled vertex Shader" << std::endl;

    return vertexShader;
}

GLuint compile_frag_shader( std::string filename )
{
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // Load the fragment shader from a file
    std::ifstream fs(filename);
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
        return 0;
    }
    else std::cout << "Compiled fragment Shader" << std::endl;

    return fragmentShader;
}
