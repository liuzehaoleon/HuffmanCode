/**
 ------------------------------
 Author:  Zehao Liu
 ID:      193074000
 Email:   liux4000@mylaurier.ca
 Version:  2022-07-04
 ------------------------------
 **/

/*reference:
 * min heap reference => https://www.geeksforgeeks.org/huffman-coding-greedy-algo-3/
 *reverse file => https://hplusacademy.com/print-contents-of-file-in-reverse-order-in-c/
 * */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#include <ctype.h>
#include <string.h>

/*the lenght of all character*/
#define LIST_SIZE 39
/*The forest size to build huffman tree*/
#define FOREST_SIZE (LIST_SIZE * 2 - 1)
/*The maximum size of each code*/
#define CODE_MAX 50
/*Input or output file path length*/
#define PATH_MAX 64
/*file flag*/
const char FILE_HEADER_FLAG[] = { 'F', 'X' };

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

/*min heap*/
typedef struct {
	// Current size of min heap
	unsigned size;
	// capacity of min heap
	unsigned capacity;
	// Array of minheap node pointers
	Node **array;
} MinHeap;

// A utility function to create
// a min heap of given capacity
MinHeap* createMinHeap() {

	MinHeap *minHeap = (MinHeap*) malloc(sizeof(MinHeap));
	// current size is 0
	minHeap->size = 0;
	//total capacity is equal 39 ie LIST_SIZE
	minHeap->capacity = LIST_SIZE;

	minHeap->array = (Node**) malloc(minHeap->capacity * sizeof(Node*));
	return minHeap;
}

// A utility function to
// swap two min heap nodes
void swapMinHeapNode(Node **a, Node **b) {
	Node *t = *a;
	*a = *b;
	*b = t;
}

// The standard minHeapify function.
void minHeapify(MinHeap *minHeap, int idx) {
	int smallest = idx;
	int left = 2 * idx + 1;
	int right = 2 * idx + 2;

	if (left < minHeap->size
			&& minHeap->array[left]->freq < minHeap->array[smallest]->freq)
		smallest = left;

	if (right < minHeap->size
			&& minHeap->array[right]->freq < minHeap->array[smallest]->freq)
		smallest = right;

	if (smallest != idx) {
		swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
		minHeapify(minHeap, smallest);
	}
}

// A utility function to check
// if size of heap is 1 or not
int isSizeOne(MinHeap *minHeap) {
	return (minHeap->size == 1);
}

// A standard function to extract
// minimum value node from heap
Node* popMin(MinHeap *minHeap) {
	if (minHeap->size <= 0)
		return NULL;
	Node *temp = minHeap->array[0];
	minHeap->array[0] = minHeap->array[minHeap->size - 1];

	--minHeap->size;
	minHeapify(minHeap, 0);

	return temp;
}

// A utility function to insert
// a new node to Min Heap
void insertMinHeap(MinHeap *minHeap, Node *node) {
	++minHeap->size;
	int i = minHeap->size - 1;

	while (i && node->freq < minHeap->array[(i - 1) / 2]->freq) {

		minHeap->array[i] = minHeap->array[(i - 1) / 2];
		i = (i - 1) / 2;
	}

	minHeap->array[i] = node;
}

// A standard function to build min heap
void buildMinHeap(MinHeap *minHeap) {
	int n = minHeap->size - 1;
	int i;

	for (i = (n - 1) / 2; i >= 0; --i)
		minHeapify(minHeap, i);
}

/*output file frequency.txt*/
void write_frequency(int times[]) {
	FILE *out_file = fopen("frequency", "w");
	if (out_file != NULL) {
		for (int i = 0; i < LIST_SIZE; i++) {
			if (i == 0)
				fprintf(out_file, "%c:%d\n", ' ', times[i]);
			else if (i == 1)
				fprintf(out_file, "%c:%d\n", ',', times[i]);
			else if (i == 2)
				fprintf(out_file, "%c:%d\n", '.', times[i]);

			else if (3 <= i && i < 13)
				fprintf(out_file, "%c:%d\n", ((char) i + 48 - 3), times[i]);
			else
				fprintf(out_file, "%c:%d\n", ((char) i + 97 - 13), times[i]);
		}
	} else
		printf("Error(write_resultss):invalid file");
	fclose(out_file);
	return;
}

/*trasver all node and insert them into min heap*/
void insert_all_node(MinHeap *minHeap, Node *root, int flag) {
	if (root) {
		if (root->type == NODE_TYPE_DATA) {
			insertMinHeap(minHeap, root);
		}
		insert_all_node(minHeap, root->left, NODE_FLAG_LEFT);
		insert_all_node(minHeap, root->right, NODE_FLAG_RIGHT);
	}
	return;
}

/*output file code.txt*/
void pop_recur(MinHeap *minHeap, FILE *out_file) {
	if (minHeap->size > 0) {
		Node *n = popMin(minHeap);
		pop_recur(minHeap, out_file);
		while (n != NULL) {
			fprintf(out_file, "%c:%s\n", n->data, n->code);
			n = popMin(minHeap);
		}
	}
	return;
}

void write_code(Node *root) {
	FILE *out_file = fopen("codes.txt", "w+");
	MinHeap *minHeap = createMinHeap();
	insert_all_node(minHeap, root, NODE_FLAG_ROOT);

	if (out_file != NULL) {
		pop_recur(minHeap, out_file);
	} else
		printf("Error(write_code):invalid outfile");
	fclose(out_file);
	return;
}

/*create Huffman tree node*/
Node* createDataNode(int index, int freq) { //index range from 0 to LIST_SIZE
	Node *node = (Node*) malloc(sizeof(Node));
	memset(node, 0, sizeof(Node));
	if (index == 0)
		node->data = (int) ' ';
	else if (index == 1)
		node->data = (int) ',';
	else if (index == 2)
		node->data = (int) '.';
	else if (index >= 3 && index < 13)
		node->data = (int) '0' + index - 3;
	else if (index >= 13 && index < 39) {
		node->data = (int) 'a' + index - 13;
//		printf("%c\n", node->data);
	}

	node->type = NODE_TYPE_DATA;
	node->freq = freq;
	return node;
}

/*create huffman tree empty node*/
Node* createBlankNode(int freq) {
	Node *node = (Node*) malloc(sizeof(Node));
	memset(node, 0, sizeof(Node));
	node->type = NODE_TYPE_BLANK;
	node->freq = freq;
	return node;
}

/*add all Huffman node into node list*/
void addNodeToList(Node *nodelist[], int size, Node *node) {
	int index;
	for (index = 0; index < size; ++index) {
		if (nodelist[index] == NULL) {
			nodelist[index] = node;
			break;
		}
	}
}

/*pop the least frequency tree node out*/
Node* popMinNodeFromList(Node *nodelist[], int size) {
	int min = -1;
	int index;
	for (index = 0; index < size; ++index) {
		if (nodelist[index]) {
			if (min == -1) {
				min = index;
			} else {
				if (nodelist[min]->freq > nodelist[index]->freq) {
					/*renew index when found smaller*/
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

/*genertate Huffman code for each node*/
void generateHuffmanCode(Node *root) {
//	printf("newRecursion:");
//	printf("%c\n", root->data);

	if (root) {
		if (root->left) {
//			printf("root left exist\n");
			strcpy(root->left->code, root->code);
			strcat(root->left->code, "0");
			generateHuffmanCode(root->left);
		}
		if (root->right) {
//			printf("root right exist\n");
			strcpy(root->right->code, root->code);
			strcat(root->right->code, "1");
			generateHuffmanCode(root->right);
		}
	}
}

/*build huffman tree by char frequency list*/
Node* buildHuffmanTree(int times[]) {
	Node *nodelist[FOREST_SIZE] = { NULL };
	Node *root = NULL;
	int index;
	/*create huffman tree*/
	for (index = 0; index < LIST_SIZE; ++index) {
		if (times[index] >= 0) {
			/*add all node to tree*/
			addNodeToList(nodelist, FOREST_SIZE,
					createDataNode(index, times[index]));
		}
	}
	/*Build Huffman tree*/
	while (1) {
		Node *left = popMinNodeFromList(nodelist, FOREST_SIZE);
		Node *right = popMinNodeFromList(nodelist, FOREST_SIZE);
		if (right == NULL) {
			/*if there is only one node left, tree built*/
			root = left;
			break;
		} else {
			//the total frequence of left node and right node
			Node *node = createBlankNode(left->freq + right->freq);
			node->left = left;
			node->right = right;
			/*take two minimum from forest and add their back into tree*/
			addNodeToList(nodelist, FOREST_SIZE, node);
		}
	}
	generateHuffmanCode(root);
	return root;
}

/*to free memeory of tree*/
void destroyHuffmanTree(Node *root) {
	if (root) {
		destroyHuffmanTree(root->left);
		destroyHuffmanTree(root->right);
		free(root);
	}
}

/*from input file generate a frequency list time[]*/
int* readFile(char in[]) {
	FILE *input = fopen(in, "rb");
	int *times = malloc(sizeof(int) * LIST_SIZE);
//initalize times array
	for (int i = 0; i < LIST_SIZE; i++)
		times[i] = 0;
	Byte byte;
//read from file and add their frequency to times array
	while (fread(&byte, sizeof(byte), 1, input) == 1) {
		if (isalpha(byte) != 0)
			times[(int) tolower(byte) - 97 + 13]++;
		else if (isdigit(byte)) {
			times[(int) byte - 48 + 3]++;
		} else if (isspace(byte))
			times[0]++;
		else if (byte == ',')
			times[1]++;
		else if (byte == '.')
			times[2]++;
	}
	fclose(input);
	return times;
}

/*convert huffman tree to huffman list*/
void convertTreeToList(Node *root, Node *nodelist[]) {
	if (root) {
		if (root->type == NODE_TYPE_DATA) {
			if (root->data == (int) ' ') {
				nodelist[0] = root;
			} else if (root->data == (int) ',')
				nodelist[1] = root;
			else if (root->data == (int) '.')
				nodelist[2] = root;
			else if (root->data >= (int) '0' && root->data <= (int) '9') {
				nodelist[root->data - 48 + 3] = root;
			} else if (root->data >= (int) 'a' && root->data <= (int) 'z') {
				nodelist[root->data - 97 + 13] = root;
			}
			printf("\n");

		}
		convertTreeToList(root->left, nodelist);
		convertTreeToList(root->right, nodelist);
	}
}

/*return actuall node of list*/
int trimNodeList(Node *nodelist[], int size) {
	int count = 0;

	for (int index = 0; index < size; ++index) {
		Node *node = nodelist[index];
		if (node) {
			nodelist[count++] = node;
		}
	}
	return count;
}

/* write file content*/
int encodeFileData(Node *root, FILE *input, FILE *output) {
	int total = 0;
	int count = 0;
	if (root) {
		Byte byte;
		int buffer = 0;
		Node *nodelist[LIST_SIZE] = { NULL };
		convertTreeToList(root, nodelist);
		while (fread(&byte, sizeof(byte), 1, input) == 1) {

			if (isalpha(byte) != 0) {
				byte = tolower(byte) - 97 + 13;

			} else if (isdigit(byte)) {
				byte = byte - 48 + 3;
			} else if (isspace(byte))
				byte = 0;
			else if (byte == ',')
				byte = 1;
			else if (byte == '.')
				byte = 2;
			else
				continue;
			char *cursor = nodelist[byte]->code;
			while (*cursor) {
				buffer <<= 1;
				if (*cursor == '0') {
					buffer |= 0;
				} else {
					buffer |= 1;
				}
				++count;
//				printf("%d", count);
				if (count == 8) {
					Byte byte = (Byte) buffer;
					fwrite(&byte, sizeof(byte), 1, output);
					count = 0;
					buffer = 0;
					++total;
				}
				++cursor;
			}
		}
		if (count > 0) {
			buffer <<= (8 - count);
			char byte = (char) buffer;
			fwrite(&byte, 1, 1, output);
			++total;
		}
	}
	return total;
}

/*a kind of code.txt information in file make decode easier*/
int writeFileHeader(Node *root, FILE *output) {
	Node *nodelist[LIST_SIZE] = { NULL };
	Byte total = 0;
	int index;
	/*write file header*/
	fwrite(FILE_HEADER_FLAG, 2, 1, output);
	convertTreeToList(root, nodelist);
	total = (Byte) (trimNodeList(nodelist, LIST_SIZE) - 1);
	/*The total number of characters written*/
	fwrite(&total, sizeof(total), 1, output);
	/* Write each character along with the weight */
	for (index = 0; index <= total; ++index) {
		Node *node = nodelist[index];
		Byte byte = (Byte) node->data;
		fwrite(&byte, sizeof(byte), 1, output);
		fwrite(&node->freq, sizeof(node->freq), 1, output);
	}
	/*Returns the total number of header bytes written*/
	return (total * 5 + 1 + 2);
}

/* verify the filie write file header and write file content*/
void encode(Node *root, char in[], char out[]) {
	FILE *input = fopen(in, "rb");
	FILE *output = fopen(out, "wb");
	if (output) {
		writeFileHeader(root, output);
		if (input) {
			encodeFileData(root, input, output);
			fclose(input);
		} else {
			printf("       error (toEncode): Failed to read input file.\n");
		}
		fclose(output);
	} else {
		printf("       error (toEncode): Failed to write file.\n");
		fclose(input); //since input file haven't being closed yet since we
	}
	printf("\n");
}

/*检测是否是可显示字符*/
int canShowChar(char ch) {
	return (ch > 32 && ch < 127);
}
/*通过递归遍历方式打印Huffman树*/
void outputHuffmanTree(FILE *output, Node *root, int flag) {
	if (root) {
		int index;
		char content[128] = { 0 };
		const char *flagname[] = { "ROOT", "LEFT", "RIGHT" };
		int offset = (int) strlen(root->code) - 1;
		for (index = 0; index < offset; ++index) {
			if (root->code[index] == '0') {
				fprintf(output, " │ ");
			} else {
				fprintf(output, "   ");
			}
		}
		switch (root->type) {
		case NODE_TYPE_DATA:
			sprintf(content, "> %-6s #%-4d 0x%02X : '%c'", flagname[flag],
					root->freq, root->data,
					canShowChar((char )root->data) ? root->data : ' ');
			break;
		case NODE_TYPE_BLANK:
			sprintf(content, "[+] %-6s #%-4d", flagname[flag], root->freq);
			break;
		}
		switch (flag) {
		case NODE_FLAG_ROOT:
			fprintf(output, "%s", content);
			break;
		case NODE_FLAG_LEFT:
			fprintf(output, " ├─%s", content);
			break;
		case NODE_FLAG_RIGHT:
			fprintf(output, " └─%s", content);
			break;
		}
		if (root->type == NODE_TYPE_DATA) {
			fprintf(output, " CODE : %s\n", root->code);
		} else {
			fprintf(output, "\n");
		}
		outputHuffmanTree(output, root->left, NODE_FLAG_LEFT);
		outputHuffmanTree(output, root->right, NODE_FLAG_RIGHT);
	}
}

int main() {
//	printf("here");

	char source_file[PATH_MAX];
	scanf("%s", source_file);
	int *times = readFile(source_file);

	Node *root = buildHuffmanTree(times);
	printf("here");

	FILE *o = fopen("test.txt", "w+");
	outputHuffmanTree(o, root, NODE_TYPE_BLANK);
	fclose(o);

	//	for (int i = 0; i < 39; i++)
	//		printf("%d\n", times[i]);
	//task1
	write_frequency(times);
	//task2
	write_code(root);
	//task3
	encode(root, source_file, "compressed.bin");

	destroyHuffmanTree(root);
	return 0;
}

