#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp> 

#include "shader.h"
#include "Mesh.h"
#include "Node.h"
#include "Camera.h"

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#include "Animation.h"
#include "Animator.h"
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

static Shader shader;

glm::mat4 matModelRoot = glm::mat4(1.0);
glm::mat4 matView = glm::mat4(1.0);
glm::mat4 matProj = glm::ortho(-2.0f,2.0f,-2.0f,2.0f, -2.0f,2.0f);

glm::vec3 scale = glm::vec3(1.0, 1.0, 1.0);

// camera settings
glm::vec3 lightPos = glm::vec3(200.0f, 300.0f, 200.0f);
glm::vec3 viewPos = glm::vec3(0.0f, 100.0f, 200.0f);
glm::vec3 viewCenter = glm::vec3(0.0f, 100.0f, 0.0f);
int wView = 800;
int hView = 800;
float fov = 70.0;
float near = 0.1;
float far = 400.0;

// Modern camera system
Camera* camera = nullptr;
bool rightMousePressed = false;
float lastMouseX = 0.0f;
float lastMouseY = 0.0f;

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool animate = false;
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


GLuint blinnShader;
GLuint texblinnShader;
GLuint normalblinnShader;
GLuint boneShader;  // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

// Initialize shader
GLuint initShader(std::string pathVert, std::string pathFrag) 
{
    shader.read_source( pathVert.c_str(), pathFrag.c_str());

    shader.compile();
    glUseProgram(shader.program);

    return shader.program;
}

void setLightPosition(glm::vec3 lightPos)
{
    GLuint lightpos_loc = glGetUniformLocation(shader.program, "lightPos" );
    glUniform3fv(lightpos_loc, 1, glm::value_ptr(lightPos));
}

void setViewPosition(glm::vec3 eyePos)
{
    GLuint viewpos_loc = glGetUniformLocation(shader.program, "viewPos" );
    glUniform3fv(viewpos_loc, 1, glm::value_ptr(eyePos));
}

void window_size_callback(GLFWwindow* window, int w, int h)
{
    //int width, height;
    //glfwGetWindowSize(window, &width, &height);

    wView = w;
    hView = h;

    glViewport(0, 0, w, h);

    matProj = glm::perspective(glm::radians(camera->GetZoom()), w /(float) h, near, far);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (rightMousePressed)
    {
        float xoffset = (float)xpos - lastMouseX;
        float yoffset = lastMouseY - (float)ypos; // reversed since y-coordinates go from bottom to top

        camera->ProcessMouseMovement(xoffset, yoffset);
    }

    lastMouseX = (float)xpos;
    lastMouseY = (float)ypos;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_PRESS)
        {
            rightMousePressed = true;
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            lastMouseX = (float)xpos;
            lastMouseY = (float)ypos;
        }
        else if (action == GLFW_RELEASE)
        {
            rightMousePressed = false;
        }
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    // Scroll wheel now controls acceleration
    camera->ProcessMouseScroll((float)yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    glm::mat4 mat = glm::mat4(1.0);

    float angleStep = 5.0f;
    float transStep = 1.0f;

    if (action == GLFW_PRESS) {

        if (mods & GLFW_MOD_CONTROL) {

            // translation in world space
            if (GLFW_KEY_LEFT == key) {
                mat = glm::translate(glm::mat4(1.0f), glm::vec3(transStep, 0.0f, 0.0f));
                matModelRoot = mat * matModelRoot;
            } else if (GLFW_KEY_RIGHT == key ) {
                mat = glm::translate(glm::mat4(1.0f), glm::vec3(-transStep, 0.0f, 0.0f));
                matModelRoot = mat * matModelRoot;
            } else if (GLFW_KEY_UP == key) {
                mat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, transStep, 0.0f));
                matModelRoot = mat * matModelRoot;
            } if (GLFW_KEY_DOWN == key) {
                mat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -transStep, 0.0f));
                matModelRoot = mat * matModelRoot;
            }
        }

        // camera control
        if (GLFW_KEY_LEFT == key) {
            // pan left, rotate around Y, CCW
            mat = glm::rotate(glm::radians(-angleStep), glm::vec3(0.0, 1.0, 0.0));
            matView = mat * matView;
        } else if (GLFW_KEY_RIGHT == key ) {
            // pan right, rotate around Y, CW
            mat = glm::rotate(glm::radians(angleStep), glm::vec3(0.0, 1.0, 0.0));
            matView = mat * matView;
        } else if (GLFW_KEY_UP == key) {
            // tilt up, rotate around X, CCW
            mat = glm::rotate(glm::radians(-angleStep), glm::vec3(1.0, 0.0, 0.0));
            matView = mat * matView;
        } if (GLFW_KEY_DOWN == key) {
            // tilt down, rotate around X, CW
            mat = glm::rotate(glm::radians(angleStep), glm::vec3(1.0, 0.0, 0.0));
            matView = mat * matView;
        } else if ( (GLFW_KEY_KP_ADD == key) || 
            (GLFW_KEY_EQUAL == key) && (mods & GLFW_MOD_SHIFT) ) {
            // std::cout << "+ pressed" << std::endl;
            // zoom in, move along -Z
            mat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, transStep));
            matView = mat * matView ;
        } else if ( (GLFW_KEY_KP_SUBTRACT == key ) || (GLFW_KEY_MINUS == key) ) {
            // std::cout << "keypad - pressed" << std::endl;
            // zoom out, move along -Z
            mat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -transStep));
            matView = mat * matView;
        } else if (GLFW_KEY_R == key) {
            //std::cout << "R pressed" << std::endl;
            // reset
            matView = glm::lookAt(viewPos, viewCenter, glm::vec3(0, 1, 0)); 
            matModelRoot = glm::mat4(1.0f);
        }

        // translation along camera axis (first person view)
        else if (GLFW_KEY_A == key ) {
            //if (modes & GLFW_MOD_CONTROL)
            // move left along -X
            mat = glm::translate(glm::mat4(1.0f), glm::vec3(transStep, 0.0f, 0.0f));
            matView = mat * matView;
        } else if (GLFW_KEY_D == key) {
            // move right along X
            mat = glm::translate(glm::mat4(1.0f), glm::vec3(-transStep, 0.0f, 0.0f));
            matView = mat * matView;
        } if (GLFW_KEY_W == key ) {
            // move forward along -Z
            mat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, transStep));
            matView = mat * matView;
        } else if (GLFW_KEY_S == key) {
            // move backward along Z
            mat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -transStep));
            matView = mat * matView;
        } 

        // translation along world axis
        else if (GLFW_KEY_H == key ) {
            //if (modes & GLFW_MOD_CONTROL)
            // move left along -X
            mat = glm::translate(glm::mat4(1.0f), glm::vec3(transStep, 0.0f, 0.0f));
            matView = matView * mat;
        } else if (GLFW_KEY_L == key) {
            // move right along X
            mat = glm::translate(glm::mat4(1.0f), glm::vec3(-transStep, 0.0f, 0.0f));
            matView = matView * mat;
        } if (GLFW_KEY_J == key ) {
            // move forward along Z
            mat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -transStep));
            matView = matView * mat;
        } else if (GLFW_KEY_K == key) {
            // move backward along -Z
            mat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, transStep));
            matView = matView * mat;
        } 
    }
    
}

int main()
{
    GLFWwindow *window;

    // GLFW init
    if (!glfwInit())
    {
        std::cout << "glfw failed" << std::endl;
        return -1;
    }

    // create a GLFW window
    window = glfwCreateWindow(wView, hView, "Skeletal Animation Student 100774097", NULL, NULL);
    glfwMakeContextCurrent(window);

    // register the key event callback function
    glfwSetKeyCallback(window, key_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);


    // loading glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Couldn't load opengl" << std::endl;
        glfwTerminate();
        return -1;
    }


    blinnShader = initShader( "shaders/blinn.vert", "shaders/blinn.frag");
    setLightPosition(lightPos);
    setViewPosition(viewPos);

    texblinnShader = initShader("shaders/texblinn.vert", "shaders/texblinn.frag");
    setLightPosition(lightPos);
    setViewPosition(viewPos);

    //normalblinnShader = initShader("shaders/normalblinn2.vert", "shaders/normalblinn2.frag");
    //setLightPosition(lightPos);
    //setViewPosition(viewPos);

    // >>>>>>>>>>>>>>>>>>>>>>>>>
    boneShader = initShader("shaders/bone.vert", "shaders/bone.frag");
    setLightPosition(lightPos);
    setViewPosition(viewPos);
    // <<<<<<<<<<<<<<<<<<<<<<<<<

    // Initialize camera system
    camera = new Camera(viewPos, glm::vec3(0, 1, 0), -90.0f, -25.0f);
    // Base speed is 50 units/second with acceleration controlled by scroll wheel

    // set the Y field of view angle to 60 degrees, width/height ratio to 1.0, and a near plane of 3.5, far plane of 6.5
    matProj = glm::perspective(glm::radians(camera->GetZoom()), wView / (float) hView, near, far);

    std::shared_ptr<Mesh> anim_model = std::make_shared<Mesh>();
    // anim_model->init("models/vampire/dancing_vampire.dae", texblinnShader);
    // anim_model->init("models/mannequin/Capoeira_Mannequin.dae", texblinnShader);

    // >>>>>>>>>>>>>>>>>>>>>>>>>>>
    // there are duplicates in the learnopengl.com design
    // these two classes should be merged into one.
    anim_model->init("models/magic/magic.dae", boneShader);
    Animation danceAnimation("models/magic/magic.dae", anim_model.get());

    // anim_model->init("models/mannequin/Capoeira_Mannequin.dae", boneShader);
    // Animation danceAnimation("models/mannequin/Capoeira_Mannequin.dae", anim_model.get());

    Animator animator(&danceAnimation);
    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<
   
    // setting the background colour, you can change the value
    glClearColor(0.4f, 0.1f, 0.1f, 1.0f);
    
    glEnable(GL_DEPTH_TEST);

    // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    animate = true;
    if (animate) {
        scale = glm::vec3(100.0, 100.0, 100.0);
        matModelRoot = glm::scale(matModelRoot, scale);
    }
    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


    // setting the event loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Process continuous keyboard input for camera movement (WASD for flying)
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera->ProcessKeyboard(Camera::Movement::FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera->ProcessKeyboard(Camera::Movement::BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera->ProcessKeyboard(Camera::Movement::LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera->ProcessKeyboard(Camera::Movement::RIGHT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            camera->ProcessKeyboard(Camera::Movement::UP, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            camera->ProcessKeyboard(Camera::Movement::DOWN, deltaTime);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (animate)
            animator.UpdateAnimation(deltaTime);

        glUseProgram(boneShader);

        // Update view matrix from camera
        matView = camera->GetViewMatrix();
        setViewPosition(camera->Position);

        // update bone matrices in the shader
        auto transforms = animator.GetFinalBoneMatrices();
        for (int i = 0; i < transforms.size(); ++i) {
            glm::mat4 mat = transforms[i];
            std::string name = "finalBonesMatrices[" + std::to_string(i) + "]";
            glUniformMatrix4fv(glGetUniformLocation(boneShader, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }
        // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

        anim_model->draw(matModelRoot, matView, matProj);
        
        glfwSwapBuffers(window);

        //break;
    }

    delete camera;
    glfwTerminate();

    return 0;
}