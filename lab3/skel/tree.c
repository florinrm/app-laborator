#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NO_THREADS 8

typedef struct binary_search_tree {
	int data;
	struct binary_search_tree *left;
	struct binary_search_tree *right;
} binary_search_tree;

binary_search_tree *insert (binary_search_tree *tree, int data) {
	if (tree == NULL) {
		binary_search_tree *new = malloc(sizeof(binary_search_tree));
		new->data = data;
		new->left = NULL;
		new->right = NULL;
		return new;
	}
	else if (tree->data >= data) {
		tree->left = insert(tree->left, data);
    } else {
		tree->right = insert(tree->right, data);
    }
	return tree;
}

void preorder(binary_search_tree *tree) {
	if (tree == NULL) {
		return;
    }

	printf("%d ", tree->data);
	preorder(tree->left);
	preorder(tree->right);
}

int maximum (int a, int b) {
	return a >= b ? a : b;
}

int height (binary_search_tree *tree) {
	if (tree == NULL)
		return 0;

    int left, right;
    
    left = height(tree->left);

    right = height(tree->right);

	return maximum(left, right) + 1;
}

int main() {
    omp_set_num_threads(NO_THREADS);

    binary_search_tree *tree = NULL;
	tree = insert(tree, 8); 
	tree = insert(tree, 3); 
	tree = insert(tree, 10);
	tree = insert(tree, 1);
	tree = insert(tree, 6);
	tree = insert(tree, 14);
	tree = insert(tree, 4);
	tree = insert(tree, 7);
	tree = insert(tree, 13); 

    preorder(tree);
	printf("\nHeight = %d\n", height(tree));

    return 0;
}
