//
//  3PuzzleLearning.c
//  3PuzzleLearning
//
//  Created by 山崎 慎太郎 on 2014/11/11.
//  Copyright (c) 2014年 山崎 慎太郎. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define STATE_SIZE 12
#define ACTION_SIZE 2
#define BOLTZMANN_TEMPERATURE 0.1
#define TRY_NUM 100
#define MAX_COUNT 100

typedef struct {
    /** 行動後の状態のindex */
    int to;
    /** 行動におけるq値 */
    double q;
} ACTION;

typedef struct {
    int index;
    /** パネルの状態 */
    int panel[4];
    /** とりうる行動 */
    ACTION action[2];
} STATE;

/** 状態 */
STATE state[STATE_SIZE] = {
    {0, {1, 2, 3, 0}, {{11, 0}, {1, 0}}},
    {1, {1, 2, 0, 3}, {{0, 0}, {2, 0}}},
    {2, {0, 2, 1, 3}, {{1, 0}, {3, 0}}},
    {3, {2, 0, 1, 3}, {{2, 0}, {4, 0}}},
    {4, {2, 3, 1, 0}, {{3, 0}, {5, 0}}},
    {5, {2, 3, 0, 1}, {{4, 0}, {6, 0}}},
    {6, {0, 3, 2, 1}, {{5, 0}, {7, 0}}},
    {7, {3, 0, 2, 1}, {{6, 0}, {8, 0}}},
    {8, {3, 1, 2, 0}, {{7, 0}, {9, 0}}},
    {9, {3, 1, 0, 2}, {{8, 0}, {10, 0}}},
    {10, {0, 1, 3, 2}, {{9, 0}, {11, 0}}},
    {11, {1, 0, 3, 2}, {{10, 0}, {0, 0}}}
};

/** ボルツマン法 */
double boltzmannMethod(int s, int a) {
    /** ボルツマン温度 */
    double t =  BOLTZMANN_TEMPERATURE;
    
    double n = exp(state[s].action[a].q / t);
    
    int i;
    double d = 0;
    for (i = 0; i < 2; i++) {
        d += exp(state[s].action[i].q / t);
    }
    
    return n / d;
}

/** 行動選択 */
int selectAction(int s) {
    int a;
    
//    if (state[s].action[0].q > state[s].action[1].q) {
//        a = 0;
//    } else if (state[s].action[0].q < state[s].action[1].q){
//        a = 1;
//    } else {
//        a = rand() % 2;
//    }
//    
//    return a;

    a = rand() % 100;
    
//    printf("%f\n", boltzmannMethod(s, 0) * 100);

    
    if (a < boltzmannMethod(s, 0) * 100) {
        return 0;
    } else {
        return 1;
    }
    
}

/** その状態におけるq値の最大値 */
double getMaxQValue(int s) {
    double max = state[s].action[0].q;
    int i;
    for (i = 1; i < ACTION_SIZE; i++) {
        if (max < state[s].action[i].q) {
            max = state[s].action[i].q;
        }
    }
    
    return max;
}

/** q値の計算 */
void calcQValue(int s, int a) {
    /** 学習率 */
    static double alpha = 0.1;
    /** 割引率 */
    static double gamma = 0.9;
    /** 報酬 */
    double r;
    
    if (state[s].action[a].to == 0) {
        r = 1.0;
    } else {
        r = 0.0;
    }
    
    /** q値の変化量 */
    double dq;
    dq = alpha * (r + gamma * getMaxQValue(state[s].action[a].to) - state[s].action[a].q);
    
    state[s].action[a].q += dq;
}


int main(int argc, const char * argv[]) {
    int i;
    int count;
    
    if (argc < 2 || atoi(argv[1]) < 0 || atoi(argv[1]) > 11) {
        printf("You should set number for initialize from 0 to 11\n");
        return 1;
    }
    
    /** 実行結果をファイルに出力 */
    FILE *datafile;
    
    // 乱数設定
    srand((unsigned) time(NULL));
    
    datafile = fopen("data.csv", "w");
    if(datafile == NULL){
        printf( "Cannot open file\n");
        return 1;
    }
    fprintf(datafile, "%d,%d\n", 0, 0);
    
    /** 状態の初期値 */
    const int initNum = atoi(argv[1]);
    /** 現在の状態 */
    int current;
    /** 選択した行動 */
    int selectedAct = 0;
    
    printf("%2d | %d %d %d %d |\n", i, state[initNum].panel[0], state[initNum].panel[1], state[initNum].panel[2], state[initNum].panel[3]);
    
    for (i = 0; i < TRY_NUM; i++) {
        current = initNum;
        printf("%d----------------------------\n", i + 1);
        for (count = 1; count <= MAX_COUNT; count++) {
            selectedAct = selectAction(current);
            
            calcQValue(current, selectedAct);
            
            printf("%2d | %d %d %d %d |q=%f\n", count, state[current].panel[0], state[current].panel[1], state[current].panel[2], state[current].panel[3], state[current].action[selectedAct].q);
            
            current = state[current].action[selectedAct].to;
            
            if (current == 0) {
                break;
            }
        }
        printf("count = %d\n", count);
        fprintf(datafile, "%d,%d\n", i + 1, count);
    }
    
    fclose(datafile);
    
    for (i = 0; i < STATE_SIZE; i++) {
        printf("%2d | %d %d %d %d | to %2d q %f : to %2d q %f\n", i, state[i].panel[0], state[i].panel[1], state[i].panel[2], state[i].panel[3], state[i].action[0].to, state[i].action[0].q, state[i].action[1].to, state[i].action[1].q);
    }
    
    return 0;
}
