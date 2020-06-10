#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <string>

#define GAME_TERMINATE -1

// ALLEGRO Variables
ALLEGRO_DISPLAY* display = NULL;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_BITMAP *image = NULL;
ALLEGRO_BITMAP *image2 = NULL;
ALLEGRO_BITMAP *image3 = NULL;
ALLEGRO_BITMAP *background = NULL;
ALLEGRO_KEYBOARD_STATE keyState ;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_TIMER *timer2 = NULL;
ALLEGRO_TIMER *timer3 = NULL;
ALLEGRO_SAMPLE *song = NULL;
ALLEGRO_FONT *font = NULL;
// ALLEGRO Variables define by ourself



//Custom Definition
const char *title = "Final Project 10xxxxxxx";
const float FPS = 60;
const int WIDTH = 400;
const int HEIGHT = 600;
typedef struct character{
    int x;
    int y;
    ALLEGRO_BITMAP *image_path;

}Character;

Character character1;
Character character2;
Character character3;

int imageWidth = 0;
int imageHeight = 0;
int draw = 0;
int done = 0;
int window = 1;
bool judge_next_window = false;
bool ture = true; //true: appear, false: disappear
bool next = false; //true: trigger
bool dir = true; //true: left, false: right
bool music_control = true; // true: play music, false: stop music

// Define by ourself
int option_y = 80; // first option location_y in menu
int option_rect_height = 40; // menu rectangle height
int option_space_y = 100; // space between every option
int menu_selection = 1; // control selection in menu


void show_err_msg(int msg);
void game_init();
void game_begin();
int process_event();
int game_run();
void game_destroy();
// Define by ourself
void draw_menu();
void draw_option();
void set_charater();
void load_music(std::string FileName);

int main(int argc, char *argv[]) {
    int msg = 0;

    game_init();
    game_begin();

    while (msg != GAME_TERMINATE) {
        msg = game_run();
        if (msg == GAME_TERMINATE)
            printf("Game Over\n");
    }

    game_destroy();
    return 0;
}

void show_err_msg(int msg) {
    fprintf(stderr, "unexpected msg: %d\n", msg);
    game_destroy();
    exit(9);
}

void game_init() {
    if (!al_init()) {
        show_err_msg(-1);
    }
    if(!al_install_audio()){
        fprintf(stderr, "failed to initialize audio!\n");
        show_err_msg(-2);
    }
    if(!al_init_acodec_addon()){
        fprintf(stderr, "failed to initialize audio codecs!\n");
        show_err_msg(-3);
    }
    if (!al_reserve_samples(1)){
        fprintf(stderr, "failed to reserve samples!\n");
        show_err_msg(-4);
    }
    // Create display
    display = al_create_display(WIDTH, HEIGHT);
    event_queue = al_create_event_queue();
    if (display == NULL || event_queue == NULL) {
        show_err_msg(-5);
    }
    // Initialize Allegro settings
    al_set_window_position(display, 0, 0);
    al_set_window_title(display, title);
    al_init_primitives_addon();
    al_install_keyboard();
    al_install_audio();
    al_init_image_addon();
    al_init_acodec_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    // Register event
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
}

void game_begin() {
    load_music("menu_background.wav");
    draw_menu();
}

int process_event(){
    // Request the event
    ALLEGRO_EVENT event;
    al_wait_for_event(event_queue, &event);

    // Our setting for controlling animation
    if(event.timer.source == timer){
        if(character2.x < -150) dir = false;
        else if(character2.x > WIDTH+50) dir = true;

        if(dir) character2.x -= 10;
        else character2.x += 10;
    }
    if(event.timer.source == timer2){
        ture = false;
        next = true;
    }
    if(event.timer.source == timer3){
        if(next) next = false;
        else ture = true;
    }

    // Keyboard
    if(event.type == ALLEGRO_EVENT_KEY_UP)
    {
        switch(event.keyboard.keycode)
        {
            // Control
            case ALLEGRO_KEY_W:
                character1.y -= 30;
                break;
            case ALLEGRO_KEY_S:
                character1.y += 30;
                break;
            case ALLEGRO_KEY_A:
                character1.x -= 30;
                break;
            case ALLEGRO_KEY_D:
                character1.x += 30;
                break;

            // For Start Menubackground
            case ALLEGRO_KEY_ENTER:
                judge_next_window = true;
                break;
            case ALLEGRO_KEY_DOWN:
                menu_selection = (menu_selection%3) + 1;
                break;
            case ALLEGRO_KEY_UP:
                menu_selection = menu_selection - 1;
                if(menu_selection == 0) menu_selection = menu_selection + 3; // 3 -> number of option
                break;

        }
    }

    // Shutdown our program
    else if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        return GAME_TERMINATE;

    return 0;
}

int game_run() {
    int error = 0;
    // First window(Menu)
    if(window == 1){
        if (!al_is_event_queue_empty(event_queue)) {
            error = process_event();
            if(judge_next_window) {
                switch(menu_selection){
                    case 1:
                        window = 2;
                        set_charater();
                        break;
                    case 2:
                        window = 3; // option's window
                        menu_selection = 1;
                        draw_option();
                        break;
                    case 3:
                        error = GAME_TERMINATE;
                        break;
                }

            }
            else{
                draw_menu();
            }
        }
    }
    // Second window(Main Game)
    else if(window == 2){
        // Change Image for animation
        al_draw_bitmap(background, 0,0, 0);
        if(ture) al_draw_bitmap(character1.image_path, character1.x, character1.y, 0);

        if(dir) al_draw_bitmap(character2.image_path, character2.x, character2.y, 0);
        else al_draw_bitmap(character3.image_path, character2.x, character2.y, 0);

        al_flip_display();
        al_clear_to_color(al_map_rgb(0,0,0));

        // Listening for new event
        if (!al_is_event_queue_empty(event_queue)) {
            error = process_event();
        }
    }
    else if(window == 3){
        judge_next_window = false;
        if (!al_is_event_queue_empty(event_queue)) {
            error = process_event();
            if(judge_next_window) {
                switch(menu_selection){
                    case 1: // change music play or not
                        music_control = !music_control;
                        if( !music_control ) al_destroy_sample(song);
                        else load_music("menu_background.wav");
                        judge_next_window = false;
                        draw_option();
                        break;
                    case 2:

                        break;
                    case 3: // Return to menu
                        window = 1;
                        menu_selection = 1;
                        judge_next_window = false;
                        draw_menu();
                        break;
                }

            }
            else{
                //printf("%d\n", menu_selection);
                draw_option();
            }
        }
    }
    return error;
}

void draw_option(){
    al_clear_to_color(al_map_rgb(100,100,100));

    // Option title
    al_draw_text(font, al_map_rgb(255,255,255), WIDTH/2, option_y , ALLEGRO_ALIGN_CENTRE, "Option");
    al_draw_rectangle(WIDTH/2-150, option_y-10, WIDTH/2+150, option_y-10+option_rect_height, al_map_rgb(255, 255, 255), 0);
    // Mute option
    al_draw_text(font, al_map_rgb(255,255,255), WIDTH/2+30, option_y+option_space_y , ALLEGRO_ALIGN_CENTRE, (music_control)?"Play music":"Stop music");
    al_draw_rectangle(WIDTH/2-90, option_y-10+option_space_y, WIDTH/2+150, option_y-10+option_space_y+option_rect_height, al_map_rgb(255, 255, 255), 0);
    // Nothing option
    al_draw_text(font, al_map_rgb(255,255,255), WIDTH/2+30, option_y+2*option_space_y , ALLEGRO_ALIGN_CENTRE, "Nothing Option");
    al_draw_rectangle(WIDTH/2-90, option_y-10+2*option_space_y, WIDTH/2+150, option_y-10+2*option_space_y+option_rect_height, al_map_rgb(255, 255, 255), 0);
    // Return to menu
    al_draw_text(font, al_map_rgb(255,255,255), WIDTH/2+30, option_y+3*option_space_y , ALLEGRO_ALIGN_CENTRE, "Return to Menu");
    al_draw_rectangle(WIDTH/2-90, option_y-10+3*option_space_y, WIDTH/2+150, option_y-10+3*option_space_y+option_rect_height, al_map_rgb(255, 255, 255), 0);
    // Option pointer
    al_draw_text(font, al_map_rgb(255,255,255), WIDTH/2-120, option_y+menu_selection*option_space_y , ALLEGRO_ALIGN_CENTRE, "=>");

    al_flip_display();
}

void draw_menu(){
    al_clear_to_color(al_map_rgb(100,100,100));
    // Load and draw text
    font = al_load_ttf_font("material/font/Balsamiq/BalsamiqSans-Italic.ttf",20,0);

    // Game title
    al_draw_text(font, al_map_rgb(255,255,255), WIDTH/2, option_y , ALLEGRO_ALIGN_CENTRE, "Game Name Here");
    al_draw_rectangle(WIDTH/2-150, option_y-10, WIDTH/2+150, option_y-10+option_rect_height, al_map_rgb(255, 255, 255), 0);
    // Game start option
    al_draw_text(font, al_map_rgb(255,255,255), WIDTH/2+30, option_y+option_space_y , ALLEGRO_ALIGN_CENTRE, "Game Start");
    al_draw_rectangle(WIDTH/2-90, option_y-10+option_space_y, WIDTH/2+150, option_y-10+option_space_y+option_rect_height, al_map_rgb(255, 255, 255), 0);
    // Game option option
    al_draw_text(font, al_map_rgb(255,255,255), WIDTH/2+30, option_y+2*option_space_y , ALLEGRO_ALIGN_CENTRE, "Options");
    al_draw_rectangle(WIDTH/2-90, option_y-10+2*option_space_y, WIDTH/2+150, option_y-10+2*option_space_y+option_rect_height, al_map_rgb(255, 255, 255), 0);
    // Game escape option
    al_draw_text(font, al_map_rgb(255,255,255), WIDTH/2+30, option_y+3*option_space_y , ALLEGRO_ALIGN_CENTRE, "Exit");
    al_draw_rectangle(WIDTH/2-90, option_y-10+3*option_space_y, WIDTH/2+150, option_y-10+3*option_space_y+option_rect_height, al_map_rgb(255, 255, 255), 0);
    // Option pointer
    al_draw_text(font, al_map_rgb(255,255,255), WIDTH/2-120, option_y+menu_selection*option_space_y , ALLEGRO_ALIGN_CENTRE, "=>");

    al_flip_display();
}

void load_music(std::string FileName){
    // Load sound
    song = al_load_sample( ("material/music/"+FileName).c_str() );
    if (!song){
        printf( "Audio clip sample not loaded!\n" );
        show_err_msg(-6);
    }
    // Loop the song until the display closes
    al_play_sample(song, 1.0, 0.0, 1.0,ALLEGRO_PLAYMODE_LOOP,NULL);
}

void set_charater(){
    // Setting Character
    character1.x = WIDTH / 2;
    character1.y = HEIGHT / 2 + 150;
    character2.x = WIDTH + 100;
    character2.y = HEIGHT / 2 - 280;
    character1.image_path = al_load_bitmap("tower.png");
    character2.image_path = al_load_bitmap("teemo_left.png");
    character3.image_path = al_load_bitmap("teemo_right.png");
    background = al_load_bitmap("stage.jpg");

    //Initialize Timer
    timer  = al_create_timer(1.0/15.0);
    timer2  = al_create_timer(1.0);
    timer3  = al_create_timer(1.0/10.0);
    al_register_event_source(event_queue, al_get_timer_event_source(timer)) ;
    al_register_event_source(event_queue, al_get_timer_event_source(timer2)) ;
    al_register_event_source(event_queue, al_get_timer_event_source(timer3)) ;
    al_start_timer(timer);
    al_start_timer(timer2);
    al_start_timer(timer3);
}

void game_destroy() {
    // Make sure you destroy all things
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_timer(timer2);
    al_destroy_bitmap(image);
    al_destroy_sample(song);
}
