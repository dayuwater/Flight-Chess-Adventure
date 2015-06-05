/**********************************************************************
 *
 *  Shader example (Mac)
 *  Assignment 5
 *  Fall 2012, CS 4554
 *
 **********************************************************************/

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <Glut/glut.h>
#else
#include "stdafx.h"
#include <GL/glut.h>
#endif

#include <math.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <time.h>

#include "PolyModel.h"
#include "util.h"
#include "Vec3.h"

GLuint g_lattice_shader=0;
GLuint g_yellow_shader=0;
GLuint g_red_shader=0;
GLuint g_green_shader=0;
GLuint g_blue_shader=0;
GLuint g_enemy_shader=0;
GLuint g_mand_shader=0;
GLuint g_bump_shader=0;
GLuint g_wall_texture_id=0;
GLuint g_iw_texture_id=0;
GLuint g_wall_bump_tex_id=0;
PolyModel g_DiceModel;
PolyModel g_innerRoom;
PolyModel g_outerRoom;
PolyModel g_middleRoom;
vector<Tile> g_tiles;
Chess g_red_chess;
Chess g_blue_chess;
Chess g_green_chess;
Chess g_yellow_chess;
vector<Chess> g_enemy_chess;
vector<int> num_slot;
GLuint g_DiceTexture;
GLuint g_EnemyTexture;
GLuint g_innerTexture;
GLuint g_outerTexture;
GLuint g_outerNormal;
GLuint g_midTexure;

float g_rotation_angle = 0.0f;

Vec3f viewpoint;
Vec3f viewdirect;
Vec3f uormal; // actually normal
Vec3f tv;
float length=0.2f;
/* globals to track the game */
int timer=0;
int mode=10; // 20=dice, 11=red, 12=green, 13=blue, 14=yellow, 10=debug mode, 4=enemy, 5=after user move
int currentDice=0;
int score=-1;

/* bools to check the state of the game */
bool dicemode=false; // true if it is the time for players to roll dice
bool decided=false; // if user decided to move a chess piece, this becomes true
bool dice_rolled=false; // if the dice is rolled, this will be true

/* ascii codes for special keys */
#define ESCAPE 27

/**********************************************************************
 * Configuration
 **********************************************************************/

#define INITIAL_WIDTH 1200
#define INITIAL_HEIGHT 1200
#define INITIAL_X_POS 30
#define INITIAL_Y_POS 0
#define DICE_PLACE 1000

//the rotation angles of dice faces
#define DICE_ONE 1583
#define DICE_TWO 3525
#define DICE_THREE 766
#define DICE_FOUR 941
#define DICE_FIVE 690
#define DICE_SIX 406

//game settings
#define HARD_OUT 6 // the number of enemy in outer court in hard mode
#define HARD_IN 2 // the number of enemy in inner court in hard mode

#define PI 3.14159265358979
#define BOARD_OFFSET 0.4
#define INNER_ROOM_SIZE 4
#define MIDDLE_ROOM_SIZE 4.5 // the outer wall of the inner room
#define OUTER_ROOM_SIZE 12



#define WINDOW_NAME     "Aeroplane Chess Adventure"

// Change this to the directory where you have the .d2 files on your
// computer, or put the files in your programs default start up directory.
#define RESOURCE_DIR   "/Users/dayuwater/Downloads/ShaderDemo-2/resources/"

/**********************************************************************
 * Set the new size of the window
 **********************************************************************/

void resize_scene(GLsizei width, GLsizei height)
{
    glViewport(0, 0, width, height);  /* reset the current viewport and 
                                       * perspective transformation */
    g_window_width  = width;
    g_window_height = height;
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    gluPerspective(60.0, double(width)/double(height), 0.1, 500.0);
    
    glMatrixMode(GL_MODELVIEW);
    
}

/**********************************************************************
 * any program initialization (set initial OpenGL state, 
 **********************************************************************/
void init()
{
    g_compile_link_log = "Welcome to Aeroplane Chess Adventure. Press o to roll dice\n";
    srand(time(NULL));
    // The dice, drawn in dice place (1000,1000,1000 away from the chessboard)
    string lattice_shader = RESOURCE_DIR + string("lattice");
    g_lattice_shader  = createShader(lattice_shader);
    
    string blue_shader = RESOURCE_DIR + string("blue");
    g_blue_shader  = createShader(blue_shader);

    string red_shader = RESOURCE_DIR + string("red");
    g_red_shader  = createShader(red_shader);

    string green_shader = RESOURCE_DIR + string("green");
    g_green_shader  = createShader(green_shader);

    string yellow_shader = RESOURCE_DIR + string("yellow");
    g_yellow_shader  = createShader(yellow_shader);
    
    string enemy_shader = RESOURCE_DIR + string("phong");
    g_enemy_shader  = createShader(enemy_shader);
    
    string mand_shader = RESOURCE_DIR + string("mand");
    g_mand_shader  = createShader(mand_shader);
    
    string bumpp_shader = RESOURCE_DIR + string("bumpp");
    g_bump_shader  = createShader(bumpp_shader);

    
    
    
    
    
    
    if(g_enemy_shader!=0){
        GLint loc=glGetUniformLocation(g_enemy_shader, "color_tex");
        cout << "Enemy Shader: "<< loc << endl;
        glUniform1i(loc, 0);
    }


    // draw the dice
    
    string model_file = RESOURCE_DIR + string("dice.d2");
    ifstream dice_fs(model_file);
    g_DiceModel.loadModel(dice_fs);
    string dice_tex_file = RESOURCE_DIR + string("dice.png");
    g_DiceTexture = getTextureFromFile(dice_tex_file.c_str());
    g_DiceModel.moveTo(DICE_PLACE, DICE_PLACE, DICE_PLACE);
    g_DiceModel.scale(-0.7);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // draw the inner room
    
    string ir_file = RESOURCE_DIR + string("dice.d2");
    ifstream ir_fs(ir_file);
    g_innerRoom.loadModel(ir_fs);
    string ir_tex_file = RESOURCE_DIR + string("brick_texture.jpg");
    g_innerTexture = getTextureFromFile(ir_tex_file.c_str());
    g_innerRoom.moveTo(0,0,0);
    g_innerRoom.scale(INNER_ROOM_SIZE/1.9685-1);
    g_innerRoom.moveTo(0, 0, INNER_ROOM_SIZE-0.5);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // draw the outer room
    // since the dice model is just a cube, perfect for modeling rooms
    string or_file = RESOURCE_DIR + string("3droom.d2");
    ifstream or_fs(or_file);
    g_outerRoom.loadModel(or_fs);
    string or_tex_file = RESOURCE_DIR + string("brick_color_map.png");
    g_outerTexture = getTextureFromFile(or_tex_file.c_str());
    string or_nor_file = RESOURCE_DIR + string("brick_normal_map.png");
    g_outerNormal=getTextureFromFile(or_nor_file.c_str());
    g_outerRoom.moveTo(0,0,0);
    g_outerRoom.scale(OUTER_ROOM_SIZE/100.0-1);
    g_outerRoom.moveTo(0, 0, OUTER_ROOM_SIZE-1.5);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // draw the middle room ( the outer wall of the inner room )
    // since the dice model is just a cube, perfect for modeling rooms
    string mr_file = RESOURCE_DIR + string("dice.d2");
    ifstream mr_fs(mr_file);
    g_middleRoom.loadModel(mr_fs);
    string mr_tex_file = RESOURCE_DIR + string("duck.png");
    g_midTexure = getTextureFromFile(mr_tex_file.c_str());
    g_middleRoom.moveTo(0,0,0);
    g_middleRoom.scale(MIDDLE_ROOM_SIZE/1.9685-1);
    g_middleRoom.moveTo(0, 0, MIDDLE_ROOM_SIZE-1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    
    // draw the chessboard(tiles), using square.d2
    g_tiles.resize(190);
    string tile_file = RESOURCE_DIR + string("square.d2");
    
    // outer court, a square ring with 100 blocks
    
    for(int i=0; i<25; i++){
        ifstream tile_fs(tile_file);
        g_tiles[i].loadModel(tile_fs);
        //string tile_tex_file = RESOURCE_DIR + string("dice.png");
        //g_DiceTexture = getTextureFromFile(dice_tex_file.c_str());
        g_tiles[i].moveTo((i+.0-12.5)/2, -6.25, 0);
        g_tiles[i].scale(0.0);
        
        
    }
    
    for(int i=25; i<50; i++){
        ifstream tile_fs(tile_file);
        g_tiles[i].loadModel(tile_fs);
        //string tile_tex_file = RESOURCE_DIR + string("dice.png");
        //g_DiceTexture = getTextureFromFile(dice_tex_file.c_str());
        g_tiles[i].moveTo(6.25, (i-25+.0-12.5)/2, 0);
        g_tiles[i].scale(0.0);
    }
    
    for(int i=50; i<75; i++){
        ifstream tile_fs(tile_file);
        g_tiles[i].loadModel(tile_fs);
        //string tile_tex_file = RESOURCE_DIR + string("dice.png");
        //g_DiceTexture = getTextureFromFile(dice_tex_file.c_str());
        g_tiles[i].moveTo((-i-.0+12.5+50)/2, 6.25, 0);
        g_tiles[i].scale(0.0);
    }
    
    for(int i=75; i<100; i++){
        ifstream tile_fs(tile_file);
        g_tiles[i].loadModel(tile_fs);
        //string tile_tex_file = RESOURCE_DIR + string("dice.png");
        //g_DiceTexture = getTextureFromFile(dice_tex_file.c_str());
        g_tiles[i].moveTo(-6.25, (-i-.0+12.5+75)/2, 0);
        g_tiles[i].scale(0.0);
    }
    
    for(int i=0; i<99; i++){
        g_tiles[i].setOwner(i%4+1);
        g_tiles[i].setNext(i+1);
        g_tiles[i].setAttributes(0);
    }
    g_tiles[99].setOwner(99%4+1);
    g_tiles[99].setNext(0);
    
    // inner court, a filled square with 9*9=81 blocks
    
    for(int i=0; i<81; i++){
        ifstream tile_fs(tile_file);
        g_tiles[i+100].loadModel(tile_fs);
        g_tiles[i+100].moveTo(-2.5+(i%9+.0)/2, 2.5-i/9*0.5, 0);
        g_tiles[i+100].scale(0.0);
    }
    
    for(int i=100; i<180; i++){
        g_tiles[i].setOwner(i%4+1);
        g_tiles[i].setNext(i+1);
        g_tiles[i].setAttributes(0);
    }
    g_tiles[180].setOwner(1);
    g_tiles[180].setNext(180); // if a chess get to this block, that chess wins
    
    // draw outside watch towers
    {
        ifstream tile_fs(tile_file);
        g_tiles[181].loadModel(tile_fs);
        g_tiles[181].moveTo(-7.25, -7.25, 0);
        g_tiles[181].setOwner(0);
        g_tiles[181].setNext(181);
        g_tiles[181].setOccupy(0);
        g_tiles[181].setAttributes(2);
    }
    {
        ifstream tile_fs(tile_file);
        g_tiles[182].loadModel(tile_fs);
        g_tiles[182].moveTo(7.25, -7.25, 0);
        g_tiles[182].setOwner(25);
        g_tiles[182].setNext(182);
        g_tiles[182].setOccupy(25);
        g_tiles[182].setAttributes(2);
    }
    {
        ifstream tile_fs(tile_file);
        g_tiles[183].loadModel(tile_fs);
        g_tiles[183].moveTo(7.25, 7.25, 0);
        g_tiles[183].setOwner(50);
        g_tiles[183].setNext(183);
        g_tiles[183].setOccupy(50);
        g_tiles[183].setAttributes(2);
    }
    {
        ifstream tile_fs(tile_file);
        g_tiles[184].loadModel(tile_fs);
        g_tiles[184].moveTo(-7.25, 7.25, 0);
        g_tiles[184].setOwner(75);
        g_tiles[184].setNext(184);
        g_tiles[184].setOccupy(75);
        g_tiles[184].setAttributes(2);
    }



    
    for(int i=0; i<100; i+=25){
        g_tiles[i].setAttributes(1);
    }
    
    for(int i=12; i<100; i+=25){
        g_tiles[i].setAttributes(4);
    }
    
    
    
    
    

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    
    
    // draw the walls(Polymodel), using square.d2
    
    // draw chess pieces (Chess) , using king.d2
    string user_chess_file = RESOURCE_DIR + string("king.d2");
    ifstream user_fs_r(user_chess_file);
    g_red_chess.loadModel(user_fs_r);
    g_red_chess.setIdentity(1);
    g_red_chess.scale(-0.9);
    g_red_chess.moveTo(100, 100, 300);
    g_red_chess.setPosition(-1);
    //g_red_chess.moveTo(6.25, 6.25, 0.5);
    ifstream user_fs_b(user_chess_file);
    g_blue_chess.loadModel(user_fs_b);
    g_blue_chess.setIdentity(2);
    g_blue_chess.scale(-0.9);
    g_blue_chess.moveTo(100, 100, -300);
    g_blue_chess.setPosition(-1);
    ifstream user_fs_g(user_chess_file);
    g_green_chess.loadModel(user_fs_g);
    g_green_chess.setIdentity(3);
    g_green_chess.scale(-0.9);
    g_green_chess.moveTo(-100, -100, 300);
    g_green_chess.setPosition(-1);
    ifstream user_fs_y(user_chess_file);
    g_yellow_chess.loadModel(user_fs_y);
    g_yellow_chess.setIdentity(4);
    g_yellow_chess.scale(-0.9);
    g_yellow_chess.moveTo(-100, -100, -300);
    g_yellow_chess.setPosition(-1);
    
    string enemy_chess_file= RESOURCE_DIR + string("seymour.d2");
    string enemy_tex_file = RESOURCE_DIR + string("seymour.png");
    g_EnemyTexture = getTextureFromFile(enemy_tex_file.c_str());

   
    g_enemy_chess.resize(HARD_IN+HARD_OUT);
    for(int i=0; i<HARD_OUT; i++){
        ifstream enemy_fs(enemy_chess_file);
        g_enemy_chess[i].loadModel(enemy_fs);
        int r=rand()%100;
        g_enemy_chess[i].setIdentity(-1);
        g_enemy_chess[i].setPosition(r);
        g_enemy_chess[i].scale(-0.9);
        g_enemy_chess[i].moveTo(g_tiles[r].getCenter().x(), g_tiles[r].getCenter().y(), g_tiles[r].getCenter().z()+0.1);
        g_tiles[r].setOccupy(-1);
        
    }
    
    for(int i=HARD_OUT; i<HARD_IN+HARD_OUT; i++){
        ifstream enemy_fs(enemy_chess_file);
        g_enemy_chess[i].loadModel(enemy_fs);
        int r=rand()%81+100;
        g_enemy_chess[i].setIdentity(-1);
        g_enemy_chess[i].setPosition(r);
        g_enemy_chess[i].scale(-0.9);
        g_enemy_chess[i].moveTo(g_tiles[r].getCenter().x(), g_tiles[r].getCenter().y(), g_tiles[r].getCenter().z()+0.1);
        g_enemy_chess[i].setOrder();
        g_enemy_chess[i].printOrder();
        g_tiles[r].setOccupy(-1);
        
        
        
    }

    
    
    glEnable(GL_LIGHT0);
    float diffuse[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    float specular[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    float ambient[4] = {0.1f, 0.1f, 0.1f, 1.0f};
    
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    
    glEnable(GL_LIGHT1);
    
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
    
    glEnable(GL_LIGHT2);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT2, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT2, GL_AMBIENT, ambient);
    
    glEnable(GL_LIGHT3);
   
    
    glLightfv(GL_LIGHT3, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT3, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT3, GL_AMBIENT, ambient);
    
    glEnable(GL_LIGHT4);
    
    glLightfv(GL_LIGHT4, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT4, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT4, GL_AMBIENT, ambient);




    
    // Textured object - not used in current lattice shader
    glMaterialf(GL_FRONT, GL_SHININESS, 30.0f);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    
    glEnable(GL_DEPTH_TEST);
    //glDisable(GL_LIGHTING);
    
    
    viewpoint.x()=0.0;
    viewpoint.y()=0.0;
    viewpoint.z()=0.0;
    
    viewdirect.x()=0.5;
    viewdirect.y()=0.5;
    viewdirect.z()=0.5;
    
    tv=viewpoint;
    
    uormal.x()=0;
    uormal.y()=0;
    uormal.z()=1;
    

}
// set the viewpoints when moving chess
void setView(Chess c){
    if(c.getPosition()<25){
        if(c.getPosition()>=-1){
            viewpoint.x()=c.getCenter().x()-2;
            viewpoint.y()=c.getCenter().y();
            viewpoint.z()=c.getCenter().z()+1;
            viewdirect.x()=c.getCenter().x()+4;
            viewdirect.y()=c.getCenter().y();
            viewdirect.z()=c.getCenter().z();
        }
    }
    else if(c.getPosition()<50){
        viewpoint.x()=c.getCenter().x();
        viewpoint.y()=c.getCenter().y()-2;
        viewpoint.z()=c.getCenter().z()+1;
        viewdirect.x()=c.getCenter().x();
        viewdirect.y()=c.getCenter().y()+4;
        viewdirect.z()=c.getCenter().z();

        
    }
    else if(c.getPosition()<75){
        viewpoint.x()=c.getCenter().x()+2;
        viewpoint.y()=c.getCenter().y();
        viewpoint.z()=c.getCenter().z()+1;
        viewdirect.x()=c.getCenter().x()-4;
        viewdirect.y()=c.getCenter().y();
        viewdirect.z()=c.getCenter().z();

        
    }
    else if(c.getPosition()<100){
        viewpoint.x()=c.getCenter().x();
        viewpoint.y()=c.getCenter().y()+2;
        viewpoint.z()=c.getCenter().z()+1;
        viewdirect.x()=c.getCenter().x();
        viewdirect.y()=c.getCenter().y()-4;
        viewdirect.z()=c.getCenter().z();

        
    }
    else if(c.getPosition()<182){
        // set to the fixed position inside the wall
        viewpoint.x()=3;
        viewpoint.y()=3;
        viewpoint.z()=2;
        viewdirect.x()=-5;
        viewdirect.y()=-5;
        viewdirect.z()=-5;


    }
    else{
        // default viewpoint
        viewpoint.x()=10;
        viewpoint.y()=10;
        viewpoint.z()=10;
        viewdirect.x()=-5;
        viewdirect.y()=-5;
        viewdirect.z()=-5;
    }

}

/**********************************************************************
 * The main drawing functions. 
 **********************************************************************/
void draw_scene(void)
{
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);	
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // set the viewpoints depending on modes
    if(mode==20){
        viewpoint.x()=DICE_PLACE-5;
        viewpoint.y()=DICE_PLACE-5;
        viewpoint.z()=DICE_PLACE-5;
        viewdirect.x()=DICE_PLACE+5;
        viewdirect.y()=DICE_PLACE+5;
        viewdirect.z()=DICE_PLACE+5;
        
    }
    else if(mode==2){
        viewpoint.x()=g_tiles[27].getCenter().x();
        viewpoint.y()=g_tiles[27].getCenter().y()-2;
        viewpoint.z()=g_tiles[27].getCenter().z()+1; //minimize z distance
        //cout << viewpoint.y()<<endl;

        viewdirect.x()=g_tiles[27].getCenter().x();
        viewdirect.y()=g_tiles[27].getCenter().y()+4;
        viewdirect.z()=g_tiles[27].getCenter().z();
    }
    else if(mode==11){
        setView(g_red_chess);
    }
    else if(mode==12){
        setView(g_green_chess);
    }
    else if(mode==13){
       setView(g_blue_chess);
    }
    else if(mode==14){
        setView(g_yellow_chess);
        
    }
    else if(mode==21){
        // blue(50)
        // yellow(75) (px-dx+)
        // green(25) (py-dy+)
        // red(0) (px-py-dx+dy+)
        viewpoint.x()=-10;
        viewpoint.y()=-10;
        viewpoint.z()=10;
        viewdirect.x()=5;
        viewdirect.y()=5;
        viewdirect.z()=-5;
    }
    else if(mode==22){
        // blue(50)
        // yellow(75) (px-dx+)
        // green(25) (py-dy+)
        // red(0) (px-py-dx+dy+)
        viewpoint.x()=10;
        viewpoint.y()=-10;
        viewpoint.z()=10;
        viewdirect.x()=-5;
        viewdirect.y()=5;
        viewdirect.z()=-5;
    }
    else if(mode==23){
        // blue(50)
        // yellow(75) (px-dx+)
        // green(25) (py-dy+)
        // red(0) (px-py-dx+dy+)
        viewpoint.x()=10;
        viewpoint.y()=10;
        viewpoint.z()=10;
        viewdirect.x()=-5;
        viewdirect.y()=-5;
        viewdirect.z()=-5;
    }
    else if(mode==24){
        // blue(50)
        // yellow(75) (px-dx+)
        // green(25) (py-dy+)
        // red(0) (px-py-dx+dy+)
        viewpoint.x()=-10;
        viewpoint.y()=10;
        viewpoint.z()=10;
        viewdirect.x()=5;
        viewdirect.y()=-5;
        viewdirect.z()=-5;
    }

    else if(mode==30){
        // blue(50)
        // yellow(75) (px-dx+)
        // green(25) (py-dy+)
        // red(0) (px-py-dx+dy+)
        viewpoint.x()=3;
        viewpoint.y()=3;
        viewpoint.z()=2;
        viewdirect.x()=-5;
        viewdirect.y()=-5;
        viewdirect.z()=-5;
    }
    else{
        viewpoint.x()=10;
        viewpoint.y()=10;
        viewpoint.z()=10;
        viewdirect.x()=-5;
        viewdirect.y()=-5;
        viewdirect.z()=-5;

        
    }
    
    // Hard code viewing position for sample code
    gluLookAt(viewpoint.x(),viewpoint.y(),viewpoint.z(),viewdirect.x(),viewdirect.y(),viewdirect.z(),uormal.x(),uormal.y(),uormal.z());
    
    // Set light position
    float lpos[4] = {0.0f, 0.0f, 18.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lpos);
    
    float lpos1[4] = {-8.0f, -8.0f, 20.0f, 1.0f};
    glLightfv(GL_LIGHT1, GL_POSITION, lpos1);

    
    float lpos2[4] = {-8.0f, 8.0f, 20.0f, 1.0f};
    glLightfv(GL_LIGHT2, GL_POSITION, lpos2);

    float lpos3[4] = {8.0f, -8.0f, 20.0f, 1.0f};
    glLightfv(GL_LIGHT3, GL_POSITION, lpos3);

    
    float lpos4[4] = {8.0f, 8.0f, 20.0f, 1.0f};
    glLightfv(GL_LIGHT4, GL_POSITION, lpos4);
    
    
    glDisable(GL_BLEND);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_CULL_FACE);
    glBindTexture(GL_TEXTURE_2D, g_innerTexture);
    glUseProgram(g_lattice_shader);
    g_innerRoom.drawTex();
    
    glDisable(GL_BLEND);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_CULL_FACE);
    glBindTexture(GL_TEXTURE_2D, g_outerTexture);
    glUseProgram(g_bump_shader);
    glActiveTexture(GL_TEXTURE0);
    GLint loc = glGetUniformLocation(g_bump_shader, "diffuse_tex");
    glUniform1i(loc, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, g_outerNormal); // normal texture
    loc = glGetUniformLocation(g_bump_shader, "normal_map");
    glUniform1i(loc, 1);
    glBindAttribLocation(g_bump_shader, 1, "tan");
    g_outerRoom.drawTex();
    
    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_BLEND);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_CULL_FACE);
    glBindTexture(GL_TEXTURE_2D, g_midTexure);
    glUseProgram(g_mand_shader);
    glBindAttribLocation(g_mand_shader, 1, "tan");
    g_middleRoom.drawTex();


    

    
    // Apply a simple continuous rotation to the object:
    glTranslatef(g_DiceModel.getCenter().x(), g_DiceModel.getCenter().y(), g_DiceModel.getCenter().z());
    glRotatef(g_rotation_angle, 0.0f, 1.0f, 0.0f);
    glRotatef(g_rotation_angle*0.8f, 1.0f, 0.0f, 0.0f);
    glRotatef(g_rotation_angle*0.6f, 0.0f, 0.0f, 1.0f);
    
    glTranslatef(-g_DiceModel.getCenter().x(), -g_DiceModel.getCenter().y(), -g_DiceModel.getCenter().z());
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_DiceTexture);
    glUseProgram(g_lattice_shader);
    g_DiceModel.drawTex();
   
    
    glLoadIdentity();
    
    //draw tiles
    gluLookAt(viewpoint.x(),viewpoint.y(),viewpoint.z(),viewdirect.x(),viewdirect.y(),viewdirect.z(),uormal.x(),uormal.y(),uormal.z());
    for(int i=0; i<185; i++){
        if(i%4==0){
            glUseProgram(g_red_shader);
            g_tiles[i].draw();
        }
        else if(i%4==1){
            glUseProgram(g_green_shader);
            g_tiles[i].draw();
        }
        else if(i%4==2){
            glUseProgram(g_blue_shader);
            g_tiles[i].draw();
        }
        else{
            glUseProgram(g_yellow_shader);
            g_tiles[i].draw();
        }
    }
    if(g_red_chess.getPosition()>=-5){
        glUseProgram(g_red_shader);
        g_red_chess.draw();
    }
    if(g_green_chess.getPosition()>=-5){
        glUseProgram(g_green_shader);
        g_green_chess.draw();
    }
    if(g_blue_chess.getPosition()>=-5){
        glUseProgram(g_blue_shader);
        g_blue_chess.draw();
    }
    if(g_yellow_chess.getPosition()>=-5){
        glUseProgram(g_yellow_shader);
        g_yellow_chess.draw();
    }
    
    //glLoadIdentity();
    //glEnable(GL_TEXTURE_2D);
    
    glBindTexture(GL_TEXTURE_2D, g_EnemyTexture);
    for(int i=0; i<HARD_OUT+HARD_IN; i++){
        glUseProgram(g_enemy_shader);
        
        g_enemy_chess[i].drawTex();
        g_enemy_chess[i].roll(PI/200);
        
        

    }
    
    // rite any shader compile/link errors to the screen here:
    if (g_compile_link_log != "")
        drawGlutString(g_compile_link_log.c_str());
    	
	glutSwapBuffers();
}

void move(int nextPos){
    // When encounter an enemy, move it to inner court
    
    if(g_tiles[nextPos].getOccupy()==-1){
        
        for(int i=0; i<g_enemy_chess.size(); i++){
            if(g_enemy_chess[i].getPosition()==nextPos){
                // if kill an enemy in inner court, move it to a random place in outer court
                if(nextPos>=100){
                    int pos=rand()%100;
                    float dx=g_tiles[pos].getCenter().x();
                    float dy=g_tiles[pos].getCenter().y();
                    float dz=g_tiles[pos].getCenter().z();
                    g_enemy_chess[i].moveTo(dx, dy, dz);
                    g_enemy_chess[i].setPosition(pos);
                    g_tiles[pos].setOccupy(-1);
                }
                else{
                    g_enemy_chess[i].setOrder();
                    int ini=rand()%9;
                    int pos;
                    if(g_enemy_chess[i].getHorv()==0){
                        pos=100+ini*9+g_enemy_chess[i].getOrder();
                        
                    }
                    else{
                        pos=100+ini+g_enemy_chess[i].getOrder()*9;
                    }
                    float dx=g_tiles[pos].getCenter().x();
                    float dy=g_tiles[pos].getCenter().y();
                    float dz=g_tiles[pos].getCenter().z();
                    g_enemy_chess[i].moveTo(dx, dy, dz);
                    g_enemy_chess[i].setPosition(pos);
                    g_tiles[pos].setOccupy(-1);
                    
                }
            }
        }
    }
}


void update(void)
{
    // Step 1: Roll the dice
    
    if(mode==20){
        
    if(timer<200){
        dicemode=true;
        g_rotation_angle += 8.0f;
    }
    else{
        if(dicemode){
            currentDice=rand()%6;
            switch(currentDice){
                case 0:
                    g_rotation_angle=DICE_ONE;
                    break;
                case 1:
                    g_rotation_angle=DICE_TWO;
                    break;
                case 2:
                    g_rotation_angle=DICE_THREE;
                    break;
                case 3:
                    g_rotation_angle=DICE_FOUR;
                    break;
                case 4:
                    g_rotation_angle=DICE_FIVE;
                    break;
                case 5:
                    g_rotation_angle=DICE_SIX;
                    break;
          
            }
            currentDice++;
            if(currentDice!=6){
                score-=6;
            }
            g_compile_link_log="Dice rolled. Press 1 for red chess, 2 for green chess, 3 for blue chess or 4 for yellow chess\n";
            dicemode=false;
            dice_rolled=true;
       
        }
    }
    timer++;
    }
    
    // Step 2: User turn
    
    // begin
    if(mode==11){
        g_compile_link_log="If you decide to move this chess, press g. If not, press 1 for red chess, 3 for blue chess or 4 for yellow chess.\n";
        
        if(decided==true){
            dice_rolled=false;
            timer++;
            int currentPos=g_red_chess.getPosition();
            int nextPos=g_tiles[currentPos].getNext();
            if(timer>=0){
                g_compile_link_log="green moves\n";
            }
            
            if(timer%20==10){
                if(g_red_chess.getPosition()==-1){
                    g_red_chess.moveTo(g_tiles[1].getCenter().x(), g_tiles[1].getCenter().y(), g_tiles[1].getCenter().z()+BOARD_OFFSET);
                    g_red_chess.setPosition(1);
                    g_tiles[1].setOccupy(1);
                    
                    decided=false;
                    mode=5;
                    
                }
                else{
                    if(nextPos==0){
                        nextPos=100;
                    }
                    float destx=g_tiles[nextPos].getCenter().x();
                    float desty=g_tiles[nextPos].getCenter().y();
                    float destz=g_tiles[nextPos].getCenter().z()+BOARD_OFFSET;
                    g_red_chess.moveTo(destx, desty, destz);
                    g_red_chess.setPosition(nextPos);
                    g_tiles[currentPos].setOccupy(0);
                    
                    move(nextPos);
                    g_tiles[nextPos].setOccupy(1);
                    
                }
            }
            
            if(timer>=20*currentDice){
                if(g_tiles[currentPos].getOwner()==g_tiles[currentPos].getOccupy()){
                    // move one step further if the color of the tile is same as the color of the chess
                    if(nextPos==0){
                        nextPos=100;
                    }
                    float destx=g_tiles[nextPos].getCenter().x();
                    float desty=g_tiles[nextPos].getCenter().y();
                    float destz=g_tiles[nextPos].getCenter().z()+BOARD_OFFSET;
                    g_red_chess.moveTo(destx, desty, destz);
                    g_red_chess.setPosition(currentPos+1);
                    g_tiles[currentPos].setOccupy(0);
                    move(nextPos);
                    g_tiles[nextPos].setOccupy(1);
                    
                }
                
                decided=false;
                mode=5;
                
            }
        }
        
    }
    
    
    
    if(mode==12){
        g_compile_link_log="If you decide to move this chess, press g. If not, press 1 for red chess, 3 for blue chess or 4 for yellow chess.\n";

        if(decided==true){
            dice_rolled=false;
            timer++;
            int currentPos=g_green_chess.getPosition();
            int nextPos=g_tiles[currentPos].getNext();
            if(timer>=0){
                g_compile_link_log="green moves\n";
            }
            
            if(timer%20==10){
                if(g_green_chess.getPosition()==-1){
                    g_green_chess.moveTo(g_tiles[26].getCenter().x(), g_tiles[26].getCenter().y(), g_tiles[26].getCenter().z()+BOARD_OFFSET);
                    g_green_chess.setPosition(26);
                    g_tiles[26].setOccupy(2);
                    
                    decided=false;
                    mode=5;
                    
                }
                else{
                    if(nextPos==25){
                        nextPos=100;
                    }
                    float destx=g_tiles[nextPos].getCenter().x();
                    float desty=g_tiles[nextPos].getCenter().y();
                    float destz=g_tiles[nextPos].getCenter().z()+BOARD_OFFSET;
                    g_green_chess.moveTo(destx, desty, destz);
                    g_green_chess.setPosition(nextPos);
                    g_tiles[currentPos].setOccupy(0);
                    
                    // move begins
                    // When encounter an enemy, move it to inner court
                    move(nextPos);
                                        // move ends
                    g_tiles[nextPos].setOccupy(2);
                    
                }
            }
            
            if(timer>=20*currentDice){
                if(g_tiles[currentPos].getOwner()==g_tiles[currentPos].getOccupy()){
                    if(nextPos==25){
                        nextPos=100;
                    }
                    // move one step further if the color of the tile is same as the color of the chess
                    float destx=g_tiles[nextPos].getCenter().x();
                    float desty=g_tiles[nextPos].getCenter().y();
                    float destz=g_tiles[nextPos].getCenter().z()+BOARD_OFFSET;
                    g_green_chess.moveTo(destx, desty, destz);
                    g_green_chess.setPosition(currentPos+1);
                    g_tiles[currentPos].setOccupy(0);
                    move(nextPos);
                    g_tiles[nextPos].setOccupy(2);

                }
                
                decided=false;
                mode=5;
                
            }
        }
        
    }
    
    if(mode==13){
        g_compile_link_log="If you decide to move this chess, press g. If not, press 1 for red chess, 3 for blue chess or 4 for yellow chess.\n";
        
        if(decided==true){
            dice_rolled=false;
            timer++;
            int currentPos=g_blue_chess.getPosition();
            int nextPos=g_tiles[currentPos].getNext();
            if(timer>=0){
                g_compile_link_log="green moves\n";
            }
            
            if(timer%20==10){
                if(g_blue_chess.getPosition()==-1){
                    g_blue_chess.moveTo(g_tiles[51].getCenter().x(), g_tiles[51].getCenter().y(), g_tiles[51].getCenter().z()+BOARD_OFFSET);
                    g_blue_chess.setPosition(51);
                    g_tiles[51].setOccupy(3);
                    
                    decided=false;
                    mode=5;
                    
                }
                else{
                    if(nextPos==50){
                        nextPos=100;
                    }
                    float destx=g_tiles[nextPos].getCenter().x();
                    float desty=g_tiles[nextPos].getCenter().y();
                    float destz=g_tiles[nextPos].getCenter().z()+BOARD_OFFSET;
                    g_blue_chess.moveTo(destx, desty, destz);
                    g_blue_chess.setPosition(nextPos);
                    g_tiles[currentPos].setOccupy(0);
                    move(nextPos);
                    g_tiles[nextPos].setOccupy(3);
                    
                }
            }
            
            if(timer>=20*currentDice){
                if(g_tiles[currentPos].getOwner()==g_tiles[currentPos].getOccupy()){
                    // move one step further if the color of the tile is same as the color of the chess
                    if(nextPos==50){
                        nextPos=100;
                    }
                    float destx=g_tiles[nextPos].getCenter().x();
                    float desty=g_tiles[nextPos].getCenter().y();
                    float destz=g_tiles[nextPos].getCenter().z()+BOARD_OFFSET;
                    g_blue_chess.moveTo(destx, desty, destz);
                    g_blue_chess.setPosition(currentPos+1);
                    g_tiles[currentPos].setOccupy(0);
                    move(nextPos);
                    g_tiles[nextPos].setOccupy(3);
                    
                }
                
                decided=false;
                mode=5;
                
            }
        }
        
    
    
    }
    
    if(mode==14){
        g_compile_link_log="If you decide to move this chess, press g. If not, press 1 for red chess, 3 for blue chess or 4 for yellow chess.\n";
        
        if(decided==true){
            dice_rolled=false;
            timer++;
            int currentPos=g_yellow_chess.getPosition();
            int nextPos=g_tiles[currentPos].getNext();
            if(timer>=0){
                g_compile_link_log="green moves\n";
            }
            
            if(timer%20==10){
                if(g_yellow_chess.getPosition()==-1){
                    g_yellow_chess.moveTo(g_tiles[76].getCenter().x(), g_tiles[76].getCenter().y(), g_tiles[76].getCenter().z()+BOARD_OFFSET);
                    g_yellow_chess.setPosition(76);
                    g_tiles[76].setOccupy(4);
                    
                    decided=false;
                    mode=5;
                    
                }
                else{
                    if(nextPos==75){
                        nextPos=100;
                    }
                    float destx=g_tiles[nextPos].getCenter().x();
                    float desty=g_tiles[nextPos].getCenter().y();
                    float destz=g_tiles[nextPos].getCenter().z()+BOARD_OFFSET;
                    g_yellow_chess.moveTo(destx, desty, destz);
                    g_yellow_chess.setPosition(nextPos);
                    g_tiles[currentPos].setOccupy(0);
                    move(nextPos);
                    g_tiles[nextPos].setOccupy(4);
                    
                }
            }
            
            if(timer>=20*currentDice){
                if(g_tiles[currentPos].getOwner()==g_tiles[currentPos].getOccupy()){
                    // move one step further if the color of the tile is same as the color of the chess
                    if(nextPos==75){
                        nextPos=100;
                    }
                    float destx=g_tiles[nextPos].getCenter().x();
                    float desty=g_tiles[nextPos].getCenter().y();
                    float destz=g_tiles[nextPos].getCenter().z()+BOARD_OFFSET;
                    g_yellow_chess.moveTo(destx, desty, destz);
                    g_yellow_chess.setPosition(currentPos+1);
                    g_tiles[currentPos].setOccupy(0);
                    move(nextPos);
                    g_tiles[nextPos].setOccupy(4);
                    
                }
                
                decided=false;
                mode=5;
                
            }
        }
        
    
    
    }
    // end
    if(mode==5){
        cout << "Red Pos: "<< g_red_chess.getPosition()<< endl;
        if(g_red_chess.getPosition()>=180){
            if(g_red_chess.getWon()==false){
                g_red_chess.wo();
                cout << g_red_chess.getWon() << endl;
                score+=300;
            }
        }
        if(g_green_chess.getPosition()>=180){
            if(g_green_chess.getWon()==false){
                g_green_chess.wo();
                score+=300;
            }
        }
        if(g_blue_chess.getPosition()>=180){
            if(g_blue_chess.getWon()==false){
                g_blue_chess.wo();
                score+=300;
            }
        }
        if(g_yellow_chess.getPosition()>=180){
            if(g_yellow_chess.getWon()==false){
                g_yellow_chess.wo();
                score+=300;
            }
        }
        if(g_red_chess.getPosition()==-6){
            if(g_green_chess.getPosition()==-6){
                if(g_blue_chess.getPosition()==-6){
                    if(g_yellow_chess.getPosition()==-6){
                        g_compile_link_log="YOU LOSE!!\n";
                        std::stringstream s;
                        s << "Final_Score: "<< score;
                        s >> g_compile_link_log;
                        mode=50;
                    }
                    
                }
                
            }
            
        }

        if(g_red_chess.getWon()||g_red_chess.getPosition()==-6){
            if(g_green_chess.getWon()||g_green_chess.getPosition()==-6){
                if(g_blue_chess.getWon()||g_blue_chess.getPosition()==-6){
                    if(g_yellow_chess.getWon()||g_yellow_chess.getPosition()==-6){
                        g_compile_link_log="YOU WIN!!\n";
                        std::stringstream s;
                        s << "Final_Score: "<< score;
                        s >> g_compile_link_log;
                        mode=50;
                    }
                    
                }
                
            }
            
        }
       
        
        timer++;
        if(timer%20==0){
            mode=4;
            timer=0;
        }


        
    }
    
    // Step 3: Enemy Turn
    
    if(mode==4){
        timer++;
        if(timer==100){
           g_compile_link_log="enemy turn\n";
            for( int i=0; i<g_enemy_chess.size(); i++){
                if(g_enemy_chess[i].getPosition()>-1){
                    // if the enemy is in outer court, move like player chess, when an enemy hit a player
                    // chess, that player chess is killed.
                    if(g_enemy_chess[i].getPosition()<100){
                        int r=rand()%6;
                        for(int j=0; j<r; j++){
                            int currentPos=g_enemy_chess[i].getPosition();
                            int nextPos=g_tiles[currentPos].getNext();
                            float destx=g_tiles[nextPos].getCenter().x();
                            float desty=g_tiles[nextPos].getCenter().y();
                            float destz=g_tiles[nextPos].getCenter().z();
                            g_enemy_chess[i].moveTo(destx, desty, destz);
                            g_enemy_chess[i].setPosition(nextPos);
                            g_tiles[currentPos].setOccupy(0);
                            if(g_tiles[nextPos].getOccupy()==1){
                                g_red_chess.setPosition(-6);
                            }
                            else if(g_tiles[nextPos].getOccupy()==2){
                                g_green_chess.setPosition(-6);
                            }
                            else if(g_tiles[nextPos].getOccupy()==3){
                                g_blue_chess.setPosition(-6);
                            }
                            else if(g_tiles[nextPos].getOccupy()==4){
                                g_yellow_chess.setPosition(-6);
                            }
                            
                            g_tiles[nextPos].setOccupy(-1);

                            
                        }
                    }
                    // if the enemy is in inner court, move in a special way that need players to figure out
                    else{
                        int currentPos=g_enemy_chess[i].getPosition();
                        int nextPos;
                        if(g_enemy_chess[i].getHorv()==0){
                             nextPos=(currentPos-100)/9*9+100+g_enemy_chess[i].getOrder();
                            
                        }
                        else{
                             nextPos=(currentPos-100)%9+g_enemy_chess[i].getOrder()*9+100;
                        }
                        cout << nextPos << endl;
                        float destx=g_tiles[nextPos].getCenter().x();
                        float desty=g_tiles[nextPos].getCenter().y();
                        float destz=g_tiles[nextPos].getCenter().z();
                        g_enemy_chess[i].moveTo(destx, desty, destz);
                        g_enemy_chess[i].setPosition(nextPos);
                        g_tiles[currentPos].setOccupy(0);
                        if(g_tiles[nextPos].getOccupy()==1){
                            g_red_chess.setPosition(-6);
                        }
                        else if(g_tiles[nextPos].getOccupy()==2){
                            g_green_chess.setPosition(-6);
                        }
                        else if(g_tiles[nextPos].getOccupy()==3){
                            g_blue_chess.setPosition(-6);
                        }
                        else if(g_tiles[nextPos].getOccupy()==4){
                            g_yellow_chess.setPosition(-6);
                        }
                        
                        g_tiles[nextPos].setOccupy(-1);

                        
                        
                    }
                    
                }
                
                
            }
        }
        if(timer>=400){
            g_compile_link_log="End of this turn. Press o to roll dice and begin a new turn\n";
        }
        else if(timer>=200){
            g_compile_link_log="";
            std::stringstream s;
            s << "Current_Score:" << score;
            
            s >> g_compile_link_log;
        }
    }
    
    glutPostRedisplay();
}

void keyboard (unsigned char key, int x, int y)
{
	switch (key) {
        case ESCAPE:
            exit(0);
        case 'r':
            init();
            break;
        case 'i': // only use for debug
            mode=2;
            break;
        case 'o': // dice
            if(!dice_rolled){
                mode=20;
                timer=0;
            }
            break;
        case 'w': // watch inside the wall
            if(dice_rolled){
                mode=30;
                currentDice-=2;
                currentDice= currentDice > 0 ? currentDice : 0;
            }
            break;
            
        
        case 'a': // view the game board at red position
        
            if(dice_rolled){
                
                mode=21;
                currentDice-=1;
                currentDice= currentDice > 0 ? currentDice : 0;
            }
            break;
            
        case 'b': // view the game board at green position
            
            if(dice_rolled){
                mode=22;
                currentDice-=1;
                currentDice= currentDice > 0 ? currentDice : 0;
            }
            break;
            
        case 'c': // view the game board at blue position
            
            if(dice_rolled){
                mode=23;
                currentDice-=1;
                currentDice= currentDice > 0 ? currentDice : 0;
            }
            break;
            
        case 'd': // view the game board at yellow position
            
            if(dice_rolled){
                mode=24;
                currentDice-=1;
                currentDice= currentDice > 0 ? currentDice : 0;
            }
            break;



        
        case '1': // red pov
            if(dice_rolled){
                if(g_red_chess.getPosition()>-2){
                    
                    mode=11;
                    timer=0;
                }
                else if(g_red_chess.getPosition()==-6){
                    g_compile_link_log="Red chess is dead.";
                }
                else{
                    g_compile_link_log="Red chess won.";
                }
            }
            break;
        case '2': // green pov
            if(dice_rolled){
                if(g_green_chess.getPosition()>-2){
                    mode=12;
                    timer=0;
                }
               else if(g_red_chess.getPosition()==-6){
                    g_compile_link_log="Green chess is dead.";
                }
               else{
                   g_compile_link_log="Green chess won.";
               }
            }
            break;
        case '3': // blue pov
            if(dice_rolled){
                if(g_blue_chess.getPosition()>-2){
                    mode=13;
                    timer=0;
                }
                else if(g_red_chess.getPosition()==-6){
                    g_compile_link_log="Blue chess is dead.";
                }
                else{
                    g_compile_link_log="Blue chess won.";
                }
            }
            break;
        case '4': // yellow pov
            if(dice_rolled){
                if(g_yellow_chess.getPosition()>-2){
                    mode=14;
                    timer=0;
                }
                else if(g_red_chess.getPosition()==-6){
                    g_compile_link_log="Yellow chess is dead.";
                }
                else{
                    g_compile_link_log="Yellow chess won.";
                }
            }
            break;
        case '6': // only use for debug
            dice_rolled=true;
            currentDice=20;
        case 'g': // press to decide a move
            if(mode>10&&mode<15){
                decided=true;
            }
            break;



        default:
            break;
    };

    glutPostRedisplay();
}

/**********************************************************************
 * this function is called whenever the mouse is moved
 **********************************************************************/

void handle_mouse_motion(int x, int y)
{
    // If you do something here that causes a change to what the user
    // would see, call glutPostRedisplay to force a redraw
    //glutPostRedisplay();
}

/**********************************************************************
 * this function is called for non-standard keys like up/down/left/right
 * arrows.
 **********************************************************************/
void special_key(int key, int x, int y)
{
    switch (key) {
        case GLUT_KEY_UP: //up arrow
            break;
        case GLUT_KEY_DOWN: //down arrow
            break;
        default:      
            break;
    }
    
    glutPostRedisplay();

}

/**********************************************************************
 * this function is called whenever a mouse button is pressed or released
 **********************************************************************/
void handle_mouse_click(int button, int state, int x, int y)
{
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE|GLUT_RGBA|GLUT_ALPHA|GLUT_DEPTH);
	g_window_width = 600;
    g_window_height = 600;
    glutInitWindowSize(INITIAL_WIDTH, INITIAL_HEIGHT);  
    glutInitWindowPosition(INITIAL_X_POS, INITIAL_Y_POS);  
    glutCreateWindow(WINDOW_NAME);  
	init();
	glutDisplayFunc(draw_scene); 
	glutReshapeFunc(resize_scene);
	glutKeyboardFunc(keyboard);
    glutIdleFunc(update);
	glutMouseFunc(handle_mouse_click);
	glutMotionFunc(handle_mouse_motion); 

	glutMainLoop();
	return 0;
}