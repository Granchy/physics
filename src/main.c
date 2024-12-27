#include <stdint.h>
#include <stdio.h>
#include <raylib.h>
#include <strings.h>
#include <math.h>
#include <time.h>


#define WIDTH 800
#define HEIGHT 600
#define PARTICLE_COUNT 1028
#define RADIUS 10.0f
#define INIT_POS_X 5+RADIUS+1
#define INIT_POS_Y 105
#define INIT_VEL_X 200
#define INIT_VEL_Y 1300
#define g 9.81f*20
#define FIXED_DT 1/60.0f
#define WALL_COLLISION_DAMPING 1
#define MIN_STARTING_DISTANCE 45
#define CR 1

struct particle {
	Vector2 position;
	Vector2 prev_position;
	Vector2 velocity;
};

struct container {
	Vector2 position;
	Vector2 size;
	struct particle particles[PARTICLE_COUNT];
};

void update_y_pos(struct particle *p, struct container *container,uint32_t index);
void update_x_pos(struct particle *p, struct container *container,uint32_t index);
float get_distance(struct particle p, struct particle np);

int frame_counter = 0;

void update( struct container *container) {
	float dt = FIXED_DT;
	/* p->position.x += dt*p->velocity.x; */

	for(int i = 0; i < PARTICLE_COUNT; i++) {
	
		if(i <= frame_counter / 100) {
		
		update_x_pos(&container->particles[i],container,i);

		update_y_pos(&container->particles[i],container,i);
		} else {
			break;
		}
	
	}
	





	/* printf("x: %f, xy %f,", p->position.x, p->position.y); */

	/* DrawCircleV(p->position, RADIUS, BLUE); */
}

void update_y_pos(struct particle *p, struct container* container,uint32_t k) {
	float dt = FIXED_DT;
	float v = p->velocity.y;
	float pos = p->position.y;
	float a = g;


	int y0 = container->position.y;
	int y1 =  container->size.y + container->position.x;


	p->position.y = 2*pos - p->prev_position.y + a*dt*dt;
	p->prev_position.y = pos;


	/* for(uint32_t i = 1+k; i < PARTICLE_COUNT; i++) { */

	/* 	float distance = get_distance(*p, container->particles[i]); */
	/* 	float dy = p->position.y - container->particles[i].position.y; */
	/* 	if(distance < 2*RADIUS) { */

	/* 		float n_y = dy/distance; */
	/* 		float delta = 0.5f * CR * (distance - 2* RADIUS); */
	/* 		p->position.y -= n_y * 0.5f * delta; */
	/* 		container->particles[i].position.y += n_y*0.5f * delta; */


	/* 	} */
	/* } */
	
	if(p->position.y + RADIUS>= y1){
		p->position.y = y1-RADIUS;

		p->velocity.y = -(p->position.y - p->prev_position.y)/dt*WALL_COLLISION_DAMPING;
		p->prev_position.y = p->position.y - p->velocity.y*dt;
	}

	if(p->position.y <=y0+RADIUS) {
		p->position.y = y0+RADIUS;
		p->velocity.y = -(p->position.y - p->prev_position.y)/dt*WALL_COLLISION_DAMPING;
		p->prev_position.y = p->position.y - p->velocity.y*dt;
	}
}

void update_x_pos(struct particle *p,struct container *container,uint32_t k) {
	float dt = FIXED_DT;
	float v = p->velocity.x;
	float pos = p->position.x;


	int x0 = container->position.x;
	int x1 = container->position.x + container->size.x;

	p->position.x = 2*pos - p->prev_position.x;
	p->prev_position.x = pos;


	/* for(uint32_t i = 1+k; i < PARTICLE_COUNT; i++) { */

	/* 	float distance = get_distance(*p, container->particles[i]); */
	/* 	float dx = p->position.x - container->particles[i].position.x; */
	/* 	if(distance < 2*RADIUS) { */

	/* 		float n_x = dx/distance; */
	/* 		float delta = 0.5f * CR * (distance - 2* RADIUS); */
	/* 		p->position.x -= n_x * 0.5f * delta; */
	/* 		container->particles[i].position.x += n_x*0.5f * delta; */


	/* 	} */
	/* } */

	if(p->position.x + RADIUS >=x1) {
		p->position.x = x1 - RADIUS;
		p->velocity.x = -(p->position.x - p->prev_position.x)/dt*WALL_COLLISION_DAMPING; }
	p->prev_position.x = p->position.x - p->velocity.x*dt;

	if(p->position.x <=x0+RADIUS) {
		p->position.x= x0+RADIUS;
		p->velocity.x = -(p->position.x - p->prev_position.x)/dt*WALL_COLLISION_DAMPING;
		p->prev_position.x = p->position.x - p->velocity.x*dt;	

	}
}

float get_distance(struct particle p, struct particle np) {
	float dy = np.position.y - p.position.y;
	float dx = np.position.x - p.position.x;
	float distance = sqrt(dy*dy + dx*dx);

	return distance;
}



void init(struct container *container) {

	container->position = (Vector2){5,5};
	container->size = (Vector2){WIDTH-10,HEIGHT-10};

	for(int i = 0; i < PARTICLE_COUNT; i++) {
		container->particles[i].position.x = INIT_POS_X;
		container->particles[i].position.y = INIT_POS_Y;
		container->particles[i].prev_position.x = INIT_POS_X;
		container->particles[i].prev_position.y = INIT_POS_Y;
		container->particles[i].velocity.x = INIT_VEL_X;
		container->particles[i].velocity.y = INIT_VEL_Y;
	}
}


int main(int argc, char *argv[])
{
	InitWindow(WIDTH,HEIGHT,"physics");
	SetTargetFPS(60);

	struct container container;
	init(&container);

	/* struct particle p; */

	/* p.position.x = INIT_POS_X; */
	/* p.position.y = INIT_POS_Y; */
	/* p.velocity.x = INIT_VEL_X; */
	/* p.velocity.y = INIT_VEL_Y; */
	/* p.prev_position.x = INIT_POS_X; */
	/* p.prev_position.y = INIT_POS_Y; */


	/* int particles_drawn =0; */

	while(!WindowShouldClose()) {
		update(&container);	
		BeginDrawing();

		ClearBackground(BLACK);
		DrawRectangleV(container.position,container.size, GRAY);
		DrawFPS(10,10);
		for(int i = 0; i < PARTICLE_COUNT; i++) {
		
			struct particle p = container.particles[i];
			if(i <= frame_counter/100) {
		
				DrawCircleV(p.position,RADIUS, BLUE);
				frame_counter++;

			} else break;

		}
		EndDrawing();
	}


	CloseWindow();

	return 0;
}
