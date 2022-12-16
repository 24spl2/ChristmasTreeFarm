#define _CRT_SECURE_NO_WARNINGS
#include "snail.cpp"
#include "cow.cpp"
#include "_cow_supplement.cpp"
#include "jim.cpp"
#include "_cow_optimization.cpp"

//__________________orbit_____________________________________
struct OrbitCamera {
    double distance;
    double angle_of_view;
    double theta;
    double phi;
};

mat4 orbit_camera_get_C(OrbitCamera *orbit) { FORNOW_UNUSED(orbit);
    // TODO (see slides)
    return RotationY(orbit->theta) * RotationX(orbit->phi) * Translation({0, 0, orbit->distance});
}

void orbit_camera_move(OrbitCamera *orbit) { FORNOW_UNUSED(orbit);
    if (input.mouse_left_held) {
        // TODO overwrite orbit->theta Y
        orbit->theta += -input._mouse_dx_NDC;
        // TODO overwrite orbit->phi (make sure you clamp phi or it will be possible to "pass over the north or south poles")
        //phi is x
        double clamp = orbit->phi + input._mouse_dy_NDC;
        orbit->phi = CLAMP(clamp, -RAD(80), RAD(80));

    }
    // TODO overwrite orbit->distance
    orbit->distance += input._mouse_wheel_offset;
}

//__________________box_____________________________________

vec3 basic_box_vertex_positions[] = {
    {-1,-1,1}, {-1,-1,-1}, {-1,1,-1}, {-1,1,1}, // left
    {1,1,1},{1,1,-1},{1,-1,-1},{1,-1,1},        // right
    {1,-1,1},{1,-1,-1},{-1,-1,-1},{-1,-1,1},    // bottom
    {-1,1,1}, {-1,1,-1}, {1,1,-1}, {1,1,1},     // top
    {-1,1,-1}, {-1,-1,-1}, {1,-1,-1}, {1,1,-1}, // back
    {1,1,1},{1,-1,1},{-1,-1,1},{-1,1,1},        // front
};


//__________________tree_____________________________________

struct Tree {
    int place;
    int color;
    double time;
    double lerp[12] = {};
};

//_______________________________________________________
//_______________________________________________________

void interact() {

    init();

    //_________initializing variables X Camera SetUp_____

    OrbitCamera orbit = { 200, RAD(75), RAD(15), RAD(-30) };

    FancyTriangleMesh3D pine = meshlib.fancy_cylinder;
    pine.texture_filename = "matcap.jpeg";

    bool drawGrid = true;

    vec3 santa = V3(0.0, 8.0, 0.0);

    Tree trees[10][10] = {};


    while(begin_frame()){
        orbit_camera_move(&orbit);

        mat4 C = orbit_camera_get_C(&orbit);
        mat4 P = tform_get_P_perspective(orbit.angle_of_view);
        mat4 V = inverse(C);

        mat4 PV_mode = P * V;

        //__________________ground_____________________________________

        vec3 duskGreen = AVG(monokai.black, V3(.2, .5, .25));
        vec3 greenGlisten = AVG(monokai.black, AVG(duskGreen, monokai.blue));
        vec3 darkSky = AVG(monokai.black, AVG(monokai.blue, monokai.black));
        
        //______________________sky box__________________________________
        
        vec3 vertex_colors[] = { // fornow: had to match orderings of the walls by trial and error
                        .5 * darkSky, .5 * darkSky, .01 * darkSky, .01 * darkSky, // left
                        .01 * darkSky, .01 * darkSky, .5 * darkSky, .5 * darkSky, // right
                        .99 * greenGlisten, .90 * greenGlisten, .80 * greenGlisten, .90 * greenGlisten, // ground
                        .01 * darkSky, .01 * darkSky, .01 * darkSky, .01 * darkSky, // sky
                        .01 * darkSky, .5 * darkSky, .5 * darkSky, .01 * darkSky, // back
                        .01 * darkSky, .5 * darkSky, .5 * darkSky, .01 * darkSky, // front
                    };

        basic_draw(QUADS, PV_mode * Translation(0, 999.0, 0) * Scaling(1000), 24, basic_box_vertex_positions, vertex_colors);
        
        //_________________________grid lines____________________________

        //show grid lines or hide
        if (input.key_pressed['-']) drawGrid = false;
        if (input.key_pressed['=']) drawGrid = true;

        double r = 100;

        vec3 gridLines = AVG(duskGreen, monokai.white);

        //draw Grid Lines
        if(drawGrid) {
            for (int i = 0 ; i < 6; i++) {
                double scale = (double) i * 20;
                vec3 pair[] = { {-r, 0.01, -scale}, {r, 0.01, -scale}};
                basic_draw(LINES, PV_mode, 2, pair, gridLines);
                vec3 pair2[] = { {-r, 0.01, scale}, {r, 0.01, scale}};
                basic_draw(LINES, PV_mode, 2, pair2, gridLines);
                vec3 pair3[] = { {-scale, 0.01, -r}, {-scale, 0.01, r}};
                basic_draw(LINES, PV_mode, 2, pair3, gridLines);
                vec3 pair4[] = { {scale, 0.01, -r}, {scale, 0.01, r}};
                basic_draw(LINES, PV_mode, 2, pair4, gridLines);
            }
        }
        
        //__________________________Santa________________________________

        //User Mouse Controls
        if (input.key_held['w']) santa.z -= .7;

        if (input.key_held['s']) santa.z += .7;

        if (input.key_held['d']) santa.x += .7;

        if (input.key_held['a']) santa.x -= .7;

        //draw Santa
        gl_PV(PV_mode);
        gl_begin(POINTS, 20.0);
        gl_color(AVG(monokai.red, monokai.black));
        gl_vertex(santa);
        gl_end();

        //_____________________trees_________________________________
        //_______________________________________________________
        int x_tree = (int) (santa.x  + 100) / 20 % 10;
        int z_tree = (int) (santa.z  + 100) / 20 % 10;

        Tree* current_set_tree = &trees[x_tree][z_tree];


        //______________________placing_________________________________
        if (input.key_pressed['j']) {
            current_set_tree->place = 1;
            current_set_tree->color = 1;
            current_set_tree->time = 0.0;
        }

        if (input.key_pressed['k'] && current_set_tree->place == 1) {
            current_set_tree->place = 0;
            current_set_tree->time = 0.0;
        }

        { //_________________coloring______________________________________

            for (int keys = 1; keys < 6; keys++) {
                if (input.key_pressed['0' + keys] && current_set_tree->place == 1) {
                    current_set_tree->color = keys;
                }
            }

        }

        //_____________________drawing__________________________________

        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {

                Tree *draw_current_tree = &trees[i][j];

                if (draw_current_tree->place == 1) {
                    draw_current_tree->time += .001;

                    double x_tree_map = (((double) i) + .5) * 20 - 100;
                    double y_tree_map = (((double) j) + .5) * 20 - 100;
                    
                    vec3 tree = V3(x_tree_map, 0.0, y_tree_map);

                    if (draw_current_tree->color == 1) {
                        pine.texture_filename = "matcap.jpeg";
                    } else if (draw_current_tree->color == 2) {
                        pine.texture_filename = "matcap1.jpeg";
                    } else if (draw_current_tree->color == 3) {
                        pine.texture_filename = "matcap2.jpeg";
                    } else if (draw_current_tree->color == 4) {
                        pine.texture_filename = "matcap3.jpeg";
                    } else if (draw_current_tree->color == 5) {
                        pine.texture_filename = "matcap4.jpeg";
                    }

                    fancy_draw(P, V, Translation(tree) * 
                        Scaling(.8, LERP(CLAMP(draw_current_tree->time, 0, 1), 0.01, 26), .8), 
                        pine);

                    //___________growing fronds____________
                    if (draw_current_tree->time > 1) {
                        double height = 0.0;
                        double length = 13.0;
                        double spiral = 0.0;

                        //Set Lerp Array
                        for (int lerpSet = 0; lerpSet < 12; lerpSet++) {
                            draw_current_tree->lerp[lerpSet] = 
                                LERP(CLAMP(draw_current_tree->time -1, 0, 1), 0.01, lerpSet + 1);
                        }
                        
                        //draw Lerp Array
                        for (int m = 0; m < 12; m++) {    
                            for (int n = 0; n < 12; n++) {
                                vec3 trans_frond = V3(tree.x, height, tree.z);
                                mat4 fronds = RotationY(RAD(spiral)) * RotationZ(RAD(90));

                                fancy_draw(P, V, Translation(trans_frond) * fronds * 
                                Scaling(.5, draw_current_tree->lerp[11 - m], .5),
                                pine);

                                spiral += 20;
                                height += .2;
                            }
                            length -= 1;
                        }
                        
                    }
                }
            }
        }
        //_______________________________________________________    
    }    
}



int main() {
    interact();
    return 0;
}
