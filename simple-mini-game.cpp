#include "DxLib.h"
#include <stdlib.h>
#include <math.h>

struct Enemy {
    float x;
    int y;
    float speed;
    bool isExist;
};

int main() {
    ChangeWindowMode(TRUE);
    SetGraphMode(960, 540, 32);
    if (DxLib_Init() == -1) return -1;
    SetDrawScreen(DX_SCREEN_BACK);

    int x = 100;
    int y = 300;
    int charaSize = 130;

    int hp = 150;
    int skyHeight = 540 * 2 / 7;

    int invincibleTimer = 0;

    float grassX = 0.0f;
    float cloudX = 0.0f;

    int charaGraph = LoadGraph("chara.png");

    Enemy enemies[3];
    for (int i = 0; i < 3; i++) {
        enemies[i].x = 960.0f + (i * 300);
        enemies[i].y = rand() % (540 - charaSize - skyHeight) + skyHeight;
        enemies[i].speed = (rand() % 16 / 10.0f) + 1.0f;
        enemies[i].isExist = true;
    }

    while (ProcessMessage() == 0 && ScreenFlip() == 0 && ClearDrawScreen() == 0) {

        if (CheckHitKey(KEY_INPUT_LEFT) == 1) x -= 3;
        if (CheckHitKey(KEY_INPUT_RIGHT) == 1) x += 3;
        if (CheckHitKey(KEY_INPUT_UP) == 1) y -= 3;
        if (CheckHitKey(KEY_INPUT_DOWN) == 1) y += 3;

        // 見えない壁の処理
        if (x < 0) x = 0;
        if (x > 750 - charaSize) x = 750 - charaSize;
        if (y < skyHeight - 30) y = skyHeight - 30;
        if (y > 540 - charaSize) y = 540 - charaSize;

        // 背景スクロールの計算
        grassX -= 1.0f;
        cloudX -= 0.2f;
        if (grassX < -60.0f)  grassX = 0.0f;
        if (cloudX < -120.0f) cloudX = 0.0f;

        // 毎フレーム1ずつタイマーを減らす
        if (invincibleTimer > 0) {
            invincibleTimer--;
        }

        // 3体の敵の移動と、出現位置の重複防止
        for (int i = 0; i < 3; i++) {
            if (enemies[i].isExist == true) {
                enemies[i].x -= enemies[i].speed;

                // キャラクターと敵の当たり判定
                if (invincibleTimer == 0) {
                    int currentCharaSize = 90 + (y / 8);
                    int enemyBaseSize = 35;
                    int enemySize = enemyBaseSize + (enemies[i].y / 10);

                    bool hitX = (x < enemies[i].x + enemySize) && (enemies[i].x < x + currentCharaSize);
                    bool hitY = abs(y - enemies[i].y) < 30;

                    if (hitX && hitY) {
                        hp -= 30;
                        invincibleTimer = 240; // 240フレームの無敵時間
                        break; // 当たったら残りの敵の判定をせず、ループを抜ける
                    }
                }

                if (enemies[i].x < -100) {
                    float newX = 960.0f;
                    int newY = rand() % (540 - charaSize - skyHeight) + skyHeight;

                    for (int j = 0; j < 3; j++) {
                        if (i == j) continue;
                        if (abs(newY - enemies[j].y) < 50 && fabsf(newX - enemies[j].x) < 200.0f) {
                            newX += 300.0f;
                        }
                    }

                    enemies[i].x = newX;
                    enemies[i].y = newY;
                    enemies[i].speed = (rand() % 16 / 10.0f) + 1.0f;
                }
            }
        }

        // --- 描画処理 ---
        DrawBox(0, 0, 960, skyHeight, GetColor(135, 206, 235), TRUE);
        DrawBox(0, skyHeight, 960, 540, GetColor(80, 80, 80), TRUE);

        for (int i = 0; i < 10; i++) {
            int drawCloudX = (int)cloudX + (i * 120);
            DrawCircle(drawCloudX, skyHeight - 40, 20, GetColor(255, 255, 255), TRUE);
            DrawCircle(drawCloudX + 15, skyHeight - 45, 25, GetColor(255, 255, 255), TRUE);
            DrawCircle(drawCloudX + 30, skyHeight - 40, 20, GetColor(255, 255, 255), TRUE);
        }

        for (int i = 0; i < 18; i++) {
            int drawGrassX = (int)grassX + (i * 60);
            DrawTriangle(drawGrassX, skyHeight, drawGrassX + 30, skyHeight - 20, drawGrassX + 60, skyHeight, GetColor(34, 139, 34), TRUE);
        }

        // 3体の敵をまとめて描画
        for (int i = 0; i < 3; i++) {
            if (enemies[i].isExist == true) {
                int enemyBaseSize = 35;
                int enemySize = enemyBaseSize + (enemies[i].y / 10);
                int ex = (int)enemies[i].x;
                DrawBox(ex, enemies[i].y, ex + enemySize, enemies[i].y + enemySize, GetColor(255, 0, 0), TRUE);
            }
        }

        // キャラクターの描画
        int currentCharaSize = 90 + (y / 8);
        if (invincibleTimer == 0 || (invincibleTimer % 16 < 8)) {
            DrawExtendGraph(x, y, x + currentCharaSize, y + currentCharaSize, charaGraph, TRUE);
        }

        // UI表示
        DrawFormatString(20, 20, GetColor(255, 0, 0), "HP : %d", hp);
        unsigned int barColor = GetColor(255, 255, 255);
        if (hp >= 91)       barColor = GetColor(0, 255, 0);
        else if (hp >= 31)  barColor = GetColor(255, 255, 0);
        else                barColor = GetColor(255, 0, 0);

        DrawBox(20, 45, 170, 65, GetColor(0, 0, 0), FALSE);
        if (hp > 0) {
            DrawBox(20, 45, 20 + hp, 65, barColor, TRUE);
        }
    }

    DxLib_End();
    return 0;
}