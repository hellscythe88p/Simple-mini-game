#include "DxLib.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>

enum GameScene {
    SCENE_TITLE,
    SCENE_PLAY,
    SCENE_GAMEOVER
};

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

    GameScene scene = SCENE_TITLE;

    int x = 100;
    int y = 300;
    int charaSize = 130;
    int hp = 150;
    int skyHeight = 540 * 2 / 7;
    int invincibleTimer = 0;
    float grassX = 0.0f;
    float cloudX = 0.0f;

    int selectIndex = 0;
    bool wasReturnPressed = false;
    bool wasUpPressed = false;
    bool wasDownPressed = false;

    int charaGraph = LoadGraph("chara.png");

    int currentCharaSize = 0;
    unsigned int barColor = 0;
    int titleWidth = 0;
    int pushEnterWidth = 0;
    int gameOverWidth = 0;
    int hpWidth = 0;
    char hpText[32] = "";

    Enemy enemies[3];
    auto InitGame = [&]() {
        x = 100;
        y = 300;
        hp = 150;
        invincibleTimer = 0;
        grassX = 0.0f;
        cloudX = 0.0f;
        selectIndex = 0;
        for (int i = 0; i < 3; i++) {
            enemies[i].x = 960.0f + (i * 300);
            enemies[i].y = rand() % (540 - charaSize - skyHeight) + skyHeight;
            enemies[i].speed = (rand() % 16 / 10.0f) + 1.0f;
            enemies[i].isExist = true;
        }
        };

    InitGame();

    while (ProcessMessage() == 0 && ScreenFlip() == 0 && ClearDrawScreen() == 0) {

        switch (scene) {

            // タイトル画面の処理
        case SCENE_TITLE:
            DrawBox(0, 0, 960, skyHeight, GetColor(135, 206, 235), TRUE);
            DrawBox(0, skyHeight, 960, 540, GetColor(80, 80, 80), TRUE);

            SetFontSize(64);
            SetFontThickness(6);
            titleWidth = GetDrawStringWidth("NENECH QUEST", (int)strlen("NENECH QUEST"));
            DrawString((960 - titleWidth) / 2, 150, "NENECH QUEST", GetColor(255, 215, 0));

            SetFontSize(24);
            SetFontThickness(3);
            pushEnterWidth = GetDrawStringWidth("Press ENTER to Start", (int)strlen("Press ENTER to Start"));
            DrawString((960 - pushEnterWidth) / 2, 350, "Press ENTER to Start", GetColor(255, 255, 255));

            if (CheckHitKey(KEY_INPUT_RETURN) == 1) {
                if (!wasReturnPressed) {
                    InitGame();
                    scene = SCENE_PLAY;
                }
                wasReturnPressed = true;
            }
            else {
                wasReturnPressed = false;
            }
            break;

            // ゲーム本編処理
        case SCENE_PLAY:
            if (CheckHitKey(KEY_INPUT_LEFT) == 1) x -= 3;
            if (CheckHitKey(KEY_INPUT_RIGHT) == 1) x += 3;
            if (CheckHitKey(KEY_INPUT_UP) == 1) y -= 3;
            if (CheckHitKey(KEY_INPUT_DOWN) == 1) y += 3;

            if (x < 0) x = 0;
            if (x > 750 - charaSize) x = 750 - charaSize;
            if (y < skyHeight - 30) y = skyHeight - 30;
            if (y > 540 - charaSize) y = 540 - charaSize;

            grassX -= 1.0f;
            cloudX -= 0.2f;
            if (grassX < -60.0f)  grassX = 0.0f;
            if (cloudX < -120.0f) cloudX = 0.0f;

            if (invincibleTimer > 0) {
                invincibleTimer--;
            }

            for (int i = 0; i < 3; i++) {
                if (enemies[i].isExist == true) {
                    enemies[i].x -= enemies[i].speed;

                    if (invincibleTimer == 0) {
                        currentCharaSize = 90 + (y / 8);
                        int enemyBaseSize = 35;
                        int enemySize = enemyBaseSize + (enemies[i].y / 10);

                        bool hitX = (x < enemies[i].x + enemySize) && (enemies[i].x < x + currentCharaSize);
                        bool hitY = abs(y - enemies[i].y) < 30;

                        if (hitX && hitY) {
                            hp -= 30;

                            if (hp <= 0) {
                                hp = 0;
                                scene = SCENE_GAMEOVER;
                            }
                            else {
                                invincibleTimer = 360;
                            }
                            break;
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

            // 描画
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

            for (int i = 0; i < 3; i++) {
                if (enemies[i].isExist == true) {
                    int enemyBaseSize = 35;
                    int enemySize = enemyBaseSize + (enemies[i].y / 10);
                    int ex = (int)enemies[i].x;
                    DrawBox(ex, enemies[i].y, ex + enemySize, enemies[i].y + enemySize, GetColor(255, 0, 0), TRUE);
                }
            }

            currentCharaSize = 90 + (y / 8);
            if (invincibleTimer == 0 || (invincibleTimer % 16 < 8)) {
                DrawExtendGraph(x, y, x + currentCharaSize, y + currentCharaSize, charaGraph, TRUE);
            }

            SetFontSize(16);
            DrawFormatString(20, 20, GetColor(255, 0, 0), "HP : %d", hp);

            barColor = GetColor(255, 255, 255);
            if (hp >= 91)       barColor = GetColor(0, 255, 0);
            else if (hp >= 31)  barColor = GetColor(255, 255, 0);
            else                barColor = GetColor(255, 0, 0);

            DrawBox(20, 45, 170, 65, GetColor(0, 0, 0), FALSE);
            DrawBox(20, 45, 20 + hp, 65, barColor, TRUE);
            break;

            // ゲームオーバー処理
        case SCENE_GAMEOVER:
            DrawBox(0, 0, 960, skyHeight, GetColor(135, 206, 235), TRUE);
            DrawBox(0, skyHeight, 960, 540, GetColor(80, 80, 80), TRUE);

            for (int i = 0; i < 10; i++) {
                int drawCloudX = (int)cloudX + (i * 120);
                DrawCircle(drawCloudX, skyHeight - 40, 20, GetColor(255, 255, 255), TRUE);
            }
            for (int i = 0; i < 18; i++) {
                int drawGrassX = (int)grassX + (i * 60);
                DrawTriangle(drawGrassX, skyHeight, drawGrassX + 30, skyHeight - 20, drawGrassX + 60, skyHeight, GetColor(34, 139, 34), TRUE);
            }
            for (int i = 0; i < 3; i++) {
                int enemyBaseSize = 35;
                int enemySize = enemyBaseSize + (enemies[i].y / 10);
                DrawBox((int)enemies[i].x, enemies[i].y, (int)enemies[i].x + enemySize, enemies[i].y + enemySize, GetColor(255, 0, 0), TRUE);
            }
            DrawExtendGraph(x, y, x + (90 + (y / 8)), y + (90 + (y / 8)), charaGraph, TRUE);

            SetFontSize(86);
            SetFontThickness(6);
            gameOverWidth = GetDrawStringWidth("GAME OVER", (int)strlen("GAME OVER"));
            DrawString((960 - gameOverWidth) / 2, 130, "GAME OVER", GetColor(255, 0, 0));

            SetFontSize(36);
            SetFontThickness(5);
            sprintf_s(hpText, "HP : %d", hp);
            hpWidth = GetDrawStringWidth(hpText, (int)strlen(hpText));
            DrawString((960 - hpWidth) / 2, 230, hpText, GetColor(0, 50, 255));

            // 選択処理
            if (CheckHitKey(KEY_INPUT_UP) == 1) {
                if (!wasUpPressed) selectIndex = 0;
                wasUpPressed = true;
            }
            else { wasUpPressed = false; }

            if (CheckHitKey(KEY_INPUT_DOWN) == 1) {
                if (!wasDownPressed) selectIndex = 1;
                wasDownPressed = true;
            }
            else { wasDownPressed = false; }

            SetFontSize(28);
            SetFontThickness(4);

            unsigned int colorRetry = (selectIndex == 0) ? GetColor(255, 255, 0) : GetColor(180, 180, 180);
            unsigned int colorTitle = (selectIndex == 1) ? GetColor(255, 255, 0) : GetColor(180, 180, 180);

            DrawFormatString(360, 310, colorRetry, "%s もう一度プレイする", (selectIndex == 0 ? ">" : "  "));
            DrawFormatString(360, 360, colorTitle, "%s タイトルに戻る", (selectIndex == 1 ? ">" : "  "));

            if (CheckHitKey(KEY_INPUT_RETURN) == 1) {
                if (!wasReturnPressed) {
                    if (selectIndex == 0) {
                        InitGame();
                        scene = SCENE_PLAY;
                    }
                    else {
                        scene = SCENE_TITLE;
                    }
                }
                wasReturnPressed = true;
            }
            else {
                wasReturnPressed = false;
            }
            break;
        }
    }

    DxLib_End();
    return 0;
}