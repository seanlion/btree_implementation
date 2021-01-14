#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define M 3
#define max_children M
#define max_keys max_children-1
#define min_keys (int)(ceil(M/2.0))-1 // 최소 키개수 구하는 식

struct BTreeNode {
    bool leaf; // leaf여부 
    bool end_leaf;
    int key[max_keys+1]; // key를 담을 배열
    int cnt_key; // 키 개수 확인용
    struct BTreeNode* child[max_children+1]; // 자식 포인터배열 (노드들이 배열로)
    int child_data[max_keys+1];
    struct BTreeNode* next;
    int cnt_child; // 자식 개수 확인용
};
struct BTreeNode* root; // root 노드 기본 설정(포인터로)
struct BTreeNode* createNode(int val);

/*SEARCH******************************************************************************************************/
int searchNode(struct BTreeNode* node, int val) {			// TO DO:: 이진탐색
	if (!node) { 											// empty tree!
		printf("Empty tree!!\n");
		return 0;
	}
	struct BTreeNode* level = node;							// root부터 leaf까지 탐색
	while (1) {
		int pos;
		for (pos = 0; pos < level->cnt_key; pos++) {
			if (val == level->key[pos]) {					// 찾으면 리턴 1
				printf("%d exist!!\n", val);
				return 1;
			}
			else if (val < level->key[pos]) {
				break;
			}
		}
		if (level->leaf) break;
		level = level->child[pos];
	}
	printf("%d not exist!!\n", val);							// leaf 까지와서도 못찾으면 리턴 0
	return 0;
}


// 포인터로 노드를 만들었고, 그걸 createNode라고 명명했다. 받아오는 값은 '넣는 데이터'.
struct BTreeNode* createNode(int val){ 
    struct BTreeNode* newNode; // 새로운 node 구조체 선언
    newNode = (struct BTreeNode*)malloc(sizeof(struct BTreeNode)); // node에 동적할당
    newNode -> leaf = false; // 처음에 리프여부는 초기값 false로
    newNode -> key[0] = val; // 새 node의 1번째 key 값에 받아온 데이터 넣기
    newNode -> cnt_key = 1;
    newNode -> cnt_child = 0;
    newNode -> end_leaf = false;
    return newNode;
}

// 노드의 값을 분리해서 다른 노드에 분배하는 함수
struct BTreeNode* splitNode(int pos, struct BTreeNode* node, struct BTreeNode* parent){ // 현재 노드, 부모 노드, 현재노드에서 넣은 값의 위치를 알고있는 pos를 인자로 받기
    int median; // 분리를 위해 중앙값 알아야 함.
    median = node->cnt_key / 2; // 중앙값은 짝수이든 홀수이든 '키개수/2' 
    struct BTreeNode* right_node; // 분리 한 값을 새로 넣어줄 오른쪽 노드 만듬.(추후 자식이 됨)

    right_node = (struct BTreeNode*)malloc(sizeof(struct BTreeNode));
    right_node-> leaf = node->leaf; // 분리만 한거니까 현재 분리하려는 노드와 리프 여부가 같음. 걔가 원래 리프 아니였으면 얘도 리프 아님.
    right_node-> cnt_key = 0;
    right_node-> cnt_child = 0;
    right_node->end_leaf =  node->end_leaf;
    right_node->next = node->next;


    if (node->leaf){ // 현재 노드가 리프이면, 자식 담기
        node->next = right_node;
        node->end_leaf = false;
        int num_iter = node->cnt_key;
        for (int i = median; i < num_iter; i++ ) { // 오른쪽 노드에 현재 노드 자식 절반 담기
            right_node->key[i-median] = node->key[i];
            right_node->child_data[i-median] = node->child_data[i];
            right_node-> cnt_key++; // 새로 채워준 노드의 자식 개수는 증가, 현재 노드에서는 빼기
            node->cnt_key--;
        }
    }
    else {
        int num_iter = node->cnt_key;
        for (int i = median+1; i < num_iter; i++ ) { // 오른쪽 노드에 현재 노드 자식 절반 담기
                right_node->key[i-(median+1)] = node->key[i];
                right_node-> cnt_key++; // 새로 채워준 노드의 자식 개수는 증가, 현재 노드에서는 빼기
                node->cnt_key--;
            }

        num_iter = node->cnt_child;
        for(int i = median+1; i< num_iter; i++){ // 분리할 노드에 키 담기(리프이든 아니든)
            right_node->child[i-(median+1)] = node->child[i];
            right_node ->cnt_child++;
            node->cnt_child--;
        }
    }

    // 분리할 때 위로 올릴 부모 노드 처리
    if (node==root) { // 루트면 새 부모 노드 만들어야 됨.
        struct BTreeNode* new_parent_node;
        new_parent_node = createNode(node->key[median]); // 중앙값 가지고 새 부모 노드 만들기
        if (!node->leaf){
            node->cnt_key--;
        }
        new_parent_node->child[0] = node; // 새부모노드의 왼쪽 자식은 현재 노드
        new_parent_node->child[1] = right_node;
        new_parent_node->cnt_child = 2;
        return new_parent_node; 
    }
    else { // 루트가 아니면, 원래 있던 부모노드 활용
        for (int i= parent->cnt_key; i> pos; i--){ // 부모 노드에 넣어야되니까 거기있던 키 배치 다시하기
            parent->key[i] = parent->key[i-1];
            parent->child[i+1] = parent->child[i];
        }

        parent->key[pos]= node->key[median]; // 부모 노드에 넣어야될 자리에 값 넣기
        parent->cnt_key++; // 부모노드에 새로 넣었으니까 키개수 추가, 원래 노드는 키 개수 줄이기
        if (!node->leaf){
            node->cnt_key--;
        }
        parent-> child[pos+1] = right_node; // 왼쪽 노드는 원래 연결되어있으니 오른쪽만 부모노드에 연결.
        parent->cnt_child+=1;
    }
    return node; //현재 노드 리턴
}

struct BTreeNode* insertNode(int parent_pos, int val, struct BTreeNode* node, struct BTreeNode* parent, int value) { // 삽입할 값,  
    int pos; // 현재 노드에서 키의 위치를 갖고 있어야 함. 왜냐면 넣으려고 하는 값의 위치를 찾아야 하기 때문.
    for (pos =0; pos < node->cnt_key; pos++ ) {// pos 위치는 0부터 해서, 현재 노드의 키 개수만큼 탐색
        if (val == node -> key[pos]){ // node의 pos번째 키와 val이 같으면
            printf("Duplicates are not permitted!\n");
            return node;
        }
        else if (val< node->key[pos]){ // val이 node의 pos번째 키보다 작으면 그 pos에서 멈춘다.
            break;
        }
    }// 만약 val이 그 node에 있는 값보다 크면 당연히 마지막 pos가 나올 것임.
    if (!node->leaf) { // node leaf 여부가 false이면, leaf가 아니면
        node -> child[pos] = insertNode(pos, val, node->child[pos] ,node, value); // node의 pos번째 자식 노드에 insertNode 값을 담는다. 재귀로 자식을 탐색하기 위해 또 들어감.
        if (node->cnt_key == max_keys +1){ // 현재 노드 키 개수가 규칙에서 벗어날거같으면
            node = splitNode(parent_pos, node,parent); // 윗 방향으로 분리를 해야 함.
        }
    }
    else { // leaf일 때의 삽입 로직
        for (int i = node->cnt_key; i > pos; i--) { // 끝에서부터 val을 삽입해야 하는 위치에 있는 노드까지의 노드들을 뒤로 땡기는 작업을 한다.
            node -> key[i] = node->key[i-1]; // 키가 뒤로 한 칸씩 가는 작업.
            node -> child[i+1] = node->child[i]; // 자식도 마찬가지.
        }

        node -> key[pos] = val; // val을 삽입해야 하는 위치에 val 삽입.
        // node -> child_data = value;
        node -> child_data[pos] = value;
        node -> cnt_key++; // 하나 십입했으니 키 개수 증가
        if (node-> cnt_key == max_keys+1){ // leaf 노드가 꽉 찼으면 분리를 해준다.
            node = splitNode(parent_pos, node,parent);
        }
    }   
    return node; // node에 값을 넣어주니까 그 node를 반환해야 됨. 그래야 재귀 종료되어서 값을 사용 가능.
}


// 삽입 함수 제작 (인자 : 받아야 하는 값)
void insert(int val, int value){
    if (!root){ // root가 없으면
        root = createNode(val); // root를 만들어라.
        root -> leaf = true ; // 처음 만들어지는거니까 root이자 leaf 노드.
        root -> end_leaf = true;
        return;
    }
    else{ // 루트가 있으면
        root = insertNode(0,val,root,root,value); // 처음에는 root가 부모이자 리프노드.
    }
}

// 못빌릴 때 합치는 함수
void mergeNode(struct BTreeNode* par_node, int node_pos, int mer_node_pos){

    if (par_node->child[node_pos]->leaf){
        int merge_idx = par_node->child[mer_node_pos]->cnt_key; // merge의 주체 노드쪽에 merge될 키 위치를 지정한다.
        for(int i=0; i<par_node->child[node_pos]->cnt_key; i++){ // 지우는 노드에서 그 키를 지워서 최소 키 개수 유지가 안될 수 있음. 남은 키 들은 merge한 노드로 옮겨야 함. 남은키가 없으면 아예 for문이 안돌아감.
            par_node->child[mer_node_pos]->key[merge_idx+i] = par_node->child[node_pos]->key[i]; // 키 개수가 2개 -> i가 0, merge idx 오른쪽 키/ 키 개수가 늘어나면 i도 하나씩 더 늘어남. 
            par_node->child[mer_node_pos]->cnt_key++;
        }
        par_node->child[mer_node_pos]->next= par_node->child[node_pos]->next; //부모노드의 키를 merge함.
        par_node->child[mer_node_pos]->end_leaf = par_node->child[node_pos]->end_leaf;

        for(int i = mer_node_pos; i < (par_node->cnt_key)-1; i++) { //부모 노드의 키 하나는 이미 자식 노드와 병합되었으니 재정비해야 함.
            par_node->key[i] = par_node->key[i+1];
        }
        par_node->cnt_key--;

        for(int i = mer_node_pos+1; i<(par_node->cnt_child)-1; i++){ // 부모 노드에는 병합 한 거 말고 그 뒤에 다른 자식도 있을 수 있으니 재배열 해야 함. merge한 노드 뒷 노드부터 대상이 됨.
            par_node->child[i] = par_node->child[i+1];
        }
        par_node->cnt_child--;
    }
    else{
        // 왼쪽 노드를 지웠을 때에는 최종 merge되는 주체가 왼쪽 노드가 되게 강제로 만듬.(편리를 위해) / node_pos가 삭제된 키를 갖고있는 노드가 될 수도 있고 안될수도 있음.
        int merge_idx = par_node->child[mer_node_pos]->cnt_key; // merge의 주체 노드쪽에 merge될 키 위치를 지정한다.
        par_node->child[mer_node_pos]->key[merge_idx]= par_node->key[mer_node_pos]; //부모노드의 키를 merge함.
        par_node->child[mer_node_pos]->cnt_key++;

        for(int i=0; i<par_node->child[node_pos]->cnt_key; i++){ // 지우는 노드에서 그 키를 지워서 최소 키 개수 유지가 안될 수 있음. 남은 키 들은 merge한 노드로 옮겨야 함. 남은키가 없으면 아예 for문이 안돌아감.
            par_node->child[mer_node_pos]->key[merge_idx+1+i] = par_node->child[node_pos]->key[i]; // 키 개수가 2개 -> i가 0, merge idx 오른쪽 키/ 키 개수가 늘어나면 i도 하나씩 더 늘어남. 
            par_node->child[mer_node_pos]->cnt_key++;
        }

        int merge_childidx = par_node->child[mer_node_pos]->cnt_child; //merge한 노드 끝부분으로 옮겨야 하니 끝부분 idx 지정.
        for (int i=0; i<par_node->child[node_pos]->cnt_child; i++){ // 지우는 노드에서 키를 지우고 남은 자식이 있으니. 걔네를 merge한 노드로 옮겨야 한다.
            par_node->child[mer_node_pos]->child[merge_childidx+i] = par_node->child[node_pos]->child[i];
            par_node->child[mer_node_pos]->cnt_child++;
        }

        free(par_node->child[node_pos]); // merge 되고 나서 반대편 노드는 필요없으니 메모리에서 날리기

        for(int i = mer_node_pos; i < (par_node->cnt_key)-1; i++) { //부모 노드의 키 하나는 이미 자식 노드와 병합되었으니 재정비해야 함.
            par_node->key[i] = par_node->key[i+1];
        }
        par_node->cnt_key--;

        for(int i = mer_node_pos+1; i<(par_node->cnt_child)-1; i++){ // 부모 노드에는 병합 한 거 말고 그 뒤에 다른 자식도 있을 수 있으니 재배열 해야 함. merge한 노드 뒷 노드부터 대상이 됨.
            par_node->child[i] = par_node->child[i+1];
        }
        par_node->cnt_child--;
    }
}

// suceessor 찾는 함수
int findSuccessor(struct BTreeNode* cur_node){
    int successor;
    if (cur_node->leaf){ //현재 탐색노드가 리프이면, 찾을 수 있음.
        return cur_node->key[0]; //현재 노드에서 가장 작은 키 주면 됨.(successor 조건)
    }
    return findSuccessor(cur_node->child[0]); // 탐색할 때마다 작은 쪽 자식으로 탐색해야 함.
}


// 왼쪽에서 빌리는 함수
void borrowFromLeft(struct BTreeNode* par_node, int cur_node_pos){ // 부모 노드와 현재 노드 pos위치를 인자로 받음. 이미 현재 노드의 키는 지워졌음. cnt_key는 최소상태 혹은 미만일거임.
    if (par_node->child[cur_node_pos]->leaf){
        int tenant_idx = 0; // 빌리는 주체노드에 빌려주는 키가 들어가야할 위치.
        int idx_from_sib_topar= par_node->child[cur_node_pos-1]->cnt_key-1;
 
        for (int i=0; i< (par_node->child[cur_node_pos]->cnt_key); i++){ 
            par_node->child[cur_node_pos]->key[i+1] = par_node->child[cur_node_pos]->key[i];
        }
        par_node->child[cur_node_pos]->key[tenant_idx] = par_node->child[cur_node_pos-1]->key[idx_from_sib_topar];
        par_node->child[cur_node_pos]->cnt_key++;
        par_node->child[cur_node_pos-1]->cnt_key--;

        int successor = findSuccessor(par_node->child[cur_node_pos]);
        par_node->key[cur_node_pos-1] = successor;
    }
    else {
            int tenant_idx = 0; // 빌리는 주체노드에 빌려주는 키가 들어가야할 위치.

            // 빌리는 노드는 자리를 마련해야 하기 때문에 남아있는거를 한칸 씩 뒤로 미뤄야 함.(남아있는게 없어도 적용). borrowFromRight와 다르게 먼저 자리 정리를 해야 함.
            for (int i=0; i< par_node->child[cur_node_pos]->cnt_key; i++){ 
                par_node->child[cur_node_pos]->key[i+1] = par_node->child[cur_node_pos]->key[i];
            }
            par_node->child[cur_node_pos]->key[tenant_idx] = par_node->key[cur_node_pos-1]; // 빌리는 키는 오름차순 상 부모에게서 빌려온다. 여기서는 왼쪽에서 빌려오니까, cur_node_pos(오른쪽 자식 가리키는 위치)에서 1을 빼야 부모의 키 위치가 됨. 그리고 형제노드의 키가 위로 올라가는 꼴.
            par_node->child[cur_node_pos]->cnt_key++;

            int idx_from_sib_topar= (par_node->child[cur_node_pos-1]->cnt_key) -1; // 부모노드 키를 밑으로 내렸으니까 형제 노드 중 마지막 키를 하나 부모로 올려야 됨. 
            par_node->key[cur_node_pos-1] = par_node->child[cur_node_pos-1]->key[idx_from_sib_topar]; // 부모노드는 빌려준 형제노드의 키를 들고온다. 여기선 왼쪽 형제노드.
            par_node->child[cur_node_pos-1]->cnt_key--;

            // 형제노드는 키를 빌려줬으니 자식도 정리를 해야함. 자식 위치 정리도 진행.(한 칸씩 앞으로 땡기기)
            if (par_node->child[cur_node_pos-1]->cnt_child>0){ // 형제 노드 자식이 있는 경우에만
                int tenant_childidx = (par_node->child[cur_node_pos-1]->cnt_child)-1; // tenant 노드에다가 형제노드의 가장 뒷 자식 위치를 줘야되기 때문에 그 인덱스 지정.
                // 자식 위치 정리. borrowFromRight와 다르게 옮기기 전 미리 세팅해야 함. 
                for (int i = par_node->child[cur_node_pos]->cnt_child; i > 0; i--){ // 뒤에서부터 시작해서 앞에껄 뒤로 옮기는 느낌.
                    par_node->child[cur_node_pos]->child[i] = par_node->child[cur_node_pos]->child[i-1];
                }

                par_node->child[cur_node_pos]->child[0] = par_node->child[cur_node_pos-1]->child[tenant_childidx]; // 형제 노드 자식 빌려옴. 빌려온 자식 놓는 위치는 현재 노드의 1번째 위치.
                par_node->child[cur_node_pos]->cnt_child++;

                par_node->child[cur_node_pos-1]->cnt_child--;
            }
    }
    
}

// 오른쪽에서 빌리는 함수
void borrowFromRight(struct BTreeNode* par_node, int cur_node_pos){ // 부모 노드와 현재 노드 pos위치를 인자로 받음. 이미 현재 노드의 키는 지워졌음. cnt_key는 최소상태 혹은 미만일거임.
    if (par_node->child[cur_node_pos]->leaf){
        int tenant_idx = par_node->child[cur_node_pos]->cnt_key; // 빌리는 주체노드에 빌려주는 키가 들어가야할 위치.
        int idx_from_sib_topar= 0;
        par_node->child[cur_node_pos]->key[tenant_idx] = par_node->child[cur_node_pos+1]->key[idx_from_sib_topar];
        par_node->child[cur_node_pos+1]->cnt_key--;

        for (int i=0; i< (par_node->child[cur_node_pos+1]->cnt_key)-1; i++){ 
            par_node->child[cur_node_pos+1]->key[i] = par_node->child[cur_node_pos+1]->key[i+1];
        }
        par_node->child[cur_node_pos+1]->cnt_key--;

        int successor = findSuccessor(par_node->child[cur_node_pos+1]);
        par_node->key[cur_node_pos] = successor;
    }
    else {
        int tenant_idx = par_node->child[cur_node_pos]->cnt_key; // 빌리는 주체노드에 빌려주는 키가 들어가야할 위치.
        par_node->child[cur_node_pos]->key[tenant_idx] = par_node->key[cur_node_pos]; // 빌리는 키는 오름차순 상 부모에게서 빌려온다. 그리고 형제노드의 키가 위로 올라가는 꼴.
        par_node->child[cur_node_pos]->cnt_key++;

        int idx_from_sib_topar= 0;
        par_node->key[cur_node_pos] = par_node->child[cur_node_pos+1]->key[idx_from_sib_topar]; // 부모노드는 빌려준 형제노드의 키를 들고온다.

        // // 형제노드는 키를 빌려줬으니까 위치 정리를 해야 한다.(한 칸씩 앞으로 땡기기)
        for (int i=0; i< (par_node->child[cur_node_pos+1]->cnt_key)-1; i++){ 
            par_node->child[cur_node_pos+1]->key[i] = par_node->child[cur_node_pos+1]->key[i+1];
        }
        par_node->child[cur_node_pos+1]->cnt_key--;

    
        int idx_from_sib = 0;
        // 형제노드는 키를 빌려줬으니 자식도 정리를 해야함. 자식 위치 정리도 진행.(한 칸씩 앞으로 땡기기)
        if (par_node->child[cur_node_pos+1]->cnt_child>0){ // 자식이 있는 경우에만
            int tenant_childidx = par_node->child[cur_node_pos]->cnt_child; // tenant 노드에 빌려온 자식이 들어갈 위치
            par_node->child[cur_node_pos]->child[tenant_childidx] = par_node->child[cur_node_pos+1]->child[idx_from_sib]; // 형제 노드 자식 빌려옴.
            par_node->child[cur_node_pos]->cnt_child++;

            // 자식 위치 정리 
            for (int i = 0; i<par_node->child[cur_node_pos+1]->cnt_child-1; i++){
                par_node->child[cur_node_pos+1]->child[i] = par_node->child[cur_node_pos+1]->child[i+1];
            }
            par_node->child[cur_node_pos+1]->cnt_child--;
        }
    }
}

void balanceNode(struct BTreeNode* node, int child_pos){ // 현재 노드와 자식노드에서의 위치를 인자로 받는 함수(빌리기,병합을 진행)
    if (child_pos==0){ // 자식노드 키 위치가 맨 왼쪽일때는 오른쪽 부모,형제를 봐야 함.
        if (node->child[child_pos+1]->cnt_key > min_keys){ // (자식노드 기준) 형제의 키개수가 최소숫자 범위 안 부서질때
            borrowFromRight(node,child_pos);
        }
        else{ // 형제의 키개수가 최소숫자 범위 부서질때
            mergeNode(node,child_pos+1,child_pos); // 부모노드(현재노드)와 자신 위치랑 자기 형제 위치를 같이 넘겨줌.  
        }
        return;
    }

    else if (child_pos == (node->cnt_key)){ // 자식노드 키 위치가 맨 오른쪽일 때는 왼쪽 부모, 형제 봐야 함.
        if (node->child[child_pos-1]->cnt_key > min_keys){ // 자식노드 기준, 왼쪽 형제의 키개수가 최소숫자 범위 안 부서질 때
            borrowFromLeft(node,child_pos);
        }
        else{ // 최소숫자 범위 부서질 때
            mergeNode(node,child_pos,child_pos-1); // 부모노드(현재노드)와 지우는 노드랑 병합대상 노드 위치를 같이 넘겨줌.  
        }
        return;
    }
    else{ // 맨 왼쪽,맨 오른쪽 말고 그 이외
        if (node->child[child_pos-1]->cnt_key > min_keys){
            borrowFromLeft(node,child_pos); 
        }
        else if (node->child[child_pos+1]->cnt_key > min_keys){
            borrowFromRight(node,child_pos);
        }
        else{
            mergeNode(node,child_pos,child_pos-1); // 극단에 있는 자식 말고 그 외 지역에 위치한 노드들이 병합할 때
        }
        return;
    }
}


// 내부 노드에서 값을 지우는 함수
void deleteInnerNode(struct BTreeNode* cur_node, int cur_node_pos){
    int successor = findSuccessor(cur_node->child[cur_node_pos+1]); // predecessor 혹은 successor가 있을 경우 || merge할 경우의 찾은 값을 담은 변수
    cur_node->key[cur_node_pos] = successor;   
}


// 노드랑 지우는 값을 넣어주면 지우는 함수.
int deleteValFromNode(int val, struct BTreeNode* node){ 
    int pos; // 지우려는 노드의 포지션 위치를 잡음.    
    int flag = false; // 지워졌는지 여부를 알기 위해 flag를 접수
    for (pos=0; pos < node->cnt_key; pos++){ // val이 지워져야하니 그 위치를 찾아야 함.현재 노드의 키 개수만큼 탐색
        if (val == node->key[pos]){ // 현재 노드의 키 배열에서 pos와 val이 같으면
            flag = true; // 찾았다는 표시
            break;
        }
        if (val< node->key[pos]){ // 키 배열의 pos 위치 값이 val보다 크면 그 위치에서 멈춰라. 거기에서 아래로 더 들어가야 한다.
            break;
        }
    } // 이게 끝났다는건 그 노드에서 (추가 탐색할) pos위치가 정해졌다는 것 
    if (flag){ // flag가 true이면 실제로 삭제하는 작업 실시
        if (node->leaf){  // 리프에서 삭제해야 하면
            for (int i = pos; i<node->cnt_key; i++){ // 저장된 pos위치부터 키 개수만큼 탐색
                node->key[i] = node->key[i+1]; // 지우려는 키 위치에 그 다음 키로 덮어쓰기
            }
            node->cnt_key--;
        }
        else {
            flag = deleteValFromNode(val,node->child[pos+1] );
            if (node->child[pos+1]->cnt_key < min_keys){
                balanceNode(node,pos+1);
                deleteValFromNode(val,node);
            }
            else { // 내부에서 삭제해야 하면
                    deleteInnerNode(node,pos); //내부 노드의 값을 삭제하는 함수 제작. 현재 노드와 현재노드에서의 값 위치를 인자로 넘김.
            }
        }
        return flag;
    }
    else { // flag가 false이면(지우려는 값을 못찾은 것)
        if (node->leaf) { //leaf 노드이면
            return flag;
        }
        else{ // 지우려는 값을 못 찾았는데 내부 노드이면 더 내려감.
            flag = deleteValFromNode(val, node->child[pos]); //val이랑 현재노드의 pos번째 자식 넘겨서 flag 받기
        }
    }
    if (node->child[pos]->cnt_key < min_keys){ // (재귀가 끝나서 다시 올라온뒤)삭제처리했던 자식 노드의 키 개수가 최소숫자 범위 부셔졌을 때
        balanceNode(node,pos); // 빌리던, 병합하던 하는 함수 제작 (현재 노드와 자식노드의 pos위치를 인자로)
    }

    return flag;    
}

// 지우는 함수
void delete(struct BTreeNode* node, int val){ // 현재 노드랑 지우려는 값을 인자로 받음.
    if (!node){ // 현재 보는 노드에 아예 값이 없으면
        printf("Empty tree!!\n");
        return;
    }
    int flag = deleteValFromNode(val,node); // 현재 노드 내에서 값을 지우는 함수 호출. 지우는 값이랑 현재 노드를 인자로 받음. 리턴은 flag로 받음.
    if (!flag){ // flag가 0이면 실행.
        printf("%d does not exist in this tree. \n", val); // 함수 내에서 못찾으면 플래그가 0인거니까 에러 메세지 출력
        return;
    }
    if (node->cnt_key == 0){  // deleteVal을 하고 나서 node의 키개수가 0일 때 = 현재 노드에 아무것도 없어서 변화가 필요함.
        node = node->child[0]; // 지금 노드를 가장 왼쪽 자식 노드로 만듬.
    }
    root = node;

}

/*PRINT******************************************************************************************************/
void printTree(struct BTreeNode* node, int level) {				 // 트리 그리기
	if (!node) { 											 // empty tree!
		printf("Empty tree!!\n");
		return;
	}
	printf("Level %d :   ", level);
	for (int i = 0; i < level - 1; i++) {
		printf("            ");
	}
	for (int i = 0; i < node->cnt_key; i++) {
		printf("%d ", node->key[i]);
	}
	printf("\n");
	level++;
	for (int i = 0; i < node->cnt_child; i++) {
		printTree(node->child[i], level);
	}
}

void printLeaves(struct BTreeNode* node) {			 // leaf 그리기
	if (!node) { 											 // empty tree!
		printf("Empty tree!!\n");
		return;
	}
	if (node->leaf) {
		for (int i = 0; i < node->cnt_key; i++) {
			printf("%d :", node->key[i]);
            printf("%d ", node->child_data[i]);
		}
		printf("| ");
		if (!node->end_leaf) {
			printLeaves(node->next);
		}
		else {
			printf("\n");
		}
	}
	else {
		printLeaves(node->child[0]);
	}
}

int main(void) {
	insert(1,6500);
	insert(4,5000);
	insert(7,3000);
	insert(10,2000);
	insert(17,8000);
	insert(21,900);
	// insert(31);
	// insert(25);
	// insert(19);
	// insert(20);
	// insert(28);
	// insert(42);

	// delete(root, 21);
	// delete(root, 31);
	// delete(root, 20);
	// delete(root, 10);
	// delete(root, 7);
	// delete(root, 25);
	// delete(root, 42);
	// delete(root, 4);

	// for (int i = 10; i < 100; i += 10) {
	// 	insert(i);
	// }





	printTree(root, 1);
	printLeaves(root);
	printf("----------------------------------------------------------------------------------------------------\n");
	// delete(root, 50);

	printTree(root, 1);
	printLeaves(root);

	searchNode(root, 20);
	searchNode(root, 120);

}