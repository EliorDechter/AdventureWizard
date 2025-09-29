#if 0
/*
 * ===============================================================
 *
 *                          PATHFINDING
 *
 * ===============================================================
 */

#define MAX_NUM_WALKING_AREA_VERTICES 128

typedef struct PathfindingSystem {
	//Vector2 num_nodes;

	NodePos* current_path_buff;
	int next_pos_index;
	//Vector2 end_pos;
	//Vector2 original_pos;
	bool is_moving;
	NodePos walking_area_vertices[MAX_NUM_WALKING_AREA_VERTICES];
	int num_vertices;

	struct { NodePos key; int value; } *obstacle_map;
} PathFindingSystem;

PathFindingSystem pathfinding_system;


void AddWalkingAreaVertex(NodePos node) {
	if (pathfinding_system.walking_area_vertices < MAX_NUM_WALKING_AREA_VERTICES - 1);
	pathfinding_system.walking_area_vertices[pathfinding_system.num_vertices++] = node;

}


NodePos GetNode(Vector2i pos) {
	assert(pos.x < NUM_NODES_X);
	assert(pos.y < NUM_NODES_Y);
	assert(pos.y < NUM_NODES_Y);
	assert(pos.y < NUM_NODES_Y);

	NodePos node = { pos };

	return node;
}

PixelPos GetPixelPosFromNodePos(NodePos node) {

	float width = GAME_SCREEN_WIDTH / NUM_NODES_X;
	float x = width * node.pos.x + width;

	float height = GAME_SCREEN_HEIGHT / NUM_NODES_Y;
	float y = height * node.pos.y + height;

	PixelPos result = (PixelPos){ x, y };

#if 0
	assert(result.pos.x < GAME_SCREEN_WIDTH);
	assert(result.pos.y < GAME_SCREEN_HEIGHT);
	assert(result.pos.x > 0);
	assert(result.pos.y > 0);
#endif

	return result;
}

void LoadPathFindingSystem() {
	//NOTE: must be before loading the vertices
	/*pathfinding_system = (PathFindingSystem){
		.num_nodes = (Vector2) {num_nodes_x, num_nodes_y}
	};*/
}

bool HasReachedPoint(PixelPos new_pos, PixelPos end_pos) {
	if (abs(end_pos.pos.x - new_pos.pos.x) < 0.01 && abs(end_pos.pos.y - new_pos.pos.y) < 0.01) return true;
	return false;
}

PixelPos Vector2iToPixelPos(Vector2i v) {
	return (PixelPos) { v };
}

NodePos GetNearestNodeNumFromPixelPos(PixelPos pos) {
	assert(pos.pos.x > 0 && pos.pos.y > 0 && pos.pos.x < GAME_SCREEN_WIDTH && pos.pos.y < GAME_SCREEN_HEIGHT);

	float width = GAME_SCREEN_WIDTH / NUM_NODES_X;
	int x = pos.pos.x / width;

	float height = GAME_SCREEN_HEIGHT / NUM_NODES_Y;
	int y = pos.pos.y / height;

	NodePos result = { x , y };

	return result;
}

bool IsMouseInsideGameScreen() {
	bool result = input_system.mouse_pos.x > GAME_SCREEN_X && input_system.mouse_pos.x > GAME_SCREEN_Y && input_system.mouse_pos.x < GAME_SCREEN_WIDTH && input_system.mouse_pos.y < GAME_SCREEN_HEIGHT;

	return result;
}

void DoWalkingAreaVertices() {

	if (!g_start_drawing && g_is_drawing) {
		g_is_drawing = false;
	}

	if (g_start_drawing) {
		if (!g_is_drawing && pathfinding_system.num_vertices != 0) {
			pathfinding_system.num_vertices = 0;
		}
		g_is_drawing = true;
	}

	if (g_is_drawing) {
		if (input_system.input_event == InputEvent_LeftClickPressed && IsMouseInsideGameScreen()) {
			AddWalkingAreaVertex(GetNearestNodeNumFromPixelPos(Vector2iToPixelPos((Vector2i) { input_system.mouse_pos.x, input_system.mouse_pos.y })));
		}
	}
}

bool IsObstacle(NodePos neighbor) {
	if (hmgeti(pathfinding_system.obstacle_map, neighbor) != -1)
		return true;

	NodePos right = (NodePos){ neighbor.pos.x + 1, neighbor.pos.y };
	NodePos left = (NodePos){ neighbor.pos.x - 1, neighbor.pos.y };
	NodePos down = (NodePos){ neighbor.pos.x, neighbor.pos.y - 1 };
	NodePos up = (NodePos){ neighbor.pos.x, neighbor.pos.y + 1 };

	if (hmgeti(pathfinding_system.obstacle_map, right) != -1 &&
		hmgeti(pathfinding_system.obstacle_map, down) != -1)
		return true;

	if (hmgeti(pathfinding_system.obstacle_map, up) != -1 &&
		hmgeti(pathfinding_system.obstacle_map, right) != -1)
		return true;

	if (hmgeti(pathfinding_system.obstacle_map, up) != -1 &&
		hmgeti(pathfinding_system.obstacle_map, left) != -1)
		return true;

	if (hmgeti(pathfinding_system.obstacle_map, left) != -1 &&
		hmgeti(pathfinding_system.obstacle_map, down) != -1)
		return true;

	if (neighbor.pos.x < 0 || neighbor.pos.y < 0 || neighbor.pos.x > GAME_SCREEN_WIDTH || neighbor.pos.y > GAME_SCREEN_HEIGHT)
		return true;

	return false;
}

float GetDistance(Vector2 pos0, Vector2 pos1) {

	float dx = fabs(pos0.x - pos1.x);
	float dy = fabs(pos0.y - pos1.y);
	//float result = dx + dy - (sqrt(2) - 2) * fmin(dx, dy);
	//float result = sqrt(pow(dx, 2) + pow(dy, 2));
	float result = sqrt(pow(dx, 2) + pow(dy, 2));

	return result;
}

float Heuristic(Vector2 pos0, Vector2 pos1) {
	float dx = fabs(pos0.x - pos1.x);
	float dy = fabs(pos0.y - pos1.y);
	float d2 = sqrt(pow(dx, 2) + pow(dy, 2));
	//float result = fabs(pos0.x - pos1.x) + fabs(pos1.y - pos1.y);
	float d1 = 1;
	float result = (dx + dy) * d1 + (d2 - 2 * d1) * fmin(dx, dy);

	return result;
}

NodePos* nodess;

Vector2* GetPath(NodePos origin_node, NodePos dest_node) {

	assert(!IsObstacle(origin_node));
	assert(!IsObstacle(dest_node));
#if FIXE_ME

	//initialize open list which contains nodes that may be expanded
	struct { Vector2 key; int value; } *open_map = 0;
	hmput(open_map, origin_node.pos, 0);

	//g map
	struct { Vector2 key; float value; } *g_map = 0;
	hmput(g_map, origin_node.pos, 0);

	//f map
	struct { Vector2 key; float value; } *f_map = 0;
	hmput(f_map, origin_node.pos, Heuristic(origin_node.pos, dest_node.pos));

	//initialize came from map
	struct { Vector2 key; Vector2 value; } *came_from_map = 0;

	while (hmlen(open_map)) {
		//current node
		Vector2 current_pos = origin_node.pos;

		//search for node with lowest f
		float min_f = FLT_MAX;

		for (int i = 0; i < hmlen(open_map); ++i) {
			float f;
			if (hmget(f_map, open_map[i].key) == -1) {
				f = FLT_MAX;
			}
			else {
				f = hmget(f_map, open_map[i].key);
			}
			if (f < min_f) {
				min_f = f;
				current_pos = open_map[i].key;
			}
		}


		assert(hmgeti(pathfinding_system.obstacle_map, current_pos) == -1);

		//remove node with lowest f from open list
		hmdel(open_map, current_pos);

		//check if we are at the destination
		if (Vector2Equals(current_pos, dest_node.pos)) {
			Vector2* path_reversed = 0;
			arrput(path_reversed, dest_node.pos);
			Vector2 current_pos = dest_node.pos;
			while (!Vector2Equals(current_pos, origin_node.pos)) {
				Vector2 previous_pos = hmget(came_from_map, current_pos);
				arrput(path_reversed, previous_pos);
				current_pos = previous_pos;
			}

			Vector2* path = 0;
			for (int i = arrlen(path_reversed) - 1; i >= 0; --i) {
				arrput(path, path_reversed[i]);
			}

			return path;
		}

		//get neighbors
		Vector2 neighbors[] = {
			{current_pos.x, current_pos.y + 1},
			{current_pos.x + 1 , current_pos.y + 1},
			{current_pos.x + 1, current_pos.y},
			{current_pos.x + 1, current_pos.y - 1},
			{current_pos.x, current_pos.y - 1},
			{current_pos.x - 1, current_pos.y - 1},
			{current_pos.x - 1, current_pos.y },
			{current_pos.x - 1, current_pos.y + 1},
		};

		for (int i = 0; i < 8; ++i) {

			if (IsObstacle((NodePos) { neighbors[i] })) {
				arrput(nodess, (NodePos) { neighbors[i] });
				continue;
			}

			//get distance to neighbor from current node
			float cost_to_neighbor = hmget(g_map, current_pos) + GetDistance(current_pos, neighbors[i]);

			//get g
			float g;

			if (hmgeti(g_map, neighbors[i]) == -1) {
				hmput(g_map, neighbors[i], g);
				g = FLT_MAX;
			}
			else {
				g = hmget(g_map, neighbors[i]);
			}

			//if new distance is shorter than the distance to  neighbor
			if (cost_to_neighbor < g) {
				/*printf("neighbor  %f %f: came from %f %f\n", neighbors[i].x, neighbors[i].y, current_pos.x, current_pos.y);
				printf("old and new g %f %f \n", g, cost_to_neighbor);*/

				//set prevous node
				if (hmgeti(came_from_map, neighbors[i]) == -1) {
					hmput(came_from_map, neighbors[i], current_pos);
				}
				else {
					hmgetp(came_from_map, neighbors[i])->value = current_pos;
				}

				//set new g
				hmgetp(g_map, neighbors[i])->value = cost_to_neighbor;

				//set new f
				if (hmgeti(f_map, neighbors[i]) == -1) {
					hmput(f_map, neighbors[i], cost_to_neighbor + Heuristic(neighbors[i], dest_node.pos));
				}
				else {
					hmgetp(f_map, neighbors[i])->value = cost_to_neighbor + Heuristic(neighbors[i], dest_node.pos);
				}


				if (hmgeti(open_map, neighbors[i]) == -1) {
					hmput(open_map, neighbors[i], 0);
				}
			}

		}
	}
#endif
	assert(0);
}

void DrawWalkingArea() {
#if FIX_ME
	//draw lines and vertices
	int _x = arrlen(pathfinding_system.walking_area_vertices);
	for (int i = 0; i < arrlen(pathfinding_system.walking_area_vertices); ++i) {
		PixelPos v0 = GetPixelPosFromNodePos(pathfinding_system.walking_area_vertices[i]).pos;
		PixelPos v1 = GetPixelPosFromNodePos(pathfinding_system.walking_area_vertices[(i + 1) %
			arrlen(pathfinding_system.walking_area_vertices)]).pos;
		DrawLineEx((Vector2) { v0.pos.x, v0.pos.y }, (Vector2) { v1.pos.x, v1.pos.y }, 1, RED);
		DrawPlatformRect(v0.x - 5, v0.y - 5, 10, 10, RED);
	}

	//draw nodes
	for (int i = 0; i < NUM_NODES_X - 1; ++i) {
		for (int j = 0; j < NUM_NODES_Y - 1; ++j) {
			PixelPos n = GetPixelPosFromNodePos((NodePos) { i, j });
			Color color = GRAY;
			color.a = 120;
			DrawCircle(n.pos.x, n.pos.y, 1, color);
		}
	}
#endif
}

PixelPos GetNearestPixelPosFromNodePos(NodePos pos) {
	float width = GAME_SCREEN_WIDTH / NUM_NODES_X;
	int x = pos.pos.x / width;

	float height = GAME_SCREEN_HEIGHT / NUM_NODES_Y;
	int y = pos.pos.y / height;

	PixelPos result = GetPixelPosFromNodePos((NodePos) { x, y });

	return result;
}

void StartMoveEntity(NodePos current_node, NodePos dest_node) {
	pathfinding_system.next_pos_index = 0;
	pathfinding_system.is_moving = true;

	pathfinding_system.current_path_buff = GetPath(current_node, dest_node);

	//StartEntityAnimation(game_state.player);

	//pathfinding_system.end_pos = input_system.mouse_pos;
	/*pathfinding_system.end_pos = GetNearestNodePos(input_system.mouse_pos);
	pathfinding_system.original_pos = GetPos(game_state.player);*/
}

void EndMoveEntity() {
	pathfinding_system.is_moving = false;
	pathfinding_system.next_pos_index = 0;
	//EndEntityAnimation(game_state.player);
}

void MoveEntity(EntityId id) {
	if (pathfinding_system.is_moving) {
		PixelPos pos = GetEntityPixelPos(id);
		NodePos next_pos = pathfinding_system.current_path_buff[pathfinding_system.next_pos_index];
		PixelPos new_pos = MovePos(pos, GetPixelPosFromNodePos(next_pos));
		//SetEntityPos(game_state.player, new_pos);
		GetEntity(id)->pixel_pos = new_pos;

		//printf("%f %f\n", new_pos.x, new_pos.y);

		if (HasReachedPoint(new_pos, GetPixelPosFromNodePos(next_pos))) {
			if (HasReachedPoint(new_pos, GetPixelPosFromNodePos(pathfinding_system.current_path_buff[arrlen(pathfinding_system.current_path_buff) - 1]))) {
				EndMoveEntity();
			}
			else {
				pathfinding_system.next_pos_index++;
				GetEntity(id)->node_pos = pathfinding_system.current_path_buff[pathfinding_system.next_pos_index];
			}
		}

		if (Vector2iSubtract(next_pos.pos, GetEntityPixelPos(id).pos).x < 0) {
			GetEntity(id)->flipped = true;
		}
		else {
			GetEntity(id)->flipped = false;
		}
	}
}

void HandlePlayerMovement() {
#if FIX_ME
	if (input_system.input_event == InputEvent_LeftClickReleased) {
		if (IsObstacle(GetNearestNodeNumFromPixelPos(Vector2iToPixelPos(input_system.mouse_pos)))) {
			return;
		}
		if (IsInsidePolygon(pathfinding_system.walking_area_vertices, GetNearestNodeNumFromPixelPos(Vector2iToPixelPos(input_system.mouse_pos)).pos)) {
			StartMoveEntity(GetEntityByName("player")->node_pos, GetNearestNodeNumFromPixelPos((PixelPos) { input_system.mouse_pos }));
		}
	}
#endif
}

NodePos* GetLine2(NodePos p0, NodePos p1) {
	NodePos* nodes = 0;

	float dx = p1.pos.x - p0.pos.x;
	float dy = p1.pos.y - p0.pos.y;

	float step = fmax(fabs(dx), fabs(dy));

	if (step) {
		float step_x = dx / step;
		float step_y = dy / step;

		for (int i = 0; i < step + 1; ++i) {
			float x = roundf(p0.pos.x + i * step_x);
			float y = roundf(p0.pos.y + i * step_y);
			NodePos n = (NodePos){ x, y };
			arrput(nodes, n);
		}
	}

	return nodes;
}

NodePos* GetLine(NodePos p0, NodePos p1) {

	NodePos* nodes = 0;


	int x0 = p0.pos.x;
	int y0 = p0.pos.y;
	int x1 = p1.pos.x;
	int y1 = p1.pos.y;

	if (x0 > x1) {
		int x = x1;
		x1 = x0;
		x0 = x;

		int y = y1;
		y1 = y0;
		y0 = y;
	}

	float m = (p1.pos.y - p0.pos.y) / (p1.pos.x - p0.pos.x);

	NodePos previous_pos = { 0 };


	for (int x = x0; x <= x1; x += 1) {
		float y = m * (x - x0) + y0;

		y = roundf(y);

		NodePos node = (NodePos){ x, y };

		if (!(node.pos.x == previous_pos.pos.x && node.pos.y == previous_pos.pos.y)) {
			arrput(nodes, node);
		}

		previous_pos = node;


	}

	int debug_num = arrlen(nodes);

	return nodes;
}

#endif