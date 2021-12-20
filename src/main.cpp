/*****************************************************************************\
 * TP CPE, 4ETI, TP synthese d'images
 * --------------
 * Luc Anchling & Timothée Teyssier
 * Programme principal des appels OpenGL
 \*****************************************************************************/

#include "declaration.h"


// Importation des différents modules utilisés 
#include <random>
#include <iostream>
#include <cstdlib>

//identifiant des shaders
GLuint shader_program_id;
GLuint gui_program_id;

camera cam;

// Pour donner le nombre d'objets présents au total
const int nb_obj = 25;
objet3d obj[nb_obj];

const int nb_text = 2;
text text_to_draw[nb_text];




/************************************************************************\
* variables globales*
\*************************************************************************/
float d_angle=0.1f;
float angle_x=0.0f;
float angle_y=0.0f;
float dL=0.05f;      //deplacement de translation

// Pour la taille de la window - Pour permettre de les actualiser in real time 
int WIDTH = 600;
int HEIGHT = 600;

//boolean crees pour gerer le deplacement
bool left=false;
bool right=false;
bool up=false;
bool down=false;
bool fps_right=false;
bool fps_left=false;
bool jump=false;

// Position de la caméra - pour actualiser la position de son centre de rotation
float cam_posX = 0.0f;
float cam_posZ = 0.0f;

//les parametres de translations
float translation_x=0.0f;
float translation_y=0.0f;
float translation_z=-3.0f;
float d_jump=1.0f;
/*****************************************************************************\
* initialisation                                                              *
\*****************************************************************************/
static void init()
{
  shader_program_id = glhelper::create_program_from_file("shaders/shader.vert", "shaders/shader.frag"); CHECK_GL_ERROR();

  cam.projection = matrice_projection(60.0f*M_PI/180.0f,1.0f,0.01f,100.0f);
  cam.tr.translation = vec3(0.0f, 1.0f, 0.0f);
  // cam.tr.translation = vec3(0.0f, 20.0f, 0.0f);
  // cam.tr.rotation_center = vec3(0.0f, 20.0f, 0.0f);
  // cam.tr.rotation_euler = vec3(M_PI/2., 0.0f, 0.0f);

  init_model_1();
  init_model_2();
  init_model_3();
  init_model_4();

  // Pour supprimer le curseur à l'écran
  glutSetCursor(GLUT_CURSOR_NONE);

  gui_program_id = glhelper::create_program_from_file("shaders/gui.vert", "shaders/gui.frag"); CHECK_GL_ERROR();
  // Partie affichage de mot à l'écran

  // text_to_draw[0].value = "CPE";
  // text_to_draw[0].bottomLeft = vec2(-0.2, 0.5);
  // text_to_draw[0].topRight = vec2(0.2, 1);
  // init_text(text_to_draw);

  // text_to_draw[1]=text_to_draw[0];
  // text_to_draw[1].value = "Lyon";
  // text_to_draw[1].bottomLeft.y = 0.0f;
  // text_to_draw[1].topRight.y = 0.5f;
}

/*****************************************************************************\
* display_callback                                                           *
\*****************************************************************************/
 static void display_callback()
{
  glClearColor(0.5f, 0.6f, 0.9f, 1.0f); CHECK_GL_ERROR();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); CHECK_GL_ERROR();

  for(int i = 0; i < nb_obj; ++i)
    draw_obj3d(obj + i, cam);

  for(int i = 0; i < nb_text; ++i)
    draw_text(text_to_draw + i);

  glutSwapBuffers();


}

/*****************************************************************************\
* keyboard_callback                                                           *
\*****************************************************************************/
static void keyboard_callback(unsigned char key, int, int)
{
  switch (key)
  {
    case 'p':
      glhelper::print_screen();
      break;
    //case 'q':
    case 'Q':
    case 27:
      exit(0);
      break;
    case 'i':
        fps_left=true;  //provisoire camera fps tourne à gauche
        break;
    case 'o':
        fps_right=true;   //provisoire camera fps tourne à droite
        break;
    
    case 'z':
            //cam.tr.translation.z-=dL;
        up=true;  //rotation avec la touche du haut
        break;
    case 's':
            //cam.tr.translation.z+=dL; 
        down=true;  //rotation avec la touche du bas
        break;
    case 'q':
              //cam.tr.translation.x-=dL; 
        left=true;  //rotation avec la touche de gauche
        break;
    case 'd':
            //cam.tr.translation.x+=dL; 
        right=true;  //rotation avec la touche de droite
        break;
    case ' ':
        jump=true;
        break;
  }
}

/*****************************************************************************\
* fonction pour éviter le temps de latence au début                           *
\*****************************************************************************/

static void deplacement() 
{

  if (left==true) {
    cam.tr.translation.x-=dL;
    cam_posX -= dL;
    }
  if (right==true) {
    cam.tr.translation.x+=dL;
    cam_posX += dL;
    }
  if (up==true) {
    cam.tr.translation.z-=dL;
    cam_posZ -= dL;
    }
  if (down==true) {
    cam.tr.translation.z+=dL;
    cam_posZ += dL;
    }

  // Déplacement sur l'axe des Y pour tester la scène avant implémentation des mouvements de souris 
  if (fps_left==true) cam.tr.rotation_euler.y-=d_angle; 
  if (fps_right==true) cam.tr.rotation_euler.y+=d_angle;

}


static void sauter()
{ float hauteur_cam=cam.tr.translation.y;
  if (jump==true && cam.tr.translation.y<15.0f) cam.tr.translation.y+=d_jump;
  if (jump==false) {
    if (cam.tr.translation.y>d_jump) cam.tr.translation.y-=d_jump;
  }
}



static void keyboard_relache(unsigned char key, int,int)
{

  switch (key)
  {
    case 'z':
      //translation_y+=dL; //rotation avec la touche du haut
      //std::cout << "relache" << std::endl; //test pour voir si la fonction relache est bien appelé
      up = false;
      break;
    case 's':
      //translation_y-=dL; //rotation avec la touche du bas
      down = false;
      break;
    case 'q':
      //translation_x-=dL; //rotation avec la touche de gauche
      left = false;
      break;
    case 'd':
      //translation_x+=dL; //rotation avec la touche de droite
      right = false;
      break;
    case 'i':
      fps_left = false;
      break;
    case 'o':
      fps_right = false;
      break;

    case ' ':
      std::cout << "j'ai saute" << std::endl;
      std::cout << cam.tr.translation.y << std::endl;
      jump = false;
      break;
  }
}
/*****************************************************************************\
* special_callback                                                            *
\*****************************************************************************/
static void special_callback(int key, int, int)
{
  switch (key)
  {

    case GLUT_KEY_UP:

        break;
      case GLUT_KEY_DOWN:

       break;
     case GLUT_KEY_LEFT:

       break;
     case GLUT_KEY_RIGHT:

       break;
  }
}

/*****************************************************************************\
* Réactualisation de la taille de la fenêtre in real time                     *
\*****************************************************************************/
static void size_actualisation() {
  WIDTH = glutGet(GLUT_WINDOW_WIDTH);
  HEIGHT = glutGet(GLUT_WINDOW_HEIGHT);
  // printf("WIDTH = %d  HEIGTH = %d\n",WIDTH,HEIGHT);
}

/*****************************************************************************\
* Rotation via l'utilisation de la souris                                     *
\*****************************************************************************/
bool capture = false; // flag pour remettre à zero ou non les coord
static void mouse_cursor(int x, int y) { 
  if (capture) {
    int tempX = x;
    int tempY = HEIGHT - y;
    
    
    cam.tr.rotation_euler.y -= 0.001f * d_angle * 2*M_PI*float(HEIGHT / 2 - tempX);
    cam.tr.rotation_euler.x += 0.001f * d_angle * 2*M_PI*float(WIDTH / 2 - tempY);
    
    //printf("x = %d\ty = %d\n",x,y);
    //printf("WIDTH = %d  HEIGTH = %d\n",WIDTH,HEIGHT);
    
    glutWarpPointer(WIDTH / 2, HEIGHT / 2); //  Pour ramener le pointeur au centre de la fenêtre
    
    
  }
};

// gestion du flag pour ne pas faire le refresh h24
static void change_capture() {
  capture = !capture;
}
/*****************************************************************************\
* timer_callback                                                              *
\*****************************************************************************/
static void timer_callback(int)
{
  glutTimerFunc(10, timer_callback, 0);
  deplacement();
  sauter();
  glutPostRedisplay();
  size_actualisation();
  change_capture();
  
  /*
  // Réactualisation du centre de rotation de la caméra
  cam.tr.rotation_center = vec3(cam_posX, 2.0f, cam_posZ);
  */  
}






/*****************************************************************************\
* main                                                                         *
\*****************************************************************************/
int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | MACOSX_COMPATIBILITY);
  glutInitWindowSize(WIDTH, HEIGHT);
  glutCreateWindow("OpenGL");

  glutDisplayFunc(display_callback);
  glutKeyboardUpFunc(keyboard_relache);
  glutKeyboardFunc(keyboard_callback);
  glutSpecialFunc(special_callback);
  
  // gestion de la souris
  //glutMouseFunc(glutMouseFunc);
  glutPassiveMotionFunc(mouse_cursor);


  glutTimerFunc(25, timer_callback, 0);


  glewExperimental = true;
  glewInit();

  std::cout << "OpenGL: " << (GLchar *)(glGetString(GL_VERSION)) << std::endl;

  init();
  glutMainLoop();

  return 0;
}

/*****************************************************************************\
* draw_text                                                                   *
\*****************************************************************************/
void draw_text(const text * const t)
{
  if(!t->visible) return;
  
  glDisable(GL_DEPTH_TEST);
  glUseProgram(t->prog);

  vec2 size = (t->topRight - t->bottomLeft) / float(t->value.size());
  
  GLint loc_size = glGetUniformLocation(gui_program_id, "size"); CHECK_GL_ERROR();
  if (loc_size == -1) std::cerr << "Pas de variable uniforme : size" << std::endl;
  glUniform2f(loc_size,size.x, size.y);     CHECK_GL_ERROR();

  glBindVertexArray(t->vao);                CHECK_GL_ERROR();
  
  for(unsigned i = 0; i < t->value.size(); ++i)
  {
    GLint loc_start = glGetUniformLocation(gui_program_id, "start"); CHECK_GL_ERROR();
    if (loc_start == -1) std::cerr << "Pas de variable uniforme : start" << std::endl;
    glUniform2f(loc_start,t->bottomLeft.x+i*size.x, t->bottomLeft.y);    CHECK_GL_ERROR();

    GLint loc_char = glGetUniformLocation(gui_program_id, "c"); CHECK_GL_ERROR();
    if (loc_char == -1) std::cerr << "Pas de variable uniforme : c" << std::endl;
    glUniform1i(loc_char, (int)t->value[i]);    CHECK_GL_ERROR();
    glBindTexture(GL_TEXTURE_2D, t->texture_id);                            CHECK_GL_ERROR();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);                    CHECK_GL_ERROR();
  }
}

/*****************************************************************************\
* draw_obj3d                                                                  *
\*****************************************************************************/
void draw_obj3d(const objet3d* const obj, camera cam)
{
  if(!obj->visible) return;

  glEnable(GL_DEPTH_TEST);
  glUseProgram(obj->prog);
  
  {
    GLint loc_projection = glGetUniformLocation(shader_program_id, "projection"); CHECK_GL_ERROR();
    if (loc_projection == -1) std::cerr << "Pas de variable uniforme : projection" << std::endl;
    glUniformMatrix4fv(loc_projection,1,false,pointeur(cam.projection));    CHECK_GL_ERROR();

    GLint loc_rotation_view = glGetUniformLocation(shader_program_id, "rotation_view"); CHECK_GL_ERROR();
    if (loc_rotation_view == -1) std::cerr << "Pas de variable uniforme : rotation_view" << std::endl;
    mat4 rotation_x = matrice_rotation(cam.tr.rotation_euler.x, 1.0f, 0.0f, 0.0f);
    mat4 rotation_y = matrice_rotation(cam.tr.rotation_euler.y, 0.0f, 1.0f, 0.0f);
    mat4 rotation_z = matrice_rotation(cam.tr.rotation_euler.z, 0.0f, 0.0f, 1.0f);
    glUniformMatrix4fv(loc_rotation_view,1,false,pointeur(rotation_x*rotation_y*rotation_z));    CHECK_GL_ERROR();

    vec3 cv = cam.tr.rotation_center;
    GLint loc_rotation_center_view = glGetUniformLocation(shader_program_id, "rotation_center_view"); CHECK_GL_ERROR();
    if (loc_rotation_center_view == -1) std::cerr << "Pas de variable uniforme : rotation_center_view" << std::endl;
    glUniform4f(loc_rotation_center_view , cv.x,cv.y,cv.z , 0.0f); CHECK_GL_ERROR();

    vec3 tv = cam.tr.translation;
    GLint loc_translation_view = glGetUniformLocation(shader_program_id, "translation_view"); CHECK_GL_ERROR();
    if (loc_translation_view == -1) std::cerr << "Pas de variable uniforme : translation_view" << std::endl;
    glUniform4f(loc_translation_view , tv.x,tv.y,tv.z , 0.0f); CHECK_GL_ERROR();
  }
  {
    GLint loc_rotation_model = glGetUniformLocation(obj->prog, "rotation_model"); CHECK_GL_ERROR();
    if (loc_rotation_model == -1) std::cerr << "Pas de variable uniforme : rotation_model" << std::endl;
    mat4 rotation_x = matrice_rotation(obj->tr.rotation_euler.x, 1.0f, 0.0f, 0.0f);
    mat4 rotation_y = matrice_rotation(obj->tr.rotation_euler.y, 0.0f, 1.0f, 0.0f);
    mat4 rotation_z = matrice_rotation(obj->tr.rotation_euler.z, 0.0f, 0.0f, 1.0f);
    glUniformMatrix4fv(loc_rotation_model,1,false,pointeur(rotation_x*rotation_y*rotation_z));    CHECK_GL_ERROR();

    vec3 c = obj->tr.rotation_center;
    GLint loc_rotation_center_model = glGetUniformLocation(obj->prog, "rotation_center_model");   CHECK_GL_ERROR();
    if (loc_rotation_center_model == -1) std::cerr << "Pas de variable uniforme : rotation_center_model" << std::endl;
    glUniform4f(loc_rotation_center_model , c.x,c.y,c.z , 0.0f);                                  CHECK_GL_ERROR();

    vec3 t = obj->tr.translation;
    GLint loc_translation_model = glGetUniformLocation(obj->prog, "translation_model"); CHECK_GL_ERROR();
    if (loc_translation_model == -1) std::cerr << "Pas de variable uniforme : translation_model" << std::endl;
    glUniform4f(loc_translation_model , t.x,t.y,t.z , 0.0f);                                     CHECK_GL_ERROR();
  }
  glBindVertexArray(obj->vao);                                              CHECK_GL_ERROR();

  glBindTexture(GL_TEXTURE_2D, obj->texture_id);                            CHECK_GL_ERROR();
  glDrawElements(GL_TRIANGLES, 3*obj->nb_triangle, GL_UNSIGNED_INT, 0);     CHECK_GL_ERROR();
}

void init_text(text *t){
  vec3 p0=vec3( 0.0f, 0.0f, 0.0f);
  vec3 p1=vec3( 0.0f, 1.0f, 0.0f);
  vec3 p2=vec3( 1.0f, 1.0f, 0.0f);
  vec3 p3=vec3( 1.0f, 0.0f, 0.0f);

  vec3 geometrie[4] = {p0, p1, p2, p3}; 
  triangle_index index[2] = { triangle_index(0, 1, 2), triangle_index(0, 2, 3)};

  glGenVertexArrays(1, &(t->vao));                                              CHECK_GL_ERROR();
  glBindVertexArray(t->vao);                                                  CHECK_GL_ERROR();

  GLuint vbo;
  glGenBuffers(1, &vbo);                                                       CHECK_GL_ERROR();
  glBindBuffer(GL_ARRAY_BUFFER,vbo);                                          CHECK_GL_ERROR();
  glBufferData(GL_ARRAY_BUFFER,sizeof(geometrie),geometrie,GL_STATIC_DRAW);   CHECK_GL_ERROR();

  glEnableVertexAttribArray(0); CHECK_GL_ERROR();
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); CHECK_GL_ERROR();

  GLuint vboi;
  glGenBuffers(1,&vboi);                                                      CHECK_GL_ERROR();
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi);                                 CHECK_GL_ERROR();
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(index),index,GL_STATIC_DRAW);   CHECK_GL_ERROR();

  t->texture_id = glhelper::load_texture("data/fontB.tga");

  t->visible = true;
  t->prog = gui_program_id;
}

GLuint upload_mesh_to_gpu(const mesh& m)
{
  GLuint vao, vbo, vboi;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1,&vbo);                                 CHECK_GL_ERROR();
  glBindBuffer(GL_ARRAY_BUFFER,vbo); CHECK_GL_ERROR();
  glBufferData(GL_ARRAY_BUFFER,m.vertex.size()*sizeof(vertex_opengl),&m.vertex[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  glEnableVertexAttribArray(0); CHECK_GL_ERROR();
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), 0); CHECK_GL_ERROR();

  glEnableVertexAttribArray(1); CHECK_GL_ERROR();
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(vertex_opengl), (void*)sizeof(vec3)); CHECK_GL_ERROR();

  glEnableVertexAttribArray(2); CHECK_GL_ERROR();
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(2*sizeof(vec3))); CHECK_GL_ERROR();

  glEnableVertexAttribArray(3); CHECK_GL_ERROR();
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(3*sizeof(vec3))); CHECK_GL_ERROR();

  glGenBuffers(1,&vboi); CHECK_GL_ERROR();
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi); CHECK_GL_ERROR();
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,m.connectivity.size()*sizeof(triangle_index),&m.connectivity[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  return vao;
}

void init_model_1()
{
  // Chargement d'un maillage a partir d'un fichier
  mesh m = load_obj_file("data/stegosaurus.obj");

  // Affecte une transformation sur les sommets du maillage
  float s = 0.2f;
  mat4 transform = mat4(   s, 0.0f, 0.0f, 0.0f,
      0.0f,    s, 0.0f, 0.0f,
      0.0f, 0.0f,   s , 0.0f,
      0.0f, 0.0f, 0.0f, 1.0f);
  apply_deformation(&m,transform);

  // Centre la rotation du modele 1 autour de son centre de gravite approximatif
  obj[0].tr.rotation_center = vec3(0.0f,0.0f,0.0f);

  update_normals(&m);
  fill_color(&m,vec3(1.0f,1.0f,1.0f));

  obj[0].vao = upload_mesh_to_gpu(m);

  obj[0].nb_triangle = m.connectivity.size();
  obj[0].texture_id = glhelper::load_texture("data/stegosaurus.tga");
  obj[0].visible = true;
  obj[0].prog = shader_program_id;

  obj[0].tr.translation = vec3(-2.0, 0.0, -10.0);
}

void init_model_2()
{

  mesh m;

  //coordonnees geometriques des sommets
  vec3 p0=vec3(-25.0f,0.0f,-25.0f);
  vec3 p1=vec3( 25.0f,0.0f,-25.0f);
  vec3 p2=vec3( 25.0f,0.0f, 25.0f);
  vec3 p3=vec3(-25.0f,0.0f, 25.0f);

  //normales pour chaque sommet
  vec3 n0=vec3(0.0f,1.0f,0.0f);
  vec3 n1=n0;
  vec3 n2=n0;
  vec3 n3=n0;

  //couleur pour chaque sommet
  vec3 c0=vec3(1.0f,1.0f,1.0f);
  vec3 c1=c0;
  vec3 c2=c0;
  vec3 c3=c0;

  //texture du sommet
  vec2 t0=vec2(0.0f,0.0f);
  vec2 t1=vec2(1.0f,0.0f);
  vec2 t2=vec2(1.0f,1.0f);
  vec2 t3=vec2(0.0f,1.0f);

  vertex_opengl v0=vertex_opengl(p0,n0,c0,t0);
  vertex_opengl v1=vertex_opengl(p1,n1,c1,t1);
  vertex_opengl v2=vertex_opengl(p2,n2,c2,t2);
  vertex_opengl v3=vertex_opengl(p3,n3,c3,t3);

  m.vertex = {v0, v1, v2, v3};

  //indice des triangles
  triangle_index tri0=triangle_index(0,1,2);
  triangle_index tri1=triangle_index(0,2,3);  
  m.connectivity = {tri0, tri1};

  obj[1].nb_triangle = 2;
  obj[1].vao = upload_mesh_to_gpu(m);

  obj[1].texture_id = glhelper::load_texture("data/grass.tga");

  obj[1].visible = true;
  obj[1].prog = shader_program_id;
}


void init_model_3()
{
  // Chargement d'un maillage a partir d'un fichier
  mesh m = load_off_file("data/armadillo_light.off");

  // Affecte une transformation sur les sommets du maillage
  float s = 0.01f;
  mat4 transform = mat4(   s, 0.0f, 0.0f, 0.0f,
      0.0f,    s, 0.0f, 0.50f,
      0.0f, 0.0f,   s , 0.0f,
      0.0f, 0.0f, 0.0f, 1.0f);
  apply_deformation(&m,matrice_rotation(M_PI/2.0f,1.0f,0.0f,0.0f));
  apply_deformation(&m,matrice_rotation(M_PI,0.0f,1.0f,0.0f));
  apply_deformation(&m,transform);

  update_normals(&m);
  fill_color(&m,vec3(1.0f,1.0f,1.0f));

  obj[2].vao = upload_mesh_to_gpu(m);

  obj[2].nb_triangle = m.connectivity.size();
  obj[2].texture_id = glhelper::load_texture("data/white.tga");

  obj[2].visible = true;
  obj[2].prog = shader_program_id;

  obj[2].tr.translation = vec3(2.0, 0.0, -10.0);
}

// Ajout d'autres objets (pour tester la copie)
void init_model_4() {
  std::random_device rd;  // Will be used to obtain a seed for the random number engine
  std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
  std::uniform_real_distribution<> dis(-10.0, 10.0);
  for (int i=3;i<25;i++) {
    obj[i] = obj[0];

    // Positionnement aléatoire des dinos 
    obj[i].tr.translation = vec3(dis(gen), 0.0, dis(gen));

    // Rotation aléatoire des dinos
    obj[i].tr.rotation_euler.y = rand()/(float)RAND_MAX*2*M_PI;
  }

}