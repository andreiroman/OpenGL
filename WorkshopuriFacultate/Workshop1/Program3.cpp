//.... Creare shader ...........
GLuint vs = glCreateShader (GL_VERTEX_SHADER);
glShaderSource (vs, 1, &vertex_shader, NULL);
glCompileShader (vs);
GLuint fs = glCreateShader (GL_FRAGMENT_SHADER);
glShaderSource (fs, 1, &fragment_shader, NULL);
glCompileShader (fs);
GLuint shader_programme = glCreateProgram ();
glAttachShader (shader_programme, fs);
glAttachShader (shader_programme, vs);
glLinkProgram (shader_programme);



//..... Randare................. 
// stergem ce s-a desenat anterior
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
// spunem ce shader vom folosi pentru desenare
  glUseProgram (shader_programme);
// facem bind la vertex buffer
  glBindBuffer(GL_ARRAY_BUFFER, position_vbo);
  // draw points 0-3 from the currently bound VAO with current in-use shader
  glDrawArrays (GL_TRIANGLES, 0, 3);
......................................  
// facem swap la buffere (Double buffer)
  glfwSwapBuffers (window);