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
const int nb_obj = 151;
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
bool dodge=false;

// boolean pour gérer l'affichage des coordonnées
bool coord = false;

// Position de la caméra - pour actualiser la position de son centre de rotation
float cam_posX = 0.0f;
float cam_posZ = 0.0f;

// Orientation de la caméra pour déterminer une position réelle de la caméra
float cam_orientation = 0.0f;

//les parametres de translations
float translation_x=0.0f;
float translation_y=0.0f;
float translation_z=-3.0f;
float d_jump=1.0f;

// Variables pour le jeu
int nb_vies = 3;
bool end = false;
bool iscollision=false;     //booléen permettant de savoir si on est en collision avec un mur ou pas

/*****************************************************************************\
* initialisation                                                              *
\*****************************************************************************/
static void init()
{
  shader_program_id = glhelper::create_program_from_file("shaders/shader.vert", "shaders/shader.frag"); CHECK_GL_ERROR();

  cam.projection = matrice_projection(60.0f*M_PI/180.0f,1.0f,0.01f,100.0f);
  cam.tr.translation = vec3(0.0f, 1.0f, 0.0f);
  cam.tr.rotation_center = vec3(0.0f, 1.0f, 0.0f);
  // cam.tr.rotation_euler = vec3(M_PI/2., 0.0f, 0.0f);

  init_model_1();
  init_model_2();
  init_model_3();
  init_model_4();
  //init_model_5(); // Laby add on blender
  init_model_6(); // Laby à la mano
  init_model_7(); // Modèle de l'arbre


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
static void keyboard_callback(unsigned char key, int x, int y )
{
  mouse_cursor(x,y);
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
        jump=true;  //sauter donc translation en y
        break;

    case 'c':
        dodge=true; //esquiver donc la translation en x ou en z est plus importante si la touche m est enfoncée
        break;
  }
}

/*****************************************************************************\
* fonction pour éviter le temps de latence au début                           *
\*****************************************************************************/

// Cette fonction permet aussi l'actualisation de la position de la caméra
static void deplacement() 
{
  // Récupération de la matrice de rotation de la caméra
  mat4 rotation_x = matrice_rotation(cam.tr.rotation_euler.x, 1.0f, 0.0f, 0.0f);
  mat4 rotation_y = matrice_rotation(-cam.tr.rotation_euler.y, 0.0f, 1.0f, 0.0f);
  mat4 rotation_z = matrice_rotation(cam.tr.rotation_euler.z, 0.0f, 0.0f, 1.0f); 
  mat4 rotation =  rotation_y;  // On récupère modifie seulement la composante en y utile pour le calcul des translation (récupération de l'orientation) 

  // Application de la translation en fonction de la touche pressée
  if ((up)&&(iscollision==false)) {       //on test si on est en collision et donc si on peut bouger
    if(dodge){cam.tr.translation += rotation * vec3(0, 0, -(3*dL));
            //std::cout << cam.tr.translation.z << std::endl;     //juste pour montrer la différence de déplacement avec et sans esquive
            }             
    else {cam.tr.translation += rotation * vec3(0, 0, -dL);
            //std::cout << cam.tr.translation.z << std::endl;
            }
  }
  if ((down)&&(iscollision==false)) {
    if(dodge)cam.tr.translation += rotation * vec3(0, 0, 3*dL);
    else cam.tr.translation += rotation * vec3(0, 0, dL);
  }
  if ((left)&&(iscollision==false)) {
    if(dodge)cam.tr.translation += rotation * vec3(-(3*dL), 0, 0);
    else cam.tr.translation += rotation * vec3(-dL, 0, 0);
  }
  if ((right)&&(iscollision==false)) {
    if(dodge)cam.tr.translation += rotation * vec3(3*dL, 0, 0);
    cam.tr.translation += rotation * vec3(dL, 0, 0);
  }

  // Pour réactualiser le centre de rotation
  cam.tr.rotation_center = cam.tr.translation;
}

/*****************************************************************************\
* Fonction pour gérer le saut                                                *
\*****************************************************************************/

static void sauter()
{ float hauteur_cam=cam.tr.translation.y;
  if (jump==true && cam.tr.translation.y<8.0f) cam.tr.translation.y+=d_jump;    //ici le 8.0f est la limite pour la hauteur de saut on ne peut pas aller plus haut
  if (jump==false) {
    if (cam.tr.translation.y>d_jump) cam.tr.translation.y-=d_jump;
  }
    cam.tr.rotation_center = cam.tr.translation;
}

/*****************************************************************************\
* gestion du relachement des touches clavier                                 *
\*****************************************************************************/

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
      // std::cout << "j'ai saute" << std::endl;
      // std::cout << cam.tr.translation.y << std::endl;
      jump = false;
      break;

    case 'c':
      dodge=false;
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
    // Affichage des coord si l'on appui sur F3
    case GLUT_KEY_F3:
      coord = !coord; // Basculement de l'état du booléen gérant l'affichage ou non des coordonnées
      break;
  }
}




//structure de base d'une sphère pour les collisions entre les différents personnages
struct Sphere
{
  float x,y,z;
  float rayon;
};

struct Cube
{
  float x,y,z;
  float w,h,d;
};

static bool Collision(Sphere S1,Sphere S2)
{
   
   int d2 = (S1.x-S2.x)*(S1.x-S2.x) + (S1.y-S2.y)*(S1.y-S2.y) + (S1.z-S2.z)*(S1.z-S2.z);
   if (d2 > (S1.rayon + S2.rayon)*(S1.rayon + S2.rayon)){
     //std::cout << "j'ai pas touche" << std::endl;
     return false;

   } 

   else {
     //std::cout << "j'ai touche" << std::endl;
     return true;
     
   }
}


bool CollisionCube(Cube box1,Cube box2)
{
   if((box2.x >= box1.x + box1.w)      // trop à droite
	|| (box2.x + box2.w <= box1.x) // trop à gauche
	|| (box2.y >= box1.y + box1.h) // trop en bas
	|| (box2.y + box2.h <= box1.y)  // trop en haut	
        || (box2.z >= box1.z + box1.d)   // trop derrière
	|| (box2.z + box2.d <= box1.z))  // trop devant
          
          return false; 
   else
          return iscollision=true;        //ici on renvoie iscollision à true pour empecher de bouger
}



/*****************************************************************************\
* Réactualisation de la taille de la fenêtre in real time                     *
\*****************************************************************************/

// N'a au final pas été utilisée 
static void size_actualisation() {
  WIDTH = glutGet(GLUT_WINDOW_WIDTH);
  HEIGHT = glutGet(GLUT_WINDOW_HEIGHT);
  // printf("WIDTH = %d  HEIGTH = %d\n",WIDTH,HEIGHT);
}

/*****************************************************************************\
* Rotation via l'utilisation de la souris                                     *
\*****************************************************************************/

void mouse_cursor(int x, int y) { 
    //traitement des coordonnées de la souris pour qu'elles soient cohérentes
    int tempX = x;
    int tempY = HEIGHT - y;
    
    cam.tr.rotation_euler.y -= 0.001f * d_angle * 2*M_PI*float(HEIGHT / 2 - tempX); // Changement d'orientation horizontale

    // Partie gestion orientation verticale :

    // Condition pour éviter des déplacements incorrects (si la caméra fait un tour complet etc...)
    if (abs(cam.tr.rotation_euler.x) <= M_PI/2) {
      cam.tr.rotation_euler.x += 0.001f * d_angle * 2*M_PI*float(WIDTH / 2 - tempY);
    }
    // Lorsqu'elle pointe trop vers le haut
    if (cam.tr.rotation_euler.x <= -M_PI/2) {
      cam.tr.rotation_euler.x += 0.0001;  // On la redécale légèrement vers le bas pour qu'on puisse la redépacer (pour que la condition du dessus soit respectée)
    }
    // Lorsqu'elle pointe trop vers le bas
    if (cam.tr.rotation_euler.x >= M_PI/2) {
      cam.tr.rotation_euler.x -= 0.0001;
    }
}

/*****************************************************************************\
* Gestion de l'affichage des coordonnées à l'écran                            *
\*****************************************************************************/

static void affichage_coord(){
  if (coord) {  // pour l'affichage des coords
    text_to_draw[0].value = std::to_string(cam.tr.translation.x); // Affichage coordonnées en X
    text_to_draw[0].bottomLeft = vec2(-0.2, 0.5);
    text_to_draw[0].topRight = vec2(0.2, 1);
    init_text(text_to_draw);

    text_to_draw[1]=text_to_draw[0];
    text_to_draw[1].value = std::to_string(cam.tr.translation.z); // Affichage coordonnées en Z
    text_to_draw[1].bottomLeft.y = 0.0f;
    text_to_draw[1].topRight.y = 0.5f;
  }
  else {  // Pour effacer l'affichage
    text_to_draw[0].value = " ";
    text_to_draw[0].bottomLeft = vec2(-0.2, 0.5);
    text_to_draw[0].topRight = vec2(0.2, 1);
    init_text(text_to_draw);

    text_to_draw[1]=text_to_draw[0];
    text_to_draw[1].value = " ";
    text_to_draw[1].bottomLeft.y = 0.0f;
    text_to_draw[1].topRight.y = 0.5f;
  }
}

/*****************************************************************************\
* Gestion du jeu                                                             *
\*****************************************************************************/

static void jeu(){
  // Conditions pour que le joueur puisse continuer de jouer
  if ((nb_vies>0) && (end == false)) {  // S'il a encore des vies ou qu'il n'a pas encore franchi la ligne d'arrivée
    // Affichage du nombre de vie sur le terminal    
    std::cout << "\x1B[2J\x1B[H"; // Pour clear
    printf("Nombre de vie : %d\n",nb_vies);
  }

  // fin du jeu
  else {
    exit(0);
  }

  
  
}

/*****************************************************************************\
* timer_callback                                                              *
\*****************************************************************************/
static void timer_callback(int)
{
  Sphere camera;                      //je crée la sphere de la camera pour les collisions
  camera.x= cam.tr.translation.x;
  camera.y= cam.tr.translation.y;
  camera.z= cam.tr.translation.z;
  camera.rayon= 0.5f;

  Sphere armadillo;                   //je crée la sphere de l'armadillo blanc pour les collisions
  armadillo.x=obj[2].tr.translation.x;
  armadillo.y=obj[2].tr.translation.y;
  armadillo.z=obj[2].tr.translation.z;
  armadillo.rayon=0.5f;


  Sphere steg;                        //je crée la sphere du premier steg pour les collisions
  steg.x=obj[0].tr.translation.x;
  steg.y=obj[0].tr.translation.y;
  steg.z=obj[0].tr.translation.z;
  steg.rayon=0.5f;

  Sphere arbre;                       // Sphere pour l'arbre 
  arbre.x=obj[150].tr.translation.x;
  arbre.y=0.0;
  arbre.z=obj[150].tr.translation.z;
  arbre.rayon=0.5f;
  
  // Lorsq que le joueur touche l'arbre de fin --> fin du jeu 
  if (Collision(camera,arbre)) {
    end = true;
  }
  iscollision=false;      //ici on remet iscollision a false pour pouvoir rebouger

  //ici je crée une boucle for pour crée des sphères pour chaque stegausors
  char struc[][22]={"steg3","steg4","steg5","steg6","steg7","steg8","steg9","steg10","steg11","steg12","steg13","steg14","steg15","steg16","steg17","steg18","steg19","steg20","steg21","steg22","steg23","steg24"};
    for (int i=3;i<25;i++) {
      Sphere struc[i-3];
      struc[i-3].x=obj[i].tr.translation.x;
      struc[i-3].y=obj[i].tr.translation.y;
      struc[i-3].z=obj[i].tr.translation.z;
      struc[i-3].rayon=0.5f;
      Collision(camera,struc[i-3]);
      if (Collision(camera,struc[i-3])){
        obj[i].visible=false;
        //std::cout << "j'ai touche" << std::endl; //test d'une collision
        
        // Gestion du nombre de vie
        nb_vies -= 1;

        obj[i].tr.translation.z+=20;    //on déplace l'objet en dehors du jeu pour ne plus avoir de collisions
      }     
      while ((Collision(struc[i-3], steg))||(Collision(struc[i-3],armadillo))){
            std::random_device rd;  // Will be used to obtain a seed for the random number engine
            std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
            std::uniform_real_distribution<> dis(-15.0, 15.0);
            // Positionnement aléatoire des dinos 
            obj[i].tr.translation = vec3(dis(gen), 0.0, dis(gen));

            // Rotation aléatoire des dinos
            obj[i].tr.rotation_euler.y = rand()/(float)RAND_MAX*2*M_PI;

      }
    }
  float s = 2.5f;
  float Largeur = s/2;
  float Longueur = s*2;
  Cube cameraAABB;
  cameraAABB.x=cam.tr.translation.x;
  cameraAABB.y=cam.tr.translation.y;
  cameraAABB.z=cam.tr.translation.z;
  cameraAABB.w=10*dL;
  cameraAABB.h=50*dL;
  cameraAABB.d=10*dL;

  /*Cube labAABB;
  labAABB.x=obj[25].tr.translation.x;
  labAABB.y=obj[25].tr.translation.y;
  labAABB.z=obj[25].tr.translation.z;
  labAABB.w=50*dL;
  labAABB.h=50*dL;
  labAABB.d=50*dL;
  */
  //on crée un tableau comportant le nom de chaque bloc du labyrinthe
  char AABB[][75]={"a","b","c","d","e","f","g","h","i","j","k","l","m","o","p","q","r","s","t","u","v","w","x","y","z","a2","b2","c2","d2","e2","f2","g2","h2","i2","j2","k2","l2","m2","o2","p2","q2","r2","s2","t2","u2","v2","w2","x2","y2","z2","a3","b3","c3","d3","e3","f3","g3","h3","i3","j3","k3","l3","m3","o3","p3","q3","r3","s3","t3","u3","v3","w3"};
  for(int i=25;i<100;i++){
    Cube AABB[i-25];                              //on crée la structure cubique de chaque bloc pour pouvoir gérer les collisions
    AABB[i-25].x=obj[i].tr.translation.x;
    AABB[i-25].y=obj[i].tr.translation.y;
    AABB[i-25].z=obj[i].tr.translation.z;
    AABB[i-25].w=Largeur;
    AABB[i-25].h=100*dL;
    AABB[i-25].d=Longueur;
    CollisionCube(AABB[i-25],cameraAABB);
    if (CollisionCube(AABB[i-25],cameraAABB)){
      //std::cout << "j'ai touche" << std::endl;
      //obj[i].visible=false;
    }
  }

  // pour gérer le déplacement de la caméra
  deplacement();
  // Pour gérer le saut
  sauter();
  
  // Affichage des coords
  affichage_coord();

  // Gestion du jeu 
  jeu();

  // Collision(camera,armadillo);
  if (Collision(camera,armadillo)){
    nb_vies -= 1;
    obj[2].visible=false;             //si on a collision on passe ka visibilité de l'objet à false
    //std::cout << "j'ai touche armadillo" << std::endl;      //test d'une collision
    obj[2].tr.translation.z+=20;    //on déplace l'objet en dehors du jeu pour ne plus avoir de collisions
  }
  

  /*for(int i=26; i<50;i++){
    obj[i].visible=false;
  }*/
  
  
  // Collision(camera,steg);
  if (Collision(camera,steg)){
    nb_vies -= 1;
    obj[0].visible=false;
    //std::cout << "j'ai touche steg" << std::endl;  //test d'une collision
    obj[0].tr.translation.z;
  }    
  

  //  Pour ramener le pointeur au centre de la fenêtre
  glutWarpPointer(WIDTH / 2, HEIGHT / 2);

  glutTimerFunc(10, timer_callback, 0);

  glutPostRedisplay();
  // size_actualisation();
  // change_capture();
  //printf("orienty = %f\t orientx = %f\n",cam.tr.rotation_euler.y,cam.tr.rotation_euler.x);
  
  
    
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
  //printf("%f",cam.tr.translation.y);

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
  std::uniform_real_distribution<> dis(-15.0, 15.0);
  for (int i=3;i<25;i++) {
    obj[i] = obj[0];

    // Positionnement aléatoire des dinos 
    obj[i].tr.translation = vec3(dis(gen), 0.0, dis(gen));

    // Rotation aléatoire des dinos
    obj[i].tr.rotation_euler.y = rand()/(float)RAND_MAX*2*M_PI;
  }

}

// Création du modèle du labyrhinte

// Modèle à partir de blender --> ne pas utiliser car les collisions will not fontionner (convexe too complexe pour OpenGL)
/*void init_model_5()
{
  // Chargement d'un maillage a partir d'un fichier
  mesh m = load_obj_file("data/maze.obj");

  // Affecte une transformation sur les sommets du maillage
  float s = 2.2f;
  mat4 transform = mat4(   s, 0.0f, 0.0f, 0.0f,
      0.0f,    s, 0.0f, 0.0f,
      0.0f, 0.0f,   s , 0.0f,
      0.0f, 0.0f, 0.0f, 1.0f);
  apply_deformation(&m,transform);

  // Centre la rotation du modele 1 autour de son centre de gravite approximatif
  obj[25].tr.rotation_center = vec3(0.0f,0.0f,0.0f);

  update_normals(&m);
  fill_color(&m,vec3(1.0f,1.0f,1.0f));

  obj[25].vao = upload_mesh_to_gpu(m);

  obj[25].nb_triangle = m.connectivity.size();
  obj[25].texture_id = glhelper::load_texture("data/wall.tga");
  obj[25].visible = true;
  obj[25].prog = shader_program_id;

  obj[25].tr.translation = vec3(0.0, -0.5, 0.0);
  
}
*/

void init_model_6()
{
  // Chargement d'un maillage a partir d'un fichier
  mesh m = load_obj_file("data/wall.obj");

  // Affecte une transformation sur les sommets du maillage
  float s = 2.5f;
  mat4 transform = mat4(   s, 0.0f, 0.0f, 0.0f,
      0.0f,    s, 0.0f, 0.0f,
      0.0f, 0.0f,   s , 0.0f,
      0.0f, 0.0f, 0.0f, 1.0f);
  apply_deformation(&m,transform);

  // Calcul des dimmensions des murs pour positionnement
  float Largeur = s/2;
  float Longueur = s*2;

  // Centre la rotation du modele 1 autour de son centre de gravite approximatif
  obj[25].tr.rotation_center = vec3(0.0f,0.0f,0.0f);

  update_normals(&m);
  fill_color(&m,vec3(1.0f,1.0f,1.0f));

  obj[25].vao = upload_mesh_to_gpu(m);

  obj[25].nb_triangle = m.connectivity.size();
  obj[25].texture_id = glhelper::load_texture("data/wall.tga");
  obj[25].visible = true;
  obj[25].prog = shader_program_id;

  obj[25].tr.translation = vec3(Longueur, -0.5, 0.0);
  

  // Génération aléatoire d'un maze

  // Liste pour générer un signe aléatoire à l'aide de disi(gen)
  float liste_signe[8];
  liste_signe[0] = -1.0;
  liste_signe[1] = 1.0;
  liste_signe[2] = 1.0;
  liste_signe[3] = -1.0;
  liste_signe[4] = -1.0;
  liste_signe[5] = 1.0;
  liste_signe[6] = -1.0;
  liste_signe[7] = 1.0;
  
  
  std::random_device rd;  // Will be used to obtain a seed for the random number engine
  std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
  std::uniform_real_distribution<> dis(1.0, 16.0);  // nombre aleat pour la position
  std::uniform_real_distribution<> disi(0.0, 7.0);
  // Partie proche --> pour éviter de pop dans un mur
  for (int i=26;i<50;i++) {

    obj[i] = obj[25];
    
    obj[i].tr.translation = vec3((liste_signe[int(disi(gen))]*dis(gen)*Longueur)/7, -0.5, (liste_signe[int(disi(gen))]*dis(gen)*Longueur)/7); // Facteur 7 pour la proximité avec l'endroit ou pop la cam
    
    if (i%2==0){
      obj[i].tr.rotation_euler.y = M_PI/2;
    }
  }

  // Partie Loin
  for (int i=50;i<100;i++) {

    obj[i] = obj[25];
    
    obj[i].tr.translation = vec3((liste_signe[int(disi(gen))]*dis(gen)*Longueur)/4, -0.5, (liste_signe[int(disi(gen))]*dis(gen)*Longueur)/4); // Facteur 4 pour être un peu loin
    
    // if (i%2==0){
    //   obj[i].tr.rotation_euler.y = M_PI/2;
    // }

    obj[i].tr.rotation_euler.y = rand()/(float)RAND_MAX*2*M_PI;

  }
  



}


// Réalisation du modèle de l'arbre
void init_model_7()
{
    // Chargement d'un maillage a partir d'un fichier
  mesh m = load_obj_file("data/Tree2.obj");

  // Affecte une transformation sur les sommets du maillage
    float s = 0.4f;
    mat4 transform = mat4(s, 0.0f, 0.0f, 0.0f,
        0.0f, s, 0.0f, -0.9f,
        0.0f, 0.0f, s, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
    apply_deformation(&m, transform);
    apply_deformation(&m, matrice_rotation(M_PI / 2.0f, 1.0f, 0.0f, 0.0f));
    apply_deformation(&m, matrice_rotation(1.5 * M_PI / 2.0f, 0.0f, 1.0f, 0.0f));

  // Calcul des dimmensions des murs pour positionnement
  float Largeur = s/2;
  float Longueur = s*2;

  // Centre la rotation du modele 1 autour de son centre de gravite approximatif
  obj[150].tr.rotation_center = vec3(0.0f,0.0f,0.0f);

  update_normals(&m);
  fill_color(&m,vec3(1.0f,1.0f,1.0f));

  obj[150].vao = upload_mesh_to_gpu(m);

  obj[150].nb_triangle = m.connectivity.size();
  obj[150].texture_id = glhelper::load_texture("data/Tree.tga");
  obj[150].visible = true;
  obj[150].prog = shader_program_id;

  // Gestion du positionnement aleatoire de l'arbre

  // Liste pour générer un signe aléatoire à l'aide de disi(gen)
  float liste_signe[8];
  liste_signe[0] = -1.0;
  liste_signe[1] = 1.0;
  liste_signe[2] = 1.0;
  liste_signe[3] = -1.0;
  liste_signe[4] = -1.0;
  liste_signe[5] = 1.0;
  liste_signe[6] = -1.0;
  liste_signe[7] = 1.0;
  
  
  std::random_device rd;  // Will be used to obtain a seed for the random number engine
  std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
  std::uniform_real_distribution<> dis(19.0, 24.0);  // nombre aleat pour la position
  std::uniform_real_distribution<> disi(0.0, 7.0);


  obj[150].tr.translation = vec3(liste_signe[int(disi(gen))]*dis(gen), -0.5, liste_signe[int(disi(gen))]*dis(gen));
}