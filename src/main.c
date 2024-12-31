#include <stdlib.h>
#include <stdio.h>
#include <raylib.h>
#include <strings.h>
#include <math.h>
#include <time.h>


#define WIDTH 800
#define HEIGHT 600
#define PARTICLE_COUNT 1000000
#define RADIUS 5.0f
#define INIT_POS_X 5+RADIUS+1
#define INIT_POS_Y 105
#define INIT_VEL_X 50
#define g 9.81f*40
#define FIXED_DT 1/60.0f
#define WALL_COLLISION_DAMPING 0.5f
#define MIN_STARTING_DISTANCE 45
#define CR 1.0f
#define GRID_WIDTH 128
#define GRID_HEIGHT 80
#define ROWS 2560/GRID_WIDTH
#define COLS 1600/GRID_HEIGHT


struct particle {
	Vector2 position;
	Vector2 prev_position;
	Color color;
};

struct container {
	Vector2 position;
	Vector2 size;
	struct particle *particles;
};

struct particle ****grid;
int grid_cell_counts[ROWS][COLS];

void append_particle(size_t i, size_t j, struct particle *p);
float get_distance(struct particle p, struct particle np);
Color get_random_color();

	int particle_count = 0;


void update(struct container *container) {
	float dt = FIXED_DT;
	
	float a = g;

	
	for(size_t i = 0; i < particle_count; i++) {
		struct particle *p = &container->particles[i];
float temp_pos_y = p->position.y;
float temp_pos_x = p->position.x;
	p->position.y = 2*temp_pos_y - p->prev_position.y + a*dt*dt;
	p->prev_position.y = temp_pos_y;
	p->position.x = 2*temp_pos_x - p->prev_position.x;
	p->prev_position.x = temp_pos_x;

	

	}
	
}


void constraint(struct container *container) {

	float dt = FIXED_DT;
	int x0 = container->position.x;
	int x1 = container->position.x + container->size.x;

	int y0 = container->position.y;
	int y1 =  container->size.y + container->position.x;

	for(size_t i = 0; i < particle_count; i++) {
		struct particle *p = &container->particles[i];

	float v_y = (p->position.y - p->prev_position.y) * WALL_COLLISION_DAMPING;
	float v_x = (p->position.x - p->prev_position.x) * WALL_COLLISION_DAMPING;

	if(p->position.x + RADIUS >=x1) {
		p->position.x = x1 - RADIUS;
		p->prev_position.x = p->position.x + v_x;
	}

	if(p->position.x <=x0+RADIUS) {
		p->position.x= x0+RADIUS;
		p->prev_position.x = p->position.x + v_x;

	}

	if(p->position.y + RADIUS>= y1){
		p->position.y = y1-RADIUS;
		p->prev_position.y = p->position.y + v_y;
	}

	if(p->position.y <=y0+RADIUS) {
		p->position.y = y0+RADIUS;
		p->prev_position.y = p->position.y + v_y;
	}
	}

	

}

void solve_collisions(struct container *container) {

	for(size_t i = 0; i < particle_count; i++) {
		append_particle(container->particles[i].position.x/GRID_WIDTH, container->particles[i].position.y/GRID_HEIGHT, &container->particles[i]);
	}

	for(int i =0; i < ROWS; i++) {
		for(int j =0; j < COLS; j++) {
			for(int k1 = 0; k1 < grid_cell_counts[i][j]; k1++) {
				for(int k2 = k1 +1; k2<grid_cell_counts[i][j]; k2++) {
					struct particle *p1 = grid[i][j][k1];
					struct particle *p2 = grid[i][j][k2];
			const float dist = get_distance(*p1,*p2);
			const float n_x = (p1->position.x - p2->position.x)/dist;
			const float n_y = (p1->position.y - p2->position.y)/dist;
			if(dist < 2*RADIUS) {
				float delta = 0.5f*CR*(dist - 2*RADIUS);
					p1->position.x -= n_x*delta;
					p2->position.x += n_x*delta;
					p1->position.y -= n_y*delta;
					p2->position.y += n_y*delta;
				}
			}
		}
	}

	// for(size_t i = 0; i < particle_count; i++) {
	// 	for(size_t k = i+1; k < particle_count; k++) {
	// 		struct particle *p1 = &container->particles[i];
	// 		struct particle *p2 = &container->particles[k];
			
	// 		const float dist = get_distance(*p1,*p2);
	// 		const float n_x = (p1->position.x - p2->position.x)/dist;
	// 		const float n_y = (p1->position.y - p2->position.y)/dist;
	// 		if(dist < 2*RADIUS) {
	// 			float delta = 0.5f*CR*(dist - 2*RADIUS);
	// 				p1->position.x -= n_x*delta;
	// 				p2->position.x += n_x*delta;
	// 				p1->position.y -= n_y*delta;
	// 				p2->position.y += n_y*delta;
	// 		}
	// 	}
	// }
}
}

void draw(struct container *container) {
	BeginDrawing();
		ClearBackground(BLACK);
		DrawRectangleV(container->position,container->size, GRAY);
		DrawFPS(10,10);
		for(size_t i = 0; i < particle_count; i++) {
			
		DrawCircleV(container->particles[i].position,RADIUS, container->particles[i].color);
		}

		EndDrawing();
	// for(size_t i = 0; i < ROWS; i++) {
	// 	for(size_t j =0; j < COLS; j++) {
	// 		for(size_t k = 0; k < grid_cell_counts[i][j]; k++) {
	// 			free(grid[i][j][k]);
	// 		}
	// 		free(grid[i][j]);
	// 	}
	// 	free(grid[i]);
	// }
	// free(grid);
}

float get_distance(struct particle p, struct particle np) {
	float dy = np.position.y - p.position.y;
	float dx = np.position.x - p.position.x;
	float distance = sqrt(dy*dy + dx*dx);

	return distance;
}

Color get_random_color() {
	Color colors[] = { RED, GREEN, BLUE, YELLOW, ORANGE, PINK, WHITE, BLACK };
	const int color_count = sizeof(colors)/sizeof(colors[0]);
	return colors[GetRandomValue(0,color_count)]; 
}


void init(struct container *container) {

	container->position = (Vector2){5,5};
	container->size = (Vector2){WIDTH-10,HEIGHT-10};
	container->particles = (struct particle *)malloc(PARTICLE_COUNT*sizeof(struct particle));

	

	for(size_t i = 0; i < PARTICLE_COUNT; i++) {
		container->particles[i].position.x = INIT_POS_X;
		container->particles[i].position.y = INIT_POS_Y;
		container->particles[i].prev_position.x = INIT_POS_X - INIT_VEL_X;
		container->particles[i].prev_position.y = INIT_POS_Y;
		container->particles[i].color = get_random_color();
	}

	grid = (struct particle ****)malloc(ROWS * sizeof(struct particle ***));
	for(size_t i = 0; i < ROWS; i++) {
		grid[i] = (struct particle ***)malloc(COLS * sizeof(struct particle **));
		for(size_t j = 0; j < COLS; j++) {
			grid[i][j] = (struct particle **)malloc(sizeof(struct particle *));
		}
		}



}

void append_particle(size_t i, size_t j, struct particle *p) {
	grid[i][j] = realloc(grid[i][j], (grid_cell_counts[i][j]+1)*sizeof(struct particle *));

	grid[i][j][grid_cell_counts[i][j]] = p;
	grid_cell_counts[i][j]++;
}


int main(int argc, char *argv[])
{
	InitWindow(WIDTH,HEIGHT,"physics");
	SetTargetFPS(60);

	float particle_delay = 0.0001f;
	float elapsed_time = 0.0f;

	struct container container;
	init(&container);


	while(!WindowShouldClose()) {
		// if (IsKeyPressed(KEY_F10)) ToggleFullscreen();
		if(IsKeyDown(KEY_R)) {
				init(&container);
				elapsed_time -=particle_delay;
				particle_count = 0;

		}
		float delta_time = GetFrameTime();
		        elapsed_time += delta_time;

		if(elapsed_time >= particle_delay && particle_count < PARTICLE_COUNT) {
			particle_count++;
			
		elapsed_time -=particle_delay;
		}


		update(&container);
		constraint(&container);
		solve_collisions(&container);
		draw(&container);
		
		
	}


	CloseWindow();

	return 0;
}
