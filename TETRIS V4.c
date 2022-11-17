 #include <stdio.h>
#include <windows.h> // system �Լ�, GetAsyncKeyState() �Լ�
#include <time.h> // time_h �ڷ��� �̿��Ͽ� �ð� ������ �� ��� Game_Start()�Լ����� ����.
#include <conio.h> // _getch(), _kbhit() �Լ� ����ϱ� ���� �������
#include <math.h>

#define BACKGROUND_WIDTH 10 // ���� �����ϴ� ����� �ʺ�
#define BACKGROUND_HEIGHT 20 // ���� �����ϴ� ����� ����

#define WALL_THICKNESS 2 // ���� �β��� ����, background_initial_x, y�� control_point_x, y�� �ʱ� ���� ������ �ش�.

#define BACKGROUND_WIDTH_TOTAL (BACKGROUND_WIDTH + (WALL_THICKNESS * 2)) // �¿� WALL_THICKNESS ĭ�� �� ���� ��� �ʺ�  
#define BACKGROUND_HEIGHT_TOTAL (BACKGROUND_HEIGHT + (WALL_THICKNESS * 2)) // ���� BWALL_THICKNESS ĭ�� �� ���� ��� ����

/*
   ���� ��
*/
#define RED 4 
#define ORANGE 12
#define YELLOW 14
#define GREEN 10
#define CYAN 11
#define BLUE 9
#define PURPLE 13
#define WHITE 15
#define GRAY 8
#define DARK_YELLOW 6
#define DARK_BLUE 1

/*
    ����Ű ��
*/
#define UP 72
#define DOWN 80
#define LEFT 75
#define RIGHT 77
#define SPACE 32
#define ENTER 13


short background[BACKGROUND_HEIGHT_TOTAL][BACKGROUND_WIDTH_TOTAL]; // ���� �¿� 2ĭ�� �� ���� ��� �迭
short background_initial_x = WALL_THICKNESS; // ��� �� �� ���� �����ϴ� ����� ���� X��
short background_initial_y = WALL_THICKNESS; // ��� �� �� ���� �����ϱ� ����� ���� Y��

int control_point_x; // �� �������� x ��ǥ, Game_Start()�Լ��� ó�� �κп��� �ʱ�ȭ �ȴ�.
int control_point_y; // �� �������� y ��ǥ, Game_Start()�Լ��� ó�� �κп��� �ʱ�ȭ �ȴ�.

short check_zone_point_x, check_zone_point_y; // üũ ���� ��ǥ�ϴ� ���� x��ǥ�� y��ǥ

void gotoxy(int x, int y) //gotoxy �Լ�
{
    COORD pos = { x,y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

/* check_zone[4][4]
(0,0), (1,0), (2,0), (3,0)
(0,1), (1,1), (2,1), (3,1)
(0,2), (1,2), (2,2), (3,2)
(0,3), (1,3), (2,3), (3,3)
*/
//O,I �� ��Ͽ� ���Ͽ� (2,1)��, S, Z, L, J, T, U�� ��Ͽ� ���Ͽ� (1,1)�� chechk_zone_point

short check_zone[4][4] = { {0, 0, 0, 0},
                       {0, 0, 0, 0},
                       {0, 0, 0, 0},
                       {0, 0, 0, 0} };
short check_zone_OI_size = 4; // O�� I�� üũ �� ũ��

short check_zone_SZLJTU_size = 3; // S�� Z�� L�� J�� T�� U�� üũ �� ũ��

/*
    O, I, S, Z, L, J, T, U �� ���� ó�� ����
*/

short type_O[4][4] = { {0,      0,      0, 0},
                       {0, YELLOW, YELLOW, 0},
                       {0, YELLOW, YELLOW, 0},
                       {0,      0,      0, 0} };

short type_I[4][4] = { {0,       0,    0,    0},
                       {CYAN, CYAN, CYAN, CYAN},
                       {0,       0,    0,    0},
                       {0,       0,    0,    0} };

short type_S[3][3] = { {0,     GREEN, GREEN},
                       {GREEN, GREEN,     0},
                       {0,         0,     0} };

short type_Z[3][3] = { {RED, RED,   0},
                       {0,   RED, RED},
                       {0,     0,   0} };

short type_L[3][3] = { {0,           0, ORANGE},
                       {ORANGE, ORANGE, ORANGE},
                       {0,           0,      0} };

short type_J[3][3] = { {BLUE,    0,    0},
                       {BLUE, BLUE, BLUE},
                       {0,       0,    0} };

short type_T[3][3] = { {0,      PURPLE,      0},
                       {PURPLE, PURPLE, PURPLE},
                       {0,           0,      0} };

short type_Item_Bomb[3][3] = { {0,	 0,  0},
                               {0, WHITE, 0},
                               {0,    0, 0} };

short type_Item_Rotation[3][3] = { {0,	 0,  0},
                               {0, DARK_YELLOW, 0},
                               {0,    0, 0} };

short type_U[3][3] = { {DARK_BLUE,   0,   DARK_BLUE},
                       {DARK_BLUE,   0,   DARK_BLUE},
                       {DARK_BLUE,   DARK_BLUE,   DARK_BLUE} };
int score = 0; // ����
int block_type; // Game_Start() �Լ� �ܿ��� Display(), Block_Renewal(), Block_Delete(), Ovelap_Test() �Լ������� ���� �����̱� ������ Game_Start() ������ ����

/*
   ���ڿ� ���� ������ �Լ�
*/
void FontColor(int color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

/*
    Ű�� ������ �� Ű�� ���� �Ǵ��ϰ� ��ȯ�ϴ� �Լ�
    �ƽ�Ű �ڵ� ���� ��ȯ�Ѵ�.
*/
int getKeyDown()
{
    if (_kbhit())
    {
        return _getch();
    }
    else return -1;
}

/*
   ȭ���� �����ϴ� �Լ�
*/
short rotation_factor = 0;

void Display()
{
    gotoxy(0, 0);

    if (rotation_factor == 0)
    {
        for (int y = 0; y < BACKGROUND_HEIGHT_TOTAL; y++)
        {
            for (int x = 0; x < BACKGROUND_WIDTH_TOTAL; x++)
            {
                if (background[y][x] == 0) // ��� �迭���� 0�� �� ������ �ǹ��Ѵ�.
                {
                    printf("��");
                }
                else if (background[y][x] == GRAY)
                {
                    FontColor(GRAY);
                    printf("��");
                    FontColor(WHITE);
                }
                else if (background[y][x] == RED)
                {
                    FontColor(RED);
                    printf("��");
                    FontColor(WHITE);
                }
                else if (background[y][x] == YELLOW)
                {
                    FontColor(YELLOW);
                    printf("��");
                    FontColor(WHITE);
                }
                else if (background[y][x] == ORANGE)
                {
                    FontColor(ORANGE);
                    printf("��");
                    FontColor(WHITE);
                }
                else if (background[y][x] == GREEN)
                {
                    FontColor(GREEN);
                    printf("��");
                    FontColor(WHITE);
                }
                else if (background[y][x] == BLUE)
                {
                    FontColor(BLUE);
                    printf("��");
                    FontColor(WHITE);
                }
                else if (background[y][x] == CYAN)
                {
                    FontColor(CYAN);
                    printf("��");
                    FontColor(WHITE);
                }
                else if (background[y][x] == PURPLE)
                {
                    FontColor(PURPLE);
                    printf("��");
                    FontColor(WHITE);
                }
                else if (background[y][x] == WHITE)
                {
                    FontColor(WHITE);
                    if (block_type == 7)
                    {
                        printf("��");
                    }
                    else if (block_type == 8)
                    {
                        printf("��");
                    }
                    FontColor(WHITE);
                }
                else if (background[y][x] == DARK_YELLOW)
                {
                    FontColor(DARK_YELLOW);
                    printf("��");
                    FontColor(WHITE);
                }
                else if (background[y][x] == DARK_BLUE)
                {
                    FontColor(DARK_BLUE);
                    printf("��");
                    FontColor(WHITE);
                }
            }
            printf("\n");
        }
    }
    else if (rotation_factor == 1)
    {
        for (int x = BACKGROUND_WIDTH_TOTAL - 1; x >= 0; x--)
        {
            for (int y = 0; y < BACKGROUND_HEIGHT_TOTAL; y++)
            {
                if (background[y][x] == 0) // ��� �迭���� 0�� �� ������ �ǹ��Ѵ�.
                {
                    printf("��");
                }
                else if (background[y][x] == GRAY)
                {
                    FontColor(GRAY);
                    printf("��");
                    FontColor(WHITE);
                }
                else if (background[y][x] == RED)
                {
                    FontColor(RED);
                    printf("��");
                    FontColor(WHITE);
                }
                else if (background[y][x] == YELLOW)
                {
                    FontColor(YELLOW);
                    printf("��");
                    FontColor(WHITE);
                }
                else if (background[y][x] == ORANGE)
                {
                    FontColor(ORANGE);
                    printf("��");
                    FontColor(WHITE);
                }
                else if (background[y][x] == GREEN)
                {
                    FontColor(GREEN);
                    printf("��");
                    FontColor(WHITE);
                }
                else if (background[y][x] == BLUE)
                {
                    FontColor(BLUE);
                    printf("��");
                    FontColor(WHITE);
                }
                else if (background[y][x] == CYAN)
                {
                    FontColor(CYAN);
                    printf("��");
                    FontColor(WHITE);
                }
                else if (background[y][x] == PURPLE)
                {
                    FontColor(PURPLE);
                    printf("��");
                    FontColor(WHITE);
                }
                else if (background[y][x] == WHITE)
                {
                    FontColor(WHITE);
                    if (block_type == 7)
                    {
                        printf("��");
                    }
                    else if (block_type == 8)
                    {
                        printf("��");
                    }
                    FontColor(WHITE);
                }
                else if (background[y][x] == DARK_YELLOW)
                {
                    FontColor(DARK_YELLOW);
                    printf("��");
                    FontColor(WHITE);
                }
                else if (background[y][x] == DARK_BLUE)
                {
                    FontColor(DARK_BLUE);
                    printf("��");
                    FontColor(WHITE);
                }
            }
            printf("\n");
        }
    }
    else if (rotation_factor == 2)
    {
        for (int y = BACKGROUND_HEIGHT_TOTAL - 1; y >= 0; y--)
        {
            for (int x = BACKGROUND_WIDTH_TOTAL - 1; x >= 0; x--)
            {
                if (background[y][x] == 0) // ��� �迭���� 0�� �� ������ �ǹ��Ѵ�.
                {
                    printf("��");
                }
                else if (background[y][x] == GRAY)
                {
                    FontColor(GRAY);
                    printf("��");
                    FontColor(WHITE);
                }
                else if (background[y][x] == RED)
                {
                    FontColor(RED);
                    printf("��");
                    FontColor(WHITE);
                }
                else if (background[y][x] == YELLOW)
                {
                    FontColor(YELLOW);
                    printf("��");
                    FontColor(WHITE);
                }
                else if (background[y][x] == ORANGE)
                {
                    FontColor(ORANGE);
                    printf("��");
                    FontColor(WHITE);
                }
                else if (background[y][x] == GREEN)
                {
                    FontColor(GREEN);
                    printf("��");
                    FontColor(WHITE);
                }
                else if (background[y][x] == BLUE)
                {
                    FontColor(BLUE);
                    printf("��");
                    FontColor(WHITE);
                }
                else if (background[y][x] == CYAN)
                {
                    FontColor(CYAN);
                    printf("��");
                    FontColor(WHITE);
                }
                else if (background[y][x] == PURPLE)
                {
                    FontColor(PURPLE);
                    printf("��");
                    FontColor(WHITE);
                }
                else if (background[y][x] == WHITE)
                {
                    FontColor(WHITE);
                    if (block_type == 7)
                    {
                        printf("��");
                    }
                    else if (block_type == 8)
                    {
                        printf("��");
                    }
                    FontColor(WHITE);
                }
                else if (background[y][x] == DARK_YELLOW)
                {
                    FontColor(DARK_YELLOW);
                    printf("��");
                    FontColor(WHITE);
                }
                else if (background[y][x] == DARK_BLUE)
                {
                    FontColor(DARK_BLUE);
                    printf("��");
                    FontColor(WHITE);
                }
            }
            printf("\n");
        }
    }
    else if (rotation_factor == 3)
    {
        for (int x = 0; x < BACKGROUND_WIDTH_TOTAL; x++)
        {
            for (int y = BACKGROUND_HEIGHT_TOTAL - 1; y >= 0; y--)
            {
                if (background[y][x] == 0) // ��� �迭���� 0�� �� ������ �ǹ��Ѵ�.
                {
                    printf("��");
                }
                else if (background[y][x] == GRAY)
                {
                    FontColor(GRAY);
                    printf("��");
                    FontColor(WHITE);
                }
                else if (background[y][x] == RED)
                {
                    FontColor(RED);
                    printf("��");
                    FontColor(WHITE);
                }
                else if (background[y][x] == YELLOW)
                {
                    FontColor(YELLOW);
                    printf("��");
                    FontColor(WHITE);
                }
                else if (background[y][x] == ORANGE)
                {
                    FontColor(ORANGE);
                    printf("��");
                    FontColor(WHITE);
                }
                else if (background[y][x] == GREEN)
                {
                    FontColor(GREEN);
                    printf("��");
                    FontColor(WHITE);
                }
                else if (background[y][x] == BLUE)
                {
                    FontColor(BLUE);
                    printf("��");
                    FontColor(WHITE);
                }
                else if (background[y][x] == CYAN)
                {
                    FontColor(CYAN);
                    printf("��");
                    FontColor(WHITE);
                }
                else if (background[y][x] == PURPLE)
                {
                    FontColor(PURPLE);
                    printf("��");
                    FontColor(WHITE);
                }
                else if (background[y][x] == WHITE)
                {
                    FontColor(WHITE);
                    if (block_type == 7)
                    {
                        printf("��");
                    }
                    else if (block_type == 8)
                    {
                        printf("��");
                    }
                    FontColor(WHITE);
                }
                else if (background[y][x] == DARK_YELLOW)
                {
                    FontColor(DARK_YELLOW);
                    printf("��");
                    FontColor(WHITE);
                }
                else if (background[y][x] == DARK_BLUE)
                {
                    FontColor(DARK_BLUE);
                    printf("��");
                    FontColor(WHITE);
                }
            }
            printf("\n");
        }
    }
    gotoxy(54, 20);
    printf("���� : %d\n", score);
}

void removeCursor(void) //Ŀ�������� ����

{

    CONSOLE_CURSOR_INFO curInfo;

    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);

    curInfo.bVisible = 0;

    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);

}

void Loading();
void Game_Start();
void Block_Renewal();
void Block_Delete();
void Overlap_Test(short type);
int Drop_Test();
void Delete_Lines();
void Game_Over_Test();
void Game_Over();
void Item_Bomb();
void Next_Display(int next_block_type);

/*
   ���� �Լ�
*/
int main()
{
    system("title ��Ʈ����_By 8��");
	removeCursor(); //Ŀ������������
    Loading();

    return 0;
}

//���� �ε� �Լ�
void Loading() {
	HANDLE hand = GetStdHandle(STD_OUTPUT_HANDLE);
	int keyInput;
	system("mode concols=110 lines=30");

	FontColor(GRAY);  printf(" �������������������������������������������������������\n");
	FontColor(GRAY);  printf(" �������������������������������������������������������\n");
	FontColor(RED); printf("  ���������  "); FontColor(ORANGE); printf("���������"); FontColor(YELLOW); printf(" ���������  "); FontColor(GREEN); printf("�������      "); FontColor(CYAN); printf("���������  "); FontColor(PURPLE); printf("���������\n");
	FontColor(RED); printf("  ���������  "); FontColor(ORANGE); printf("���������"); FontColor(YELLOW); printf(" ���������  "); FontColor(GREEN); printf("��������    "); FontColor(CYAN); printf("���������  "); FontColor(PURPLE); printf("���������\n");
	FontColor(RED); printf("        ���        "); FontColor(ORANGE); printf("���            "); FontColor(YELLOW); printf("       ���        "); FontColor(GREEN); printf("���      ����  "); FontColor(CYAN); printf("      ���        "); FontColor(PURPLE); printf("���            \n");
	FontColor(RED); printf("        ���        "); FontColor(ORANGE); printf("���            "); FontColor(YELLOW); printf("       ���        "); FontColor(GREEN); printf("���      ����  "); FontColor(CYAN); printf("      ���        "); FontColor(PURPLE); printf("���            \n");
	FontColor(RED); printf("        ���        "); FontColor(ORANGE); printf("���������"); FontColor(YELLOW); printf("       ���        "); FontColor(GREEN); printf("��������    "); FontColor(CYAN); printf("      ���        "); FontColor(PURPLE); printf("���������\n");
	FontColor(RED); printf("        ���        "); FontColor(ORANGE); printf("���������"); FontColor(YELLOW); printf("       ���        "); FontColor(GREEN); printf("��������    "); FontColor(CYAN); printf("      ���        "); FontColor(PURPLE); printf("���������\n");
	FontColor(RED); printf("        ���        "); FontColor(ORANGE); printf("���            "); FontColor(YELLOW); printf("       ���        "); FontColor(GREEN); printf("���      ���    "); FontColor(CYAN); printf("      ���        "); FontColor(PURPLE); printf("            ���\n");
	FontColor(RED); printf("        ���        "); FontColor(ORANGE); printf("���            "); FontColor(YELLOW); printf("       ���        "); FontColor(GREEN); printf("���      ���    "); FontColor(CYAN); printf("      ���        "); FontColor(PURPLE); printf("            ���\n");
	FontColor(RED); printf("        ���        "); FontColor(ORANGE); printf("���������"); FontColor(YELLOW); printf("       ���        "); FontColor(GREEN); printf("���      ���    "); FontColor(CYAN); printf("���������  "); FontColor(PURPLE); printf("���������\n");
	FontColor(RED); printf("        ���        "); FontColor(ORANGE); printf("���������"); FontColor(YELLOW); printf("       ���        "); FontColor(GREEN); printf("���      ���    "); FontColor(CYAN); printf("���������  "); FontColor(PURPLE); printf("���������\n");
	FontColor(GRAY);  printf(" �������������������������������������������������������\n");
	FontColor(GRAY);  printf(" �������������������������������������������������������\n");

	FontColor(15);
	gotoxy(4, 20);
	printf("���ȸ�� :    ��   \n");
	gotoxy(4, 21);
	printf("����̵� : �� �� ��\n");
	gotoxy(4, 22);
	printf("�����ϰ�:  space   \n");
	gotoxy(83, 20);
	printf("�� : �ݽð� ȭ��ȸ��\n");
	gotoxy(83, 21);
	printf("�� : �������� ��ź\n");
	gotoxy(83, 22);
	printf("�� : ���ڹ��� ��ź\n");

	FontColor(14);
	gotoxy(8, 19);
	printf("<���۹�>\n");
	gotoxy(88, 19);
	printf("<������>\n");

	FontColor(15);
	gotoxy(80, 25);
	printf(" ������ǻ�����α׷��� ");
	gotoxy(80, 26);
	printf(" �� MADE BY TEAM 8 ��");

	FontColor(79);
	gotoxy(32, 25);
	printf("��ȭ�� ȸ���� ��� : ����Ű�� �ٲ�ϴ١�\n");


	while (1) {
		keyInput = getKeyDown();
		if (keyInput == ENTER)
		{
			system("cls");
			Game_Start();
		}

		gotoxy(35, 20);
		FontColor(10);
		printf("�����Ϸ��� < ENTER > Ű�� ��������");
		
	}
}

void Game_Start()
{
    score = 0;
    rotation_factor = 0;

    srand(time(NULL));

    int next_block_type = rand() % 11;

    /*
       ó�� ���� �� ��� �迭�� ���� ����� ������ �ֱ�
    */
    for (int y = 0; y < BACKGROUND_HEIGHT_TOTAL; y++)
    {
        for (int x = 0; x < BACKGROUND_WIDTH_TOTAL; x++)
        {
            if ((x >= background_initial_x && x <= background_initial_x + BACKGROUND_WIDTH - 1)
                && (y >= background_initial_y && y <= background_initial_y + BACKGROUND_HEIGHT - 1))
            {
                background[y][x] = 0;
            }
            else
            {
                background[y][x] = GRAY;
            }
        }
    }

    Display();

    short is_first = 1; // ���� �����ǰ� ó�� �������� 1, ó�� �������� ���� �ƴϸ� 0

    while (1)
    {
        clock_t start_time = clock();

        if (is_first == 1)
        {
            /*
               ������� ���� ó�� ������ ��, control_point_x, y�� ���������� �ʱ�ȭ �Ѵ�.
               ���� ������ �� �ִ� ����� ���� ���� �� ��ǥ�� (0,0)���� ������ �������� ��ǥ�� �׻� (5,1)�� �ǵ��� �Ѵ�.
            */
            control_point_x = WALL_THICKNESS + 5;
            control_point_y = WALL_THICKNESS + 1;

            /*
               ���� ó�� �����Ǿ��� �� �� ���� �������� �����Ͽ� check_zone �� �����ϱ�
            */

            // next_block_type�� block_type�� �����ϰ�, next_block_type�� ������ ���.
            block_type = next_block_type;

            next_block_type = rand() % 11;
            //block_type = 11;

            if (block_type == 0) // O�� ����� ���
            {
                check_zone_point_x = 2, check_zone_point_y = 1;

                for (int y = 0; y < check_zone_OI_size; y++)
                {
                    for (int x = 0; x < check_zone_OI_size; x++)
                    {
                        check_zone[y][x] = type_O[y][x];
                    }
                }
            }
            else if (block_type == 1) // I�� ����� ���
            {
                check_zone_point_x = 2, check_zone_point_y = 1;

                for (int y = 0; y < check_zone_OI_size; y++)
                {
                    for (int x = 0; x < check_zone_OI_size; x++)
                    {
                        check_zone[y][x] = type_I[y][x];
                    }
                }
            }
            else if (block_type == 2) // S�� ����� ���
            {
                check_zone_point_x = 1, check_zone_point_y = 1;

                for (int y = 0; y < check_zone_SZLJTU_size; y++)
                {
                    for (int x = 0; x < check_zone_SZLJTU_size; x++)
                    {
                        check_zone[y][x] = type_S[y][x];
                    }
                }
            }
            else if (block_type == 3) // Z�� ����� ���
            {
                check_zone_point_x = 1, check_zone_point_y = 1;

                for (int y = 0; y < check_zone_SZLJTU_size; y++)
                {
                    for (int x = 0; x < check_zone_SZLJTU_size; x++)
                    {
                        check_zone[y][x] = type_Z[y][x];
                    }
                }
            }
            else if (block_type == 4) // L�� ����� ���
            {
                check_zone_point_x = 1, check_zone_point_y = 1;

                for (int y = 0; y < check_zone_SZLJTU_size; y++)
                {
                    for (int x = 0; x < check_zone_SZLJTU_size; x++)
                    {
                        check_zone[y][x] = type_L[y][x];
                    }
                }
            }
            else if (block_type == 5) // J�� ����� ���
            {
                check_zone_point_x = 1, check_zone_point_y = 1;

                for (int y = 0; y < check_zone_SZLJTU_size; y++)
                {
                    for (int x = 0; x < check_zone_SZLJTU_size; x++)
                    {
                        check_zone[y][x] = type_J[y][x];
                    }
                }
            }
            else if (block_type == 6) // T�� ����� ���
            {
                check_zone_point_x = 1, check_zone_point_y = 1;

                for (int y = 0; y < check_zone_SZLJTU_size; y++)
                {
                    for (int x = 0; x < check_zone_SZLJTU_size; x++)
                    {
                        check_zone[y][x] = type_T[y][x];
                    }
                }
            }
            else if (block_type == 7) // ��ź �������� ��� (type 1)
            {
                check_zone_point_x = 1, check_zone_point_y = 1;

                for (int y = 0; y < check_zone_SZLJTU_size; y++)
                {
                    for (int x = 0; x < check_zone_SZLJTU_size; x++)
                    {
                        check_zone[y][x] = type_Item_Bomb[y][x];
                    }
                }
            }
            else if (block_type == 8) // ��ź �������� ���(type_2)
            {
                check_zone_point_x = 1, check_zone_point_y = 1;

                for (int y = 0; y < check_zone_SZLJTU_size; y++)
                {
                    for (int x = 0; x < check_zone_SZLJTU_size; x++)
                    {
                        check_zone[y][x] = type_Item_Bomb[y][x];
                    }
                }
            }
            else if (block_type == 9) // ȭ�� ȸ�� �������� ���
            {
                check_zone_point_x = 1, check_zone_point_y = 1;

                for (int y = 0; y < check_zone_SZLJTU_size; y++)
                {
                    for (int x = 0; x < check_zone_SZLJTU_size; x++)
                    {
                        check_zone[y][x] = type_Item_Rotation[y][x];
                    }
                }
            }
            else if (block_type == 10) // U�� ����� ���
            {
                check_zone_point_x = 1, check_zone_point_y = 1;

                for (int y = 0; y < check_zone_SZLJTU_size; y++)
                {
                    for (int x = 0; x < check_zone_SZLJTU_size; x++)
                    {
                        check_zone[y][x] = type_U[y][x];
                    }
                }
            }
            Game_Over_Test();
            Next_Display(next_block_type);
            Display();
        }

        while (1)
        {

            clock_t finish_time = clock();
            clock_t interval = finish_time - start_time;

            /*
               ��, �Ʒ�, ����, ������ ����Ű, �����̽�Ű�� ������ �� ����.
            */

            int key = getKeyDown();

            if (key == UP)
            {
                if (rotation_factor == 0)
                {
                    Overlap_Test(0);
                    Display();
                }
                else if (rotation_factor == 1)
                {
                    Overlap_Test(3);
                    Display();
                }
                else if (rotation_factor == 2)
                {
                    Overlap_Test(1);
                    Display();
                }
                else if (rotation_factor == 3)
                {
                    Overlap_Test(2);
                    Display();
                }
            }
            if (key == DOWN)
            {
                if (rotation_factor == 0)
                {
                    Overlap_Test(1);
                    Display();
                }
                else if (rotation_factor == 1)
                {
                    Overlap_Test(2);
                    Display();
                }
                else if (rotation_factor == 2)
                {
                    Overlap_Test(0);
                    Display();
                }
                else if (rotation_factor == 3)
                {
                    Overlap_Test(3);
                    Display();
                }
            }
            if (key == LEFT)
            {
                if (rotation_factor == 0)
                {
                    Overlap_Test(2);
                    Display();
                }
                else if (rotation_factor == 1)
                {
                    Overlap_Test(0);
                    Display();
                }
                else if (rotation_factor == 2)
                {
                    Overlap_Test(3);
                    Display();
                }
                else if (rotation_factor == 3)
                {
                    Overlap_Test(1);
                    Display();
                }
            }
            if (key == RIGHT)
            {
                if (rotation_factor == 0)
                {
                    Overlap_Test(3);
                    Display();
                }
                else if (rotation_factor == 1)
                {
                    Overlap_Test(1);
                    Display();
                }
                else if (rotation_factor == 2)
                {
                    Overlap_Test(2);
                    Display();
                }
                else if (rotation_factor == 3)
                {
                    Overlap_Test(0);
                    Display();
                }
            }
            if (key == SPACE)
            {
                while (1)
                {
                    if (Drop_Test() == 1)
                        break;
                }

                if (block_type == 7)
                {
                    Item_Bomb(1);

                }
                else if (block_type == 8)
                {
                    Item_Bomb(2);

                }
                else if (block_type == 9)
                {
                    ++rotation_factor;
                    rotation_factor = rotation_factor % 4;
                    system("cls");
                }

                Delete_Lines();
                Next_Display(next_block_type);
                Display();
                is_first = 1;
                break;
            }

            if (interval >= pow(1.1, -(score / 500)) * 1000)
            {
                if (Drop_Test() == 1)
                {
                    if (block_type == 7)
                    {
                        Item_Bomb(1);
                    }
                    else if (block_type == 8)
                    {
                        Item_Bomb(2);
                    }
                    else if (block_type == 9)
                    {
                        ++rotation_factor;
                        rotation_factor = rotation_factor % 4;
                        system("cls");
                    }

                    Delete_Lines();
                    Next_Display(next_block_type);
                    Display();
                    is_first = 1;
                    break;
                }
                else
                {
                    Display();
                    is_first = 0;
                    break;
                }
            }
        }
    }
}

/*
   ���� �����ϰ� �ִ� ����� ��濡 ���� �Լ� (check_zone�� �ִ� ���� ��濡 �������� ��´ٰ� ����)
*/

void Block_Renewal()
{
    // (block_type <= 1 ? check_zone_OI_size : check_zone_SZLJTU_size) �� �� ������ ���� üũ �� ũ�⸦ �����ϴ� ���� ������
    for (int y = 0; y < (block_type <= 1 ? check_zone_OI_size : check_zone_SZLJTU_size); y++)
    {
        for (int x = 0; x < (block_type <= 1 ? check_zone_OI_size : check_zone_SZLJTU_size); x++)
        {
            int real_point_x = x - check_zone_point_x + control_point_x;
            int real_point_y = y - check_zone_point_y + control_point_y;

            if (check_zone[y][x] != 0)
            {
                background[real_point_y][real_point_x] = check_zone[y][x];
            }
        }
    }
}

/*
   ���� �����ϰ� �ִ� ����� ��濡�� ���� �Լ� (check_zone���� �� ���� ���شٰ� ����)
*/
void Block_Delete()
{
    // (block_type <= 1 ? check_zone_OI_size : check_zone_SZLJTU_size) �� �� ������ ���� üũ �� ũ�⸦ �����ϴ� ���� ������
    for (int y = 0; y < (block_type <= 1 ? check_zone_OI_size : check_zone_SZLJTU_size); y++)
    {
        for (int x = 0; x < (block_type <= 1 ? check_zone_OI_size : check_zone_SZLJTU_size); x++)
        {
            int real_point_x = x - check_zone_point_x + control_point_x;
            int real_point_y = y - check_zone_point_y + control_point_y;

            if (check_zone[y][x] != 0)
            {
                background[real_point_y][real_point_x] = 0;
            }
        }
    }
}

void Overlap_Test(short type)
{
    short is_overlap = 0;

    Block_Delete();

    if (type == 0)
    {
        int check_zone_next[4][4];

        /*
           ���� check_zone�� ��ϵ��� ���� �ӽ� check_zone�� ȸ������ �����ϱ�
        */
        for (int y = 0; y < (block_type <= 1 ? check_zone_OI_size : check_zone_SZLJTU_size); y++)
        {
            for (int x = 0; x < (block_type <= 1 ? check_zone_OI_size : check_zone_SZLJTU_size); x++)
            {
                check_zone_next[x][(block_type <= 1 ? check_zone_OI_size : check_zone_SZLJTU_size) - y - 1] = check_zone[y][x];
            }
        }

        for (int y = 0; y < (block_type <= 1 ? check_zone_OI_size : check_zone_SZLJTU_size); y++)
        {
            for (int x = 0; x < (block_type <= 1 ? check_zone_OI_size : check_zone_SZLJTU_size); x++)
            {
                int real_point_x = x - check_zone_point_x + control_point_x;
                int real_point_y = y - check_zone_point_y + control_point_y;

                if (background[real_point_y][real_point_x] != 0 && check_zone_next[y][x] != 0)
                {
                    is_overlap = 1;
                    break;
                }
            }
            if (is_overlap == 1)
            {
                break;
            }
        }

        if (is_overlap == 1)
        {
            /*
               ���� ����ϰ� ȸ�� ���� ����� ��ģ�ٸ� �ƹ��ϵ� �Ͼ�� �ʰ� �Ͽ� ȸ������ �ʰ� �Ѵ�.
            */
        }
        else
        {
            /*
               ���� ����ϰ� ȸ�� ���� ����� ��ġ�� �ʴ´ٸ�, ���� check_zone �迭 �� ���� ������ check_zone �迭�� �����Ͽ�
               ��濡 ȸ���� ����� ���� �� �ְ� �Ѵ�..
            */

            for (int y = 0; y < (block_type <= 1 ? check_zone_OI_size : check_zone_SZLJTU_size); y++)
            {
                for (int x = 0; x < (block_type <= 1 ? check_zone_OI_size : check_zone_SZLJTU_size); x++)
                {
                    check_zone[y][x] = check_zone_next[y][x];
                }
            }
        }
    }
    else if (type == 1)
    {
        control_point_y += 1;

        for (int y = 0; y < (block_type <= 1 ? check_zone_OI_size : check_zone_SZLJTU_size); y++)
        {
            for (int x = 0; x < (block_type <= 1 ? check_zone_OI_size : check_zone_SZLJTU_size); x++)
            {
                int real_point_x = x - check_zone_point_x + control_point_x;
                int real_point_y = y - check_zone_point_y + control_point_y;

                if (background[real_point_y][real_point_x] != 0 && check_zone[y][x] != 0)
                {
                    is_overlap = 1;
                    break;
                }
            }
            if (is_overlap == 1)
            {
                break;
            }
        }

        if (is_overlap == 1)
        {
            control_point_y -= 1;
        }
    }
    else if (type == 2)
    {
        control_point_x -= 1;

        for (int y = 0; y < (block_type <= 1 ? check_zone_OI_size : check_zone_SZLJTU_size); y++)
        {
            for (int x = 0; x < (block_type <= 1 ? check_zone_OI_size : check_zone_SZLJTU_size); x++)
            {
                int real_point_x = x - check_zone_point_x + control_point_x;
                int real_point_y = y - check_zone_point_y + control_point_y;

                if (background[real_point_y][real_point_x] != 0 && check_zone[y][x] != 0)
                {
                    is_overlap = 1;
                    break;
                }
            }
            if (is_overlap == 1)
            {
                break;
            }
        }

        if (is_overlap == 1)
        {
            control_point_x += 1;
        }
    }
    else if (type == 3)
    {
        control_point_x += 1;

        for (int y = 0; y < (block_type <= 1 ? check_zone_OI_size : check_zone_SZLJTU_size); y++)
        {
            for (int x = 0; x < (block_type <= 1 ? check_zone_OI_size : check_zone_SZLJTU_size); x++)
            {
                int real_point_x = x - check_zone_point_x + control_point_x;
                int real_point_y = y - check_zone_point_y + control_point_y;

                if (background[real_point_y][real_point_x] != 0 && check_zone[y][x] != 0)
                {
                    is_overlap = 1;
                    break;
                }
            }
            if (is_overlap == 1)
            {
                break;
            }
        }

        if (is_overlap == 1)
        {
            control_point_x -= 1;
        }
    }

    Block_Renewal();
}

int Drop_Test()
{
    short is_overlap = 0;

    Block_Delete();

    control_point_y += 1;

    for (int y = 0; y < (block_type <= 1 ? check_zone_OI_size : check_zone_SZLJTU_size); y++)
    {
        for (int x = 0; x < (block_type <= 1 ? check_zone_OI_size : check_zone_SZLJTU_size); x++)
        {
            int real_point_x = x - check_zone_point_x + control_point_x;
            int real_point_y = y - check_zone_point_y + control_point_y;

            if (background[real_point_y][real_point_x] != 0 && check_zone[y][x] != 0)
            {
                is_overlap = 1;
                break;
            }
        }
        if (is_overlap == 1)
        {
            break;
        }
    }

    if (is_overlap == 1)
    {
        control_point_y -= 1;
        Block_Renewal();
        return 1;
    }
    else
    {
        Block_Renewal();
        return 0;
    }
}

/*
   ä���� �� ���ִ� �Լ�
*/

void Delete_Lines()
{

    int next_background[BACKGROUND_HEIGHT][BACKGROUND_WIDTH]; // ���� ���

    /*
       next_background�� 0���� ä���ֱ�
    */
    for (int y = 0; y < BACKGROUND_HEIGHT; y++) // ��
    {
        for (int x = 0; x < BACKGROUND_WIDTH; x++) // ��
        {
            next_background[y][x] = 0;
        }
    }

    /*
       ä���� �� ���ִ� �ݺ���
    */

    for (int y = 0; y < BACKGROUND_HEIGHT; y++) // ��
    {
        for (int x = 0; x < BACKGROUND_WIDTH; x++) // ��
        {
            if (background[background_initial_y + y][background_initial_x + x] == 0)
            {
                break;
            }

            if (x == BACKGROUND_WIDTH - 1)
            {
                for (int i = 0; i < BACKGROUND_WIDTH; i++)
                {
                    background[background_initial_y + y][background_initial_x + i] = 0;
                }
                score += 100;

                Beep(300, 100);
                /*
                   ä���� �� �������� �� ���̰� �ϴ� �ð���ȿ��.
                */
                Display();
                Sleep(100);
            }
        }
    }

    /*
       �����ִ� ���� ������ �״� �ݺ���
    */

    int stack_count = 0;

    for (int y = BACKGROUND_HEIGHT - 1; y >= 0; y--) // ��
    {
        int empty_count = 0;

        for (int x = 0; x < BACKGROUND_WIDTH; x++) // ��
        {
            if (background[background_initial_y + y][background_initial_x + x] == 0)
            {
                empty_count += 1;
                /*
                   ����ִ� ���� stack_count�� ������ �ʰ��Ѵ�.
                */
                if (empty_count == BACKGROUND_WIDTH)
                {
                    break;
                }
            }
            else
            {
                /*
                   ��� ���� ���� ���� stack_count �� 1 �����ϰ� �Ͽ� ��� ������ ����Ѵ�.
                */

                stack_count += 1;

                for (int i = 0; i < BACKGROUND_WIDTH; i++)
                {
                    next_background[BACKGROUND_HEIGHT - stack_count][i] = background[background_initial_y + y][background_initial_x + i];
                }
                break;
            }
        }
    }

    for (int y = 0; y < BACKGROUND_HEIGHT; y++) // ��
    {
        for (int x = 0; x < BACKGROUND_WIDTH; x++) // ��
        {
            background[background_initial_y + y][background_initial_x + x] = next_background[y][x];
        }
    }
}

void Game_Over_Test()
{
    short is_overlap = 0;

    for (int y = 0; y < (block_type <= 1 ? check_zone_OI_size : check_zone_SZLJTU_size); y++)
    {
        for (int x = 0; x < (block_type <= 1 ? check_zone_OI_size : check_zone_SZLJTU_size); x++)
        {
            int real_point_x = x - check_zone_point_x + control_point_x;
            int real_point_y = y - check_zone_point_y + control_point_y;

            if (background[real_point_y][real_point_x] != 0 && check_zone[y][x] != 0)
            {
                if (background[real_point_y][real_point_x] == GRAY)
                {
                    Block_Renewal();
                    Display();
                    Game_Over();
                }

                is_overlap = 1;
                break;
            }
        }
        if (is_overlap == 1)
        {
            break;
        }
    }

    if (is_overlap == 1)
    {
        control_point_y -= 1;
        Game_Over_Test();
    }
    else
    {
        Block_Renewal();
    }
}

void Game_Over()
{
	gotoxy(18, 2);
	FontColor(WHITE);
	printf("�ƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢ�\n");
	for (int i = 3; i <= 10; i++)
	{
		gotoxy(18, i);
		FontColor(0);
		printf("                                    ");
	}
	gotoxy(29, 4);
	FontColor(RED);  printf("!! GAME OVER !!");
    gotoxy(32, 6);
	FontColor(WHITE);
    printf("���� : %d", (score / 500) + 1);
    gotoxy(32, 7);
    printf("���� : %d\n", score);
	gotoxy(25, 9);
	FontColor(YELLOW); printf("���θ޴��� �̵� : <ENTER>");
	gotoxy(18, 11);
	FontColor(WHITE);
	printf("�ƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢƢ�\n");
    

    while (1)
    {
        if (getKeyDown() == ENTER)
        {
            system("cls");
            Loading();
        }
    }
}

void Item_Bomb(short type)
{
    if (type == 1)
    {
        for (int y = 0; y < check_zone_SZLJTU_size; y++)
        {
            for (int x = 0; x < check_zone_SZLJTU_size; x++)
            {
                int real_point_x = x - check_zone_point_x + control_point_x;
                int real_point_y = y - check_zone_point_y + control_point_y;

                if (background[real_point_y][real_point_x] != GRAY && background[real_point_y][real_point_x] != 0)
                {
                    background[real_point_y][real_point_x] = 0;
                    Display();

                    score += 20;
                }

            }
        }
    }
    else if (type == 2)
    {
        /*
           ��ź ��ġ�� ���Ͽ� ���ڸ������ ��� ��� ���ֱ�
        */
        // �����ϰ� �� ���ֱ�
        for (int i = 0; i < BACKGROUND_HEIGHT; i++)
        {
            if (background[background_initial_y + i][control_point_x] != 0)
            {
                background[background_initial_y + i][control_point_x] = 0;

                score += 20;
            }
        }
        //�����ϰ� �� ���ֱ�

        for (int j = 0; j < BACKGROUND_WIDTH; j++)
        {
            if (background[control_point_y][background_initial_x + j] != 0)
            {
                background[control_point_y][background_initial_x + j] = 0;

                score += 20;
            }
        }
        Display();
    }
    Beep(100, 100);
}

void Next_Display(int next_block_type)
{
    //���� ȭ�� ���� ���� ��� ǥ��

    gotoxy(52, 5);
    printf("���� <Next>����\n");
    gotoxy(52, 6);
    printf("��         ��\n");
    gotoxy(52, 7);
    printf("��         ��\n");
    gotoxy(52, 8);
    printf("��         ��\n");
    gotoxy(52, 9);
    printf("��         ��\n");
    gotoxy(52, 10);
    printf("����������������������\n");


    if (next_block_type == 0)
    {
        FontColor(YELLOW);
        gotoxy(56, 7);
        printf("�â�");
        gotoxy(56, 8);
        printf("�â�");
        FontColor(WHITE);

    }
    else if (next_block_type == 1)
    {
        FontColor(CYAN);
        gotoxy(54, 8);
        printf("�âââ�");
        FontColor(WHITE);
    }
    else if (next_block_type == 2)
    {
        FontColor(GREEN);
        gotoxy(57, 7);
        printf("�â�");
        gotoxy(55, 8);
        printf("�â�");
        FontColor(WHITE);
    }
    else if (next_block_type == 3)
    {
        FontColor(RED);
        gotoxy(55, 7);
        printf("�â�");
        gotoxy(57, 8);
        printf("�â�");
        FontColor(WHITE);
    }
    else if (next_block_type == 4)
    {
        FontColor(ORANGE);
        gotoxy(59, 7);
        printf("��");
        gotoxy(55, 8);
        printf("�ââ�");
        FontColor(WHITE);
    }
    else if (next_block_type == 5)
    {
        FontColor(BLUE);
        gotoxy(55, 7);
        printf("��");
        gotoxy(55, 8);
        printf("�ââ�");
        FontColor(WHITE);
    }
    else if (next_block_type == 6)
    {
        FontColor(PURPLE);
        gotoxy(57, 7);
        printf("��");
        gotoxy(55, 8);
        printf("�ââ�");
        FontColor(WHITE);
    }
    else if (next_block_type == 7)
    {

        gotoxy(57, 8);
        printf("��");

    }
    else if (next_block_type == 8)
    {
        gotoxy(57, 8);
        printf("��");
    }
    else if (next_block_type == 9)
    {
        FontColor(DARK_YELLOW);
        gotoxy(57, 8);
        printf("��");
        FontColor(WHITE);
    }
    else if (next_block_type == 10)
    {
        FontColor(DARK_BLUE);
        gotoxy(55, 7);
        printf("��  ��");
        gotoxy(55, 8);
        printf("��  ��");
        gotoxy(55, 9);
        printf("�ââ�");
        FontColor(WHITE);
    }
    gotoxy(54, 19);
    printf("���� : %d", (score / 500) + 1);
}
  