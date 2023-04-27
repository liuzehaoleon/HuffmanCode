/**
 ------------------------------
 Author:  Zehao Liu
 ID:      193074000
 Email:   liux4000@mylaurier.ca
 Version:  2022-07-04
 ------------------------------
 **/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#define CHR_SIZE 256
#define FOREST_SIZE (CHR_SIZE * 2 - 1)
#define CODE_MAX 12
#define PATH_MAX 64

/*node flag*/
enum {
	NODE_FLAG_ROOT, NODE_FLAG_LEFT, NODE_FLAG_RIGHT
};

/*node type*/
enum {
	NODE_TYPE_DATA, /*Node with data*/
	NODE_TYPE_BLANK /*blank Node*/
};

typedef unsigned char Byte;

/*Huffman tree node*/
typedef struct tNode {
	int type;
	int data;
	int freq;
	char code[CODE_MAX]; /*Huffman code base on all element and their input*/
	struct tNode *left; /*left child*/
	struct tNode *right; /*right child*/
} Node;

/*Huffman tree*/
typedef struct {
	Node *root; /*root node*/
	int total /*total node*/;
} HuffmanTree;

Node* createDataNode(int data, int freq) {
	Node *node = (Node*) malloc(sizeof(Node));
	memset(node, 0, sizeof(Node));
	node->type = NODE_TYPE_DATA;
	node->data = data;
	node->freq = freq;
	return node;
}

Node* createBlankNode(int freq) {
	Node *node = (Node*) malloc(sizeof(Node));
	memset(node, 0, sizeof(Node));
	node->type = NODE_TYPE_BLANK;
	node->freq = freq;
	return node;
}

void addNodeToList(Node *nodelist[], int size, Node *node) {
	int index;
	for (index = 0; index < size; ++index) {
		if (nodelist[index] == NULL) {
			nodelist[index] = node;
			break;
		}
	}
}

Node* popMinNodeFromList(Node *nodelist[], int size) {
	int min = -1;
	int index;
	for (index = 0; index < size; ++index) {
		if (nodelist[index]) {
			if (min == -1) {
				min = index;
			} else {
				if (nodelist[min]->freq > nodelist[index]->freq) {
					min = index;
				}
			}
		}
	}
	if (min != -1) {
		Node *node = nodelist[min];
		nodelist[min] = NULL;
		return node;
	}
	return NULL;
}

void generateHuffmanCode(Node *root) {
	if (root) {
		if (root->left) {
			strcpy(root->left->code, root->code);
			strcat(root->left->code, "0");
			generateHuffmanCode(root->left);
		}
		if (root->right) {
			strcpy(root->right->code, root->code);
			strcat(root->right->code, "1");
			generateHuffmanCode(root->right);
		}
	}
}

Node* buildHuffmanTree(int times[]) {
	Node *nodelist[FOREST_SIZE] = { NULL };
	Node *root = NULL;
	int index;
	for (index = 0; index < CHR_SIZE; ++index) {
		if (times[index] > 0) {
			addNodeToList(nodelist, FOREST_SIZE,
					createDataNode(index, times[index]));
		}
	}
	while (1) {
		Node *left = popMinNodeFromList(nodelist, FOREST_SIZE);
		Node *right = popMinNodeFromList(nodelist, FOREST_SIZE);
		if (right == NULL) {
			root = left;
			break;
		} else {
			Node *node = createBlankNode(left->freq + right->freq);
			node->left = left;
			node->right = right;
			addNodeToList(nodelist, FOREST_SIZE, node);
		}
	}
	generateHuffmanCode(root);
	return root;
}

void destroyHuffmanTree(Node *root) {
	if (root) {
		destroyHuffmanTree(root->left);
		destroyHuffmanTree(root->right);
		free(root);
	}
}

int countHuffmanTreefreqTotal(Node *root) {
	int total = 0;
	if (root) {
		if (root->type == NODE_TYPE_DATA) {
			total = root->freq;
		}
		total += countHuffmanTreefreqTotal(root->left);
		total += countHuffmanTreefreqTotal(root->right);
	}
	return total;
}

void decodeFileData(Node *root, FILE *input, FILE *output, int count) {
	if (root) {
		Byte byte;
		Node *currentNode = root;
		while (fread(&byte, sizeof(byte), 1, input) == 1) {
			int buffer = byte;
			int index;
			for (index = 0; index < 8; ++index) {
				buffer <<= 1;
				if (!currentNode->left || !currentNode->right) {
					Byte data = (Byte) currentNode->data;
					fwrite(&data, sizeof(data), 1, output);
					if (--count == 0) {
						break;
					}
					currentNode = root;
				}
				if (buffer & ~0xff) {
					currentNode = currentNode->right;
				} else {
					currentNode = currentNode->left;
				}
				buffer &= 0xff;
			}
		}
	}
}

void readFileHeader(FILE *input, int times[]) {
	Byte total;
	int index;
	fseek(input, 2, SEEK_CUR);
	fread(&total, sizeof(total), 1, input);
	for (index = 0; index <= total; ++index) {
		Byte byte;
		int freq;
		fread(&byte, sizeof(byte), 1, input);
		fread(&freq, sizeof(freq), 1, input);
		times[byte] = freq;
	}
}

void toDecode(Node *root, FILE *input) {
	char filename[PATH_MAX] = "decode.txt";
	FILE *output = NULL;
	output = fopen(filename, "wb");
	if (output) {
		decodeFileData(root, input, output, countHuffmanTreefreqTotal(root));
		fclose(output);
	} else {
		printf("       error : Failed to write file.\n");
	}
	printf("\n");
}

/*decode both header and file content*/
void decode(const char filename[]) {
	FILE *input = fopen(filename, "rb");
	if (input) {
		int tell = 0;
		int times[CHR_SIZE] = { 0 };
		readFileHeader(input, times);
		tell = (int) ftell(input);
		Node *root = buildHuffmanTree(times);
		if (root) {
			do {
				fseek(input, tell, SEEK_SET);
				toDecode(root, input);
				break;
			} while (1);
			destroyHuffmanTree(root);
		} else {
			printf("       error : Failed to build Huffman tree.\n");
		}
		fclose(input);
	} else {
		printf("       error : Failed to read file.\n");
	}
}

int main() {
	//task4
	decode("compressed.bin");
	return 0;
}
