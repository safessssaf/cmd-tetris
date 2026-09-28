#include <string>
#include <windows.h>
#include <chrono>
#include <random>
using namespace std;
using namespace std::chrono;

const int screen_width = 120;
const int screen_hight = 80;
wchar_t *screen = new wchar_t [screen_hight*screen_width];

string tetrominoes[6];
string tetris_pallete = "@.#a";
char play_area[10*20];
int rotation_c = 0;
int current_piece = 0;
float pos_x = 0;
float pos_y = 0;
auto drop_down_delay = 500ms; 
auto lock_timer = steady_clock::now();
bool time_take = true;
bool game_over = false;
int score = 0;
int level = 1;

void platform_draw()
{
  /* for(int c = -1; c <= 10; c++)
        for(int h = -1; h <= 20; h++)
            screen[(50 + c) + (h + 10) * screen_width] = (c == 10 || h == 20 || c == -1 || h == -1)? tetris_pallete[0] : play_area[c + (h * 10)]; */
    for(int c = 0; c < 10; c++)
    {
        for(int h = 0; h < 20; h++)
        {
            screen[(49) + (h + 10) * screen_width] = tetris_pallete[3];
            screen[60 + (h + 10) * screen_width] = tetris_pallete[3];
            screen[50 + c + (30) * screen_width] = tetris_pallete[3];
            screen[(50 + c) + (h + 10) * screen_width] = play_area[c + (h * 10)];
            screen[(50 + c) + 9 * screen_width] = ' ';
        }
    }
            

}
void tetrominoes_setup()
{
    tetrominoes[0].append(".x..");
    tetrominoes[0].append(".x..");
    tetrominoes[0].append(".x..");
    tetrominoes[0].append(".x..");

    tetrominoes[1].append(".x..");
    tetrominoes[1].append(".x..");
    tetrominoes[1].append(".xx.");
    tetrominoes[1].append("....");

    tetrominoes[2].append("..x.");
    tetrominoes[2].append("..x.");
    tetrominoes[2].append(".xx.");
    tetrominoes[2].append("....");

    tetrominoes[3].append("....");
    tetrominoes[3].append("xx..");
    tetrominoes[3].append(".xx.");
    tetrominoes[3].append("....");

    tetrominoes[4].append("....");
    tetrominoes[4].append("..xx");
    tetrominoes[4].append(".xx.");
    tetrominoes[4].append("....");
    
    tetrominoes[4].append("....");
    tetrominoes[4].append(".x..");
    tetrominoes[4].append("xxx.");
    tetrominoes[4].append("....");

    tetrominoes[5].append("....");
    tetrominoes[5].append(".xx.");
    tetrominoes[5].append(".xx.");
    tetrominoes[5].append("....");
} 
int level_calulations(int score)
{
  int level_current =(score / 2400) + 1;
  auto current_drop_time = 500ms;
  for(int i = 0 ; i < level_current; i++)
  {
    current_drop_time -= 40ms;
  }
  drop_down_delay = (current_drop_time < 0ms)? 0ms: current_drop_time;
  return level_current; 

}
int score_calculations(int cleared_lines, int level)
{
  int added_score;
  switch(cleared_lines)
  {
    case 0:
      added_score = 0;
      break;
    case 1:
      added_score = 100; 
      break; 
    case 2: 
      added_score = 300; 
      break;
    case 3:
      added_score = 500; 
      break; 
    case 4: 
      added_score = 800;
      break;
  }
  return added_score * level;
}
char rotate(int current_rotation, int current_piece, int x, int y)
{
    switch(current_rotation)
    {
        case 0:
            return tetrominoes[current_piece][x + (y * 4)];
            break;
        case 1:
            return tetrominoes[current_piece][(12 - (x * 4)) + y];
            break;
        case 2:
            return tetrominoes[current_piece][(15 - x) - (y * 4)];
            break;
        case 3:
            return tetrominoes[current_piece][(3 + (x * 4)) - y];
            break;
    }
    return '.';
}
void clear_line(int y)
{
  char saved_piece;

  for(int x = 0; x < 10 ;x++)
    play_area[x + y * 10] = tetris_pallete[1];
    
  for(int Dy = y; Dy > 0; --Dy)  
    for(int x = 0; x < 10; x++)
    {
      saved_piece = play_area[x + (Dy-1) * 10];
      play_area[x + (Dy - 1) * 10] = play_area[x + Dy * 10];
      play_area[x + Dy * 10] = saved_piece;
    } 
}

void line_clear_dectection(const int& sprite_y)
{
  int cleared_line = 0;
  for (int y = 0; y < 4; y++) 
  {
     
 
    if(y + sprite_y >= 20) continue;

    bool full_line =  true;
    for(int row = 0; row < 10; row++)
    {
      if(play_area[row + (sprite_y + y) * 10] == tetris_pallete[1]) full_line = false;
      if(sprite_y + y < 0)
      {
        
        game_over = true;
      }  
 
    }
    if(full_line == true)
    {
      clear_line(y + sprite_y);
      cleared_line ++; 
    }
  }
  score += score_calculations(cleared_line, level);
}
int random_int(int min, int max)
{
    static random_device rd;          // entropy source
    static mt19937 gen(rd());          // Mersenne Twister engine
    uniform_int_distribution<int> dist(min, max);

    return dist(gen);
}
void inilization()
{
    pos_x = 3; 
    pos_y = -1;
    rotation_c = 0;
    current_piece = random_int(0, 5);
}

bool lock_now()
{

  if (time_take)
  {
    lock_timer = steady_clock::now();
    time_take = false;
  }
  auto lock_current_time = steady_clock::now();
  if(lock_current_time - lock_timer >= 500ms)
  {
    time_take = true;
    return true;
  }
  return false;
  
  
}

void lock_piece(const int& current_postion_x, const int& current_postion_y, const int& current_rotation, const int& current_tetrominoes)
{
 if(!lock_now()) return;
  for (int x = 0; x < 4; x++) 
  {
    for (int y = 0; y < 4; y++) 
      {
        auto rotated_piece = rotate(current_rotation, current_tetrominoes, x, y);
        if (rotated_piece != '.')
        {   
          play_area[(x + current_postion_x) + ((y + current_postion_y) * 10)] = tetris_pallete[2];
                
        }
      }
  }
  line_clear_dectection(current_postion_y);
  inilization();
}

bool does_tetrominoes_fit(const int& current_postion_x, const int& current_postion_y, const int& current_rotation, const int& current_tetrominoes)
{  
    for (int x = 0; x < 4; x++) 
    {
        for (int y = 0; y < 4; y++) 
        {
            auto rotated_piece = rotate(current_rotation, current_tetrominoes, x, y);
            
            if (rotated_piece == 'x')
            {
              if((x + current_postion_x) > 9 || (x + current_postion_x) <0 || (y + current_postion_y) > 20 || play_area[(x + current_postion_x) + ((y + current_postion_y) * 10)] != '.')
                return false;
            }
        }
    }
    return true;
}

void game_over_fun()
{
  for(auto& i : play_area) i = tetris_pallete[0];
}
void input_handeling()
{
    
    if((GetKeyState(VK_LEFT) & 0x8000) &&  does_tetrominoes_fit((int)(pos_x - 1), (int)pos_y, rotation_c, current_piece))
    {
        pos_x -= 0.02;
        time_take = true;
    }
    if((GetKeyState(VK_RIGHT) & 0x8000) &&  does_tetrominoes_fit((int)(pos_x + 1), (int)pos_y, rotation_c, current_piece))
    {
        pos_x += 0.02;
        time_take = true;
    }
    if(does_tetrominoes_fit((int)(pos_x), (int)pos_y + 1, rotation_c, current_piece))
    {
        if((GetKeyState(VK_DOWN) & 0x8000))
        {
            pos_y += 0.02;
        }
    }
    else
    {
        lock_piece((int)(pos_x), (int)pos_y, rotation_c, current_piece);
    }
    if (rotation_c > 3) rotation_c = 0;
    if((GetAsyncKeyState((unsigned short)VK_UP) & 1) && does_tetrominoes_fit((int)pos_x, (int)pos_y ,rotation_c + 1, current_piece))
    {
        rotation_c += 1;
        time_take = true;
    }
}
void render_next_tetrominoes(int current_postion_x, int current_postion_y, int current_rotation, int current_tetrominoes)
{
    for(int x = 0; x < 4; x++)
        for(int y = 0; y < 4; y++)
            if (rotate(current_rotation, current_tetrominoes, x, y) == 'x')
            {
                int real_postion_x = x + (current_postion_x + 50);
                int real_postion_y = ((current_postion_y + 10) + y ) * screen_width;
                screen[real_postion_x + real_postion_y] = tetris_pallete[2];
                
            }
}

int main()
{
    HANDLE hBuffer = CreateConsoleScreenBuffer(
    GENERIC_READ | GENERIC_WRITE,
    0,
    NULL,
    CONSOLE_TEXTMODE_BUFFER,
    NULL
    );
    SetConsoleActiveScreenBuffer(hBuffer);
    DWORD bytesWritten = 0;
    tetrominoes_setup();
    inilization();
    for (int i = 0; i < 200; i++)
    play_area[i] = tetris_pallete[1];
    
    for(int i = 0; i < screen_hight * screen_width; i++)
      screen[i] = ' ';
    auto drop_timer_start = steady_clock::now(); 
    platform_draw();
    while(1)
    {

      string score_string = "score :" + to_string(score);
      string level_string = "level : " + to_string(level);
      string drop_down_delay_string = "update_time :" + to_string(duration_cast<milliseconds>(drop_down_delay).count()) + "ms";
      for(int i = 0; i < score_string.length(); i++) screen[i] = score_string[i];
      for(int n = 0; n < level_string.length(); n++) screen[n + 2 * screen_width] = level_string[n];
      for(int n = 0; n < drop_down_delay_string.length(); n++) screen[n + 10 * screen_width] = drop_down_delay_string[n];
      platform_draw();
      if(!game_over)
      {
        render_next_tetrominoes((int)pos_x, pos_y, rotation_c, current_piece);
        input_handeling();
        if(steady_clock::now() - drop_timer_start >= drop_down_delay &&  does_tetrominoes_fit((int)(pos_x), (int)pos_y + 1, rotation_c, current_piece))
        {
          drop_timer_start = steady_clock::now();
          pos_y += 1;
        } 
        level = level_calulations(score);
      }else
      {
        game_over_fun();
      }
      WriteConsoleOutputCharacterW(hBuffer, screen, screen_width * screen_hight, {0,0}, &bytesWritten);
    }
}
