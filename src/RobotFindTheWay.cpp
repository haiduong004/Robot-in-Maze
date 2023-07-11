#include <iostream>
#include "console.h"
#include <windows.h>
#include <conio.h>
#include <cstdlib>
#include <stdlib.h>
#include <algorithm>
#include <fstream>
#include <vector>
#include <iomanip>

using namespace std;

// Đặt màu cho chữ. Kết hợp làm nhấp nháy chữ
void SetColor(WORD color) {
	HANDLE hConsoleOutput;
	hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;
	GetConsoleScreenBufferInfo(hConsoleOutput, &screen_buffer_info);

	WORD wAttributes = screen_buffer_info.wAttributes;
	color &= 0x000f;
	wAttributes &= 0xfff0;
	wAttributes |= color;

	SetConsoleTextAttribute(hConsoleOutput, wAttributes);
}
// Ẩn con trỏ trong Console
void hideCursor(){
    // Lấy handle của console
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    // Lấy thông tin về con trỏ
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(consoleHandle, &cursorInfo);
    // Ẩn con trỏ
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(consoleHandle, &cursorInfo);
}
// Di chuyển con trỏ tới vị trí (x,y)
void gotoXY (int column, int line)
{
 COORD coord;
 coord.X = column;
 coord.Y = line;
 SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),coord);
}




/* Khởi tạo các giá trị biến ban đầu */
vector<int> path,path1,path2; // Vector chứa đường đi của Robot
vector<vector<int>> robot,robot1,robot2,robot3; // Vector đại diện cho ma trận Robot phải đi
int ouputMode = 1; // Chế độ in file output. CĐ 1: ghi đè, CĐ 2: không ghi đè
int total_score,total_score1,total_score2; // Điểm số
int rows, cols; // Hàng của cột của ma trận
bool isContinue = true; // Tiếp tục chương trình hay không
int option; // Chế độ chơi

int poisonXtoShowRobot = 47, poisonXtoShowRobot1 = 3, poisonXtoShowRobot2 = 40, poisonXtoShowRobot3 = 34; // Toạ độ X để in ma trận Robot mô phỏng
int maxDigits = 4; // Khoảng cách để in giữa các phần tử của ma trận
const string YELLOW_COLOR_CODE = "\033[33m";
const string RED_COLOR_CODE = "\033[31m";
const string GREEN_COLOR_CODE = "\033[32m";
const string VIOLET_COLOR_CODE = "\033[34m";
const string BLUE_COLOR_CODE = "\033[36m";
const string RESET_COLOR_CODE = "\033[0m";

// Nhập dữ liệu cho mảng và số hàng, số cột từ file input.data
void inputFile(vector<vector<int>>& robot){
    ifstream inputFile("data/input.data");
    inputFile >> rows >> cols;
    for (int i = 0; i < rows; i++) {
        vector<int> row;
        for (int j = 0; j < cols; j++) {
            int value;
            inputFile >> value;
            row.push_back(value);
        }
        robot.push_back(row);
    }
    inputFile.close();
}
// Xuất kết quả ra file output.data
void outputFile(vector<int> &path, int ouputMode){
    if (ouputMode == 1) {
        ofstream outputFile("data/output.data");
        outputFile << "Đường đi của Robot: ";
        for (int i = 0; i < path.size(); i++)
            outputFile << path[i] << " ";
        outputFile << endl;
        outputFile.close();
    } else {
        ofstream outputFile("data/output.data", ios::app);
        outputFile << "Đường đi của Robot: ";
        for (int i = 0; i < path.size(); i++)
            outputFile << path[i] << " ";
        outputFile << endl;
        outputFile.close();
    }
}


// Kiểm tra xem ô (x,y) có nằm trong mê cung hay không
bool isValidCell(int x, int y) {
    return (x >= 0 && x < rows && y >= 0 && y < cols);
}
// Hàm tìm hướng đi có giá trị lớn nhất từ ô (x,y)
int findPath(vector<vector<int>>& robot, vector<vector<bool>>& visited, int x, int y) {
    int maxVal = 0;
    int rows = robot.size();
    int cols = robot[0].size();
    
    int direction = 0; 
    /* 0 -- Không có hướng đi. 
    1. Up -- 2. Right -- 3. Down -- 4. Left */

    // Kiểm tra hướng lên
    if (isValidCell(x - 1, y) && !visited[x - 1][y] && robot[x - 1][y] > maxVal) {
        maxVal = robot[x - 1][y];
        direction = 1; //'U';
    }

    // Kiểm tra hướng sang phải
    if (isValidCell(x, y + 1) && !visited[x][y + 1] && robot[x][y + 1] > maxVal) {
        maxVal = robot[x][y + 1];
        direction = 2; //'R';
    }

    // Kiểm tra hướng xuống
    if (isValidCell(x + 1, y) && !visited[x + 1][y] && robot[x + 1][y] > maxVal) {
        maxVal = robot[x + 1][y];
        direction = 3; //'D';
    }

    // Kiểm tra hướng sang trái
    if (isValidCell(x, y - 1) && !visited[x][y - 1] && robot[x][y - 1] > maxVal) {
        maxVal = robot[x][y - 1];
        direction = 4; //'L';
    }

    return direction;
}
// Hàm di chuyển 1 robot trên 1 mê cung
void moveRobot(vector<vector<int>>& robot, vector<int>& path, int start_x, int start_y, int& total_score) {
    total_score = 0;

    int rows = robot.size();
    int cols = robot[0].size();

    vector<vector<bool>> visited(rows, vector<bool>(cols, false));

    int x = start_x;
    int y = start_y;

    while (true) {
        total_score += robot[x][y];
        visited[x][y] = true;

        // In ra màn hình ma trận với kí tự 'X' đánh dấu ô đã đi qua
        for (int i = 0; i < rows; i++) {
            // Xoá dòng để chuẩn bị mô phỏng
			gotoXY(poisonXtoShowRobot,i+4);
    		cout << "       ";

			// Mô phỏng bước đi robot
            gotoXY(poisonXtoShowRobot,i+4);
            for (int j = 0; j < cols; j++) {
                if (visited[i][j]) {
                    cout << RED_COLOR_CODE << setw(maxDigits) << "x" << RESET_COLOR_CODE;
                } else {
                    cout << setw(maxDigits) << robot[i][j];
                }
            }
            cout << endl;
        }
        Sleep(800); // Delay màn hình

        // Nếu robot[x][y] ko thuộc <path> thì push_back vào
        auto it = find(path.begin(), path.end(), robot[x][y]);
        if (it == path.end()) {
            path.push_back(robot[x][y]);
        }

        int direction = findPath(robot,visited,x,y);
        if (direction == 0) {
            break; // Không còn hướng đi nào có giá trị lớn hơn
        }

        // Di chuyển theo hướng có giá trị lớn nhất
        if (direction == 1 /*'U'*/) {
            x--;
        } else if (direction == 3 /*'D'*/) {
            x++;
        } else if (direction == 4 /*'L'*/) {
            y--;
        } else if (direction == 2 /*'R'*/) {
            y++;
        }
    }
}


// Hàm tìm hướng đi có giá trị lớn nhất từ ô (x,y) 2 Robot. Nếu ô tiếp theo trùng nhau thì tìm hướng khác
int findPath2(vector<vector<int>>& robot, vector<vector<bool>>& visited1,vector<vector<bool>>& visited2, int x, int y) {
    int maxVal = 0;
    int rows = robot.size();
    int cols = robot[0].size();
    
    int direction = 0; 
    /* 0 -- Không có hướng đi. 
    1. Up -- 2. Right -- 3. Down -- 4. Left */

    // Kiểm tra hướng lên
    if (isValidCell(x - 1, y) && !visited1[x - 1][y] && !visited2[x - 1][y] && robot[x - 1][y] > maxVal) {
        maxVal = robot[x - 1][y];
        direction = 1; //'U';
    }

    // Kiểm tra hướng sang phải
    if (isValidCell(x, y + 1) && !visited1[x][y + 1] && !visited2[x][y + 1] && robot[x][y + 1] > maxVal) {
        maxVal = robot[x][y + 1];
        direction = 2; //'R';
    }

    // Kiểm tra hướng xuống
    if (isValidCell(x + 1, y) && !visited1[x + 1][y] && !visited2[x + 1][y] && robot[x + 1][y] > maxVal) {
        maxVal = robot[x + 1][y];
        direction = 3; //'D';
    }

    // Kiểm tra hướng sang trái
    if (isValidCell(x, y - 1) && !visited1[x][y - 1] && !visited2[x][y - 1] && robot[x][y - 1] > maxVal) {
        maxVal = robot[x][y - 1];
        direction = 4; //'L';
    }

    return direction;
}
// Hàm di chuyển 2 robot trên 2 mê cung
void moveTwoRobot(vector<vector<int>>& robot1, vector<vector<int>>& robot2,  vector<int>& path, vector<int>& path2, int start_1x, int start_1y, int start_2x, int start_2y, int& total_score1, int& total_score2) {
    total_score1 = 0;
    total_score2 = 0;

    int rows = robot1.size();
    int cols = robot1[0].size();

    vector<vector<bool>> visited1(rows, vector<bool>(cols, false));
    vector<vector<bool>> visited2(rows, vector<bool>(cols, false));

    int x1 = start_1x;
    int y1 = start_1y;
    int x2 = start_2x;
    int y2 = start_2y;

    while (true) {
        total_score1 += robot1[x1][y1];
        visited1[x1][y1] = true;
        total_score2 += robot2[x2][y2];
        visited2[x2][y2] = true;

        // In ra màn hình ma trận với kí tự 'X' đánh dấu ô đã đi qua
        for (int i = 0; i < rows; i++) {
            // Xoá dòng để chuẩn bị mô phỏng
			gotoXY(poisonXtoShowRobot1,i+4);
    		cout << "       ";

			// Mô phỏng bước đi robot
            gotoXY(poisonXtoShowRobot1,i+4);
            for (int j = 0; j < cols; j++) {
                if (visited1[i][j]) {
                    cout << RED_COLOR_CODE << setw(maxDigits) << "x" << RESET_COLOR_CODE;
                } else {
                    cout << setw(maxDigits) << robot1[i][j];
                }
            }
            cout << endl;
        }
        Sleep(300); // Delay màn hình

        // Nếu robot[x][y] ko thuộc <path> thì push_back vào
        auto it1 = find(path1.begin(), path1.end(), robot1[x1][y1]);
        if (it1 == path1.end()) {
            path1.push_back(robot1[x1][y1]);
        }

        // In ra màn hình ma trận với kí tự 'O' đánh dấu ô đã đi qua
        for (int i = 0; i < rows; i++) {
            // Xoá dòng để chuẩn bị mô phỏng
			gotoXY(poisonXtoShowRobot2,i+4);
    		cout << "       ";

			// Mô phỏng bước đi robot
            gotoXY(poisonXtoShowRobot2,i+4);
            for (int j = 0; j < cols; j++) {
                if (visited2[i][j]) {
                    cout << BLUE_COLOR_CODE << setw(maxDigits) << "o" << RESET_COLOR_CODE;
                } else {
                    cout << setw(maxDigits) << robot2[i][j];
                }
            }
            cout << endl;
        }
        Sleep(300); // Delay màn hình

        // Nếu robot[x][y] ko thuộc <path> thì push_back vào
        auto it2 = find(path2.begin(), path2.end(), robot2[x2][y2]);
        if (it2 == path2.end()) {
            path2.push_back(robot2[x2][y2]);
        }

        int direction1 = findPath(robot1,visited1,x1,y1);
        int direction2 = findPath(robot2,visited2,x2,y2);

        if (direction1 == 0 && direction2 == 0) {
            break; // Không còn hướng đi nào có giá trị lớn hơn
        }

        // Di chuyển theo hướng có giá trị lớn nhất
        if (direction1 == 1 /*'U'*/) {
            x1--;
        } else if (direction1 == 3 /*'D'*/) {
            x1++;
        } else if (direction1 == 4 /*'L'*/) {
            y1--;
        } else if (direction1 == 2 /*'R'*/) {
            y1++;
        }
        // Di chuyển Robot 2 theo hướng có giá trị lớn nhất
        if (direction2 == 1 /*'U'*/) {
            x2--;
        } else if (direction2 == 3 /*'D'*/) {
            x2++;
        } else if (direction2 == 4 /*'L'*/) {
            y2--;
        } else if (direction2 == 2 /*'R'*/) {
            y2++;
        }
    }
}



// Hàm di chuyển 2 robot di chuyển lần lượt trên 1 mê cung
void moveTwoRobotInOneMatrix(vector<vector<int>>& robot1, vector<vector<int>>& robot2, vector<int>& path1, vector<int>& path2, int start_1x, int start_1y, int start_2x, int start_2y, int& total_score1, int& total_score2) {
    total_score1 = 0;
    total_score2 = 0;

    int rows = robot1.size();
    int cols = robot1[0].size();

    vector<vector<bool>> visited1(rows, vector<bool>(cols, false));
    vector<vector<bool>> visited2(rows, vector<bool>(cols, false));

    int x1 = start_1x;
    int y1 = start_1y;
    int x2 = start_2x;
    int y2 = start_2y;

    while (true) {
        total_score1 += robot1[x1][y1];
        visited1[x1][y1] = true;
        total_score2 += robot2[x2][y2];
        visited2[x2][y2] = true;

        // In ra màn hình ma trận với kí tự 'X' đánh dấu ô đã đi qua
        for (int i = 0; i < rows; i++) {
            // Xoá dòng để chuẩn bị mô phỏng
            gotoXY(poisonXtoShowRobot3, i + 1);
            cout << "       ";

            // Mô phỏng bước đi robot
            gotoXY(poisonXtoShowRobot3, i + 1);
            for (int j = 0; j < cols; j++) {
                if (visited1[i][j]) {
                    cout << RED_COLOR_CODE << setw(maxDigits) << "X" << RESET_COLOR_CODE;
                } else if (visited2[i][j]) {
                    cout << VIOLET_COLOR_CODE << setw(maxDigits) << "O" << RESET_COLOR_CODE;
                } else {
                    cout << setw(maxDigits) << robot1[i][j];
                }
            }
            cout << endl;
        }
        Sleep(400); // Delay màn hình

        // Nếu robot[x][y] không thuộc <path> thì push_back vào
        auto it1 = find(path1.begin(), path1.end(), robot1[x1][y1]);
        if (it1 == path1.end()) {
            path1.push_back(robot1[x1][y1]);
        }

        // Nếu robot[x][y] không thuộc <path> thì push_back vào
        auto it2 = find(path2.begin(), path2.end(), robot2[x2][y2]);
        if (it2 == path2.end()) {
            path2.push_back(robot2[x2][y2]);
        }

        int direction1 = findPath2(robot1, visited1,visited2, x1, y1);
        int direction2 = findPath2(robot2, visited2,visited1, x2, y2);

        if (direction1 == 0 && direction2 == 0) {
            break; // Không còn hướng đi nào có giá trị lớn hơn
        }

        // Di chuyển theo hướng có giá trị lớn nhất
        if (direction1 == 1 /*'U'*/) {
            x1--;
        }
        else if (direction1 == 3 /*'D'*/) {
            x1++;
        }
        else if (direction1 == 4 /*'L'*/) {
            y1--;
        }
        else if (direction1 == 2 /*'R'*/) {
            y1++;
        }
        // Di chuyển Robot 2 theo hướng có giá trị lớn nhất
        if (direction2 == 1 /*'U'*/) {
            x2--;
        }
        else if (direction2 == 3 /*'D'*/) {
            x2++;
        }
        else if (direction2 == 4 /*'L'*/) {
            y2--;
        }
        else if (direction2 == 2 /*'R'*/) {
            y2++;
        }
    }
}



// Trang đầu tiên: hiển thị thông tin cá nhân
void firstPg() {
    system("cls");
    cout << YELLOW_COLOR_CODE;
    gotoXY(0,0);
    cout << "╔";
    Sleep(1);

    for (int i = 1; i < 119; i++ ){
        gotoXY(i,0);
        cout << "═";
        Sleep(1);
    }

    gotoXY(0,120);
    cout << "╗";
    Sleep(1);

    for (int i = 1; i < 29; i++ ){
        gotoXY(119,i);
        cout << "║";
        Sleep(1);
    }

    gotoXY(119,29);
    cout << "╝";
    Sleep(1);

    for (int i = 118; i > 0; i-- ){
        gotoXY(i,29);
        cout << "═";
        Sleep(1);
    }

    gotoXY(0,29);
    cout << "╚";
    Sleep(1);

    for (int i = 28; i >= 1; i-- ){
        gotoXY(0,i);
        cout << "║";
        Sleep(1);
    }

    gotoXY(44,2);
    cout << RED_COLOR_CODE << "BÀI TẬP LỚN - MÔN KỸ THUẬT LẬP TRÌNH" << RESET_COLOR_CODE;

    for (int i = 0; i < 19; i++) {
        SetColor(i);
        gotoXY(8,5);
        cout << " ____  _____  ____  _____  ____    ____  ____  _  _  ____     ____  _   _  ____    _    _    __    _  _";
        gotoXY(8,6);
        cout << "(  _ \\(  _  )(  _ \\(  _  )(_  _)  ( ___)(_  _)( \\( )(  _ \\   (_  _)( )_( )( ___)  ( \\/\\/ )  /__\\  ( \\/ )";
        gotoXY(8,7);
        cout << " )   / )(_)(  ) _ < )(_)(   )(     )__)  _)(_  )  (  )(_) )    )(   ) _ (  )__)    )    (  /(__)\\  \\  /";
        gotoXY(8,8);
        cout << "(_)\\_)(_____)(____/(_____) (__)   (__)  (____)(_)\\_)(____/    (__) (_) (_)(____)  (__/\__)(__)(__) (__)";
        Sleep(30);
    }
    
    Sleep(100);
 
    gotoXY(44,11);
    cout << VIOLET_COLOR_CODE << "Giảng viên hướng dẫn: Cô Bùi Thị Thà";

    cout << YELLOW_COLOR_CODE;
    for (int i = 45; i <= 79; i++ ){
        gotoXY(i,15);
        cout << "-";
        Sleep(1);
    }
    for (int i = 45; i <= 79; i++ ){
        gotoXY(i,23);
        cout << "-";
        Sleep(1);
    }
    cout << BLUE_COLOR_CODE;
    for ( int i = 1; i < 15; i++ ){
        SetColor(i);
        gotoXY(50,17);
        cout << "Author: Cao Hải Dương";
        Sleep(10);
    }
    Sleep(100);
    for ( int i = 1; i < 15; i++ ){
        SetColor(i);
        gotoXY(50,18);
        cout << "Class: CN22D";
        Sleep(10);
    }
    Sleep(100);
    for ( int i = 1; i < 15; i++ ){
        SetColor(i);
        gotoXY(50,19);
        cout << "Student code: 2251120144";
        Sleep(10);
    }
    Sleep(100);
    gotoXY(49,21);
    cout << GREEN_COLOR_CODE << "Press any key to continue!!" << RESET_COLOR_CODE;
    getch();
}
// Chế độ chơi 1: Một Robot di chuyển độc lập trên mê cung
void playModeOne() {
    system("cls");

    // Vẽ khung in điểm số
    gotoXY(0,17);
    cout << "╔";
    Sleep(1);

    for (int i = 1; i < 46; i++ ){
        gotoXY(i,17);
        cout << "═";
        Sleep(1);
    }
    
    gotoXY(46,17);
    cout << "╝";
    Sleep(1);

    gotoXY(46,16);
    cout << "╔";
    Sleep(1);

    for (int i = 47; i < 73 ; i++ ){
        gotoXY(i,16);
        cout << "═";
        Sleep(1);
    }

    gotoXY(73,16);
    cout << "╗";
    Sleep(1);

    gotoXY(73,17);
    cout << "╚";
    Sleep(1);

    for (int i = 74; i < 119; i++ ){
        gotoXY(i,17);
        cout << "═";
        Sleep(1);
    }

    gotoXY(17,120);
    cout << "╗";
    Sleep(1);

    for (int i = 18; i < 29; i++ ){
        gotoXY(119,i);
        cout << "║";
        Sleep(1);
    }

    gotoXY(119,29);
    cout << "╝";
    Sleep(1);

    for (int i = 118; i > 0; i-- ){
        gotoXY(i,29);
        cout << "═";
        Sleep(1);
    }

    gotoXY(0,29);
    cout << "╚";
    Sleep(1);

    for (int i = 28; i >= 18; i-- ){
        gotoXY(0,i);
        cout << "║";
        Sleep(1);
    }
    Sleep(500);
    gotoXY(48,17);
    cout << GREEN_COLOR_CODE << "Bạn đang chơi ở chế độ 1" << RESET_COLOR_CODE;
    Sleep(500);
    inputFile(robot);
	int start_x, start_y; // Vị trí ban đầu của Robot
    gotoXY(43,19);
	cout << "Nhập vị trí bắt đầu của Robot: " << RED_COLOR_CODE;
	cin >> start_x >> start_y; 
    cout << RESET_COLOR_CODE;
    cout << endl;

	moveRobot(robot,path,start_x,start_y,total_score);

    outputFile(path, ouputMode = 1);

    gotoXY(2,21);
	cout << "Robot đã đi qua các điểm" << BLUE_COLOR_CODE;
    gotoXY(2,22);
    cout << ">";
	for (int i = 0; i < path.size(); i++) {
        cout << path[i];
        if (i < path.size()-1 ) cout << "=> ";
    }
	gotoXY(2,24);
    cout << RESET_COLOR_CODE << "Tổng điểm Robot đạt được: ";
    for ( int i = 1; i < 15; i++ ){
        SetColor(i);
        gotoXY(29,24);
        cout << total_score;
        Sleep(5);
    }
}
// Chế độ chơi 2: Hai Robot di chuyển độc lập trên mê cung
void playModeTwo() {
    system("cls");

    gotoXY(0,15);
    cout << "╔";
    Sleep(1);

    for (int i = 1; i < 46; i++ ){
        gotoXY(i,15);
        cout << "═";
        Sleep(1);
    }

    gotoXY(46,15);
    cout << "╝";
    Sleep(1);

    gotoXY(46,14);
    cout << "╔";
    Sleep(1);

    for (int i = 47; i < 73 ; i++ ){
        gotoXY(i,14);
        cout << "═";
        Sleep(1);
    }

    gotoXY(73,14);
    cout << "╗";
    Sleep(1);

    gotoXY(73,15);
    cout << "╚";
    Sleep(1);

    for (int i = 74; i < 119; i++ ){
        gotoXY(i,15);
        cout << "═";
        Sleep(1);
    }

    gotoXY(15,120);
    cout << "╗";
    Sleep(1);

    for (int i = 16; i < 29; i++ ){
        gotoXY(119,i);
        cout << "║";
        Sleep(1);
    }

    gotoXY(119,29);
    cout << "╝";
    Sleep(1);

    for (int i = 118; i > 0; i-- ){
        gotoXY(i,29);
        cout << "═";
        Sleep(1);
    }

    gotoXY(0,29);
    cout << "╚";
    Sleep(1);

    for (int i = 28; i >= 16; i-- ){
        gotoXY(0,i);
        cout << "║";
        Sleep(1);
    }
    Sleep(500);
    gotoXY(48,15);
    cout << GREEN_COLOR_CODE << "Bạn đang chơi ở chế độ 2" << RESET_COLOR_CODE;
    Sleep(500);

    inputFile(robot1);
    inputFile(robot2);
	int start_1x, start_1y; // Vị trí ban đầu của Robot 1
    int start_2x, start_2y; // Vị trí ban đầu của Robot 1
    gotoXY(43,16);
	cout << RED_COLOR_CODE << "Nhập vị trí bắt đầu của Robot 1: ";
	cin >> start_1x >> start_1y;
    gotoXY(43,17);
    cout << BLUE_COLOR_CODE << "Nhập vị trí bắt đầu của Robot 2: ";
	cin >> start_2x >> start_2y;

    moveTwoRobot(robot1, robot2,  path1, path2, start_1x, start_1y, start_2x, start_2y, total_score1, total_score2);
    
    // OutputFile Robot
    outputFile(path1,ouputMode = 1); // Xoá hết dữ liệu và ghi mới
    outputFile(path2,ouputMode = 2); // Ghi đè không mất dữ liệu

    // Output Robot 1
    gotoXY(2,18);
	cout << RED_COLOR_CODE << "Robot 1 đã đi qua các điểm";
    gotoXY(3,19);
	for (int i = 0; i < path1.size(); i++) {
        cout << path1[i];
        if (i < path1.size()-1 ) cout << "=> ";
    }
    gotoXY(2,21);
    cout << "Tổng điểm Robot 1 đạt được: " << VIOLET_COLOR_CODE << total_score1;

    // Output Robot 2
    gotoXY(2,23);
	cout << BLUE_COLOR_CODE << "Robot 2 đã đi qua các điểm";
    gotoXY(3,24);
	for (int i = 0; i < path2.size(); i++) {
        cout << path2[i];
        if (i < path2.size()-1 ) cout << "=> ";
    }
	gotoXY(2,26);
    cout << "Tổng điểm Robot 2 đạt được: " << VIOLET_COLOR_CODE << total_score2;

    // In kết quả Robot nào thắng
    Sleep(100);
    if (total_score1 > total_score2) {
        for (int i = 1; i < 15; i++){
            SetColor(i);
            gotoXY(50,27);
            cout << ">> Robot 1 đã thắng!!";
            Sleep(10);
        }
    } else {
        for (int i = 1; i < 15; i++){
            SetColor(i);
            gotoXY(50,27);
            cout << ">> Robot 2 đã thắng!!";
            Sleep(10);
        }
    }
    Sleep(1000);
    cout << RESET_COLOR_CODE;
}
// Chế độ chơi 3: Hai Robot lần lượt di chuyển trên một mê cung
void playModeThree() {
    system("cls");

    gotoXY(0,15);
    cout << "╔";
    Sleep(1);

    for (int i = 1; i < 46; i++ ){
        gotoXY(i,15);
        cout << "═";
        Sleep(1);
    }

    gotoXY(46,15);
    cout << "╝";
    Sleep(1);

    gotoXY(46,14);
    cout << "╔";
    Sleep(1);

    for (int i = 47; i < 73 ; i++ ){
        gotoXY(i,14);
        cout << "═";
        Sleep(1);
    }

    gotoXY(73,14);
    cout << "╗";
    Sleep(1);

    gotoXY(73,15);
    cout << "╚";
    Sleep(1);

    for (int i = 74; i < 119; i++ ){
        gotoXY(i,15);
        cout << "═";
        Sleep(1);
    }

    gotoXY(15,120);
    cout << "╗";
    Sleep(1);

    for (int i = 16; i < 29; i++ ){
        gotoXY(119,i);
        cout << "║";
        Sleep(1);
    }

    gotoXY(119,29);
    cout << "╝";
    Sleep(1);

    for (int i = 118; i > 0; i-- ){
        gotoXY(i,29);
        cout << "═";
        Sleep(1);
    }

    gotoXY(0,29);
    cout << "╚";
    Sleep(1);

    for (int i = 28; i >= 16; i-- ){
        gotoXY(0,i);
        cout << "║";
        Sleep(1);
    }
    Sleep(500);
    gotoXY(48,15);
    cout << GREEN_COLOR_CODE << "Bạn đang chơi ở chế độ 3" << RESET_COLOR_CODE;
    Sleep(500);

    inputFile(robot1);
    inputFile(robot2);
	int start_1x, start_1y; // Vị trí ban đầu của Robot 1
    int start_2x, start_2y; // Vị trí ban đầu của Robot 1
    gotoXY(43,16);
	cout << RED_COLOR_CODE << "Nhập vị trí bắt đầu của Robot 1: ";
	cin >> start_1x >> start_1y;
    gotoXY(43,17);
    cout << BLUE_COLOR_CODE << "Nhập vị trí bắt đầu của Robot 2: ";
	cin >> start_2x >> start_2y;

    moveTwoRobotInOneMatrix(robot1, robot2, path1, path2, start_1x, start_1y, start_2x, start_2y, total_score1, total_score2);
    
    // OutputFile Robot
    outputFile(path1,ouputMode = 1); // Xoá hết dữ liệu và ghi mới
    outputFile(path2,ouputMode = 2); // Ghi đè không mất dữ liệu

    // Output Robot 1
    gotoXY(2,18);
	cout << RED_COLOR_CODE << "Robot 1 đã đi qua các điểm";
    gotoXY(3,19);
	for (int i = 0; i < path1.size(); i++) {
        cout << path1[i];
        if (i < path1.size()-1 ) cout << "=> ";
    }
    gotoXY(2,21);
    cout << "Tổng điểm Robot 1 đạt được: " << VIOLET_COLOR_CODE << total_score1;

    // Output Robot 2
    gotoXY(2,23);
	cout << BLUE_COLOR_CODE << "Robot 2 đã đi qua các điểm";
    gotoXY(3,24);
	for (int i = 0; i < path2.size(); i++) {
        cout << path2[i];
        if (i < path2.size()-1 ) cout << "=> ";
    }
	gotoXY(2,26);
    cout << "Tổng điểm Robot 2 đạt được: " << VIOLET_COLOR_CODE << total_score2;

    // In kết quả Robot nào thắng
    Sleep(100);
    if (total_score1 > total_score2) {
        for (int i = 1; i < 15; i++){
            SetColor(i);
            gotoXY(50,27);
            cout << ">> Robot 1 đã thắng!!";
            Sleep(10);
        }
    } else {
        for (int i = 1; i < 15; i++){
            SetColor(i);
            gotoXY(50,27);
            cout << ">> Robot 2 đã thắng!!";
            Sleep(10);
        }
    }
    Sleep(1000);
    cout << RESET_COLOR_CODE;
}
// Chuyển sang trạng thái hỏi Bạn có muốn tiếp tục hay không?
void frameContinue(bool& isContinue) {
    char key; // Chứa kí tự nhập vào từ bàn phím
    // Vẽ khung
    gotoXY(97,0);
    cout << "╔";
    Sleep(1);
    for (int i = 98; i < 119; i++ ){
        gotoXY(i,0);
        cout << "═";
        Sleep(1);
    }
    gotoXY(0,120);
    cout << "╗";
    Sleep(1);
    for (int i = 1; i < 8; i++ ){
        gotoXY(119,i);
        cout << "║";
        Sleep(1);
    }
    gotoXY(119,8);
    cout << "╝";
    Sleep(1);
    for (int i = 118; i > 97; i-- ){
        gotoXY(i,8);
        cout << "═";
        Sleep(1);
    }
    gotoXY(97,8);
    cout << "╚";
    Sleep(1);
    for (int i = 7; i > 0; i-- ){
        gotoXY(97,i);
        cout << "║";
        Sleep(1);
    }

    // Câu hỏi tiếp tục
    cout << GREEN_COLOR_CODE;
    gotoXY(103,2);
    cout << "Exit now ?";
	gotoXY(101,5);
    cout << RED_COLOR_CODE << ">> " << GREEN_COLOR_CODE << "Continue";
    gotoXY(101,6);
    cout << "   Exit";
	do {
		key = getch();
		if (key == 's' || key == 'S') {
            // clearScreen
            gotoXY(101,5);
            cout << "            ";
            gotoXY(101,6);
            cout << "            ";
			gotoXY(101,5);
    		cout << GREEN_COLOR_CODE << "   Continue";
    		gotoXY(101,6);
    		cout << RED_COLOR_CODE << ">> " << GREEN_COLOR_CODE << "Exit" << RESET_COLOR_CODE;
			isContinue = false;
		} else if (key == 'w' || key =='W') {
            // clearScreen
            gotoXY(101,5);
            cout << "            ";
            gotoXY(101,6);
            cout << "            ";
            
			gotoXY(101,5);
    		cout << RED_COLOR_CODE << ">> " << GREEN_COLOR_CODE << "Continue";
    		gotoXY(101,6);
    		cout << "   Exit" << RESET_COLOR_CODE;
			isContinue = true;
		}
	} while (key != 13);
    cout << RESET_COLOR_CODE;
}
// Trang thứ 2: chọn chế độ chơi
void secondPg(int& option){
    do{
        system("cls");
        for (int i = 34; i <= 88; i++ ){
            gotoXY(i,10);
            cout << "-";
            Sleep(1);
        }
        for (int i = 34; i <= 88; i++ ){
            gotoXY(i,19);
            cout << "-";
            Sleep(1);
        }

        gotoXY(37,12);
        cout << "1. Một Robot di chuyển độc lập" << endl;
        gotoXY(37,13);
        cout << "2. Hai Robot di chuyển độc lập" << endl;
        gotoXY(37,14);
        cout << "3. Hai Robot lần lượt di chuyển trên một mê cung" << endl;
        gotoXY(37,15);
        cout << "4. Thoát khỏi chương trình" << endl;
        gotoXY(37,17);
        cout << "Nhập chế độ bạn muốn chơi: "; 
        cin >> option;

        while (option < 0 && option > 4){
            cout << "Không hợp lệ!! Bạn hãy chọn lại chế độ bạn muốn chơi: "; 
            cin >> option;
        }

        switch (option) {
            case 1: // Chế độ chơi 1: Một Robot di chuyển độc lập trên mê cung
	    		playModeOne();
                break;
            case 2: // Chế độ chơi 2: Hai Robot di chuyển độc lập trên mê cung
                playModeTwo();
                break;
            case 3: // Chế độ chơi 3: Hai Robot lần lượt di chuyển trên một mê cung
                playModeThree();
                break;
            case 4:
                exit;
                break;
            default:
                exit;
                break;
        }

        frameContinue(isContinue);
    } while (isContinue);
    exit;
}



int main(){
    // Setting Font tiếng việt và Title console
    SetConsoleOutputCP(65001);
    SetConsoleTitle(TEXT("Game Robot Find The Path"));
    hideCursor();

    firstPg();
    secondPg(option);

    gotoXY(2,29);
    return 0;
}