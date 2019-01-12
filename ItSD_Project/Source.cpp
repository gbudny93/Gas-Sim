/*****************************************************************
******************************************************************
*******************ITSD Project 2015******************************
******************Grzegorz Budny CEIE*****************************
******************All rights reserved*****************************
*******************************************************************/

//LIBRARIES
#include<allegro5\allegro.h>
#include<allegro5\allegro_primitives.h>
#include<allegro5\allegro_font.h>
#include<allegro5\allegro_ttf.h>
#include<math.h>
#include<iostream>
#include<fstream>
#include<string>

//Constants
#define FPS 60
#define PI 3.14
#define N 10	//number of particles  

//GLOBALS
ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_EVENT event;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_FONT *font;

enum KEYS{RIGHT,LEFT};
bool key[2] = { false, false };

const int width = 800;	//width of the screen
const int height = 480;	//height of the screen
double A;				//angle between two coliding particles
int j, i;				//loops variables
double dx, dy;			//distances of proper axises
double distance;		//distance between centre of two particles
double g = 9.81;			//gravity acceleration
double Q;				//gravty force

const double k = 1.380e-23;	//Bolzmann constant
const double mass = 2e-22;  //mass of every red particle
double P1, P2;				//pressure
double T1 = 273.15, T2;		//Temperature
double V1 = 2, V2;			//Volume
double T, P, V;				//Final parameters after state change
double Vi, Vf;				//initial velocity, final velocity
int counter = 0;			//counter for particle control

//each new particle parameters
double Ek, Vc;

bool finish;	//loop variable

struct Particle{

	double x;
	double y;
	double vx;
	double vy;
	double mass;
	double radius;
	double phi = atan2(vy, vx);
	double v = sqrt(pow(vx, 2) + pow(vy, 2));
	
};
struct Box{

	double LeftX;
	double RightX;
	double UpX;
	double DownX;
	double LeftY;
	double RightY;
	double UpY;
	double DownY;
};
//PROTOTYPES
void CreateParticle(Particle &particle, double x, double y, double mass, double radius, double vx, double vy);	//creates particle 
void MoveParticle(Particle &particle);		 //moves particle
void UpdateParticle(Particle &particle);	
void Bounce(Particle &particle );			//Bounce particle from borders
void Collsision(Particle* particle);		//Collision detector and extecutor
void Sim(Particle* particle, Box &box);		//Simulation function
void InitParameters(Particle* particle);	//Parametres initialization
void Isothermal(Particle* particle);		//Isothermal process
void Isobaric(Particle* particle);			//Isobaric process
void Isochoric(Particle* particle);			//Isochoric process
void Nochange(Particle* particle);			//No state change process
void GravityAccelearation(Particle* particle);//Gravity acceleration change 

double Gravity();	//gravity force

void CreateBox(Box &box, double leftx, double lefty, double rightx, double righty, double upx, double upy, double downx, double downy);	//creates box

void logo(); //displays logo 


int main(){
	
	//Creating objects
	Particle particle[N];	//array of particles
	Box box;

	//Menu
	int option, main_option;
	
	while (1){
		finish = false;
		system("cls");
		logo();
		std::cout << std::endl;
		
		InitParameters(particle);
	start:
		std::cout << "MAIN MENU" << std::endl;
		std::cout << "1. Simulate !" << std::endl;
		std::cout << "2. How to simulate?" << std::endl;
		std::cout << "3. Exit" << std::endl;
		std::cin >> main_option;
		switch (main_option){

		case 1:{

			system("cls");
			logo();
			std::cout << std::endl;
			std::cout << "The initial values fot the system's first state are:" << std::endl;
			std::cout << "Temperature: " << T1 << " [K]" << std::endl;
			std::cout << "Volume: " << V1 << " [l]" << std::endl;
			P1 = (N * k*T1) / V1;
			std::cout << "Pressure: " << P1 << " [Pa]" << std::endl;
			std::cout << "Gravity force: " << Gravity() << "\n\n";

			std::cout << "1. Isothermal Process" << std::endl;
			std::cout << "2. Isobaric Process" << std::endl;
			std::cout << "3. Isochoric Process" << std::endl;
			std::cout << "4. Start Simulation with initial values" << std::endl;
			std::cout << "5. Change gravity acceleration" << std::endl;
			std::cout << "6. Return to main menu" << std::endl;
			std::cin >> option;

			switch (option){

			case 1:{

				Isothermal(particle);
				break;
			}
			case 2:{
				Isobaric(particle);
				break;
			}
			case 3:{
				Isochoric(particle);
				break;
			}
			case 4:
			{
				Nochange(particle);
				break;
			}
			case 5:
			{
				GravityAccelearation(particle);
				break;
			}
			case 6:{
				system("cls");
				logo();
				goto start;
				break;
			}
			default:{
				system("cls");
				logo();
				std::cout << "Wrong option number! Please enter number from 1 to 5" << std::endl<<std::endl;
				goto start;
			}
			}
			break;
		}
		case 2:{	
			std::fstream file("info.txt");
			std::string str;
			while (std::getline(file, str)){
				std::cout << str << std::endl;
			}
			std::cin.get();
			std::cin.get();
			goto start;
			break;
		}
		case 3:{
			exit(0);
		}
		default:
			system("cls");
			logo();
			std::cout << "Wrong number! Please enter number from 1 to 3" << std::endl << std::endl;
			getchar();
			goto start;
			break;
		}
		//Allegro initialization
		if (!al_init()){
			return -1;
		}
		//Creating display
		display = al_create_display(width, height);
		al_set_window_title(display, "Ideal Gas Simulation by Grzegorz Budy ItSD Project 2014");
		if (!display){
			return -1;
		}

		//Allegro initialization parameters
		al_init_primitives_addon();
		al_init_font_addon();
		al_init_ttf_addon();
		al_install_keyboard();

		//loading font
		font = al_load_ttf_font("PressStart2P.ttf", 12, 0);
		if (!font){
			return -1;
		}

		//create an event queue
		event_queue = al_create_event_queue();
		timer = al_create_timer(1.0 / FPS);

		al_register_event_source(event_queue, al_get_display_event_source(display));
		al_register_event_source(event_queue, al_get_timer_event_source(timer));
		al_register_event_source(event_queue, al_get_keyboard_event_source());

		al_start_timer(timer);

		//Create new particle!
		CreateParticle(particle[0], 102, 100, 2, 15, particle[0].vx / 100, particle[0].vy / 100);
		CreateParticle(particle[1], 104, 320, 2, 15, particle[1].vx / 100, particle[1].vy / 100);
		CreateParticle(particle[2], 55, 210, 2, 15, particle[2].vx / 100, particle[2].vy / 100);
		CreateParticle(particle[3], 210, 70, 2, 15, particle[3].vx / 100, particle[3].vy / 100);
		CreateParticle(particle[4], 240, 300, 2, 15, particle[4].vx / 100, particle[4].vy / 100);
		CreateParticle(particle[5], 55, 250, 2, 15, particle[5].vx / 100, particle[5].vy / 100);
		CreateParticle(particle[6], 80, 150, 2, 15, particle[6].vx / 100, particle[6].vy / 100);
		CreateParticle(particle[7], 150, 250, 2, 15, particle[7].vx / 100, particle[7].vy / 100);
		CreateParticle(particle[8], 120, 280, 2, 15, particle[8].vx / 100, particle[8].vy / 100);
		CreateParticle(particle[9], 80, 75, 2, 15, particle[9].vx / 100, particle[9].vy / 100);

		while (!finish){

			al_wait_for_event(event_queue, &event);
			if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
								
				finish = true;
			}
			else if (event.type == ALLEGRO_EVENT_TIMER){

				Sim(particle,box);

			}
			else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
				switch (event.keyboard.keycode) {

				case ALLEGRO_KEY_LEFT:
					key[LEFT] = true;
					if (counter == 0)break;
					counter--;

					break;

				case ALLEGRO_KEY_RIGHT:
					key[RIGHT] = true;
					if (counter == N - 1)break;
					counter++;

					break;
				}
			}
			else if (event.type == ALLEGRO_EVENT_KEY_UP) {
				switch (event.keyboard.keycode) {

				case ALLEGRO_KEY_LEFT:
					if (counter > N || counter < 0)break;
					key[LEFT] = false;
					break;

				case ALLEGRO_KEY_RIGHT:
					if (counter > N || counter < 0)break;
					key[RIGHT] = false;
					break;
				}
			}
		}

		//destroy
		al_destroy_display(display);
		al_stop_timer(timer);
		al_destroy_font(font);
		al_destroy_event_queue(event_queue);
		al_destroy_timer(timer);
		

	}
		return 0;
	
}
//PARTICLE
//FUNCTIONS
double Gravity(){

	
	Q = mass*g;
	return Q;

}
void CreateParticle(Particle &particle, double x, double y,double mass,double radius, double vx,double vy){

	particle.x = x;
	particle.y = y;
	particle.mass = mass;
	particle.radius = radius;
	particle.vx = vx;
	particle.vy = vy;

}
void MoveParticle(Particle &particle){

	particle.x += particle.vx;
	particle.y += (particle.vy + Gravity());
}

void Bounce(Particle &particle){

		MoveParticle(particle);
		
			if (particle.x >= 400 - particle.radius){
				particle.vx *= -1;
			}
			if (particle.x <= 35){
				particle.vx *= -1;
			}
			if (particle.y >= 50 + particle.radius){
				particle.vy *= -1;
			}
			if (particle.y <= 400 - particle.radius){
				particle.vy *= -1;
			}
		
}
void Collsision(Particle *particle){

	for (int a = 0; a < 100; a++){
		for (i = 0; i < N; i++){
			Bounce(particle[i]);
			for (j = i + 1; j < N; j++){
				
				dx = particle[i].x - particle[j].x;
				dy = particle[i].y - particle[j].y;
				distance = sqrt((pow(dx, 2) + pow(dy, 2)));

				if (distance <= (particle[i].radius + particle[j].radius)){
					A = atan2(dy, dx);
					particle[i].v = sqrt(pow(particle[i].vx, 2) + pow(particle[i].vy, 2));
					particle[j].v = sqrt(pow(particle[j].vx, 2) + pow(particle[j].vy, 2));
					particle[i].phi = atan2(particle[i].vy, particle[i].vx);
					particle[j].phi = atan2(particle[j].vy, particle[j].vx);

					particle[i].vx = ((particle[i].v*cos(particle[i].phi - A)*(particle[i].mass - particle[j].mass) + 2 * particle[j].mass*particle[j].v*cos(particle[j].phi - A)) / (particle[i].mass + particle[j].mass))*cos(A) + ((particle[i].v*sin(particle[i].phi - A)*cos(A + (PI / 2))));
					particle[i].vy = ((particle[i].v*cos(particle[i].phi - A)*(particle[i].mass - particle[j].mass) + 2 * particle[j].mass*particle[j].v*cos(particle[j].phi - A)) / (particle[i].mass + particle[j].mass))*sin(A) + ((particle[i].v*sin(particle[i].phi - A)*sin(A + (PI / 2))));

					particle[j].vx = ((particle[j].v*cos(particle[j].phi - A)*(particle[j].mass - particle[i].mass) + 2 * particle[i].mass*particle[i].v*cos(particle[i].phi - A)) / (particle[i].mass + particle[j].mass))*cos(A) + ((particle[j].v*sin(particle[j].phi - A)*cos(A + (PI / 2))));
					particle[j].vy = ((particle[j].v*cos(particle[j].phi - A)*(particle[j].mass - particle[i].mass) + 2 * particle[i].mass*particle[i].v*cos(particle[i].phi - A)) / (particle[i].mass + particle[j].mass))*sin(A) + ((particle[j].v*sin(particle[j].phi - A)*sin(A + (PI / 2))));
					
				}
			}
		}
		
	}	
}
//Box
void CreateBox(Box &box, double leftx, double lefty, double rightx, double righty, double upx, double upy, double downx, double downy){


	al_draw_line(leftx, lefty, upx, upy, al_map_rgb(255, 255, 255), 3);
	al_draw_line(upx, upy, rightx, righty, al_map_rgb(255, 255, 255), 3);
	al_draw_line(rightx, righty, downx, downy, al_map_rgb(255, 255, 255), 3);
	al_draw_line(downx, downy, leftx, lefty, al_map_rgb(255, 255, 255), 3);
	al_draw_filled_rectangle(leftx, lefty, rightx, righty, al_map_rgb(32, 32, 32));

	/*al_draw_line(20, 400, 20, 50, al_map_rgb(255, 255, 255), 3);
	al_draw_line(20, 50, 400, 50, al_map_rgb(255, 255, 255), 3);
	al_draw_line(400, 50, 400, 400, al_map_rgb(255, 255, 255), 3);
	al_draw_line(400, 400, 20, 400, al_map_rgb(255, 255, 255), 3);*/

}
void Sim(Particle* particle, Box &box){

	//Calc particle parameters

	Vc = particle[counter].v * 100;
	Ek = (0.5* pow(particle[counter].v, 2) * 1000 * 2.2);

	//Colission detection and execution
	Collsision(particle);
	//Box creatio
	CreateBox(box, 20, 400, 400, 50, 20, 50, 400, 400);

	//Draw all particles on the screen
	for (i = 0; i < N; i++){
		al_draw_filled_circle(particle[i].x, particle[i].y, particle[i].radius, al_map_rgb(255, 0, 0));
	}

	al_draw_filled_circle(particle[counter].x, particle[counter].y, particle[counter].radius, al_map_rgb(128, 255, 0));

	al_flip_display();
	al_clear_to_color(al_map_rgb(0, 0, 0));

	//Draw title
	al_draw_text(font, al_map_rgb(255, 255, 255), 100, 15, 0, "IDEAL GAS SIMULATOR");

	//Draw paramets for first and second state
	al_draw_text(font, al_map_rgb(255, 255, 255), 430, 20, 0, "Starting values");
	al_draw_text(font, al_map_rgb(255, 255, 255), 430, 50, 0, "Pressure: "); //dopisac jednostke bo jest mala fest
	al_draw_text(font, al_map_rgb(255, 255, 255), 430, 80, 0, "Temperature: ");
	al_draw_text(font, al_map_rgb(255, 255, 255), 430, 110, 0, "Volume: ");
	al_draw_text(font, al_map_rgb(255, 255, 255), 430, 140, 0, "Gravity: ");
	al_draw_text(font, al_map_rgb(255, 255, 255), 430, 170, 0, "Start velocity: ");

	al_draw_textf(font, al_map_rgb(255, 255, 255), 550, 50, 0, "%.2f", P1);
	al_draw_textf(font, al_map_rgb(255, 255, 255), 580, 80, 0, "%.2f", T1);
	al_draw_textf(font, al_map_rgb(255, 255, 255), 540, 110, 0, "%.2f", V1);
	al_draw_textf(font, al_map_rgb(255, 255, 255), 530, 140, 0, "%f", (Gravity()*100000*100000*100000*100000));
	al_draw_textf(font, al_map_rgb(255, 255, 255), 620, 170, 0, "%.2f", Vi);

	al_draw_text(font, al_map_rgb(255, 255, 255), 600, 50, 0, "[Pa E-20]");
	al_draw_text(font, al_map_rgb(255, 255, 255), 660, 80, 0, "[K]");
	al_draw_text(font, al_map_rgb(255, 255, 255), 600, 110, 0, "[l]");
	al_draw_text(font, al_map_rgb(255, 255, 255), 680, 140, 0, "[J E-22]");
	al_draw_text(font, al_map_rgb(255, 255, 255), 680, 170, 0, "[mps]");


	al_draw_text(font, al_map_rgb(255, 255, 255), 430, 210, 0, "Final Values");
	al_draw_text(font, al_map_rgb(255, 255, 255), 430, 240, 0, "Pressure: "); 
	al_draw_text(font, al_map_rgb(255, 255, 255), 430, 270, 0, "Temperature: ");
	al_draw_text(font, al_map_rgb(255, 255, 255), 430, 300, 0, "Volume: ");
	al_draw_text(font, al_map_rgb(255, 255, 255), 430, 330, 0, "Gravity: ");
	al_draw_text(font, al_map_rgb(255, 255, 255), 430, 360, 0, "Start velocity: ");

	al_draw_textf(font, al_map_rgb(255, 255, 255), 550, 240, 0, "%.2f", P2);
	al_draw_textf(font, al_map_rgb(255, 255, 255), 580, 270, 0, "%.2f", T2);
	al_draw_textf(font, al_map_rgb(255, 255, 255), 550, 300, 0, "%.2f", V2);
	al_draw_textf(font, al_map_rgb(255, 255, 255), 530, 330, 0, "%f", (Gravity() * 100000 * 100000 * 100000 * 100000));
	al_draw_textf(font, al_map_rgb(255, 255, 255), 620, 360, 0, "%.2f", Vf);

	al_draw_text(font, al_map_rgb(255, 255, 255), 600, 240, 0, "[Pa E-20]");
	al_draw_text(font, al_map_rgb(255, 255, 255), 660, 270, 0, "[K]");
	al_draw_text(font, al_map_rgb(255, 255, 255), 600, 300, 0, "[l]");
	al_draw_text(font, al_map_rgb(255, 255, 255), 680, 330, 0, "[J E-22]");
	al_draw_text(font, al_map_rgb(255, 255, 255), 680, 360, 0, "[mps]");

	al_draw_text(font, al_map_rgb(255, 255, 255), 100, 420, 0, "Velocity: ");
	al_draw_text(font, al_map_rgb(255, 255, 255), 100, 450, 0, "Kinetic Energy: ");

	al_draw_textf(font, al_map_rgb(255, 255, 255), 240, 420, 0, "%.2f", Vc);
	al_draw_textf(font, al_map_rgb(255, 255, 255), 290, 450, 0, "%.2f", Ek);

	al_draw_text(font, al_map_rgb(255, 255, 255), 320, 420, 0, "[mps]");
	al_draw_text(font, al_map_rgb(255, 255, 255), 360, 450, 0, "[J E-20]");

}
void InitParameters(Particle* particle){

	for (int i = 0; i < N; i++){
		particle[i].v = sqrt((3 * k*T1) / mass);
		Vi = particle[i].v;
		particle[i].phi = 45;
		particle[i].vx = particle[i].v*cos(particle[i].phi);
		particle[i].vy = particle[i].v*sin(particle[i].phi);
		//P1 = (N * mass*pow(particle[i].v, 2) / (3 * V1));
	}
}
void Isothermal(Particle* particle){

	//Calculate initial values		
	P1 = (N * k*T1) / V1;
	std::cout << "Enter Volume: " << std::endl;;
	std::cin >> V2;

	T2 = T1;
	//P2 = (5 * mass*pow(particle[0].v, 2) / (3 * V2));
	P2 = P1*(V1 / V2);
	for (int i = 0; i < N; i++){
		particle[i].v *= (P2 / P1);
		Vf = particle[i].v;
		particle[i].phi = 45;
		particle[i].vx = particle[i].v*cos(particle[i].phi);
		particle[i].vy = particle[i].v*sin(particle[i].phi);
	}
	//scaled because P is something to -20 :)
	P1 = P1 * 100000 * 100000 * 100000 * 100000;
	P2 = P2 * 100000 * 100000 * 100000 * 100000;
}
void Isobaric(Particle* particle){

	//Calculate initial values
	std::cout << "Enter Temperature " << std::endl;;
	std::cin >> T2;
	std::cout << "Enter Volume " << std::endl;;
	std::cin >> V2;
	P1 = (N * k*T1) / V1;
	T2 = T1*(T2 / T1);
	V2 = V1*(V2 / V1);
	P2 = P1;
	//P2 = (5 * mass*pow(particle[0].v, 2) / (3 * V2));
	for (int i = 0; i < N; i++){
		particle[i].v *= (T2 / T1)*(V1 / V2);
		Vf = particle[i].v;
		particle[i].phi = 45;
		particle[i].vx = particle[i].v*cos(particle[i].phi);
		particle[i].vy = particle[i].v*sin(particle[i].phi);
	}
	//scaled
	P1 = P1 * 100000 * 100000 * 100000 * 100000;
	P2 = P2 * 100000 * 100000 * 100000 * 100000;
}
void Isochoric(Particle* particle){
	//Calculate initial values
	std::cout << "Enter Pressure " << std::endl;
	std::cin >> P2;
	P2 *= 1e-20; // scale
	std::cout << "Enter Temperature " << std::endl;
	std::cin >> T2;
	P1 = (N * k*T1) / V1;
	P2 = P1*(P2 / P1);
	T2 = T1*(P2 / P1);
	V2 = V1;
	//P2 = (5 * mass*pow(particle[0].v, 2) / (3 * V2));
	for (int i = 0; i < N; i++){
		particle[i].v *= (T2 / T1);// *(P2 / P1);
		Vf = particle[i].v;
		particle[i].phi = 45;
		particle[i].vx = particle[i].v*cos(particle[i].phi);
		particle[i].vy = particle[i].v*sin(particle[i].phi);
	}
	//scaled
	P1 = P1 * 100000 * 100000 * 100000 * 100000;
	P2 = P2 * 100000 * 100000 * 100000 * 100000;
}
void Nochange(Particle* particle){
	for (int i = 0; i < N; i++){
		particle[i].v = sqrt((3 * k*T1) / mass);
		Vf = particle[i].v;
		particle[i].phi = 45;
		particle[i].vx = particle[i].v*cos(particle[i].phi);
		particle[i].vy = particle[i].v*sin(particle[i].phi);
	}
	P1 = (N * k*T1) / V1;
	P1 = P1 * 100000 * 100000 * 100000 * 100000;
	P2 = P1;
	V2 = V1;
	T2 = T1;
	Vi = Vf;
}
void GravityAccelearation(Particle* particle){
	std::cout << "Enter new gravity acceleration: " << std::endl;
	std::cin >> g;
	for (int i = 0; i < N; i++){
		particle[i].v = sqrt((3 * k*T1) / mass);
		Vf = particle[i].v;
		particle[i].phi = 45;
		particle[i].vx = particle[i].v*cos(particle[i].phi);
		particle[i].vy = particle[i].v*sin(particle[i].phi);
	}
	P1 = (N * k*T1) / V1;
	P1 = P1 * 100000 * 100000 * 100000 * 100000;
	P2 = P1;
	V2 = V1;
	T2 = T1;
	Vi = Vf;
}
void logo(){

	std::cout << " @@@@@@@@       @       @@@@@@@@     @@@@@@@@    @   @@   @@   " << std::endl;
	std::cout << " @             @ @      @            @           @   @ @ @ @   " << std::endl;
	std::cout << " @            @   @     @            @           @   @  @  @   " << std::endl;
	std::cout << " @  @@@@@    @     @    @@@@@@@@     @@@@@@@@    @   @     @   " << std::endl;
	std::cout << " @      @   @@@@@@@@@          @            @    @   @     @   " << std::endl;
	std::cout << " @@@@@@@@  @         @  @@@@@@@@     @@@@@@@@    @   @     @   " << std::endl;
	std::cout << "                                            by Grzegorz Budny  " << std::endl;
	std::cout << "                               CEIE ItSD Project 2015 January  " << std::endl;



}

