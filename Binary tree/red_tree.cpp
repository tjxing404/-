#include <iostream>

struct Node{
    int val;
    int color;
    Node *left,*right;
};

Node __NIL;//NIL节点的定义
#define NIL (&__NIL)
__attribute__ ((constructor))
void init_NIL(){
    NIL->val = 0;
    NIL->color = 1;
    NIL->left = NIL;
    NIL->right = NIL;
}

//创建一个新的结点
Node *getNode(int val){
    Node *p = (Node*)malloc(sizeof(Node));
    p->val = val;
    p->color = 0;
    p->left = p->right = NIL;
    return p;
}

//判断是否有红色子孩子
bool has_red_child(Node *root){ 
    return root->left->color == 0 || root->right->color == 0;
}

//左旋
Node *left_rotate(Node *root){
    Node *temp = root->right;
    root->right = temp->left;
    temp->left = root;
    return temp;
}


//右旋
Node *right_rotate(Node *root){
    Node *temp = root->left;
    root->left = temp->right;
    temp->right = root;
    return temp;
}

//找到前驱节点，有一定的BUG，但是在这个红黑树中无问题对于前驱节点是父节点的情况找不到但是对于红黑树而言不存在这种情况
Node *pre(Node *root){              
    Node *temp = root->left;
    while(temp->right != NIL) temp = temp->right;
    return temp;
}

//插入调整操作
Node *insert_change(Node *root) {                                   
    int flag = 0;                                                   //定义flag，插入调整在祖父节点看，有冲突时才去调整
    if(!has_red_child(root)) return root;                           //没有红色子孩子直接返回
    if(root->left->color == 0 && root->right->color == 0){          //有两个红色子孩子
        if(has_red_child(root->left) || has_red_child(root->right)){//任意一个子孩子下有红色子孩子必然冲突flag=1;
            flag = 1;
        }
    }else if(root->left->color == 0 && has_red_child(root->left)){  //左边为红色节点，且有红色子孩子
        if(root->left->right->color == 0){                          //判断是否是LR的情况
            root->left = left_rotate(root->left);                   //小左旋
        }
        root = right_rotate(root);                                  //大右旋
        flag = 1;
    }else if(root->right->color == 0 && has_red_child(root->right)){//右边为红色节点，且有红色子孩子
        if(root->right->left->color == 0){                          //判断是否为RL情况
            root->right = right_rotate(root->right);                //小右旋
        }
        root = left_rotate(root);                                   //大左旋
        flag = 1;
    }
    if(flag){                  //采取红色上浮的策略，通过旋转成为小帽子结构，然后染色
        root->color = 0;
        root->left->color = root->right->color = 1;
    }
    return root;
}

//插入，和AVL树无区别
Node *__insert(Node *root, int key){     
    if(root == NIL) return getNode(key);
    if(root->val == key) return root;
    if(root->val > key){
        root->left = __insert(root->left,key);
    }else{
        root->right = __insert(root->right,key);
    }
    return insert_change(root);
}


//多封装一层insert,强制将根节点染成黑色
Node *insert(Node *root,int key){        
    root = __insert(root,key);
    root->color = 1;
    return root;
}

//删除调整操作，站在父节点看，如果出现双重黑节点就要调整
Node *erase_change(Node *root) {         
    if (root->left->color != 2 && root->right->color != 2) return root;   //两个子孩子都不是双重黑节点，直接返回
    if (has_red_child(root)) {           //有红色节点，进行旋转
        int flag = 0;
        root->color = 0;                 //原来的根节点变红，为了防止每条路径黑色节点数量不一样
        if (root->left->color == 0) {
            root = right_rotate(root); flag = 1;    //通过右旋将红色节点旋转为根节点
        } else {
            root = left_rotate(root); flag = 2;     //通过左旋将红色节点旋转为根节点
        }
        root->color = 1;                 //新的根节点变黑
        if (flag == 1) root->right = erase_change(root->right);
        else root->left = erase_change(root->left);
        return root;
    }
    if ((root->left->color == 2 && !has_red_child(root->right)) ||
        (root->right->color == 2 && !has_red_child(root->left))) {  //有一个双重黑节点且他的兄弟节点没有红色子孩子
        root->left->color -= 1;                                     //调整策略：两个孩子减一重黑，自己加一重黑，双重黑节点上浮
        root->right->color -= 1;
        root->color += 1;
        return root;
    }
    if (root->left->color == 2) {               //左边是双重黑节点
        root->left->color -= 1;                 //减一重黑色
        if (root->right->right->color != 0) {   //判断RR情况成不成立，如果成立，RR情况的优先级高于RL，只能判断RR情况不成立
            root->right->color = 0;             //原来的节点染红
            root->right = right_rotate(root->right);
            root->right->color = 1;             //新的节点染黑
        }
        root = left_rotate(root);               //大左旋
        root->color = root->left->color;        //新的节点颜色等于原来节点的颜色
    } else {
        root->right->color -= 1;                //右边是双重黑节点，减一重黑色
        if (root->left->left->color != 0) {     //同上
            root->left->color = 0;              //原来节点染红
            root->left = left_rotate(root->left);
            root->left->color = 1;              //新的节点染黑
        }
        root = right_rotate(root);              //大右旋
        root->color = root->right->color;       //新的节点等于原来节点颜色
    }
    root->left->color = root->right->color = 1; //左右孩子都成黑色
    return root;
}

//删除调整操作，和AVL树一样
Node *__erase(Node *root, int key) {            
    if (root == NIL) return NIL;
    if (key < root->val) {
        root->left = __erase(root->left, key);
    } else if (key > root->val) {
        root->right = __erase(root->right, key);
    } else {
        if (root->left == NIL || root->right == NIL) {
            Node *temp = root->left != NIL ? root->left : root->right;
            temp->color += root->color;
            free(root);
            return temp;
        } else {
            Node *temp = pre(root);
            root->val = temp->val;
            root->left = __erase(root->left, temp->val);
        }
    }
    return erase_change(root);
}

//封装一层函数，根节点强制染黑
Node *erase(Node *root, int key) {      
    root = __erase(root, key);
    root->color = 1;
    return root;
}

//清除节点操作
void clear(Node *root){                 
    if(root == NIL) return;
    clear(root->left);
    clear(root->right);
    free(root);
}

//打印函数和主函数
void print(Node *root){
    if(root != NIL){
        printf("%d %d %d %d\n",root->val,root->color,root->left->val,root->right->val);
    }
}

void travel(Node *root){
    if(root != NIL){
        travel(root->left);
        print(root);
        travel(root->right);
    }
}

int main(){
    int op,val;
    Node *root = NIL;
    while(~scanf("%d %d",&op,&val)){
        if(op == 1){
            root = insert(root,val);
        }
        if(op == 2){
            root = erase(root,val);
        }
        if(op == 3){
            travel(root);
        }
    }
    clear(root);
    return 0;
}
