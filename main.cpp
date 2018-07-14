#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "allegro5/allegro_image.h"
#include <iostream>
#include <vector>

using namespace std;

int w = 10;	// szrokosc planszy
int h = 10;	// wysokosc planszy

int w_tail = 80;	// szerokosc kafelka
int h_tail = 80;	// wysokosc kafelka

const int screen_w = w*w_tail;   // szerokosc ekranu (screen width)
const int screen_h = screen_w;    // wysokosc ekranu (screen height)

bool key[ALLEGRO_KEY_MAX];

enum TailType
{
    EMPTY = 0,
    WHITE,
    BLACK,
    ANT,
    TAILS_TYPE_NUM,
};

class Tail
{
public:
    enum TailType type;

    Tail() : type(WHITE){};
};

const string file_with_pngs[TAILS_TYPE_NUM] =
{
    "tiles/empty.png",
    "tiles/white.png",
    "tiles/black.png",
    "tiles/ant.png",
};

class Ant
{
public:
    int x, y;
    int direction;

    Ant(int x, int y)
    {
        this->x = (x/w_tail);
        this->y = (y/h_tail);
        direction = 0;
    }
};

class Board
{
public:
    const float FPS = 8;       // Frames Per Second

    bool start = false;
    vector<vector<Tail> > tails = vector<vector<Tail>>(w, vector<Tail>(h));
    vector<Ant> ants;

    ALLEGRO_BITMAP* bitmaps[TAILS_TYPE_NUM];

    void addNewAnt(int x, int y)
    {
        ants.push_back(Ant(x, y));
    }

    bool initBitmaps()
    {
        for (int i = 0; i < TAILS_TYPE_NUM; ++i)
        {
            bitmaps[i] = al_load_bitmap(file_with_pngs[i].c_str());

            if (!bitmaps[i])
            {
                cerr << "Blad podczas wczytywania bitmapy " << file_with_pngs[i] << "." << endl;
                return false;
            }
        }

        return true;
    }

    bool check_ant_on_edge(){
        for (Ant &ant : ants){
            if(ant.x == 0 || ant.x == (w-1) || ant.y == 0 || ant.y ==(h-1)){
                return true;
            }
        }
        return false;
    }

    void resize(){
        vector<vector<Tail> > tmpV = tails;
        w *= 2;
        h *= 2;
        w_tail /= 2;
        h_tail /= 2;

        //Resize and clean
        tails.resize(w);
        for (int i = 0; i < w; ++i)
        {
            tails[i].resize(h, Tail());
        }

        for(int i=0; i < w; ++i){
            for(int j=0; j < h; ++j)
                tails[i][j].type = WHITE;
        }

        //Scale ants
        for (Ant &ant : ants){
            ant.x += (w/4);
            ant.y += (h/4);
        }

        //Scale drawings
        for(unsigned i=0; i < tmpV.size(); ++i){
            for(unsigned j=0; j < tmpV[i].size(); ++j){
                if(tmpV[i][j].type == BLACK){
                    unsigned tmp_w = i+(w/4);
                    unsigned tmp_h = j+(h/4);
                    tails[tmp_w][tmp_h].type = BLACK;
                }
            }
        }
    }

    void draw()
    {
        al_clear_to_color(al_map_rgb(0,0,0));
        for (int i = 0; i < w; i++) {
            for (int j = 0; j < h; j++) {
                ALLEGRO_BITMAP *target_bitmap = bitmaps[tails[i][j].type];
                al_draw_scaled_bitmap(target_bitmap,
                                      0, 0, // source origin
                                      al_get_bitmap_width(target_bitmap), //source width
                                      al_get_bitmap_height(target_bitmap), //source height
                                      i*w_tail, j*h_tail, //target origin
                                      w_tail, h_tail, //target dimensions
                                      0); //flags

                if(!ants.empty()){
                    for(Ant &ant : ants)
                    {
                        if (ant.x == i && ant.y == j)
                        {
                            al_draw_scaled_bitmap(bitmaps[ANT],
                                                0, 0, // source origin
                                                al_get_bitmap_width(bitmaps[ANT]), //source width
                                                al_get_bitmap_height(bitmaps[ANT]), //source height
                                                i*w_tail, j*h_tail, //target origin
                                                w_tail, h_tail, //target dimensions
                                                0); //flags
                        }
                    }
                }
            }
        }
    }

    void refresh()
    {
        if (ants.empty()) return;

        for(Ant &ant : ants){
            int direction = ant.direction;
            int x = ant.x;
            int y = ant.y;

            if(start)
            {
                if (direction == 0 && tails[x][y].type == WHITE) {
                    tails[x][y].type = BLACK;
                    direction = 1;
                    x = (x+1+w)%w;
                }
                else if (direction == 0 && tails[x][y].type == BLACK) {
                    tails[x][y].type = WHITE;
                    direction = 3;
                    x = (x-1+w)%w;

                }
                else if (direction == 1 && tails[x][y].type == WHITE)
                {
                    tails[x][y].type = BLACK;
                    direction = 2;
                    y = (y+1+h)%h;

                }
                else if (direction == 1 && tails[x][y].type == BLACK)
                {
                    tails[x][y].type = WHITE;
                    direction = 0;
                    y = (y-1+h)%h;

                }
                else if (direction == 2 && tails[x][y].type == WHITE)
                {
                    tails[x][y].type = BLACK;
                    direction = 3;
                    x = (x-1+w)%w;

                }
                else if (direction == 2 && tails[x][y].type == BLACK)
                {
                    tails[x][y].type = WHITE;
                    direction = 1;
                    x = (x+1+w)%w;

                }
                else if (direction == 3 && tails[x][y].type == WHITE)
                {
                    tails[x][y].type = BLACK;
                    direction = 0;
                    y = (y-1+h)%h;

                }
                else if (direction == 3 && tails[x][y].type == BLACK)
                {
                    tails[x][y].type = WHITE;
                    direction = 2;
                    y = (y+1+h)%h;

                }
            }
            ant.direction = direction;
            ant.x = x;
            ant.y = y;
        }
    }
};

ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_MOUSE_STATE state;

int init(Board &board)
{
    if(!al_init())
    {
        cerr << "Blad podczas inicjalizacji allegro." << endl;
        return -1;
    }

    if (!al_init_primitives_addon())
    {
        cerr << "Blad podczas inicjalizacji dodatku 'primitives'." << endl;
        return -1;
    }
    if(!al_install_keyboard())
    {
        cerr << "Blad podczas inicjalizacji klawiatury." << endl;
        return -1;
    }

    if(!al_init_image_addon())
    {
        cerr << "Blad podczas inicjalizacji dodatku 'image'." << endl;
        return -1;
    }

    if(!al_install_mouse())
    {
        cerr << "Blad podczas inicjalizacji mywy." << endl;
        return -1;
    }

    timer = al_create_timer(1.0 / board.FPS);
    if(!timer)
    {
        cerr << "Blad podczas inicjalizacji zegara." << endl;
        return -1;
    }

    display = al_create_display(screen_w, screen_h);
    if(!display)
    {
        cerr << "Blad podczas inicjalizacji ekranu." << endl;
        al_destroy_timer(timer);
        return -1;
    }

    event_queue = al_create_event_queue();
    if(!event_queue)
    {
        cerr << "Blad podczas inicjalizacji kolejki zdarzen." << endl;
        al_destroy_display(display);
        al_destroy_timer(timer);
        return -1;
    }

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_clear_to_color(al_map_rgb(0,0,0));

    al_flip_display();


    return 0;
}

int main(int argc, char ** argv)
{
    class Board board;

    if (init(board) != 0)
    {
        cerr << "Inicjalizacja nie powiod³a siê." << endl;
        return -1;
    }

    if (!board.initBitmaps()) return -1;

    bool isRedraw = true;
    bool isExitGame = false;

    al_start_timer(timer);
    while(!isExitGame)
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        // Resume game
        if(ev.keyboard.keycode == ALLEGRO_KEY_SPACE)
        {
            board.start =! board.start;
        }

        if(ev.type == ALLEGRO_EVENT_TIMER)
        {
            if(board.check_ant_on_edge()){
                board.resize();
            }
            else{
                board.refresh();
                isRedraw = true;
            }
        }

        // Exit game
        if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
        {
            isExitGame = true;
        }

        al_get_mouse_state(&state);
        if (state.buttons & 1)
        {
            board.addNewAnt(state.x, state.y);
        }

        // Redraw board
        if(isRedraw && al_is_event_queue_empty(event_queue))
        {
            isRedraw = false;
            board.draw();
            al_flip_display();
        }
    }
    return 0;
}
