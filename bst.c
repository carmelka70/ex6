#include <stdlib.h>
#include "bst.h"


BST* createBST(Compare cmp, Print print, FreeData freeData)
{
    BST *myBST = (BST*) malloc(sizeof(BST));
    myBST->compare = cmp;
    myBST->print = print;
    myBST->freeData = freeData;

    myBST->root = NULL;
    return myBST;
}



BSTNode* bstInsert(BSTNode** rootPtr, void* data, Compare cmp)
{
    if (*rootPtr == NULL) // We reached an empty place
    {
        BSTNode *myNode = (BSTNode*) malloc(sizeof(BSTNode));
        myNode->left = myNode->right = NULL;
        myNode->data = data;

        return (*rootPtr = myNode);
    }

    BSTNode *root = *rootPtr;

    if (cmp(data, root->data) == SMALLER)
        return bstInsert(&root->left, data, cmp);
    else
        return bstInsert(&root->right, data, cmp);
}


void* bstFind(BSTNode* root, void* data, Compare cmp)
{
    if (root == NULL) return NULL;


    cmpValue c = cmp(data, root->data);

    if (c == EQUAL)
        return root;
    else if (c == SMALLER)
        return bstFind(root->left, data, cmp);
    else
        return bstFind(root->right, data, cmp);
}


void bstInorder(BSTNode* root, Print print)
{
    if (root == NULL) return;

    bstInorder(root->left, print);

    print(root->data);

    bstInorder(root->right, print);
}

void bstPreorder(BSTNode* root, Print print)
{
    if (root == NULL) return;

    print(root->data);

    bstInorder(root->left, print);
    bstInorder(root->right, print);
}

void bstPostorder(BSTNode* root, Print print)
{
    if (root == NULL) return;

    bstInorder(root->left, print);
    bstInorder(root->right, print);

    print(root->data);
}

void bstFree(BSTNode* root, FreeData freeData)
{
    if (root == NULL) return;

    bstFree(root->left, freeData);
    bstFree(root->right, freeData);

    freeData(root->data);
    free(root);
}
