#ifndef BST_H
#define BST_H


typedef enum {SMALLER, EQUAL, BIGGER} cmpValue;

// Compare should return 1 if a <= b
typedef cmpValue (*Compare)(void *a, void *b);
typedef void (*Print)(void *);
typedef void (*FreeData)(void *);


typedef struct BSTNode {
    void* data;
    struct BSTNode* left;
    struct BSTNode* right;
} BSTNode;

typedef struct {
    BSTNode* root;
    Compare compare;
    Print print;
    FreeData freeData;
} BST;



BST* createBST(Compare cmp, Print print, FreeData freeData);
BSTNode* bstInsert(BSTNode** root, void* data, Compare cmp);
void* bstFind(BSTNode* root, void* data, Compare cmp);
void bstInorder(BSTNode* root, Print print);
void bstPreorder(BSTNode* root, Print print);
void bstPostorder(BSTNode* root, Print print);
void bstFree(BSTNode* root, FreeData freeData);

#endif
