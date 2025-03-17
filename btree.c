#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <stdio.h>
#define M 5
#define T 3

typedef struct btreenode {
    int keys[M - 1];
    struct btreenode* children[M];
    int nOKeys;
    int nOChildren;
} btnode;
typedef struct tree {
    int m;
    int t;
    btnode* root;
} btree;

// create functions

void initTree(btree* tree); 
btnode* btree_createNode(btree* tree);
btnode* btree_createNodeWithVal(btree* tree, int val);

// main operations

void btree_insert(btree* tree, int val);
btnode* btree_insertIntoNode(btree* tree, btnode* node, int val);
void btree_delete(btree* tree, int val);
void btree_deleteFromNode(btree* tree, btnode* node, int val);
int btree_search(btnode* root, int val);


// support functions

int binarySearchPos(btnode* node, int key);
void btree_splitNode(btree* tree, btnode* node, btnode* newRoot, int pos);
void btree_rebalanceNode(btree* tree, btnode* root, int val);
void printBtree(btnode* root);

btnode* getInorderPredecessor(btree* tree, btnode* node, int idx);

// free tree 

void freeTree(btnode* root);

int main(void) {
    #define INSERTLEN 17
    int insertElements[INSERTLEN] = {10, 11, 15, 12, 6, 18, 13, 8, 14, 25, 30, 9, 5, 16, 35, 40, 27};
    btree tree;
    initTree(&tree);
    int i;
        for (i = 0; i < INSERTLEN; i++)
    {
        btree_insert(&tree, insertElements[i]);
    }
    printBtree(tree.root);
    printf("\n");
    btree_search(tree.root, 14);
    btree_delete(&tree, 14);
    printBtree(tree.root);
    printf("\n");
    btree_search(tree.root, 14);
    btree_search(tree.root, 35
    );
    freeTree(tree.root);
    return 0;
}
void initTree(btree* tree) {
    tree->m = M;
    tree->t = T;
    tree->root = NULL;
}
btnode* btree_createNode(btree* tree) {
    btnode* node = malloc(sizeof(btnode));
    node->nOKeys = 0;
    node->nOChildren = 0;
    return node;
}
btnode* btree_createNodeWithVal(btree* tree, int val) {
    btnode *node = btree_createNode(tree);
    node->keys[0] = val;
    node->nOKeys = 1;
    return node;
}
void btree_insert(btree* tree, int val) {
    if (!tree->root) {
        tree->root = btree_createNodeWithVal(tree, val);
    }
    else {
        btnode* ret = btree_insertIntoNode(tree, tree->root, val);
        if (ret) {
            tree->root = ret;
        }
    }
}
void btree_delete(btree* tree, int val) {
    if (tree->root) {
        btree_deleteFromNode(tree, tree->root, val);
        if (tree->root->nOKeys == 0) {
            btnode* prevRoot = tree->root;
            tree->root = prevRoot->children[0];
            free(prevRoot);
        }
    }
}

void btree_deleteFromNode(btree* tree, btnode* node, int val) {
    int pos = binarySearchPos(node, val);
    if (pos < node->nOKeys && node->keys[pos] == val) {
        if (!node->nOChildren) {
            // is a leaf, just delete
            // left shift to delete
            for (int i = pos; i < node->nOKeys - 1; i++) {
                node->keys[i] = node->keys[i + 1];
            }
            node->nOKeys--;
            if (node->nOKeys < tree->t - 1) {
                btree_rebalanceNode(tree, tree->root, node->keys[0]);
            }
        }
        else {
            // deleting from internal node
            btnode* predecessor = getInorderPredecessor(tree, node, pos);
            node->keys[pos] = predecessor->keys[predecessor->nOKeys - 1];
            predecessor->nOKeys--;
            if (predecessor->nOKeys < tree->t - 1) {
                btree_rebalanceNode(tree, tree->root, predecessor->keys[0]);
            }
        }
    }
    else {
        if (node->nOChildren) {
            btree_deleteFromNode(tree, node->children[pos], val);
        }
        else {
            return;
        }
    }

}
void btree_rebalanceNode(btree* tree, btnode* root, int val) { // val helps find the node we need to rebelance
    if (!root->nOChildren) {
        // we need to rebalance this node, it is done from the parent
        return;
    }
    int pos = binarySearchPos(root, val);
    // recursively find the node we child we need to rebalance (deletion always first removes a key from a child: via normal deletion or replacing with inorder predecessor)
    btree_rebalanceNode(tree, root->children[pos], val);
    if (root->children[pos]->nOKeys < tree->t - 1) {
        // the child needs rebalancing (might be in an internal node whos child has been rebalanced)

        if (pos < root->nOChildren - 1 && root->children[pos + 1]->nOKeys > tree->t - 1) {
            // we have a right sibling who has a key to spare
            // rotate left (copy right separator to end)
            root->children[pos]->keys[root->children[pos]->nOKeys] = root->keys[pos];
            root->keys[pos] = root->children[pos + 1]->keys[0];
            // shift left in right sibling
            for (int i = 0; i < root->children[pos + 1]->nOKeys - 1; i++) {
                root->children[pos + 1]->keys[i] = root->children[pos + 1]->keys[i + 1];
            }
            // update key counts
            root->children[pos]->nOKeys++;
            root->children[pos + 1]->nOKeys--;

            // CHILDREN

            if (root->children[pos]->nOChildren) {
                // leftmost child of right sibling becomes rightmost child of previously defecient node
                root->children[pos]->children[root->children[pos]->nOChildren] = root->children[pos + 1]->children[0];
                // update children counts
                root->children[pos]->nOChildren++;
                root->children[pos + 1]->nOChildren--;
            }
        }
        else if (pos > 0 && root->children[pos - 1]->nOKeys > tree->t - 1) {
            // we have a left sibling who has a key to spare
            // rotate right (copy left separator to start)
            // shift keys in deficient node right
            for (int i = root->children[pos]->nOKeys; i > 0; i--) {
                root->children[pos]->keys[i] = root->children[pos]->keys[i - 1];
            }
            root->children[pos]->keys[0] = root->keys[pos - 1];
            root->keys[pos - 1] = root->children[pos - 1]->keys[root->children[pos - 1]->nOKeys - 1];
            // update key counts
            root->children[pos]->nOKeys++;
            root->children[pos - 1]->nOKeys--;
            
            // CHILDREN

            if (root->children[pos]->nOChildren) {
                // move rightmost child of left sibling to leftmost position at previously deficient node
                // right shift children in our node
                for (int i = root->children[pos]->nOChildren; i > 0; i--) {
                    root->children[pos]->children[i] = root->children[pos]->children[i - 1];   
                }
                root->children[pos]->children[0] = 
                root->children[pos - 1]->children[root->children[pos - 1]->nOChildren - 1];
                // update children counts
                root->children[pos]->nOChildren++;
                root->children[pos - 1]->nOChildren--;
            }
        }
        else {
            // neither left or right sibling has enough keys (or exists) -> we need to merge 2 siblings and 1 key from the parent
            int leftIdx = pos > 0 ? pos - 1 : pos;
            int rightIdx = leftIdx + 1;
            // move separator to leftIdx
            root->children[leftIdx]->keys[root->children[leftIdx]->nOKeys] =
            root->keys[leftIdx];
            // left shift keys in parent
            for (int i = leftIdx; i < root->nOKeys - 1; i++) {
                root->keys[i] = root->keys[i + 1];
            }
            // update key counts
            root->children[leftIdx]->nOKeys++;
            root->nOKeys--;
            // copy keys of rightIdx to the leftIdx 
            for (int i = 0; i < root->children[rightIdx]->nOKeys; i++) {
                root->children[leftIdx]->keys[root->children[leftIdx]->nOKeys] = 
                root->children[rightIdx]->keys[i];
                root->children[leftIdx]->nOKeys++;
            }
            // copy children of rightIdx to the leftIdx
            for (int i = 0; i < root->children[rightIdx]->nOChildren; i++) {
                root->children[leftIdx]->children[root->children[leftIdx]->nOChildren] =
                root->children[rightIdx]->children[i];
                root->children[leftIdx]->nOChildren++;
            }
            // delete rightIdx
            free(root->children[rightIdx]);

            // left shift children in parent
            for (int i = rightIdx; i < root->nOChildren - 1; i++) {
                root->children[i] = root->children[i + 1];
            }
            root->nOChildren--;
        }

    }


}

btnode* btree_insertIntoNode(btree* tree, btnode* node, int val) {
    int pos = binarySearchPos(node, val);
    // pos points to a new suggested position, current keys[pos] is bigger than val
    // points to a proper child
    if (node->keys[pos] == val) {
        // val already inserted
        return NULL;
    }
    btnode* ret = NULL;
    bool is_leaf = !node->nOChildren;
    if (is_leaf) {
        // is a leaf -> insert here
        if (node->nOKeys == tree -> m - 1) {
            // will overflow -> store the val to be inserted in ret
            // ret will contain the value that is popped up and 2 children (first  node, second new)
            ret = btree_createNodeWithVal(tree, val);
            btree_splitNode(tree, node, ret, pos);
        }
        else {
            // has space -> just insert
            // shift keys to the right of pos to the right
            for (int i = node->nOKeys; i > pos; i--) {
                node->keys[i] = node->keys[i - 1];
            }
            // insert val
            node->keys[pos] = val;
            node->nOKeys++;
        }
    }
    else {
        // has children -> insert into proper child, then see if a value popped up in ret and correctly insert it into this parent node
        ret = btree_insertIntoNode(tree, node->children[pos], val);
        if (ret) {
            // the child has overflown, the new value and splitted children have to be inserted in this node
            if (node->nOKeys == tree->m - 1) {
                // parent is full -> first split the parent
                // ret contains value to be inserted
                // splitNode will take care of inserting the value and dividing all the children
                btree_splitNode(tree, node, ret, pos);
            }
            else {
                // insert the popped up key, insert the children
                // right shift keys
                for (int i = node->nOKeys; i > pos; i--) {
                    node->keys[i] = node->keys[i - 1];
                }
                // insert key
                node->keys[pos] = ret->keys[0];
                // right shift children
                // pos + 1 because the child at pos is already equal to the child that we split, we do not need to move it
                for (int i = node->nOChildren; i > pos + 1; i--) {
                    node->children[i] = node->children[i - 1];
                }
                node->children[pos] = ret->children[0]; // useless but better for visualization
                node->children[pos + 1] = ret->children[1]; // puts the second half of the child we split
                // update counts
                node->nOKeys++;
                node->nOChildren++;
                // we have dealt with ret, now it needs to be emptied so we do not assign root to it
                free(ret);
                ret = NULL;
            }

        }
    }
    
    return ret;
}
int binarySearchPos(btnode* node, int key) {
    int left = 0;
    int right = node->nOKeys - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (node->keys[mid] == key) {
            return mid;
        }
        else if (node->keys[mid] > key) {
            right = mid - 1;
        }
        else {
            left = mid + 1;
        }
    }
    return left;
}
void btree_splitNode(btree* tree, btnode* node, btnode* newRoot, int pos) {
    // newRoot contains value we want to insert and then split, pos already has the position
    btnode* tmp = btree_createNodeWithVal(tree, newRoot->keys[0]);
    // if newRoot containts children then the oveflow from a child caused an overflow at node, so we will need to properly divide the children
    tmp->children[0] = newRoot->children[0];
    tmp->children[1] = newRoot->children[1];
    bool hasChildren = node->nOChildren;
    // first take care of keys
    // t - 1 is index of middle if we overflow (it is basically ceil(m / 2) - 1)
    if (pos < tree->t - 1) {
        // our value will end up in the left child, the middle will be an element at t - 2 as it will get shifted forwards
        // remember the to be popped up element in tmp
        tmp->keys[0] = node->keys[tree->t - 2];
        // right shift elements to fill the gap (which is current t - 2)
        for (int i = tree->t - 2; i > pos; i--) {
            node->keys[i] = node->keys[i - 1];
        }
        // insert val
        node->keys[pos] = newRoot->keys[0];
    }
    else if (pos > tree->t - 1) {
        // our value will end up in the right child
        // the new middle will be current t - 1, so it will be deleted from the array
        // so pos we do not need to move the key at pos to the right but insert to the left of it as there will be guaranteed space
        // remember the element to be popped up in tmp
        tmp->keys[0] = node->keys[tree->t - 1];
        // left shift elements to fill in the gap at t - 1
        // pos - 1 as at pos there is an element that is more than val, so it has to stay to the right of it and not get shifted
        for (int i = tree->t - 1; i < pos - 1; i++) {
            node->keys[i] = node->keys[i + 1];
        }
        // insert our val
        node->keys[pos - 1] = newRoot->keys[0];
    }
    else {
        // our val will be popped up
        tmp->keys[0] = newRoot->keys[0]; // useless but good for visualization
    }
    // put the element that will be upshifted in the new root
    newRoot->keys[0] = tmp->keys[0];
    newRoot->children[0] = node;
    newRoot->children[1] = btree_createNode(tree);
    // divide the keys between the children
    for (int i = tree->t - 1; i < tree->m - 1; i++) {
        newRoot->children[1]->keys[i - tree->t + 1] = newRoot->children[0]->keys[i];
        newRoot->children[0]->keys[i] = INT_MAX;
    }
    if (hasChildren) {
        if (pos < tree->t - 1) {
            // copy children to the right side, child at t - 1 goes to the right, because the popped up value was at t - 2 -> child at t - 2 is the biggest one on the left
            for (int i = tree->t - 1; i < tree->m; i++) {
                newRoot->children[1]->children[i - tree->t + 1] = newRoot->children[0]->children[i];
            }
            // at left child shift children to the right to make space for the divided children of newRoot
            for (int i = tree->t - 1; i > pos + 1; i--) {
                newRoot->children[0]->children[i] = newRoot->children[0]->children[i - 1];
            }
            // insert the children from newRoot that we saved in tmp
            newRoot->children[0]->children[pos] = tmp->children[0]; // useless but anyway
            newRoot->children[0]->children[pos + 1] = tmp->children[1];
        }
        else {
            // we have to do almost the same thing with > t - 1 as with = t - 1
            // copy children to the right side, child at t - 1 stays on the left, as it after key t - 2, which stays on the left
            for (int i = tree->t; i < tree->m; i++) {
                newRoot->children[1]->children[i - tree->t] = newRoot->children[0]->children[i];
            }
            // at right child shift children to the right to make space for the divided children of newRoot, also remember real position is pos - 1
            // position of val in the right child is calculated as pos - 1 - (t - 1) = pos - t
            for (int i = tree->t; i > pos - tree->t + 1; i--) {
                newRoot->children[1]->children[i] = newRoot->children[1]->children[i - 1];
            }
            // insert the childrent from newRoot
            // if our val popped up into newRoot, it is useless but better for visualization to show that left child contains the 0 child of newRoot at index t - 1
            newRoot->children[1]->children[pos - tree->t + 1] = tmp->children[1];
            if (pos == tree->t) {
                newRoot->children[0]->children[tree->t - 1] = tmp->children[0]; // useless
            }
            else {
                newRoot->children[1]->children[pos - tree->t] = tmp->children[0]; // again, useless
            }
        }
        // update counts of children
        newRoot->children[0]->nOChildren = tree->t;
        newRoot->children[1]->nOChildren = tree->m - tree->t + 1;
    }
    // update counts of keys
    newRoot->children[0]->nOKeys = tree->t - 1;
    newRoot->children[1]->nOKeys = tree->m - tree->t;
    newRoot->nOChildren = 2;
    newRoot->nOKeys = 1;
    free(tmp);

    
}

void printBtree(btnode *root)
{
    if (root)
    {
        printf("( ");

        for (int i = 0; i < root->nOKeys; i++)
        {
            if (root->nOChildren)
            {
                printBtree(root->children[i]);
            }
            printf(" %d ", root->keys[i]);
        }
        if (root->nOChildren)
        {
            printBtree(root->children[root->nOKeys]);
        }

        printf(" )");
    }
    else
    {
        printf("<>");
    }
    
}

void freeTree(btnode* root) {
    if (root == NULL) return;
    for (int i = 0; i < root->nOChildren; i++) {
        freeTree(root->children[i]);
    }
    free(root);
    
}

btnode* getInorderPredecessor(btree* tree, btnode* node, int idx) {
    btnode* child = node->children[idx];
    if (!node->nOChildren) return NULL; // no inorder predecessor
    while (child->nOChildren) {
        child = child->children[child->nOChildren - 1];
    }
    return child;
    // child contains in order predecessor as last key
}
   

int btree_search(btnode* root, int val) {
    int pos = binarySearchPos(root, val);
    if (root->keys[pos] == val) {
        printf("Found %d\n", val);
        return 0;
    }
    else {
        if (root->nOChildren) {
            return btree_search(root->children[pos], val);
        }
        else {
            printf("No key %d in the tree\n", val);
            return 1;
        }
    }

}