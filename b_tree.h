#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>


#define M 5
#define max_children M
#define max_keys max_children-1
#define min_keys (int)(ceil(M/2.0))-1 // 최소 키개수 구하는 식

struct BTreeNode{
    bool leaf; // leaf여부 
    int key[max_keys+1]; // key를 담을 배열
    int cnt_key; // 키 개수 확인용
    struct BTreeNode* child[max_children+1]; // 자식 포인터배열 (노드들이 배열로)
    int cnt_child; // 자식 개수 확인용
};

int deleteValFromNode(int val, struct BTreeNode* node);

struct BTreeNode* root; // root 노드 기본 설정(포인터로)