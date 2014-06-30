void glfw_error_callback (int error, const char* description) {
  gl_log_err ("GLFW ERROR: code %i msg: %s\n", error, description);
}
//.................................
glfwSetErrorCallback (glfw_error_callback);

//............Antialiasing...............
glfwWindowHint (GLFW_SAMPLES, 4);
// ........Resolution si Fullscreen..........
GLFWmonitor* mon = glfwGetPrimaryMonitor ();
const GLFWvidmode* vmode = glfwGetVideoMode (mon);
GLFWwindow* window = glfwCreateWindow (
  vmode->width, vmode->height, "Extended GL Init", mon, NULL
);
//................. Keys Exiting.................. 
//http://www.glfw.org/docs/latest/group__input.html sau autocomplete :)
if (GLFW_PRESS == glfwGetKey (window, GLFW_KEY_ESCAPE)) {
  glfwSetWindowShouldClose (window, 1);
}
//........................Resize fereastra ......................
// daca face cineva resize pe fereastra ar trebui sa stim ca sa putem recalcula
// viewportul si coordonatele mouseului in scena
int g_gl_width = 640;
int g_gl_height = 480;

// se va chema aceasta functie pe window resize 
void glfw_window_size_callback (GLFWwindow* window, int width, int height) {
  g_gl_width = width;
  g_gl_height = height;
  
  /* putem modifica aici matricile de perspectiva */
}
// setam callbackul 
 glfwSetWindowSizeCallback (window, glfw_window_size_callback);

 // ....................Calculam FPS-ul si il afisam in titlul ferestrei.................
 void _update_fps_counter (GLFWwindow* window) {
  static double previous_seconds = glfwGetTime ();
  static int frame_count;
  double current_seconds = glfwGetTime ();
  double elapsed_seconds = current_seconds - previous_seconds;
  if (elapsed_seconds > 0.25) {
    previous_seconds = current_seconds;
    double fps = (double)frame_count / elapsed_seconds;
    char tmp[128];
    sprintf (tmp, "opengl @ fps: %.2f", fps);
    glfwSetWindowTitle (window, tmp);
    frame_count = 0;
  }
  frame_count++;
}
.......
 while (!glfwWindowShouldClose (window)) {
  _update_fps_counter (window);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport (0, 0, g_gl_width, g_gl_height);
  
  /* DRAW STUFF HERE */
  
  glfwSwapBuffers (window);
  glfwPollEvents ();
  if (GLFW_PRESS == glfwGetKey (window, GLFW_KEY_ESCAPE)) {
    glfwSetWindowShouldClose (window, 1);
  }
}
 