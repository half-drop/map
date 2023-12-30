#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

typedef struct EdgeNode {
	int adjvex; // 邻接点索引
	int weight; // 边的权重
	struct EdgeNode* next; // 下一条边
} EdgeNode;
typedef struct VertexNode {
	int in; // 顶点的入度（主要用于拓扑排序）
	void* data; // 顶点数据
	EdgeNode* firstEdge; // 边的链表头
} VertexNode;
typedef struct {
	VertexNode* vertexArray; // 顶点数组
	int numVertices; // 顶点数量
	int numEdges; // 边的数量
	int isDirected; // 是否为有向图
} Graph;

Graph* globalGraph = NULL; // 全局图变量

void DFSUtil(Graph* g, int v, int visited[]) {
	visited[v] = 1; // 标记当前顶点为已访问
	printf("%d ", v); // 输出顶点或进行其他操作

	// 遍历所有邻接顶点
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
	int* visited = (int*)calloc(g->numVertices, sizeof(int)); // 创建访问标记数组

	// 从开始顶点调用辅助函数
	DFSUtil(g, startVertex, visited);

	free(visited); // 释放访问标记数组
}

void BFS(Graph* g, int startVertex) {
	int* visited = (int*)calloc(g->numVertices, sizeof(int)); // 创建访问标记数组
	int* queue = (int*)malloc(g->numVertices * sizeof(int)); // 动态分配队列数组

	if (queue == NULL) {
		fprintf(stderr, "内存分配失败\n");
		exit(EXIT_FAILURE);
	}

	int front = 0, rear = 0; // 队列的前端和后端指针

	// 访问起始顶点
	visited[startVertex] = 1;
	queue[rear++] = startVertex; // 入队

	while (front != rear) {
		int v = queue[front++]; // 出队
		printf("%d ", v);

		// 遍历所有邻接顶点
		EdgeNode* temp = g->vertexArray[v].firstEdge;
		while (temp != NULL) {
			int adjV = temp->adjvex;
			if (!visited[adjV]) {
				visited[adjV] = 1;
				queue[rear++] = adjV; // 邻接顶点入队
			}
			temp = temp->next;
		}
	}

	free(queue); // 释放队列数组
	free(visited); // 释放访问标记数组
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

	int* parent = (int*)malloc(V * sizeof(int)); // 存储最小生成树
	int* key = (int*)malloc(V * sizeof(int));    // 保存最小权值的边
	int* mstSet = (int*)malloc(V * sizeof(int)); // 标记顶点是否已在MST中

	// 检查内存分配是否成功
	if (!parent || !key || !mstSet) {
		fprintf(stderr, "内存分配失败\n");
		exit(EXIT_FAILURE);
	}

	// 初始化所有键值为无穷大，mstSet为false
	for (int i = 0; i < V; i++) {
		key[i] = INT_MAX;
		mstSet[i] = 0;
	}

	// 将起始顶点的键值设为0，这样它会被首先选中
	key[startVertex] = 0;
	parent[startVertex] = -1; // 第一个节点总是MST的根

	for (int count = 0; count < V - 1; count++) {
		// 从尚未包含在MST中的顶点集合中选择键值最小的顶点
		int u = minKey(key, mstSet, V);

		// 将选中的顶点添加到MST集合中
		mstSet[u] = 1;

		// 更新相邻顶点的键值和父节点索引
		for (EdgeNode* temp = g->vertexArray[u].firstEdge; temp != NULL; temp = temp->next) {
			int v = temp->adjvex;
			// g->graph[u][v]是从u到v的权值
			if (mstSet[v] == 0 && temp->weight < key[v]) {
				parent[v] = u;
				key[v] = temp->weight;
			}
		}
	}

	// 输出构建的MST
	printf("Edge   Weight\n");
	for (int i = 1; i < V; i++) {
		printf("%d - %d    %d \n", parent[i], i, key[i]);
	}

	// 释放内存
	free(parent);
	free(key);
	free(mstSet);
}

//前置：边、比较权重、查并集查找、查并集合并
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
	int E = 0; // 图中边的数量

	// 计算图中边的数量
	for (int i = 0; i < V; i++) {
		EdgeNode* temp = g->vertexArray[i].firstEdge;
		while (temp != NULL) {
			E++;
			temp = temp->next;
		}
	}
	E /= 2; // 因为无向图中的每条边被计算了两次

	// 动态分配内存以存储所有边
	Edge* edges = (Edge*)malloc(E * sizeof(Edge));
	if (!edges) {
		fprintf(stderr, "内存分配失败\n");
		exit(EXIT_FAILURE);
	}

	// 填充边数组
	int k = 0;
	for (int i = 0; i < V; i++) {
		EdgeNode* temp = g->vertexArray[i].firstEdge;
		while (temp != NULL) {
			if (i < temp->adjvex) { // 避免重复计算无向图中的每条边
				edges[k].src = i;
				edges[k].dest = temp->adjvex;
				edges[k].weight = temp->weight;
				k++;
			}
			temp = temp->next;
		}
	}

	// 对边按权重排序
	qsort(edges, E, sizeof(Edge), compareEdges);

	// 创建结果数组
	Edge* result = (Edge*)malloc((V - 1) * sizeof(Edge));
	if (!result) {
		fprintf(stderr, "内存分配失败\n");
		free(edges);
		exit(EXIT_FAILURE);
	}

	// 创建并查集
	int* parent = (int*)malloc(V * sizeof(int));
	memset(parent, -1, V * sizeof(int));

	// Kruskal算法的核心部分
	int e = 0; // 边的索引
	int r = 0; // 结果数组的索引
	while (r < V - 1 && e < E) {
		Edge next_edge = edges[e++];
		int x = findSet(parent, next_edge.src);
		int y = findSet(parent, next_edge.dest);

		if (x != y) {
			result[r++] = next_edge;
			unionSet(parent, x, y);
		}
	}

	// 打印构成最小生成树的边
	for (int i = 0; i < r; i++) {
		printf("%d -- %d == %d\n", result[i].src, result[i].dest, result[i].weight);
	}

	// 释放内存
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

	// 动态分配内存
	int* dist = (int*)malloc(V * sizeof(int));
	int* sptSet = (int*)malloc(V * sizeof(int));

	if (!dist || !sptSet) {
		fprintf(stderr, "内存分配失败\n");
		exit(EXIT_FAILURE);
	}

	// 初始化所有距离为无限大，sptSet[]为false
	for (int i = 0; i < V; i++)
		dist[i] = INT_MAX, sptSet[i] = 0;

	dist[src] = 0; // 源顶点到自己的距离总是0

	for (int count = 0; count < V - 1; count++) {
		int u = minDistance(dist, sptSet, V);
		sptSet[u] = 1;

		for (EdgeNode* node = g->vertexArray[u].firstEdge; node != NULL; node = node->next) {
			int v = node->adjvex;
			if (!sptSet[v] && dist[u] != INT_MAX && dist[u] + node->weight < dist[v])
				dist[v] = dist[u] + node->weight;
		}
	}

	// 打印构造的最短路径
	printf("Vertex   Distance from Source\n");
	for (int i = 0; i < V; i++)
		printf("%d \t\t %d\n", i, dist[i]);

	// 释放动态分配的内存
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

	// 动态分配内存
	int* inDegree = (int*)malloc(V * sizeof(int));
	int* queue = (int*)malloc(V * sizeof(int));

	if (!inDegree || !queue) {
		fprintf(stderr, "内存分配失败\n");
		// 如果任一分配失败，释放已分配的内存并退出
		if (inDegree) free(inDegree);
		if (queue) free(queue);
		exit(EXIT_FAILURE);
	}

	// 初始化所有顶点的入度为0
	for (int i = 0; i < V; i++) {
		inDegree[i] = 0;
	}

	// 计算每个顶点的入度
	calculateInDegree(g, inDegree);

	// 创建一个队列并将所有入度为0的顶点入队
	int front = 0, rear = 0;
	for (int i = 0; i < V; i++) {
		if (inDegree[i] == 0) {
			queue[rear++] = i;
		}
	}

	// 拓扑排序的主要循环
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
		printf("\n图中存在环，无法进行拓扑排序。\n");
	}

	// 释放动态分配的内存
	free(inDegree);
	free(queue);
}

void inputGraph(Graph* g) {
	printf("输入顶点数和边数:\n");
	scanf("%d %d", &g->numVertices, &g->numEdges);

	// 分配空间和初始化图
	g->vertexArray = (VertexNode*)malloc(g->numVertices * sizeof(VertexNode));
	for (int i = 0; i < g->numVertices; i++) {
		g->vertexArray[i].firstEdge = NULL;
		// 如有需要，还可以初始化其他顶点信息
	}

	printf("输入边的信息（起点 终点 权重）:\n");
	for (int i = 0; i < g->numEdges; i++) {
		int start, end, weight;
		scanf("%d %d %d", &start, &end, &weight);

		// 添加边到图中
		EdgeNode* newEdge = (EdgeNode*)malloc(sizeof(EdgeNode));
		newEdge->adjvex = end;
		newEdge->weight = weight;
		newEdge->next = g->vertexArray[start].firstEdge;
		g->vertexArray[start].firstEdge = newEdge;

		// 如果是无向图，还需要添加另一个方向的边
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
	printf("图的信息：\n");
	printf("顶点数：%d\n", g->numVertices);
	printf("边数：%d\n", g->numEdges);
	printf("是否为有向图：%d\n", g->isDirected);
	printf("顶点信息：\n");
	for (int i = 0; i < g->numVertices; i++) {
		printf("顶点%d：", i);
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
	globalGraph = graph; // 设置全局图变量

	printf("输入图的类型（0为无向图，1为有向图）：");
	scanf("%d", &graph->isDirected);
	inputGraph(graph); // 输入图
	showGraph(graph); // 显示图
	printf("确认无误后，");
	while (1) {
		system("pause");
		showGraph(graph);
		printf("————————————————————————\n");
		printf("1. 深度优先搜索(DFS)\n");
		printf("2. 广度优先搜索(BFS)\n");
		printf("3. Prim算法\n");
		printf("4. Kruskal算法\n");
		printf("5. Dijkstra算法\n");
		printf("6. 拓扑排序\n");
		printf("7. 退出\n");
		printf("————————————————————————\n");
		printf("选择：");

		int choice;
		int startVertex;
		scanf("%d", &choice);

		switch (choice) {
		case 1:
			printf("输入开始搜索的顶点索引：");
			scanf("%d", &startVertex);
			DFS(graph, startVertex);
			printf("\n\n");
			break;
		case 2:
			printf("输入开始搜索的顶点索引：");
			scanf("%d", &startVertex);
			BFS(graph, startVertex);
			printf("\n\n");
			break;
		case 3:
			printf("输入开始顶点的索引：");
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
			printf("退出程序。\n");
			exit(0);
			return 0;
		default:
			printf("无效选择，请重新输入。\n");
		}
	}

	return 0;
}