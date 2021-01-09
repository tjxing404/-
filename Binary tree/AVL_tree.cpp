#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

//Node 节点定义
struct Node{
    int val;
    int high;
    Node *left,*right;
};

//NIL节点定义
Node __NIL;
#define NIL (&__NIL)
__attribute__((constructor))
void INIT_NIL(){
    NIL->val = 0;
    NIL->high = 1;
    NIL->left = NIL->right = NIL;
}

//创建节点
Node *get_Node(int val){
    Node *p = new Node;
    p->val = val;
    p->high = 1;
    p->left = p->right = NIL;
    return p;
}

//更新节点高度
void update_high(Node *root){
    root->high = max(root->left->high,root->right->high) + 1;
}

//左旋调整
Node *left_rotate(Node *root){
    Node *temp = root->right;
    root->right = temp->left;
    temp->left = root;
    update_high(root);
    update_high(temp);
    return temp;
}
//右旋调整
Node *right_rotate(Node *root){
    Node *temp = root->left;
    root->left = temp->right;
    temp->right = root;
    update_high(root);
    update_high(temp);
    return temp;
}

//调整树平衡
Node *change(Node *root){
    if(abs(root->left->high - root->right->high) <= 1) return root;
    if(root->left->high > root->right->high){
        if(root->left->right > root->left->left){
            root->left = left_rotate(root->left);
        }
        root = right_rotate(root);
    }else{
        if(root->right->left > root->right->right){
            root->right = right_rotate(root->right);
        }
        root = left_rotate(root);
    }
    return root;
}

//插入节点，最后需要更新高度和检查树是否平衡
Node *insert(Node* root, int val){
    if(root == NIL) return get_Node(val);
    if(root->val == val) return NIL;
    if(root->val < val){
        root->right = insert(root->right, val);
    }else{
        root->left = insert(root->left, val);
    }
    update_high(root);
    return change(root);
}

//寻找前驱节点，此版本有BUG对于前驱节点是父节点的情况无法找到
Node *pre(Node *root){
    Node *temp = root->left;
    while(temp->right != NIL){
        temp = temp->right;
    }
    return temp;
}

//删除函数，也需要调整高度。对于度为0和度为1的情况做了整合
Node *erase(Node *root,int key){
    if(root == NIL) return NIL;
    if(root->val < key){
        root->right = erase(root->right,key);
    }else if(root->val > key){
        root->left = erase(root->left,key);
    }else{
        if(root->left == NIL || root->right == NIL){
            Node *temp = root->left != NIL ? root->left : root->right;
            free(root);
            return temp;
        }else {
            Node *temp = pre(root);
            root->val = temp->val;
            root->left = erase(root->left,temp->val);
        }
    }
    update_high(root);
    return change(root);
}

//释放节点函数，new delete 成对出现
void clear(Node *root){
    if(root == NIL) return;
    clear(root->left);
    clear(root->right);
    delete root;
}

//打印函数和主函数
void print(Node *root){
    if(root == NIL){
        return;
    }
    printf("%d (%d %d)\n",root->key,root->left->key,root->right->key);
}

void travel(Node *root){
    if(root == NIL) return;
    print(root);
    travel(root->left);
    travel(root->right);
}


int main(){
    int op,num;
    Node *root = NIL;
    for(int i = 0; i < 5; i++){
        scanf("%d %d",&op,&num);
        if(op == 1){
            root = insert(root,num);
        }
        if(op == 2){
            root = erase(root,num);
        }
    }
    travel(root);
    return 0;
}
