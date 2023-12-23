#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<stdlib.h>
#include"conio2.h"
#include<windows.h>
#include<time.h>

#define NUM_POINTS 24
#define NUM_ROWS 15
#define WIDTH 26
#define SAVEF "backgammonSave.txt"
#define INV_INPUT "\nInvalid input. Please enter a number.\n"
#define INV_INPUT1 "Invalid input. Try again.\n"
#define CANTRE "Cannot re-enter at position %d. Choose the other dice or try again.\n"
#define CANTMOV "Error, you can't make this move\n"
#define VALIDOPT "The only valid options are 1 or 2\n"
#define INVPOS "Invalid position. Places are numbered from 1 to 24\n"
#define REEN "Pawn re-enters\n"
#define WHICHRE "Choose which dice to use to re-enter the board (1 or 2): "
#define WHICHDI "Choose which dice to use first (1 or 2): "
#define ENTPOS "Enter the position for the move [by %d]: "
#define COURTOUT "Courting from the outest point: %d\n"
#define SECMOV "Enter the position for the second move: "
#define CANTREM "You can't remove your pawns to the court yet"
#define CHOICE "For two moves (two values seperately) type 1, for one move (two values summed) type 2 : "
#define FORCE "You've been forced to attack a pawn. (The closest possible to opponent's base).\n"
#define ENTPOS2 "Enter the position for your move"
#define DICE_FUN Board* board, Court* court, int dice1, int dice2, bool& isMovDone, int& ind
#define BOARD_BTM "*-12--11--10--9---8---7----------6---5---4---3---2---1---*\n"
#define BOARD_TOP "*13--14--15--16--17--18----------19--20--21--22--23--24--*\n"
#define TURN "\nPlayer %d's turn. Press space to roll dice."
#define REP_MENU "Replay mode. Press:\n[o] - Previous game state\n[p] - Next game state\n[u] - First game state\n[i] - Latest game state\n"
#define MAL_ERR "Error allocating memory"
#define NO_FILE "No file to open from, pick a different mode.\n"
#define ID6 17
#define ID24 11
#define ID1 12
#define ID19 6


// struct handling board, bar and dices
typedef struct {
    int pts[NUM_POINTS];
    int currPlayer;
    int bar[2];
    int die1;
    int die2;
    int dice[2];
    int shouldSave;
} Board;


//board handling courted pawns (dwor)
typedef struct {
    int court[2];
    int toCourt[2];
} Court;


//board replay mode
typedef struct gameSt {
    Board board;
    Court court;
    struct gameSt* next;
    struct gameSt* prev;
} gameSt;

gameSt* currState = NULL;


void rollDice(int& die1, int& die2) {
    die1 = rand() % 6 + 1;
    die2 = rand() % 6 + 1;
}

void displayDice(Board* board) {
    printf("\nDice: %d, %d\n", board->dice[0], board->dice[1]);
}


//function asking for a number (chared)
int getNum() {
    int number = 0;
    char ch;
    while (1) {
        ch = getch();

        if (ch >= '0' && ch <= '9') {
            number = number * 10 + (ch - '0');
            printf("%c", ch);
        }
        else if (ch == '\r' || ch == '\n') {
            printf("\n");
            break;
        }
        else {
            printf(INV_INPUT);
            return -1;
        }
    }
    return number;
}


//function adjusting position that player moves from
int startPos() {
    int place = getNum();
    if (place > 12 && place < 25)
    {
        place -= 12;
    }
    else if (place > 0 && place < 13)
    {
        place += 12;
    }
    place -= 1;
    return place;
}


// adjusts index in checking if pawn to capture does exist
int adjIndex(int toAdjust)
{
    if (toAdjust > 12 && toAdjust < 25)
    {
        toAdjust -= 12;
    }
    else if (toAdjust > 0 && toAdjust < 13)
    {
        toAdjust += 12;
    }
    toAdjust -= 1;
    return toAdjust;
}


//pauses for s seconds
void pause2s()
{
    for (int i = 0; i < 20; i += 1) {
        while (kbhit()) {
            getch();
        }
        Sleep(100);
    }
}

//pauses for 3 seconds
void pause3s()
{
    for (int i = 0; i < 30; i += 1) {
        while (kbhit()) {
            getch();
        }
        Sleep(100);
    }
}


//saves game state to the file (backgammon.txt)
void save(const Board* board, const Court* court, const char* fname) {
    if (board->shouldSave)
    {
        FILE* file = fopen(fname, "a");
        if (!file)
        {
            perror("Error opening file for writing");
            return;
        }

        fprintf(file, "%d %d %d %d %d ", board->currPlayer, board->bar[0], board->bar[1], board->die1, board->die2); //writes  side values in the file

        fprintf(file, "%d %d %d %d ", court->court[0], court->court[1], court->toCourt[0], court->toCourt[1]); // writes  court struct values in the file

        for (int i = 0; i < NUM_POINTS; ++i) //writes board values in the file
        {
            fprintf(file, "%d ", board->pts[i]);
        }
        fprintf(file, "\n");

        fclose(file);
    }
}



//plain move without any forcing to capture/removing pawns/ reputting bars
void plainMove1_2(Board* board, int dice1, int place)
{
    while (true)
    {
        if (board->pts[place + dice1] <= 1 && board->pts[place + dice1] > -15 && board->pts[place] < 0)
        {
            if (board->pts[place + dice1] != 1)
            {
                board->pts[place] += 1;
                board->pts[place + dice1] -= 1;
                break;
            }
            if (board->pts[place + dice1] == 1)
            {
                board->pts[place] += 1;
                board->pts[place + dice1] -= 2;
                board->bar[0]++;
                break;
            }
        }
        else
        {
            printf(CANTMOV);
            continue;
        }
    }
}

//rounding move without any forcing to capture/removing pawns/ reputting bars
void roundMove1_2(Board* board, int dice1, int place) {
    while (true)
    {
        int carry = 0;
        carry = place + dice1 - NUM_POINTS;
        if (board->pts[carry] <= 1 && board->pts[carry] > -15 && board->pts[place] < 0)
        {
            if (board->pts[place + dice1] != 1)
            {
                board->pts[place] += 1;
                board->pts[carry] -= 1;
                break;
            }
            else if (board->pts[place + dice1] != 1)
            {
                board->pts[place] += 1;
                board->pts[carry] -= 2;
                board->bar[0]++;
                break;
            }
        }
        else
        {
            printf(CANTMOV);
            continue;
        }
    }
}


//checks if player 1 can remove any pawns from the board (dice1)
void canRe12(Board* board, Court* court, int dice1, int place, bool isMovDone, bool canRem)
{
    while (!isMovDone)
    {
        if (!canRem)
        {
            if (place + dice1 < NUM_POINTS)
            {
                plainMove1_2(board, dice1, place);
                break;
            }
            else if (place + dice1 > NUM_POINTS)
            {
                roundMove1_2(board, dice1, place);
                break;
            }
        }
        else if ((place >= ID19 && place <= ID24) && (place + dice1 >= ID1) && canRem)
        {
            for (int i = 6; i <= ID24; i++)
            {
                if (i + dice1 > ID24 && board->pts[i] < 0)
                {
                    board->pts[i] += 1;
                    court->court[1] += 1;
                    court->toCourt[1] += 1;
                    isMovDone = true;
                    printf(COURTOUT, i);
                    pause3s();
                }
            }
        }
        else
        {
            printf(CANTREM);
            continue;
        }
        break;
    }
}


//first dice move when the second is forced to capture (player 2)
void dice1Mov2(Board* board, Court* court, int dice1, bool& isMovDone)
{
    while (!isMovDone)
    {
        printf(ENTPOS, dice1);
        int place = startPos();
        if (place < 0 || place >= NUM_POINTS)
        {
            printf(INVPOS);
            continue;
        }

        int sum2 = 0;
        for (int i = ID19; i <= ID24; i++)
        {
            if (board->pts[i] < 0)
            {
                sum2 += board->pts[i];
            }
        }

        bool canRem = (sum2 == court->toCourt[1]);

        canRe12(board, court, dice1, place, isMovDone, canRem);
        break;
    }
}


//plain move without any forcing to capture/removing pawns/ reputting bars
void plainMove2_2(Board* board, int dice2, int place) {
    while (true)
    {
        if (board->pts[place + dice2] <= 1 && board->pts[place + dice2] > -15 && board->pts[place] < 0)
        {
            if (board->pts[place + dice2] != 1)
            {
                board->pts[place] += 1;
                board->pts[place + dice2] -= 1;
                break;
            }
            if (board->pts[place + dice2] == 1)
            {
                board->pts[place] += 1;
                board->pts[place + dice2] -= 2;
                board->bar[0]++;
                break;
            }
        }
        else
        {
            printf(CANTMOV);
            continue;
        }
    }
}

//rounding move without any forcing to capture/removing pawns/ reputting bars
void roundMove2_2(Board* board, int dice2, int place)
{
    while (true)
    {
        int carry = place + dice2 - NUM_POINTS;
        if (board->pts[carry] <= 1 && board->pts[carry] > -15 && board->pts[place] < 0)
        {
            if (board->pts[place + dice2] != 1)
            {
                board->pts[place] += 1;
                board->pts[carry] -= 1;
                break;
            }
            else if (board->pts[place + dice2] != 1)
            {
                board->pts[place] += 1;
                board->pts[carry] -= 2;
                board->bar[0]++;
                break;
            }
        }
        else
        {
            printf(CANTMOV);
            continue;
        }
    }
}


//checks if player 2 can remove any pawns from the board (dice2)
void canRe22(Board* board, Court* court, int dice2, int place, bool isMovDone, bool canRem)
{
    while (!isMovDone)
    {
        if (!canRem)
        {
            if (place + dice2 < NUM_POINTS)
            {
                plainMove2_2(board, dice2, place);
                break;
            }
            else if (place + dice2 > NUM_POINTS)
            {
                roundMove2_2(board, dice2, place);
                break;
            }
        }
        else if ((place >= ID19 && place <= ID24) && (place + dice2 >= ID1) && canRem)
        {
            for (int i = ID19; i <= ID24; i++)
            {
                if (i + dice2 > ID24 && board->pts[i] < 0)
                {
                    board->pts[i] += 1;
                    court->court[1] += 1;
                    court->toCourt[1] += 1;
                    isMovDone = true;
                    printf(COURTOUT, i);
                    pause3s();

                }
            }
        }
        else
        {
            printf(CANTREM);
            continue;
        }
        break;
    }
}


//second dice move when the first one is forced to capture (player 2)
void dice2Mov2(Board* board, Court* court, int dice2, bool& isMovDone)
{
    while (!isMovDone)
    {
        printf(SECMOV);
        int place = startPos();

        if (place < 0 || place >= NUM_POINTS)
        {
            printf(INVPOS);
            continue;
        }

        int sum2 = 0;
        for (int i = ID19; i <= ID24; i++)
        {
            if (board->pts[i] < 0)
            {
                sum2 += board->pts[i];
            }
        }

        bool canRem = (sum2 == court->toCourt[1]);
        canRe22(board, court, dice2, place, isMovDone, canRem);
        break;
    }
}


// normal move for player 1, no removing, no forcing to capture, no rounding
void plainMove1(Board* board, int dice1, int place)
{
    while (true)
    {
        if (board->pts[place - dice1] >= -1 && board->pts[place - dice1] < 15 && board->pts[place] > 0)
        {
            if (board->pts[place - dice1] != -1)
            {
                board->pts[place] -= 1;
                board->pts[place - dice1] += 1;
                break;
            }
            else if (board->pts[place - dice1] == -1)
            {
                board->pts[place] -= 1;
                board->pts[place - dice1] += 2;
                board->bar[1]++;
                break;
            }
        }
        else
        {
            printf(CANTMOV);
            break;
        }
    }
}


// normal move for player 1, no removing, no forcing to capture, rounding
void roundMove1(Board* board, int dice1, int place)
{
    while (true)

    {
        int carry = place - dice1 + NUM_POINTS;
        if (board->pts[carry] >= -1 && board->pts[carry] < 15 && board->pts[place] > 0)
        {
            if (board->pts[place + carry] != -1)
            {
                board->pts[place] -= 1;
                board->pts[carry] += 1;
                break;
            }
            else if (board->pts[place + carry] == -1)
            {
                board->pts[place] -= 1;
                board->pts[carry] += 2;
                board->bar[1]++;
                break;
            }
        }
        else
        {
            printf(CANTMOV);
            break;
        }
    }
}

//checks if player 1 can remove any pawns from the board (dice1)
void canRe11(Board* board, Court* court, int dice1, int place, bool isMovDone, bool canRem) {
    while (!isMovDone)
    {
        if (!canRem) // if not goes to normal move without possibilty to remove pawns from the board
        {
            if (place - dice1 > 0)
            {
                plainMove1(board, dice1, place);
                break;
            }
            else if (place - dice1 < 0) //if there's any rounding (bar below/ bar above)
            {
                roundMove1(board, dice1, place);
                break;
            }
        }
        else if ((place >= ID1 && place <= ID6) && (place - dice1 <= ID24) && canRem) // checks if player can remove pawns from the board
        {
            for (int i = ID6; i >= ID1; i--)
            {
                if (i - dice1 < 11 && board->pts[i] > 0) // removing functionality
                {
                    board->pts[i] -= 1;
                    court->court[0] += 1;
                    court->toCourt[0] -= 1;
                    isMovDone = true;
                    printf(COURTOUT, i);
                    pause3s();
                }
            }
        }
        else
        {
            printf(CANTREM);
            break;
        }
        break;
    }
}


// first dice move for player 1 (when second one is force to capture)
void dice1Mov1(Board* board, Court* court, int dice1, bool& isMovDone)
{
    while (!isMovDone)
    {
        printf(ENTPOS, dice1);
        int place = startPos();
        if (place < 0 || place >= NUM_POINTS)
        {
            printf(INVPOS);
            continue;
        }

        int sum = 0;
        for (int i = ID1; i <= ID6; i++)
        {
            if (board->pts[i] > 0)
            {
                sum += board->pts[i];
            }
        }

        bool canRem = (sum == court->toCourt[0]);

        canRe11(board, court, dice1, place, isMovDone, canRem);
        break;
    }
}


//plain move without any forcing to capture/removing pawns/ reputting bars
void plainMove2(Board* board, int dice2, int place)
{
    while (true)
    {
        if (board->pts[place - dice2] >= -1 && board->pts[place - dice2] < 15 && board->pts[place] > 0)
        {
            if (board->pts[place - dice2] != -1)
            {
                board->pts[place] -= 1;
                board->pts[place - dice2] += 1;
                break;
            }
            else if (board->pts[place - dice2] == -1)
            {
                board->pts[place] -= 1;
                board->pts[place - dice2] += 2;
                board->bar[1]++;
                break;
            }
        }
        else
        {
            printf(CANTMOV);
            break;
        }
    }
}


// normal move for player 2, no removing, no forcing to capture, rounding
void roundMove2(Board* board, int dice2, int place)
{
    while (true)
    {
        int carry = place - dice2 + NUM_POINTS;
        if (board->pts[carry] >= -1 && board->pts[carry] < 15 && board->pts[place] > 0)
        {
            if (board->pts[place + carry] != -1)
            {
                board->pts[place] -= 1;
                board->pts[carry] += 1;
                break;
            }
            else if (board->pts[place + carry] == -1)
            {
                board->pts[place] -= 1;
                board->pts[carry] += 2;
                board->bar[1]++;
                break;
            }
        }
        else
        {
            printf(CANTMOV);
            break;
        }
    }
}

//checks if player 1 can remove any pawns from the board (dice2)
void canRe21(Board* board, Court* court, int dice2, int place, bool isMovDone, bool canRem)
{
    while (!isMovDone)
    {
        if (!canRem)
        {
            if (place - dice2 > 0)
            {
                plainMove2(board, dice2, place);
                break;
            }
            else if (place - dice2 < 0)
            {
                roundMove2(board, dice2, place);
                break;
            }

        }
        else if ((place >= ID1 && place <= ID6) && (place - dice2 <= ID24) && canRem)
        {
            for (int i = ID6; i >= ID1; i--)
            {
                if (i - dice2 < ID24 && board->pts[i] > 0)
                {
                    board->pts[i] -= 1;
                    court->court[0] += 1;
                    court->toCourt[0] -= 1;
                    isMovDone = true;
                    printf(COURTOUT, i);
                    pause3s();
                }
            }
        }
        else
        {
            printf(CANTREM);
            continue;
        }
        break;
    }
}


// when dice 1 is forced to capture, the second dice function this is
void dice2Mov1(Board* board, Court* court, int dice2, bool& isMovDone)
{
    while (!isMovDone)
    {
        printf(ENTPOS, dice2);
        int place = startPos();
        if (place < 0 || place >= NUM_POINTS)
        {
            printf(INVPOS);
            continue;
        }

        int sum = 0;
        for (int i = ID1; i <= ID6; i++)
        {
            if (board->pts[i] > 0)
            {
                sum += board->pts[i];
            }
        }

        bool canRem = (sum == court->toCourt[0]);

        canRe21(board, court, dice2, place, isMovDone, canRem);
        break;
    }
}




// move without forcing to capture (player 1)
void defMov1(Board* board, Court* court, bool& isMovDone) {
    printf(WHICHDI);
    int diceChoice = getNum(); // which dice first

    int dice1 = (diceChoice == 1) ? board->dice[0] : board->dice[1];
    int dice2 = (diceChoice == 1) ? board->dice[1] : board->dice[0];
    dice1Mov1(board, court, dice1, isMovDone);
    save(board, court, SAVEF);
    if (!isMovDone)
    {
        dice2Mov1(board, court, dice2, isMovDone);
        save(board, court, SAVEF);
    }
    isMovDone = true;
}


// move without forcing to capture (player 2)
void defMov2(Board* board, Court* court, bool& isMovDone)
{
    printf(WHICHDI);
    int diceChoice = getNum();

    int dice1 = (diceChoice == 1) ? board->dice[0] : board->dice[1];
    int dice2 = (diceChoice == 1) ? board->dice[1] : board->dice[0];
    dice1Mov2(board, court, dice1, isMovDone);
    save(board, court, SAVEF);
    if (!isMovDone)
    {
        dice2Mov2(board, court, dice2, isMovDone);
        save(board, court, SAVEF);
    }
    isMovDone = true;
}


//checks if player 1has pawns on the board, if yes it forces to put them back on the board
void barLog1(Board* board, Court* court, int dice1, int dice2, bool& isMovDone) {
    if (board->bar[0] > 0)
    {
        int choice = 0;
        while (choice != 1 && choice != 2)
        {
            printf(WHICHRE);
            choice = getNum();
        }
        while (!isMovDone)
        {
            int entry = 12 - ((choice == 1) ? dice1 : dice2);

            if (entry >= 0 && entry < NUM_POINTS && board->pts[entry] >= -1)
            {
                board->bar[0]--;
                if (board->pts[entry] == -1)
                {

                    board->pts[entry] = 1;
                    board->bar[1]++;
                }
                else
                {
                    board->pts[entry]++;
                }
                isMovDone = true;
                printf(REEN);
                save(board, court, SAVEF);
                pause3s();
            }
            else
            {
                printf(CANTRE, entry + 1);
            }
        }
    }
}


//checks if player 2 has pawns on the board, if yes it forces to put them back on the board
void barLog2(Board* board, Court* court, int dice1, int dice2, bool& isMovDone)
{
    if (board->bar[1] > 0) {
        int choice = 0;
        while (choice != 1 && choice != 2) {
            printf(WHICHRE);
            choice = getNum();
        }
        while (!isMovDone) {
            int entry = 11 + ((choice == 1) ? dice1 : dice2);


            if (entry >= 0 && entry < NUM_POINTS && board->pts[entry] <= 1) {
                board->bar[1]--;
                if (board->pts[entry] == 1) {

                    board->pts[entry] = -1;
                    board->bar[0]++;
                }
                else {
                    board->pts[entry]--;
                }
                isMovDone = true;
                printf(REEN);
                save(board, court, SAVEF);
                pause3s();
            }
            else {
                printf(CANTRE, NUM_POINTS - entry);
            }
        }
    }
}


// dice1 is greater than dice2 for player 1 
void dice1Gr1(DICE_FUN)
{
    if (board->pts[ind] > 0 && board->pts[ind] <= 15)
    {
        if (board->pts[ind - dice1] == -1)
        {
            board->pts[ind] -= 1;
            board->pts[ind - dice1] += 2;
            board->bar[1]++;
            printf(FORCE);
            save(board, court, SAVEF);
            dice2Mov1(board, court, dice2, isMovDone);
            save(board, court, SAVEF);
            isMovDone = true;
        }
        else if (board->pts[ind - dice2] == -1)
        {
            board->pts[ind] -= 1;
            board->pts[ind - dice2] += 2;
            board->bar[1]++;
            printf(FORCE);
            save(board, court, SAVEF);
            dice1Mov1(board, court, dice1, isMovDone);
            save(board, court, SAVEF);
            isMovDone = true;
        }
    }
}


//dice2 grater than dice1 for player 1
void dice2Gr1(DICE_FUN)
{
    if (board->pts[ind] > 0 && board->pts[ind] <= 15)
    {
        if (board->pts[ind - dice2] == -1)
        {
            board->pts[ind] -= 1;
            board->pts[ind - dice2] += 2;
            board->bar[1]++;
            printf(FORCE);
            save(board, court, SAVEF);
            dice1Mov1(board, court, dice1, isMovDone);
            save(board, court, SAVEF);
            isMovDone = true;
        }
        if (board->pts[ind - dice1] == -1)
        {
            board->pts[ind] -= 1;
            board->pts[ind - dice1] += 2;
            board->bar[1]++;
            printf(FORCE);
            save(board, court, SAVEF);
            dice2Mov1(board, court, dice1, isMovDone);
            save(board, court, SAVEF);
            isMovDone = true;
        }
    }
}



// if no rounding for player 1 option 2 then it does this (no carry variable to change-adjust indexes)
void ifNoRound(Board* board, Court* court, int& diceSum, bool& isMovDone, int& place)
{
    if (place - diceSum > 0)
    {
        if (board->pts[place - diceSum] >= -1 && board->pts[place - diceSum] < 15 && board->pts[place] > 0)
        {
            if (board->pts[place - diceSum] != -1)
            {
                board->pts[place] -= 1;
                board->pts[place - diceSum] += 1;
                save(board, court, SAVEF);
            }
            else if (board->pts[place - diceSum] == -1)
            {
                board->pts[place] -= 1;
                board->pts[place - diceSum] += 2;
                board->bar[1]++;
                save(board, court, SAVEF);
            }
        }
        else if (board->pts[place - diceSum] < -1)
        {
            printf(CANTMOV);
        }
    }
}

// if rounding for player 1 option 2 then it does this (carry variable to change-adjust indexes)
void ifRound(Board* board, Court* court, int& diceSum, bool& isMovDone, int& place)
{
    if (place - diceSum < 0)
    {
        int carry = place - diceSum + NUM_POINTS;
        if (board->pts[carry] >= -1 && board->pts[carry] < 15 && board->pts[place] > 0)
        {
            if (board->pts[place + carry] != -1)
            {
                board->pts[place] -= 1;
                board->pts[carry] += 1;
                save(board, court, SAVEF);
            }
            else if (board->pts[place + carry] == -1)
            {
                board->pts[place] -= 1;
                board->pts[carry] += 2;
                board->bar[1]++;
                save(board, court, SAVEF);
            }
        }
        else
        {
            printf(CANTMOV);
        }

    }
}

//checks either there is rounding or not for player 1 option 2
void rounding(Board* board, Court* court, int& diceSum, bool& isMovDone, int& place)
{
    ifNoRound(board, court, diceSum, isMovDone, place);
    ifRound(board, court, diceSum, isMovDone, place);
}


// option 1 for player 1 (two values seperately)
void moveDecision1_1(Board* board, Court* court, int dice1, int dice2, bool& isMovDone)
{
    for (int i = NUM_POINTS; i > 1; i--)
    {
        int ind = adjIndex(i);
        if (dice1 < dice2) //this if which dice value is greater to firstly check its possibility to capture (made as a result cuz of some past bugs)
        {
            dice1Gr1(board, court, dice1, dice2, isMovDone, ind);
        }
        else if (dice1 > dice2)
        {
            dice2Gr1(board, court, dice1, dice2, isMovDone, ind);
        }
    }

    while (!isMovDone)
    {
        defMov1(board, court, isMovDone);
    }
}

//option 2 (two values summed) for player 1 
void moveDecision2_1(Board* board, Court* court, bool& isMovDone)
{
    printf(ENTPOS2);
    int place = startPos();
    int diceSum = board->dice[0] + board->dice[1];
    if (place < 0 || place >= NUM_POINTS)
    {
        printf(INVPOS);
        return;
    }
    rounding(board, court, diceSum, isMovDone, place);
}


//function for player's 1 move decision making
void decision1(Board* board, Court* court, int dice1, int dice2, bool& isMovDone)
{
    while (!isMovDone)
    {
        printf(CHOICE);
        int movDec = getNum();
        if (movDec == 1)
        {
            moveDecision1_1(board, court, dice1, dice2, isMovDone); // two values seperately
            break;
        }
        else if (movDec == 2)
        {
            moveDecision2_1(board, court, isMovDone); // two values summed
        }
        else if (movDec != 1 && movDec != 2)
        {
            printf(VALIDOPT);
        }
    }
}



// if no rounding for player 2 option 2 then it does this (no carry variable to change-adjust indexes)
void ifNoRound2(Board* board, Court* court, int& diceSum, bool& isMovDone, int& place) {
    if (place + diceSum < NUM_POINTS)
    {
        if (board->pts[place + diceSum] <= 1 && board->pts[place + diceSum] > -15 && board->pts[place] < 0)
        {
            if (board->pts[place + diceSum] != 1)
            {
                board->pts[place] += 1;
                board->pts[place + diceSum] -= 1;
                save(board, court, SAVEF);
            }
            else if (board->pts[place + diceSum] == 1)
            {
                board->pts[place] += 1;
                board->pts[place + diceSum] -= 2;
                board->bar[0]++;
                save(board, court, SAVEF);
            }
        }
        else if (board->pts[place + diceSum] > 1)
        {
            printf(CANTMOV);
        }
    }
}

// if rounding for player 2 option 2 then it does this (carry variable to change-adjust indexes)
void ifRound2(Board* board, Court* court, int& diceSum, bool& isMovDone, int& place)
{
    if (place + diceSum > NUM_POINTS)
    {
        int carry = place + diceSum - NUM_POINTS;
        if (board->pts[carry] <= 1 && board->pts[carry] > -15 && board->pts[place] < 0)
        {
            if (board->pts[place + carry] != 1)
            {
                board->pts[place] += 1;
                board->pts[carry] -= 1;
                save(board, court, SAVEF);
            }
            else if (board->pts[place + carry] == 1)
            {
                board->pts[place] += 1;
                board->pts[carry] -= 2;
                board->bar[0]++;
                save(board, court, SAVEF);
            }
        }
        else
        {
            printf(CANTMOV);
        }

    }
}

//checks either there is rounding or not for player 2 option 2
void rounding2(Board* board, Court* court, int& diceSum, bool& isMovDone, int& place)
{
    ifNoRound2(board, court, diceSum, isMovDone, place);
    ifRound2(board, court, diceSum, isMovDone, place);
}

// dice1 is greater than dice2 for player 2 
void dice1gr2(DICE_FUN) {
    if (board->pts[ind] < 0 && board->pts[ind] >= -15)
    {
        if (board->pts[ind + dice1] == 1)
        {
            board->pts[ind] += 1;
            board->pts[ind + dice1] -= 2;
            board->bar[0]++;
            printf(FORCE);
            save(board, court, SAVEF);
            isMovDone = true;
        }
        if (board->pts[ind + dice2] == 1)
        {
            board->pts[ind] += 1;
            board->pts[ind + dice2] -= 2;
            board->bar[0]++;
            printf(FORCE);
            save(board, court, SAVEF);
            isMovDone = true;
        }
    }
}

// dice2 is greater than dice1 for player 2
void dice2gr2(DICE_FUN)
{
    if (board->pts[ind] < 0 && board->pts[ind] >= -15)
    {
        if (board->pts[ind + dice2] == 1)
        {
            board->pts[ind] += 1;
            board->pts[ind + dice2] -= 2;
            board->bar[0]++;
            printf(FORCE);
            save(board, court, SAVEF);
            isMovDone = true;
        }
        if (board->pts[ind + dice1] == 1)
        {
            board->pts[ind] += 1;
            board->pts[ind + dice1] -= 2;
            board->bar[0]++;
            printf(FORCE);
            save(board, court, SAVEF);
            isMovDone = true;
        }
    }
}

// option 1 for player 2 (two values seperately)
void moveDecision1_2(Board* board, Court* court, int dice1, int dice2, bool& isMovDone)
{
    for (int i = 0; i < NUM_POINTS - 1; i++)
    {
        int ind = adjIndex(i);
        dice1gr2(board, court, dice1, dice2, isMovDone, ind);
        dice2gr2(board, court, dice1, dice2, isMovDone, ind);

    }
    while (!isMovDone)
    {
        defMov2(board, court, isMovDone);
    }
}

//option 2 (two values summed) for player 1 
void moveDecision2_2(Board* board, Court* court, bool& isMovDone)
{
    printf(ENTPOS2);
    int place = startPos();
    int diceSum = board->dice[0] + board->dice[1];
    if (place < 0 || place >= NUM_POINTS)
    {
        printf(INVPOS);
        return;
    }
    rounding2(board, court, diceSum, isMovDone, place);
}


//function for player's 2 move decision making
void decision2(Board* board, Court* court, int dice1, int dice2, bool& isMovDone)
{
    while (!isMovDone)
    {
        printf(CHOICE);
        int movDec = getNum();
        if (movDec == 1)
        {
            moveDecision1_2(board, court, dice1, dice2, isMovDone);
        }

        else if (movDec == 2)
        {
            moveDecision2_2(board, court, isMovDone);
        }
        else if (movDec != 1 && movDec != 2)
        {
            printf(VALIDOPT);
        }
    }
}


// creates the first state in save file (backgammon.txt)
void initFirstState(const Board* board, const char* fname) {

    FILE* file = fopen(fname, "r+");
    if (file != NULL)
    {

        fclose(file);
        return;
    }
    file = fopen(fname, "w");
    if (file != NULL)
    {
        fprintf(file, "0 0 0 0 0 0 5 0 0 0 0 -3 -5 0 0 0 0 2 -2 0 0 0 0 5 3 0 0 0 0 -5\n");
        fclose(file);
    }
    else {
        perror("Error creating save file");
    }
}

// mother function for moving pawns 
void movePawn(Board* board, Court* court) {
    bool isMovDone = false;
    if (board->currPlayer == 0)
    {
        int dice1 = board->dice[0];
        int dice2 = board->dice[1];

        barLog1(board, court, dice1, dice2, isMovDone);
        if (!isMovDone)
        {
            decision1(board, court, dice1, dice2, isMovDone);
        }
    }
    else if (board->currPlayer == 1)
    {
        int dice1 = board->dice[0];
        int dice2 = board->dice[1];

        barLog2(board, court, dice1, dice2, isMovDone);
        if (!isMovDone)
        {
            decision2(board, court, dice1, dice2, isMovDone);
        }
    }

    if (isMovDone) {
        board->currPlayer = 1 - board->currPlayer;
    }
}


// initializes board positions and all the game specification (initial values of dices to zero, what value in court needed to win)
void initBoard(Board* board, Court* court) {
    gotoxy(1, 1);
    clrscr();
    for (int i = 0; i < NUM_POINTS; i++)
    {
        board->pts[i] = 0;
    }


    board->pts[0] = 5;
    board->pts[11] = 2;
    board->pts[17] = 5;
    board->pts[19] = 3;
    board->pts[23] = -5;
    board->pts[4] = -3;
    board->pts[6] = -5;
    board->pts[12] = -2;

    board->bar[0] = 0;
    board->bar[1] = 0;

    board->dice[0] = 0;
    board->dice[1] = 0;

    court->toCourt[0] = 15;
    court->toCourt[1] = -15;

    board->shouldSave = 0;
}

// margin from the left (to center things)
void printLeftMargin() {
    printf("      ");
}

//prints board points depending on values in the board array (positives for player1, negatives for player2)
void printBoardPoint(Board* board, Court* court, int pointIndex, int row, int isUp) {
    int pieces = board->pts[pointIndex];
    char piece = (pieces > 0) ? 'X' : 'O';
    char empty = '-';

    if ((isUp && pointIndex == 23) || (!isUp && pointIndex == 0))
    {
        int courted = isUp ? court->court[0] : court->court[1];
        printf("%c   ", (courted > row) ? piece : empty);
    }
    else
    {
        if (isUp)
        {
            if (abs(pieces) > row)
            {
                printf("%c   ", piece);
            }
            else
            {
                printf("%c   ", empty);
            }
        }
        else
        {

            int maxPawns = abs(pieces) > NUM_ROWS ? NUM_ROWS : abs(pieces);
            if (maxPawns >= (NUM_ROWS - row))
            {
                printf("%c   ", piece);
            }
            else
            {
                printf("%c   ", empty);
            }
        }
    }
}



//prints rows of the board (from left to right)
void printRow(Board* board, Court* court, int row, int isUp) {
    printLeftMargin();
    printf("| ");

    for (int i = 0; i < 12; i++)
    {
        int pointIndex = isUp ? i : 23 - i;
        printBoardPoint(board, court, pointIndex, row, isUp);

        if (i == 5)
        {
            printf("|    | ");
        }
    }

    printf("|");
}

//sets up whats needed to be displayed in the console 
void setBoard(Board* board, Court* court) {
    printf("Captured: Player 1 (X): %d, Player 2 (O): %d | Courted: Player 1 (X): %d, Player 2 (O): %d\n\n",
        board->bar[0], board->bar[1], court->court[0], court->court[1]);

    printLeftMargin();
    printf(BOARD_TOP);
    for (int i = 0; i < NUM_ROWS; i++)
    {
        printRow(board, court, i, 1);
        printf("   |\n");
    }
    printLeftMargin();
    printf("|                         |    |                         |\n");
    for (int i = 0; i < NUM_ROWS; i++)
    {
        printRow(board, court, i, 0);
        printf("   |\n");
    }
    printLeftMargin();
    printf(BOARD_BTM);
}


// for replay mode, loads the previous state 
void loadPrevState(Board* board, Court* court) {
    if (currState != NULL && currState->prev != NULL)
    {
        currState = currState->prev;
        *board = currState->board;
        clrscr();
        setBoard(board, court);
    }
    else
    {
        printf("No previous game state available.\n");
    }
}

// for replay mode, loads the next state
void loadNextState(Board* board, Court* court) {
    if (currState && currState->next)
    {
        currState = currState->next;
        *board = currState->board;
        clrscr();
        setBoard(board, court);
    }
    else
    {
        printf("No next game state available.\n");
    }
}


// regular turn function (handles rolling dice, displaying it and moving the pawn)
void regTurn(Board* board, Court* court)
{
    printf(TURN, board->currPlayer + 1);

    while (1) {
        if (kbhit()) {
            char ch = getch();
            if (ch == ' ')
            {
                board->shouldSave = 1;
                rollDice(board->dice[0], board->dice[1]);
                displayDice(board);
                movePawn(board, court);
                break;
            }
        }
    }
}

// next state in replay mode
void optP(Board* board, Court* court)
{
    gotoxy(1, 1);
    clrscr();
    loadNextState(board, court);
}

// previous state in replay mode
void optO(Board* board, Court* court)
{
    gotoxy(1, 1);
    clrscr();
    loadPrevState(board, court);
}


// replay mode handling and regular turn
void playerTurn(Board* board, Court* court, bool isRMode) {
    if (isRMode) {
        printf(REP_MENU);
        while (1)
        {
            char ch = getch();
            if (ch == 'o')
            {
                optO(board, court);
                break;
            }
            else if (ch == 'p')
            {
                optP(board, court);
                break;
            }
            else if (ch == 'u')
            {

                while (currState && currState->prev)
                {
                    currState = currState->prev;
                }
                *board = currState->board;
                clrscr();
                setBoard(board, court);
                break;
            }
            else if (ch == 'i') {

                while (currState && currState->next)
                {
                    currState = currState->next;
                }
                *board = currState->board;
                clrscr();
                setBoard(board, court);
                break;
            }
        }
    }
    else //regular turn if not in replay mode
    {
        regTurn(board, court);
    }

    clrscr();
    setBoard(board, court);
    save(board, court, SAVEF);
}


// parses through the line in save file (backgammon.txt)
void parseLine(const char* line, gameSt* state) {
    const char* ptr = line;
    int num;

    sscanf(ptr, "%d %d %d %d %d %d %d %d %d",
        &state->board.currPlayer, &state->board.bar[0], &state->board.bar[1],
        &state->board.die1, &state->board.die2, &state->court.court[0],
        &state->court.court[1], &state->court.toCourt[0], &state->court.toCourt[1]);

    for (int i = 0; i < 9; ++i)
    {
        while (*ptr != ' ' && *ptr != '\0')
        {
            ptr++;
        }
        if (*ptr != '\0')
        {
            ptr++;
        }
    }

    for (int i = 0; i < NUM_POINTS; ++i)
    {
        sscanf(ptr, "%d", &num);
        while (*ptr != ' ' && *ptr != '\0')
        {
            ptr++;
        }
        if (*ptr != '\0')
        {
            ptr++;
        }
        state->board.pts[i] = num;
    }
}


void readStates(const char* fname) {
    FILE* file = fopen(fname, "r");
    if (!file)
    {
        perror("ERR");
        return;
    }

    gameSt* head = NULL, * tail = NULL;

    gameSt* st1 = (gameSt*)malloc(sizeof(gameSt));
    if (!st1)
    {
        perror("ERR");
        fclose(file);
        return;
    }

    initBoard(&st1->board, &st1->court);
    st1->prev = NULL;
    st1->next = NULL;
    head = st1;
    tail = st1;

    char line[1024];
    while (fgets(line, sizeof(line), file))
    {
        gameSt* newSt = (gameSt*)malloc(sizeof(gameSt));
        if (!newSt)
        {
            perror("ERR");
            fclose(file);
            return;
        }

        parseLine(line, newSt);
        newSt->next = NULL;
        newSt->prev = tail;
        if (tail)
        {
            tail->next = newSt;
        }
        tail = newSt;
    }

    fclose(file);
    currState = tail;
}


//checks if any save does exist in the save file (backgammon.txt)
bool checkSave() {
    FILE* file = fopen(SAVEF, "r");
    if (file)
    {
        fclose(file);
        return true;
    }
    return false;
}


// main menu
int choiceAsk() {
    printf("Do you want to continue the saved game or start a new one?\n");
    printf("1. Continue saved game\n");
    printf("2. Start new game\n");
    printf("3. Replay mode (from the loaded save)\n");
    printf("Enter your choice [1] / [2] / [3]");

    char ch;
    do {
        ch = getch();
        if (ch == '1' || ch == '2' || ch == '3')
        {
            printf("%c\n", ch);
            return ch - '0';
        }
    } while (ch != '\r' && ch != '\n');

    printf("INV_INPUT");
    return -1;
}


//parses through court values in save file (backgammon.txt)
void parseCourt(Court* court, int num, int i)
{
    if (i == 0)
    {
        court->court[0] = num;
    }
    else if (i == 1)
    {
        court->court[1] = num;
    }
    else if (i == 2)
    {
        court->toCourt[0] = num;
    }
    else if (i == 3)
    {
        court->toCourt[1] = num;
    }
}


//parses through board values in save file (backgammon.txt)
void parseBoard(Board* board, int num, int i)
{
    if (i == 0)
    {
        board->currPlayer = num;
    }
    else if (i == 1)
    {
        board->bar[0] = num;
    }
    else if (i == 2)
    {
        board->bar[1] = num;
    }
    else if (i == 3)
    {
        board->die1 = num;
    }
    else if (i == 4)
    {
        board->die2 = num;
    }
}


void parse(int lastLine, Court* court, Board* board, char* line, FILE* file) {
    if (lastLine != -1) {
        fseek(file, lastLine, SEEK_SET);
        fgets(line, sizeof(line), file);
        char* ptr = line;
        int num;

        for (int i = 0; i < 5; ++i)
        {
            sscanf(ptr, "%d", &num);
            while (*ptr != ' ' && *ptr != '\0')
            {
                ptr++;
            }
            if (*ptr != '\0')
            {
                ptr++;
            }
            parseBoard(board, num, i);
        }

        for (int i = 0; i < 4; ++i)
        {
            sscanf(ptr, "%d", &num);
            while (*ptr != ' ' && *ptr != '\0')
            {
                ptr++;
            }
            if (*ptr != '\0')
            {
                ptr++;
            }
            parseCourt(court, num, i);
        }
        for (int i = 0; i < NUM_POINTS; i++)
        {
            sscanf(ptr, "%d", &num);
            board->pts[i] = num;

            while (*ptr != ' ' && *ptr != '\0')
            {
                ptr++;
            }
            if (*ptr != '\0')
            {
                ptr++;
            }
        }
    }
}

// loads the game from the save file (backgammon.txt)
void loadGame(Board* board, Court* court, const char* fname) {
    FILE* file = fopen(fname, "r");
    if (file == NULL)
    {
        printf("Error\n");
        exit(1);
    }

    char line[1024];
    int lastLine = -1;

    while (fgets(line, sizeof(line), file))
    {
        lastLine = ftell(file);
    }

    parse(lastLine, court, board, line, file);
    fclose(file);
}


void parseGameState(Board* board, const char* line) {
    const char* ptr = line;
    int num;

    for (int i = 0; i < 3; ++i)
    {
        sscanf(ptr, "%d", &num);
        while (*ptr != ' ' && *ptr != '\0')
        {
            ptr++;
        }
        if (*ptr != '\0') {
            ptr++;
        }

        if (i == 0)
        {
            board->currPlayer = num;
        }
        else if (i == 1)
        {
            board->die1 = num;
        }
        else
        {
            board->die2 = num;
        }
    }


    for (int ind = 0; ind < NUM_POINTS; ++ind)
    {
        sscanf(ptr, "%d", &num);
        while (*ptr != ' ' && *ptr != '\0')
        {
            ptr++;
        }
        if (*ptr != '\0')
        {
            ptr++;
        }
        board->pts[ind] = num;
    }
}

//loads the latest state of the game from the save file (backgammon.txt)
int loadGameState(Board* board, const char* fname) {
    FILE* file = fopen(fname, "r");
    if (!file)
    {
        perror("Error opening file for reading");
        return 0;
    }

    char line[1024];
    long lastLine = -1;

    while (fgets(line, sizeof(line), file))
    {
        lastLine = ftell(file);
    }
    if (lastLine != -1)
    {
        fseek(file, lastLine, SEEK_SET);
        fgets(line, sizeof(line), file);

        parseGameState(board, line);
    }

    fclose(file);
    return 1;
}


// updates the board in replay mode
void update(Board& gameBoard, Court& gameCourt) {
    readStates(SAVEF);
    if (currState != NULL)
    {
        gameBoard = currState->board;
        gameCourt = currState->court;
        setBoard(&gameBoard, &gameCourt);
    }
    else
    {
        printf("Error\n");
        initBoard(&gameBoard, &gameCourt);
    }
}


// load game option in menu
void opt1(Board& gameBoard, Court& gameCourt, bool& isLoaded) {
    loadGame(&gameBoard, &gameCourt, SAVEF);
    gameBoard.currPlayer = 1 - gameBoard.currPlayer;
    isLoaded = true;
}

// start game option in menu
void opt2(Board& gameBoard, Court& gameCourt)
{
    FILE* file = fopen(SAVEF, "w");
    if (file)
    {
        fclose(file);
    }
    else
    {
        printf("Error\n");
    }
    initBoard(&gameBoard, &gameCourt);
}

// rolling dices to check who starts the game
void start(Board* board)
{
    gotoxy(1, 1);
    clrscr();
    int dice1, dice2 = 0;
    while (true)
    {
        rollDice(dice1, dice2);
        printf("Player 1 rolled: %d, Player 2 rolled: %d\n\n", dice1, dice2);
        if (dice1 > dice2)
        {
            printf("Player 1 starts");
            board->currPlayer = 0;
            break;
        }
        else if (dice1 < dice2)
        {
            printf("Player 2 starts");
            board->currPlayer = 1;
            break;
        }
        else
        {
            continue;
        }
    }
    pause3s();
    pause2s();
}


// intializes the mode (game POV) based on if player wants to start a new game, load the saved game or watch the replay
void gameInit(Board& gameBoard, Court& gameCourt, bool& isLoaded, bool& isRMode) {
    if (checkSave())
    {
        int c;
        while (true)
        {
            gotoxy(1, 1);
            clrscr();
            c = choiceAsk();

            if (c == 1)
            {
                opt1(gameBoard, gameCourt, isLoaded);
                break;
            }
            else if (c == 2)
            {
                start(&gameBoard);
                opt2(gameBoard, gameCourt);
                break;
            }
            else if (c == 3)
            {
                FILE* file = fopen(SAVEF, "r");
                if (file)
                {
                    fseek(file, 0, SEEK_END);
                    long fileSize = ftell(file);
                    fclose(file);

                    if (fileSize == 0)
                    {
                        printf(NO_FILE);
                        pause2s();
                        continue;
                    }

                    isRMode = true;
                    update(gameBoard, gameCourt);
                    break;
                }
                else
                {
                    printf(NO_FILE);
                    continue;
                }
            }
            else
            {
                printf(INV_INPUT1);
            }
        }
    }
    else
    {
        initBoard(&gameBoard, &gameCourt);
    }
}


void checkForWin(bool over, Court* court)
{
    if (court->toCourt[0] <= 0)
    {
        over = true;
        gotoxy(1, 1);
        clrscr();
        printf("PLAYER 1 WON!");
    }
    else if (court->toCourt[1] >= 0)
    {
        over = true;
        gotoxy(1, 1);
        clrscr();
        printf("PLAYER 2 WON!");
    }
}

// visual aspect + conio intialization
void init()
{
    srand(time(NULL));
#ifndef __cplusplus
    Conio2_Init();
#endif
    settitle("Jakub Kwiatkowski s198074");
    _setcursortype(_NOCURSOR);
    textbackground(BLUE);
    textcolor(7);
}

int main() {
    init();

    Board gameBoard;
    Court gameCourt = { 0, 0 };
    bool isLoaded = false;
    bool isRMode = false;

    initFirstState(&gameBoard, SAVEF);
    gameInit(gameBoard, gameCourt, isLoaded, isRMode);

    if (isLoaded)
    {
        readStates(SAVEF);
        if (currState != NULL)
        {
            gameBoard = currState->board;
        }
    }

    clrscr();
    setBoard(&gameBoard, &gameCourt);


    bool over = false;

    // MAIN GAMEPLAY//
    while (!over) {
        playerTurn(&gameBoard, &gameCourt, isRMode);
        gotoxy(1, 1);
        clrscr();
        setBoard(&gameBoard, &gameCourt);
        checkForWin(over, &gameCourt);
    }

    printf("Game Over\n");
    return 0;
}