#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

typedef struct EdgeNode {
	int adjvex; // �ڽӵ�����
	int weight; // �ߵ�Ȩ��
	struct EdgeNode* next; // ��һ����
} EdgeNode;
typedef struct VertexNode {
	int in; // �������ȣ���Ҫ������������
	void* data; // ��������
	EdgeNode* firstEdge; // �ߵ�����ͷ
} VertexNode;
typedef struct {
	VertexNode* vertexArray; // ��������
	int numVertices; // ��������
	int numEdges; // �ߵ�����
	int isDirected; // �Ƿ�Ϊ����ͼ
} Graph;

Graph* globalGraph = NULL; // ȫ��ͼ����

void DFSUtil(Graph* g, int v, int visited[]) {
	visited[v] = 1; // ��ǵ�ǰ����Ϊ�ѷ���
	printf("%d ", v); // �������������������

	// ���������ڽӶ���
	EdgeNode* temp = g->vertexArray[v].firstEdge;
	while (temp != NULL) {
		int adjV = temp->adjvex;
		if (!visited[adjV]) {
			DFSUtil(g, adjV, visited);
		}
		temp = temp->next;
	}
}
void DFS(Graph* g, int startVertex) {
	int* visited = (int*)calloc(g->numVertices, sizeof(int)); // �������ʱ������

	// �ӿ�ʼ������ø�������
	DFSUtil(g, startVertex, visited);

	free(visited); // �ͷŷ��ʱ������
}

void BFS(Graph* g, int startVertex) {
	int* visited = (int*)calloc(g->numVertices, sizeof(int)); // �������ʱ������
	int* queue = (int*)malloc(g->numVertices * sizeof(int)); // ��̬�����������

	if (queue == NULL) {
		fprintf(stderr, "�ڴ����ʧ��\n");
		exit(EXIT_FAILURE);
	}

	int front = 0, rear = 0; // ���е�ǰ�˺ͺ��ָ��

	// ������ʼ����
	visited[startVertex] = 1;
	queue[rear++] = startVertex; // ���

	while (front != rear) {
		int v = queue[front++]; // ����
		printf("%d ", v);

		// ���������ڽӶ���
		EdgeNode* temp = g->vertexArray[v].firstEdge;
		while (temp != NULL) {
			int adjV = temp->adjvex;
			if (!visited[adjV]) {
				visited[adjV] = 1;
				queue[rear++] = adjV; // �ڽӶ������
			}
			temp = temp->next;
		}
	}

	free(queue); // �ͷŶ�������
	free(visited); // �ͷŷ��ʱ������
}

int minKey(int key[], int mstSet[], int V) {
	int min = INT_MAX, min_index;
	for (int v = 0; v < V; v++)
		if (mstSet[v] == 0 && key[v] < min)
			min = key[v], min_index = v;
	return min_index;
}
void Prim(Graph* g, int startVertex) {
	int V = g->numVertices;

	int* parent = (int*)malloc(V * sizeof(int)); // �洢��С������
	int* key = (int*)malloc(V * sizeof(int));    // ������СȨֵ�ı�
	int* mstSet = (int*)malloc(V * sizeof(int)); // ��Ƕ����Ƿ�����MST��

	// ����ڴ�����Ƿ�ɹ�
	if (!parent || !key || !mstSet) {
		fprintf(stderr, "�ڴ����ʧ��\n");
		exit(EXIT_FAILURE);
	}

	// ��ʼ�����м�ֵΪ�����mstSetΪfalse
	for (int i = 0; i < V; i++) {
		key[i] = INT_MAX;
		mstSet[i] = 0;
	}

	// ����ʼ����ļ�ֵ��Ϊ0���������ᱻ����ѡ��
	key[startVertex] = 0;
	parent[startVertex] = -1; // ��һ���ڵ�����MST�ĸ�

	for (int count = 0; count < V - 1; count++) {
		// ����δ������MST�еĶ��㼯����ѡ���ֵ��С�Ķ���
		int u = minKey(key, mstSet, V);

		// ��ѡ�еĶ�����ӵ�MST������
		mstSet[u] = 1;

		// �������ڶ���ļ�ֵ�͸��ڵ�����
		for (EdgeNode* temp = g->vertexArray[u].firstEdge; temp != NULL; temp = temp->next) {
			int v = temp->adjvex;
			// g->graph[u][v]�Ǵ�u��v��Ȩֵ
			if (mstSet[v] == 0 && temp->weight < key[v]) {
				parent[v] = u;
				key[v] = temp->weight;
			}
		}
	}

	// ���������MST
	printf("Edge   Weight\n");
	for (int i = 1; i < V; i++) {
		printf("%d - %d    %d \n", parent[i], i, key[i]);
	}

	// �ͷ��ڴ�
	free(parent);
	free(key);
	free(mstSet);
}

//ǰ�ã��ߡ��Ƚ�Ȩ�ء��鲢�����ҡ��鲢���ϲ�
typedef struct {
	int src, dest, weight;
} Edge;
int compareEdges(const void* a, const void* b) {
	Edge* edgeA = (Edge*)a;
	Edge* edgeB = (Edge*)b;
	return edgeA->weight - edgeB->weight;
}
int findSet(int parent[], int i) {
	if (parent[i] == -1)
		return i;
	return findSet(parent, parent[i]);
}
void unionSet(int parent[], int x, int y) {
	int xset = findSet(parent, x);
	int yset = findSet(parent, y);
	if (xset != yset) {
		parent[xset] = yset;
	}
}
void Kruskal(Graph* g) {
	int V = g->numVertices;
	int E = 0; // ͼ�бߵ�����

	// ����ͼ�бߵ�����
	for (int i = 0; i < V; i++) {
		EdgeNode* temp = g->vertexArray[i].firstEdge;
		while (temp != NULL) {
			E++;
			temp = temp->next;
		}
	}
	E /= 2; // ��Ϊ����ͼ�е�ÿ���߱�����������

	// ��̬�����ڴ��Դ洢���б�
	Edge* edges = (Edge*)malloc(E * sizeof(Edge));
	if (!edges) {
		fprintf(stderr, "�ڴ����ʧ��\n");
		exit(EXIT_FAILURE);
	}

	// ��������
	int k = 0;
	for (int i = 0; i < V; i++) {
		EdgeNode* temp = g->vertexArray[i].firstEdge;
		while (temp != NULL) {
			if (i < temp->adjvex) { // �����ظ���������ͼ�е�ÿ����
				edges[k].src = i;
				edges[k].dest = temp->adjvex;
				edges[k].weight = temp->weight;
				k++;
			}
			temp = temp->next;
		}
	}

	// �Ա߰�Ȩ������
	qsort(edges, E, sizeof(Edge), compareEdges);

	// �����������
	Edge* result = (Edge*)malloc((V - 1) * sizeof(Edge));
	if (!result) {
		fprintf(stderr, "�ڴ����ʧ��\n");
		free(edges);
		exit(EXIT_FAILURE);
	}

	// �������鼯
	int* parent = (int*)malloc(V * sizeof(int));
	memset(parent, -1, V * sizeof(int));

	// Kruskal�㷨�ĺ��Ĳ���
	int e = 0; // �ߵ�����
	int r = 0; // ������������
	while (r < V - 1 && e < E) {
		Edge next_edge = edges[e++];
		int x = findSet(parent, next_edge.src);
		int y = findSet(parent, next_edge.dest);

		if (x != y) {
			result[r++] = next_edge;
			unionSet(parent, x, y);
		}
	}

	// ��ӡ������С�������ı�
	for (int i = 0; i < r; i++) {
		printf("%d -- %d == %d\n", result[i].src, result[i].dest, result[i].weight);
	}

	// �ͷ��ڴ�
	free(edges);
	free(result);
	free(parent);
}

int minDistance(int dist[], int sptSet[], int V) {
	int min = INT_MAX, min_index;
	for (int v = 0; v < V; v++)
		if (sptSet[v] == 0 && dist[v] <= min)
			min = dist[v], min_index = v;
	return min_index;
}
void Dijkstra(Graph* g, int src) {
	int V = g->numVertices;

	// ��̬�����ڴ�
	int* dist = (int*)malloc(V * sizeof(int));
	int* sptSet = (int*)malloc(V * sizeof(int));

	if (!dist || !sptSet) {
		fprintf(stderr, "�ڴ����ʧ��\n");
		exit(EXIT_FAILURE);
	}

	// ��ʼ�����о���Ϊ���޴�sptSet[]Ϊfalse
	for (int i = 0; i < V; i++)
		dist[i] = INT_MAX, sptSet[i] = 0;

	dist[src] = 0; // Դ���㵽�Լ��ľ�������0

	for (int count = 0; count < V - 1; count++) {
		int u = minDistance(dist, sptSet, V);
		sptSet[u] = 1;

		for (EdgeNode* node = g->vertexArray[u].firstEdge; node != NULL; node = node->next) {
			int v = node->adjvex;
			if (!sptSet[v] && dist[u] != INT_MAX && dist[u] + node->weight < dist[v])
				dist[v] = dist[u] + node->weight;
		}
	}

	// ��ӡ��������·��
	printf("Vertex   Distance from Source\n");
	for (int i = 0; i < V; i++)
		printf("%d \t\t %d\n", i, dist[i]);

	// �ͷŶ�̬������ڴ�
	free(dist);
	free(sptSet);
}

void calculateInDegree(Graph* g, int* inDegree) {
	for (int i = 0; i < g->numVertices; i++) {
		EdgeNode* temp = g->vertexArray[i].firstEdge;
		while (temp) {
			inDegree[temp->adjvex]++;
			temp = temp->next;
		}
	}
}
void TopologicalSort(Graph* g) {
	int V = g->numVertices;

	// ��̬�����ڴ�
	int* inDegree = (int*)malloc(V * sizeof(int));
	int* queue = (int*)malloc(V * sizeof(int));

	if (!inDegree || !queue) {
		fprintf(stderr, "�ڴ����ʧ��\n");
		// �����һ����ʧ�ܣ��ͷ��ѷ�����ڴ沢�˳�
		if (inDegree) free(inDegree);
		if (queue) free(queue);
		exit(EXIT_FAILURE);
	}

	// ��ʼ�����ж�������Ϊ0
	for (int i = 0; i < V; i++) {
		inDegree[i] = 0;
	}

	// ����ÿ����������
	calculateInDegree(g, inDegree);

	// ����һ�����в����������Ϊ0�Ķ������
	int front = 0, rear = 0;
	for (int i = 0; i < V; i++) {
		if (inDegree[i] == 0) {
			queue[rear++] = i;
		}
	}

	// �����������Ҫѭ��
	int count = 0;
	while (front != rear) {
		int u = queue[front++];
		printf("%d ", u);
		count++;

		for (EdgeNode* temp = g->vertexArray[u].firstEdge; temp; temp = temp->next) {
			int v = temp->adjvex;
			if (--inDegree[v] == 0) {
				queue[rear++] = v;
			}
		}
	}

	if (count != V) {
		printf("\nͼ�д��ڻ����޷�������������\n");
	}

	// �ͷŶ�̬������ڴ�
	free(inDegree);
	free(queue);
}

void inputGraph(Graph* g) {
	printf("���붥�����ͱ���:\n");
	scanf("%d %d", &g->numVertices, &g->numEdges);

	// ����ռ�ͳ�ʼ��ͼ
	g->vertexArray = (VertexNode*)malloc(g->numVertices * sizeof(VertexNode));
	for (int i = 0; i < g->numVertices; i++) {
		g->vertexArray[i].firstEdge = NULL;
		// ������Ҫ�������Գ�ʼ������������Ϣ
	}

	printf("����ߵ���Ϣ����� �յ� Ȩ�أ�:\n");
	for (int i = 0; i < g->numEdges; i++) {
		int start, end, weight;
		scanf("%d %d %d", &start, &end, &weight);

		// ��ӱߵ�ͼ��
		EdgeNode* newEdge = (EdgeNode*)malloc(sizeof(EdgeNode));
		newEdge->adjvex = end;
		newEdge->weight = weight;
		newEdge->next = g->vertexArray[start].firstEdge;
		g->vertexArray[start].firstEdge = newEdge;

		// ���������ͼ������Ҫ�����һ������ı�
		if (!g->isDirected) {
			EdgeNode* newEdge2 = (EdgeNode*)malloc(sizeof(EdgeNode));
			newEdge2->adjvex = start;
			newEdge2->weight = weight;
			newEdge2->next = g->vertexArray[end].firstEdge;
			g->vertexArray[end].firstEdge = newEdge2;
		}
	}
}
void showGraph(Graph* g) {
	system("cls");
	printf("ͼ����Ϣ��\n");
	printf("��������%d\n", g->numVertices);
	printf("������%d\n", g->numEdges);
	printf("�Ƿ�Ϊ����ͼ��%d\n", g->isDirected);
	printf("������Ϣ��\n");
	for (int i = 0; i < g->numVertices; i++) {
		printf("����%d��", i);
		EdgeNode* edge = g->vertexArray[i].firstEdge;
		while (edge != NULL) {
			printf("->%d(%d)", edge->adjvex, edge->weight);
			edge = edge->next;
		}
		printf("\n");
	}
}

int main() {
	Graph* graph = (Graph*)malloc(sizeof(Graph));
	globalGraph = graph; // ����ȫ��ͼ����

	printf("����ͼ�����ͣ�0Ϊ����ͼ��1Ϊ����ͼ����");
	scanf("%d", &graph->isDirected);
	inputGraph(graph); // ����ͼ
	showGraph(graph); // ��ʾͼ
	printf("ȷ�������");
	while (1) {
		system("pause");
		showGraph(graph);
		printf("������������������������������������������������\n");
		printf("1. �����������(DFS)\n");
		printf("2. �����������(BFS)\n");
		printf("3. Prim�㷨\n");
		printf("4. Kruskal�㷨\n");
		printf("5. Dijkstra�㷨\n");
		printf("6. ��������\n");
		printf("7. �˳�\n");
		printf("������������������������������������������������\n");
		printf("ѡ��");

		int choice;
		int startVertex;
		scanf("%d", &choice);

		switch (choice) {
		case 1:
			printf("���뿪ʼ�����Ķ���������");
			scanf("%d", &startVertex);
			DFS(graph, startVertex);
			printf("\n\n");
			break;
		case 2:
			printf("���뿪ʼ�����Ķ���������");
			scanf("%d", &startVertex);
			BFS(graph, startVertex);
			printf("\n\n");
			break;
		case 3:
			printf("���뿪ʼ�����������");
			scanf("%d", &startVertex);
			Prim(graph, startVertex);
			break;
		case 4:
			Kruskal(graph);
			printf("\n\n");
			break;
		case 5:
			Dijkstra(graph, startVertex);
			printf("\n\n");
			break;
		case 6:
			TopologicalSort(graph);
			printf("\n\n");
			break;
		case 7:
			printf("�˳�����\n");
			exit(0);
			return 0;
		default:
			printf("��Чѡ�����������롣\n");
		}
	}

	return 0;
}