#include "DxLib.h"
#include "Header/Object.h"
#define UP 1
#define RIGHT 2
#define DOWN 3
#define LEFT 4

void LogicMain();

int RobotJobFlag;

bool ForkFlag; //FALSE:分かれ道を一度通った。TRUE:まだ一本道

class RobotStaticForm {
public:
	RobotStaticForm(); //コンストラクタ
	int RobotHere[2];
	int RobotDirection;
	void RobotMove(int Direction);
	int SetDirection(int Direction); //ロボットの向きを基準に向きを指定
};

class mapForm {
public:
	int GivenWallData[2][8][8]; //下参照
	int AquiredWallData[2][8][8]; //0:未検知　1:壁有り　2:壁無し　3:壁なし(行き止まり)  [0:上　1:右][x軸][y軸]
	bool SeparatedPath[8][8]; //TRUE 分かれ道あり　False：分かれ道なし
	int MapNumber[8][8]; //番号付けで利用
public:
	mapForm();
	RobotStaticForm RobotStatic;
	void AquireWallData();
	void SetAquiredWallData(int Direction, int WallType);
	void SetAquiredWallData(int x, int y, int WallType, int);
	void CheckAroundRobot(bool WallData[4]);
	int CheckAroundRobot(int Direction);
	int CheckAroundRobot();
	int CheckWall(int x, int y, int Direction);
	void SetNextMove();
	void AquireSeparatedPath();
	void SetPath(int GoalX, int GoalY);
};

class DrawForm {
private:
	int RobotHandle;
	int WallHandle[4];
	int BackGroundHandle;
	int ButtonRun[2];
	int ButtonSearch[2];
public:
	DrawForm();
	void DrawMain();

};
/*ループメインで考えて行く
 壁は常に右上のみを変数に記録 番号は上上がり、右上がり（二次元座標と同じ）この仕様上、描画は少し手間取る。
 */
mapForm MapData;
int main() {

	ChangeWindowMode(true);
	SetWindowSizeChangeEnableFlag(TRUE);
	SetUseMenuFlag(TRUE);
	SetAlwaysRunFlag(TRUE);
	SetGraphMode(1280, 960, 16);
	if (DxLib_Init() == -1) // ＤＸライブラリ初期化処理
			{
		return -1;			// エラーが起きたら直ちに終了
	}
	DrawForm Draw;

	ForkFlag = true;
	while (CheckHitKey(KEY_INPUT_ESCAPE) == 0 && !ProcessMessage()) {

		LogicMain();

		Draw.DrawMain();

	}
	return 0;
}
DrawForm::DrawForm() {
	RobotHandle = LoadGraph("images/robot.png");
	WallHandle[0] = LoadGraph("images/bar.png");
	WallHandle[1] = LoadGraph("images/bar2.png");
	WallHandle[2] = LoadGraph("images/bar3.png");
	WallHandle[3] = LoadGraph("images/bar4.png");
	BackGroundHandle = LoadGraph("images/BG.png");
	ButtonRun[0] = LoadGraph("images/RunButton.png");
	ButtonRun[1] = LoadGraph("images/RunButtonlight.png");
	ButtonSearch[0] = LoadGraph("images/SearchButton.png");
	ButtonSearch[1] = LoadGraph("images/SearchButtonlight.png");
}

int mapForm::CheckWall(int x, int y, int Direction) {
	if (Direction == UP) { //上
		return AquiredWallData[0][x][y];
	} else if (Direction == RIGHT) { //右
		return AquiredWallData[1][x][y];
	} else if (Direction == DOWN) { //下
		if (x == 0) {
			return 1;
		} else {
			return AquiredWallData[0][x][y - 1];
		}
	} else if (Direction == LEFT) { //左
		if (y == 0) {
			return 1;
		} else {
			return AquiredWallData[1][x - 1][y];
		}
	} else {

		return -1;
	}
}

void mapForm::SetPath(int GoalX, int GoalY) { //足立法
	int SimulateAxis[2] = { RobotStatic.RobotHere[0], RobotStatic.RobotHere[1] };
	for (int i = 0; i < 3/*走行回数*/; i++) {
		while (SimulateAxis[0] != GoalX && SimulateAxis[1] != GoalY) {
			if (SimulateAxis[0] <= GoalX ) { //左下
				if(SimulateAxis[1] <= GoalY)
				if (CheckWall(SimulateAxis[0], SimulateAxis[1], RIGHT) == 2) {
					MapNumber[GoalX + 1][GoalY] = MapNumber[GoalX][GoalY] + 1;
				} else if (CheckWall(SimulateAxis[0], SimulateAxis[1], UP)
						== 2) {
					MapNumber[GoalX][GoalY + 1] = MapNumber[GoalX][GoalY] + 1;
				} else if (CheckWall(SimulateAxis[0], SimulateAxis[1], LEFT)
						== 2) {
					MapNumber[GoalX - 1][GoalY] = MapNumber[GoalX][GoalY] + 1;
				} else if (CheckWall(SimulateAxis[0], SimulateAxis[1], RIGHT)
						== 2) {
					MapNumber[GoalX][GoalY - 1] = MapNumber[GoalX][GoalY] + 1;
				}
				}
			}
		}
	}

}
void DrawForm::DrawMain() {
	// 画面を初期化
	ClearDrawScreen();

	// 描画先画面を裏にする
	SetDrawScreen( DX_SCREEN_BACK);

	DrawGraph(0, 0, BackGroundHandle, FALSE);
	for (int x = 0; x <= 9; x++) {
		for (int y = 0; y <= 9; y++) {
			if (x == 0 ||x==9) {
				DrawGraph(20 + x * 50, 20 + y * 50, WallHandle[0],
				FALSE);
			} else {
				DrawGraph(20 + x * 50, 20 + y * 50, WallHandle[1],
				FALSE);
			}
			if (y == 0||y==9) {
				DrawRotaGraph(45 + x * 50, 20 + y * 50, 1.0f, 3.1415926535 / 2,
						WallHandle[0], FALSE);
			} else {
				DrawRotaGraph(45 + x * 50, 20 + y * 50, 1.0f, 3.1415926535 / 2,
						WallHandle[1], FALSE);
			}
			if (x != 0 && y != 0 &&x!=9&&y!=9) {
				if (MapData.AquiredWallData[0][x][y] == 1) {
					DrawRotaGraph(45 + x * 50, 20 + y * 50, 1.0f,
							3.1415926535 / 2, WallHandle[0], FALSE);
				} else {
					DrawRotaGraph(45 + x * 50, 20 + y * 50, 1.0f,
							3.1415926535 / 2, WallHandle[1], FALSE);
				}
			}
			/*if (LogicVar.mapWall[0][x][y] == 1) {
			 DrawGraph(20 + x * 50, 20 + y * 50, WallHandle[2],
			 FALSE);
			 } else if (LogicVar.mapWall[0][x][y] == 2) {
			 DrawGraph(20 + x * 50, 20 + y * 50, WallHandle[3],
			 FALSE);
			 } else if (map[0][x][y] == 1) {
			 DrawGraph(20 + x * 50, 20 + y * 50, WallHandle[0],
			 FALSE);
			 } else {
			 DrawGraph(20 + x * 50, 20 + y * 50, WallHandle[1],
			 FALSE);
			 }*/
		}
	}
	/*for (int x = 0; x < 16; x++) {
	 for (int y = 0; y < 17; y++) {
	 if (LogicVar.mapWall[1][x][y] == 1) {
	 DrawRotaGraph(45 + x * 50, 20 + y * 50, 1.0f, 3.1415926535 / 2,
	 BarHandle[2], FALSE);
	 } else if (LogicVar.mapWall[1][x][y] == 2) {
	 DrawRotaGraph(45 + x * 50, 20 + y * 50, 1.0f, 3.1415926435 / 2,
	 BarHandle[3], FALSE);
	 } else if (map[1][x][y] == 1) {
	 DrawRotaGraph(45 + x * 50, 20 + y * 50, 1.0f, 3.1415926535 / 2,
	 BarHandle[0], FALSE);
	 } else {
	 DrawRotaGraph(45 + x * 50, 20 + y * 50, 1.0f, 3.1415926435 / 2,
	 BarHandle[1], FALSE);
	 }
	 }
	 }*/
	ScreenFlip();
}
void LogicMain() {

	if (RobotJobFlag == 1) { //探索  右に向かって、上に行く方針
		MapData.AquireWallData();
		MapData.SetNextMove();
		//if(RobotStatic.RobotHere)
	} else if (RobotJobFlag == 2) { //番号付け

	} else if (RobotJobFlag == 3) { //走破

	}

}
void mapForm::AquireSeparatedPath() {
	CheckAroundRobot(
			SeparatedPath[RobotStatic.RobotHere[0]][RobotStatic.RobotHere[1]]);

}

void RobotStaticForm::RobotMove(int Direction) {
	if (Direction == UP && RobotHere[1] < 8) {
		RobotHere[1]++;
	} else if (Direction == RIGHT && RobotHere[0] < 8) {
		RobotHere[0]++;
	} else if (Direction == DOWN && RobotHere[1] > 0) {
		RobotHere[1]--;
	} else if (Direction == LEFT && RobotHere[0] > 0) {
		RobotHere[0]--;
	} else {
		printf("unexpected error");
	}
}

void mapForm::SetAquiredWallData(int Direction, int WallType) { //行き止まり設定用
	if (Direction == UP) {
		AquiredWallData[0][RobotStatic.RobotHere[0]][RobotStatic.RobotHere[1]] =
				WallType;
	} else if (Direction == RIGHT) {
		AquiredWallData[1][RobotStatic.RobotHere[0]][RobotStatic.RobotHere[1]] =
				WallType;
	} else if (Direction == DOWN && RobotStatic.RobotHere[0] != 0) {
		AquiredWallData[0][RobotStatic.RobotHere[0]][RobotStatic.RobotHere[1]
				- 1] = WallType;
	} else if (Direction == LEFT & RobotStatic.RobotHere[1] != 0) {
		AquiredWallData[1][RobotStatic.RobotHere[0] - 1][RobotStatic.RobotHere[1]] =
				WallType;
	}
}
void mapForm::SetAquiredWallData(int x, int y, int WallType, int) { //おそらくデバッグ用　座標で管理
//未実装
}
void mapForm::AquireWallData() {
	int x = RobotStatic.RobotHere[0];
	int y = RobotStatic.RobotHere[1];
	AquiredWallData[0][x][y] = GivenWallData[0][x][y];
	AquiredWallData[1][x][y] = GivenWallData[1][x][y];
}

int RobotStaticForm::SetDirection(int Direction) { //ロボット視点からの方向を返す

	return (Direction + RobotDirection - 1) % 4;
}

RobotStaticForm::RobotStaticForm() {
	RobotHere[0] = 0;
	RobotHere[1] = 0;
	RobotDirection = 1;
}
mapForm::mapForm() {
	//ファイルの入出力

}
int mapForm::CheckAroundRobot(int Direction) { //ロボットの周囲を確認する関数、一度取得したWallDataを参照している。
	if (Direction == UP) { //上
		return AquiredWallData[0][RobotStatic.RobotHere[0]][RobotStatic.RobotHere[1]];
	} else if (Direction == RIGHT) { //右
		return AquiredWallData[1][RobotStatic.RobotHere[0]][RobotStatic.RobotHere[1]];
	} else if (Direction == DOWN) { //下
		if (RobotStatic.RobotHere[1] == 0) {
			return 1;
		} else {
			return AquiredWallData[0][RobotStatic.RobotHere[0]][RobotStatic.RobotHere[1]
					- 1];
		}
	} else if (Direction == LEFT) { //左
		if (RobotStatic.RobotHere[1] == 0) {
			return 1;
		} else {
			return AquiredWallData[1][RobotStatic.RobotHere[0] - 1][RobotStatic.RobotHere[1]];
		}
	} else {

		return -1;
	}
}

int mapForm::CheckAroundRobot() { //周りの壁の枚数を調べる
	int Wall[4] = { 0, 0, 0, 0 };
	for (int Direction = 1; Direction++; Direction <= 4) {
		if (CheckAroundRobot(Direction) == 2
				|| CheckAroundRobot(Direction) == 3)
			Wall[Direction - 1] = 1;
	}
	return Wall[0] + Wall[1] + Wall[2] + Wall[3];
}

void mapForm::CheckAroundRobot(bool WallData[4]) { //周囲のどこの壁が開いているかを配列に入れる空いている向きのデータを前から代入する
//WallData[1]=TRUE; で代入できるらしい。
	int i = 0;
	while (i <= 3) {
		WallData[i] = FALSE;
		i++;
	}
	if (CheckAroundRobot(UP) == 1 || CheckAroundRobot(UP) == 3) {
		WallData[UP] = TRUE;

	}
	if (CheckAroundRobot(RIGHT) == 1 || CheckAroundRobot(RIGHT) == 3) {
		WallData[RIGHT] = TRUE;

	}
	if (CheckAroundRobot(DOWN) == 1 || CheckAroundRobot(DOWN) == 3) {
		WallData[DOWN] = TRUE;

	}
	if (CheckAroundRobot(LEFT) == 1 || CheckAroundRobot(LEFT) == 3) {
		WallData[LEFT] = TRUE;

	}
}
void mapForm::SetNextMove() {
	if (CheckAroundRobot() == 3 && ForkFlag == TRUE) { //道がひとつのみ&&分かれ道を一度通過(Start地点封鎖防止用)
		if (CheckAroundRobot(UP) == 2)
			RobotStatic.RobotMove(UP);
		if (CheckAroundRobot(RIGHT) == 2)
			RobotStatic.RobotMove(RIGHT);
		if (CheckAroundRobot(DOWN) == 2)
			RobotStatic.RobotMove(DOWN);
		if (CheckAroundRobot(LEFT) == 2)
			RobotStatic.RobotMove(LEFT);
	} else {
		ForkFlag = FALSE;
		switch (RobotStatic.RobotDirection) {
		case 1:

			if (CheckAroundRobot(RIGHT) == 2) { //壁がない方向に向かう（優先順位は上を参照）
				RobotStatic.RobotMove(RIGHT);
			} else if (CheckAroundRobot(UP) == 2) {
				RobotStatic.RobotMove(UP);
			} else if (CheckAroundRobot(LEFT) == 2) {
				RobotStatic.RobotMove(LEFT);
			} else { //行き止まりの場合は行き止まりと設定して帰る
				SetAquiredWallData(DOWN, 3);
				RobotStatic.RobotMove(DOWN);
			}
			break;
		case 2:
			if (CheckAroundRobot(RIGHT) == 2) {
				RobotStatic.RobotMove(RIGHT);
			} else if (CheckAroundRobot(UP) == 2) {
				RobotStatic.RobotMove(UP);
			} else if (CheckAroundRobot(DOWN) == 2) {
				RobotStatic.RobotMove(DOWN);
			} else {
				RobotStatic.RobotMove(LEFT);
				SetAquiredWallData(LEFT, 3);
			}
			break;
		case 3:
			if (CheckAroundRobot(RIGHT) == 2) {
				RobotStatic.RobotMove(RIGHT);
			} else if (CheckAroundRobot(DOWN) == 2) {
				RobotStatic.RobotMove(DOWN);
			} else if (CheckAroundRobot(LEFT) == 2) {
				RobotStatic.RobotMove(LEFT);
			} else {
				RobotStatic.RobotMove(UP);
				SetAquiredWallData(UP, 3);
			}
			break;
		case 4:
			if (CheckAroundRobot(UP) == 2) {
				RobotStatic.RobotMove(UP);
			} else if (CheckAroundRobot(DOWN) == 2) {
				RobotStatic.RobotMove(DOWN);
			} else if (CheckAroundRobot(LEFT) == 2) {
				RobotStatic.RobotMove(LEFT);
			} else {
				RobotStatic.RobotMove(RIGHT);
				SetAquiredWallData(RIGHT, 3);
			}
			break;
			deafult: printf("unexpected error");
			break;
		}

	}
}
