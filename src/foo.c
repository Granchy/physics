#include <stdlib.h>
#include <stdio.h>
#include <raylib.h>
#include <strings.h>
#include <math.h>
#include <time.h>


#define WIDTH 800
#define HEIGHT 600
#define PARTICLE_COUNT 10
#define RADIUS 5.0f*2
#define INIT_POS_X 5+RADIUS+1
#define INIT_POS_Y 105
#define INIT_VEL_X 5
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
	struct particle ***particles;
	int grid_cell_counts[ROWS][COLS];

};

struct particle ***grid;

int count = 0;

float get_distance(struct particle p, struct particle np);
Color get_random_color();

	int particle_count = 0;


void update(struct container *container) {
	float dt = FIXED_DT;
	
	float a = g;

	for(size_t i = 0; i < ROWS; i++) {
		for(size_t j = 0; j < COLS; j++) {
			for(size_t k = 0; k < container->grid_cell_counts[i][j]; k++) {
				struct particle *p = &container->particles[i][j][k];
				float temp_pos_y = p->position.y;
		float temp_pos_x = p->position.x;
		p->position.y = 2*temp_pos_y - p->prev_position.y + a*dt*dt;
		p->prev_position.y = temp_pos_y;
		p->position.x = 2*temp_pos_x - p->prev_position.x;
		p->prev_position.x = temp_pos_x;
			}
		}
	}



	
}


void constraint(struct container *container) {

	const float dt = FIXED_DT;
	const int x0 = container->position.x;
	const int x1 = container->position.x + container->size.x;

	const int y0 = container->position.y;
	const int y1 =  container->size.y + container->position.y;

	for(size_t i = 0; i < ROWS; i++) {
		for(size_t j = 0; j < COLS; j++) {
			for(size_t k = 0; k < container->grid_cell_counts[i][j]; k++) {
				struct particle *p = &container->particles[i][j][k];

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
	}


	

}

void solve_collisions(struct container *container) {
	int range = 1;

	for(int i = 0; i < ROWS; i++) {
		for(int j = 0; j < COLS; j++) {
			for(int k1 = 0; k1 < container->grid_cell_counts[i][j]; k1++) {
				struct particle *p1 = &container->particles[i][j][k1];
				for(int dr = -range; dr <= range; dr++) {
					for(int dc = -range; dc <= range; dc++) {
						if(i + dr < 0 || i + dr >= ROWS || j + dc < 0 || j + dc >= COLS) {
							continue;

						}
						for(int k2 = 0; k2 < container->grid_cell_counts[i+dr][j+dc]; k2++) {
							struct particle *p2 = &container->particles[i+dr][j+dc][k2];
							const float dx = p1->position.x - p2->position.x;
							const float dy = p1->position.y - p2->position.y;
							const float min_distance = 2 * RADIUS;
							const float dist2 = dx*dx + dy*dy;
							if(dist2 < min_distance*min_distance) {
								const float dist = sqrt(dist2);
								const float n_x = (p1->position.x - p2->position.x)/dist;
								const float n_y = (p1->position.y - p2->position.y)/dist;
								const float delta = 0.5f*CR*(dist - min_distance);
								p1->position.x -= n_x*delta;
								p2->position.x += n_x*delta;
								p1->position.y -= n_y*delta;
								p2->position.y += n_y*delta;
							}
						}
					}
				}
			}
		}
	}


}

void draw(struct container *container) {
	BeginDrawing();
		ClearBackground(BLACK);
		DrawRectangleV(container->position,container->size, GRAY);
		DrawFPS(10,10);
		for(size_t i = 0; i < ROWS; i++) {
			for(size_t j = 0; j < COLS; j++) {
				for(size_t k = 0; k < container->grid_cell_counts[i][j]; k++) {
					DrawCircleV(container->particles[i][j][k].position,RADIUS,container->particles[i][j][k].color);
				}
			}
		}
	

		EndDrawing();

}

void update_grid(struct container *container) {
	struct particle ***new_grid = malloc(ROWS*sizeof(struct particle **));
	
	int new_count[ROWS][COLS];
	memset(new_count,0,sizeof(new_count));
	for(size_t i = 0; i < ROWS; i++) {
		new_grid[i] = malloc(COLS*sizeof(struct particle *));
		for(size_t j =0; j < COLS; j++) {
			new_grid[i][j] = malloc(1000*sizeof(struct particle));
			for(size_t k = 0; k < container->grid_cell_counts[i][j]; k++) {
				struct particle p = container->particles[i][j][k];
				const int x = p.position.x/GRID_WIDTH;
				const int y = p.position.y/GRID_HEIGHT;
				new_grid[x][y][new_count[x][y]++] = p;
			}
		}
	}
	for(size_t i = 0; i < ROWS; i++) {
		for(size_t j =0; j < COLS; j++) {
			memcpy(&container->particles[i][j],&new_grid[i][j],new_count[i][j]*sizeof(struct particle));
		}
	}
}

float get_distance(struct particle p, struct particle np) {
	float dy = np.position.y - p.position.y;
	float dx = np.position.x - p.position.x;
	float distance = sqrt(dy*dy + dx*dx);

	return distance;
}

Color get_random_color() {
	Color colors[] = { RED, GREEN, BLUE, YELLOW, ORANGE, PINK};
	const int color_count = sizeof(colors)/sizeof(colors[0]);
	return colors[GetRandomValue(0,color_count)]; 
}


void init(struct container *container) {
	memset(container->grid_cell_counts, 0, sizeof(container->grid_cell_counts));

	container->position = (Vector2){5,5};
	container->size = (Vector2){WIDTH-10,HEIGHT-10};


	container->particles = malloc(ROWS*sizeof(struct particle **));
	for(size_t i = 0; i < ROWS; i++) {
		container->particles[i] = malloc(COLS*sizeof(struct particle*));
		for(size_t j = 0; j < COLS; j++) {
			container->particles[i][j] = malloc(1000*sizeof(struct particle));
		}
	}
	
}



void add_particle(struct container *container) {
	const int row = INIT_POS_X/GRID_WIDTH;
	const int col = INIT_POS_Y/GRID_HEIGHT;
	const int grid_cell_particle_count = container->grid_cell_counts[row][col];

	container->particles[row][col][grid_cell_particle_count].position.x = INIT_POS_X;
	container->particles[row][col][grid_cell_particle_count].position.y = INIT_POS_Y;
	container->particles[row][col][grid_cell_particle_count].prev_position.x = INIT_POS_X - INIT_VEL_X;
	container->particles[row][col][grid_cell_particle_count].prev_position.y = INIT_POS_Y;
	container->particles[row][col][grid_cell_particle_count].color = get_random_color();
	
	container->grid_cell_counts[row][col]++;
	particle_count++;
	}

void print_particle_positions(struct container *container) {
	int count = 0;
	for(size_t i =0; i < ROWS; i++) {
		for(size_t j=0; j < COLS; j++) {
			for(size_t k =0; k < container->grid_cell_counts[i][j]; k++) {
				struct particle p = container->particles[i][j][k];
				printf("particle %d, x: %f y: %f\n", ++count, p.position.x, p.position.y);
			}
		}
	}
}

int main(int argc, char *argv[])
{
	InitWindow(WIDTH,HEIGHT,"physics");
	SetTargetFPS(60);

	float particle_delay = 1.0f;
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

		if(elapsed_time >= particle_delay && particle_count < PARTICLE_COUNT) {\
		add_particle(&container);
			
		elapsed_time -=particle_delay;
		}


		update(&container);

		constraint(&container);
		solve_collisions(&container);
		update_grid(&container);

		// copy_particles_from_grid(&container);
		draw(&container);
		
		
	}


	CloseWindow();

	return 0;
}
