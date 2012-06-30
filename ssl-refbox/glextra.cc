/**
 * @file glextra.cc
 * @brief GLExtra source file
 *
 */
#include <GL/gl.h>
#include <iostream>
#include <GL/glut.h>
#include <stdio.h>

#include <libbsmart/field.h>
#include <libbsmart/math.h>
#include "glextra.h"
#include "global.h"
#include <assert.h>

using namespace log4cxx;
LoggerPtr GLExtra::logger(Logger::getLogger("GLExtra"));

/**
 * @brief Initialize GLExtra
 */
GLExtra::GLExtra() {
	current_ball_percepts.clear();
	ball_samples.clear();
	ball_model = Ball_Sample();
	tmp_perc_robots.clear();
	current_robot_percepts.clear();
	tmp_sample_robots.clear();
	robot_samples.clear();
	robot_models.clear();
	broken_rule_vector.clear();
}

/**
 * @brief Initialize GLExtra
 * @param filter_data_
 */
GLExtra::GLExtra(Filter_Data* filter_data_) {
	filter_data = filter_data_;
	current_ball_percepts.clear();
	ball_samples.clear();
	ball_model = Ball_Sample();
	tmp_perc_robots.clear();
	current_robot_percepts.clear();
	tmp_sample_robots.clear();
	robot_samples.clear();
	robot_models.clear();
	broken_rule_vector.clear();
	internal_play_states = BSmart::Int_Vector(0, 0);
	gamestate = new BSmart::Game_States;
}

GLExtra::~GLExtra() {
	;
}

/**
 * @brief Plot all symmetric points to given (x,y).
 * \see bglBresCircle
 * @param x
 * @param y
 * @param q quadrant
 */
inline void GLExtra::symmPlotPoints(const int& x, const int& y, const Quadrant& q) {
	if (q & Q_I) {
		glVertex2i(x, y);
		glVertex2i(y, x);
	}
	if (q & Q_II) {
		glVertex2i(-x, y);
		glVertex2i(-y, x);
	}
	if (q & Q_III) {
		glVertex2i(-x, -y);
		glVertex2i(-y, -x);
	}
	if (q & Q_IV) {
		glVertex2i(x, -y);
		glVertex2i(y, -x);
	}
}

/**
 * @brief Draw a circle with radius @c r using the Bresenham algorithm
 * (center is the current position)
 * @see http://www.cs.fit.edu/~wds/classes/graphics/Rasterize/rasterize/
 * @param r radius
 * @param q Enables drawing partial bow of a circle (optional)
 */
inline void GLExtra::bglBresCircle(const int& r, const Quadrant& q) {
	int x = 0;
	int y = r;
	int g = 3 - 2 * r;
	int d = 10 - 4 * r;
	int ri = 6;
	glBegin(GL_POINTS);
	while (x <= y) {
		symmPlotPoints(x, y, q);
		if (g >= 0) {
			g += d;
			d += 8;
			y -= 1;
		} else {
			g += ri;
			d += 4;
		}
		ri += 4;
		x += 1;
	}
	glEnd();
}

/**
 * @brief Wrapper for glPoint
 * Since glPointSize is not available on all graphic cards
 * we have to use a wrapper
 * @param x
 * @param y
 * @param sz
 */
inline void GLExtra::bglPoint(const float x, const float y, const float sz) {
	const float off = sz * 0.5;
	glRectf(x - off, y - off, x + off, y + off);
}

/**
 * @brief Draw field lines (border, middle) and call other draw functions
 * NOTE keep in sync with field.h
 */
void GLExtra::bglDrawField() {
	//Border + Middle line
	glBegin(GL_LINE_STRIP);
	{
		glVertex2i(0, -BSmart::Field::half_field_height);
		glVertex2i(0, BSmart::Field::half_field_height);
		glVertex2i(-BSmart::Field::half_field_width, BSmart::Field::half_field_height);
		glVertex2i(-BSmart::Field::half_field_width, -BSmart::Field::half_field_height);
		glVertex2i(BSmart::Field::half_field_width, -BSmart::Field::half_field_height);
		glVertex2i(BSmart::Field::half_field_width, BSmart::Field::half_field_height);
		glVertex2i(0, BSmart::Field::half_field_height);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	{
		glVertex2i(-BSmart::Field::half_width, BSmart::Field::half_height);
		glVertex2i(BSmart::Field::half_width, BSmart::Field::half_height);
		glVertex2i(BSmart::Field::half_width, -BSmart::Field::half_height);
		glVertex2i(-BSmart::Field::half_width, -BSmart::Field::half_height);
		glVertex2i(-BSmart::Field::half_width, BSmart::Field::half_height);
	}
	glEnd();

	glPushMatrix();
	{
		draw_defense_area(0);
		//        glLoadIdentity();
		//glRotatef(180.0, 0.0, 0.0, 1.0); <=> Scale(-1, -1, 0)
		glScalef(-1.0, 1.0, 1.0);
		draw_defense_area(0);
	}
	glPopMatrix();
	draw_marks();
}

/**
 * @brief Draw defense area
 * NOTE keep in sync with field.h
 * @param offset Offset that is added to defense radius
 */
void GLExtra::draw_defense_area(int offset) {
	static const float half_dline = BSmart::Field::defense_line * 0.5;
	glPushMatrix();
	glTranslatef(-BSmart::Field::half_field_width, half_dline, 0.0);
	bglBresCircle(BSmart::Field::defense_radius + offset, Q_I);
	glBegin(GL_LINES);
	glVertex2i(BSmart::Field::defense_radius + offset, 0);
	glVertex2i(BSmart::Field::defense_radius + offset, -BSmart::Field::defense_line);
	glEnd();
	glTranslatef(0, -BSmart::Field::defense_line, 0.0);
	bglBresCircle(BSmart::Field::defense_radius + offset, Q_IV);
	glPopMatrix();
}

/**
 * @brief Draw both goals
 * NOTE keep in sync with field.h
 */
void GLExtra::draw_goal() {
	static const int half_goal_width = BSmart::Field::goal_width / 2;
	static const int goal_back = (BSmart::Field::half_field_width + BSmart::Field::goal_depth);
	glPushAttrib(GL_LINE_BIT);
	glLineWidth(3.0);
	glBegin(GL_LINE_STRIP);
	glVertex2i(BSmart::Field::half_field_width, -half_goal_width);
	glVertex2i(goal_back, -half_goal_width);
	glVertex2i(goal_back, half_goal_width);
	glVertex2i(BSmart::Field::half_field_width, half_goal_width);
	glEnd();
	glPopAttrib();
}

/**
 * @brief Draw marks of field (goal, penalty mark, center mark)
 * NOTE keep in sync with field.h
 */
void GLExtra::draw_marks() {
	static const float pt = (BSmart::Field::half_field_width - BSmart::Field::penalty_mark_distance);
	bglBresCircle(BSmart::Field::center_radius); //Center circle
	glColor3f(1, 0, 0); //Center mark color
	bglPoint(0, 0, 20); //center mark

	glColor3f(0.0, 0.3, 1.0); //right team color
	bglPoint(pt, 0, 20); //right penalty mark
	draw_goal(); //right goal

	glColor3f(1.0, 1.0, 0.0); //left team color

	glPushMatrix();
	{
		glScalef(-1.0, -1.0, 0.0); //mirror
		bglPoint(pt, 0, 20); //left penalty mark
		draw_goal(); //right goal
	}
	glPopMatrix();
}

/**
 * @brief Draw filter data (objects that move like robots, ball)
 */
void GLExtra::bglDrawFilterData() {
	tmp_perc_robots.clear();
	current_robot_percepts.clear();
	tmp_sample_robots.clear();
	//    robot_samples.clear();
	robot_models.clear();

	//get data
	current_ball_percepts = filter_data->get_current_ball_percepts();
	//    ball_samples = filter_data->get_ball_samples();
	ball_model = filter_data->get_ball_model();
	for (int team = 0; team < Filter_Data::NUMBER_OF_TEAMS; ++team) {
		for (int id = 0; id < Filter_Data::NUMBER_OF_IDS; ++id) {
			if (filter_data->get_robot_seen(team, id)) {
				tmp_perc_robots = filter_data->get_current_robot_percepts(team, id);
				current_robot_percepts.insert(current_robot_percepts.end(), tmp_perc_robots.begin(),
						tmp_perc_robots.end());

				robot_models.push_back(filter_data->get_robot_model(team, id));
			}
		}
	}

	//draw data
	//current robot percepts
	/*
	 * Ein Percept ist die gemessene Position, also die Position die von
	 * der SSL-Vision kommt. Optimalerweise gibt es ein Percept pro Roboter
	 * und Ball pro Frame.
	 */
	for (Robot_Percept_List::iterator it = current_robot_percepts.begin(); it != current_robot_percepts.end(); it++) {
		double rotation = 0.0;
		if (it->rotation_known) {
			rotation = it->rotation;
		}
		assert(rotation < 7 && rotation > -7);
		draw_robot(it->x, it->y, it->color, rotation, -1, -1, false);
	}

	//robot samples
	/*
	 * Ein Sample ist eine von vielen (afair 100 in der refbox) Hypothesen,
die der Particle Filter über die Roboterposition aufstellt. Zu jedem
Roboter und für den Ball gibt es eine eigene Partikelmenge (jaja,
schönes deutsch-Englisch-Gemisch :) ). Die Particle werden nach
"physikalischen" Kriterien bewegt und anschließend über die Messung
(das Percept) gewichtet. Anschließend werden aus der gewichteten Menge
die "guten" Particle beibehalten und die "schlechten" entfernt.
Particle können im Gegensatz zu den Percepten eine Geschwindigkeit
haben. Dadurch, dass die Particle auch weiter bewegt werden, wenn der
Gegenstand ein paar Frames nicht gesehen wird (kein Percept), kann die
ungefähre Position auch in den folgenden Frames geschätzt werden.
Durch höhere Streuung wegen fehlender Gewichtung natürlich ungenauer.
	 */
	for (Robot_Sample_List::iterator it = robot_samples.begin(); it != robot_samples.end(); it++) {
		draw_robot(it->pos.x, it->pos.y, SSLRefbox::Colors::WHITE, it->pos.rotation, -1, -1, false);
		LOG4CXX_DEBUG(logger,
				"Whoohu, there was a sample. Dont know what it is actually and it does not appear in log file play mode...");
	}

	//robot models
	/*
	 *  Das Model ist die aktuell vom Particle Filter geschätzte Position.
Es wird aus der Partikelmenge extrahiert und hat dann ebenso wie die
Particle eine Position, eine Geschwindigkeit und eine
Kollisionsinformation. Dieses Model wird dann im Folgenden für die
Regelüberprüfungen verwendet, pro Objekt gibt es nur ein Model.
	 */
	bool last_touched;
	glLineWidth(2);
	for (Robot_Sample_List::iterator it = robot_models.begin(); it != robot_models.end(); it++) {
		last_touched = ((it->team == ball_model.last_touched_robot.x) && (it->id == ball_model.last_touched_robot.y));
//		printf("m ",it->pos.rotation);
		draw_robot(it->pos.x, it->pos.y, SSLRefbox::Colors::RED, it->pos.rotation, it->team, it->id, last_touched);
	}
	glLineWidth(1);

	//current ball percepts
	for (Ball_Percept_List::iterator it = current_ball_percepts.begin(); it != current_ball_percepts.end(); it++) {
		draw_ball(it->x, it->y, 0., SSLRefbox::Colors::ORANGE);
	}

	/*
	 //ball samples
	 for (Ball_Sample_List::iterator it = ball_samples.begin(); it != ball_samples.end(); it++)
	 {
	 draw_ball(it->pos.x, it->pos.y, it->pos.z, SSLRefbox::Colors::WHITE);
	 }
	 */

	//shadow for ball_model
	BSmart::Pose shadow = BSmart::Pose(1., -1.);
	shadow.normalize(ball_model.pos.z);
	draw_ball(ball_model.pos.x + shadow.x, ball_model.pos.y + shadow.y, 0, SSLRefbox::Colors::MAGENTA);

	//ball model
	draw_ball(ball_model.pos.x, ball_model.pos.y, ball_model.pos.z, SSLRefbox::Colors::RED);

}

/**
 * @brief Draw a robot with given color to given position and mark, if it was last touched robot.
 * @param x x_position
 * @param y y_position
 * @param color SSLRefbox::Colors::Color, also important for team, sample and model
 * @param team team
 * @param id robot id
 * @param last_touched is this the bot that was last touched?
 */
void GLExtra::draw_robot(int x, int y, SSLRefbox::Colors::Color color, double rotation, int team, int id,
		bool last_touched) {
	glPushMatrix();
	glTranslatef(x, y, 0.);

	switch (color) {
	case SSLRefbox::Colors::YELLOW:
		glColor3d(1., 1., 0.);
		glBegin(GL_TRIANGLE_FAN);
		glVertex3d(0., 0., 0.);
		break;

	case SSLRefbox::Colors::BLUE:
		glColor3d(0., 0., 1.);
		glBegin(GL_TRIANGLE_FAN);
		glVertex3d(0., 0., 0.);
		break;

	case SSLRefbox::Colors::WHITE: //white: sample
		glColor3d(1., 1., 1.);
		glBegin(GL_LINE_STRIP);
		if (rotation != 0)
			glVertex3d(0., 0., 0.);
		break;

	case SSLRefbox::Colors::RED: //red: model
		glColor3d(1., 0., 0.);
		glBegin(GL_LINE_STRIP);
		if (rotation != 0)
			glVertex3d(0., 0., 0.);
		break;

	default: //black: default, should not happen
		std::ostringstream o;
		o << "unknown robot at (" << x << "|" << y << ") color: " << color << std::endl;
		LOG4CXX_WARN(logger, o.str());
		glColor3d(0., 0., 0.);
		glBegin(GL_TRIANGLE_FAN);
	}

	double f = rotation;
//	if (abs(f) > 7) {
//		printf("%f aaaaaaaaa\n", f);
//	} else
//		printf("%f\n", f);
	for (int i = 0; i <= 12; ++i) {
		glVertex3d(cos(f) * BSmart::Field::robot_radius, sin(f) * BSmart::Field::robot_radius, 0);
		f = f + (2 * BSmart::pi / 12);
	}
	glEnd();

	// mark robot that last touched ball with a grey square
	if (last_touched) {
		glColor3d(0.6, 0.6, 0.6); //grey
		glBegin(GL_QUADS); // Draw a square
		glVertex3f(-BSmart::Field::robot_radius / 2, BSmart::Field::robot_radius / 2, 0.0f); // top left
		glVertex3f(BSmart::Field::robot_radius / 2, BSmart::Field::robot_radius / 2, 0.0f); // top right
		glVertex3f(BSmart::Field::robot_radius / 2, -BSmart::Field::robot_radius / 2, 0.0f); // bottom right
		glVertex3f(-BSmart::Field::robot_radius / 2, -BSmart::Field::robot_radius / 2, 0.0f); // bottom left
		glEnd();
	}

	glPopMatrix();

	//print the number
	if (id != -1) {
		std::string string = "";
		int len = int_to_string(string, id);

		glPushMatrix();
		if (team == 1) {
			glColor3d(1., 1., 1.); //white
		} else if (team == 0) {
			glColor3d(0., 0., 0.); //black
		}
		int bitmapWidth = glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, '0') * len;
		// center number in robot
		glRasterPos2f(x - (BSmart::Field::robot_radius - bitmapWidth) / 2, y - 55);
		for (int j = 0; j < len; j++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[j]);
		}
		glPopMatrix();
	}
}

/**
 * @brief Draw ball onto surface with given position and color
 * Color can be one of ORANGE (Percepts), WHITE (Filter Samples), MAGENTA (Ball shadow) or RED (Filter Model)
 * @param x
 * @param y
 * @param z
 * @param color
 */
void GLExtra::draw_ball(double x, double y, double z, SSLRefbox::Colors::Color color) {
	glPushMatrix();
	glTranslatef(x, y, z + BSmart::Field::ball_radius);

	// handle color
	switch (color) {
	case SSLRefbox::Colors::ORANGE: // Percepts
		glColor3d(0.96875, 0.55078125, 0.09765625);
		break;

	case SSLRefbox::Colors::WHITE: // Filter Samples
		glColor3d(1., 1., 1.);
		break;

	case SSLRefbox::Colors::MAGENTA: // Ball shadow
		glColor3d(1., 0.250980392, 1.);
		break;

	case SSLRefbox::Colors::RED: // Filter Model
		glColor3d(1., 0., 0.);
		break;

	default: //default, should not happen
		std::ostringstream o;
		o << "unknown ball at (" << x << "|" << y << ") color: " << color << std::endl;
		LOG4CXX_WARN(logger, o.str());
		glColor3d(0., 0., 0.); //black, default
	}

	glBegin(GL_TRIANGLE_FAN);
	glVertex3d(0., 0., 0.);
	double f = 0.;
	for (int i = 0; i <= 12; ++i) {
		glVertex3d(cos(f) * BSmart::Field::ball_radius, sin(f) * BSmart::Field::ball_radius, 0);
		f = f + (2 * BSmart::pi / 12);
	}
	glEnd();

	glPopMatrix();
}

/**
 * @brief Convert i into a string and store it in given string object.
 * Additionally returns the length of the number.
 * @param string string object to store the resulting string
 * @param i number to be converted
 * @return length of given number
 */
int GLExtra::int_to_string(std::string& string, int i) {
	int len = 0;
	int tmp = i;
	string = "";

	while (tmp > 0) {
		len++;
		tmp /= 10;
		string += " ";
	}
	if (i == 0) {
		len = 1;
		string = " ";
	}

	tmp = i;

	for (int j = 0; j < len; ++j) {
		int a = tmp % 10;
		string[len - j - 1] = (char) a + 48;
		tmp /= 10;
	}

	return len;
}

/**
 * Draw the following parts of GUI:
 * <ul>
 * <li>rule_breaker (the robot, who broke the rule)</li>
 * <li>freekick position</li>
 * <li>circle around ball</li>
 * <li>defense area</li>
 * <li>line for smth (something?!)</li>
 * <li>string output for rules</li>
 * <li>Play_States</li>
 * </ul>
 *
 */
void GLExtra::bglDrawRulesystemData() {
	broken_rule_vector.clear();
	broken_rule_vector = filter_data->get_broken_rules();
	internal_play_states = filter_data->get_internal_play_states();
	cur_timestamp = filter_data->get_timestamp();
	int rule_counter = 0;
	glLineWidth(2);

	for (std::vector<Broken_Rule>::reverse_iterator brit = broken_rule_vector.rbegin();
			brit != broken_rule_vector.rend(); ++brit) {
		if ((cur_timestamp - brit->when_broken) > 5000) {
			break;
		}

		//rule_breaker (the robot, who broke the rule)
		for (Robot_Sample_List::iterator it = robot_models.begin(); it != robot_models.end(); it++) {
			if ((it->team == brit->rule_breaker.x) && (it->id == brit->rule_breaker.y)) {
				glPushMatrix();
				glTranslatef(it->pos.x, it->pos.y, 0.);

				double f = 0.;
				glColor3d(1., 0., 0.);
				glBegin(GL_LINE_STRIP);
				for (int i = 0; i <= 24; ++i) {
					glVertex3d(cos(f) * (BSmart::Field::robot_radius + 100.),
							sin(f) * (BSmart::Field::robot_radius + 100.), 0);
					f = f + (2 * BSmart::pi / 24);
				}
				glEnd();
				glPopMatrix();
			}
		}

		//freekick_pos
		if (brit->freekick_pos.x != -1) {
			glPushMatrix();
			glLineWidth(3);
			glTranslatef(brit->freekick_pos.x, brit->freekick_pos.y, 0.);
			glColor3d(1., 0., 0.);
			int l = 90;
			glBegin(GL_LINES);
			glVertex3d(-l, -l, 0.);
			glVertex3d(l, l, 0.);
			glVertex3d(-l, l, 0.);
			glVertex3d(l, -l, 0.);
			glEnd();
			glLineWidth(2);
			glPopMatrix();
		}

		//circle around ball
		if (brit->circle_around_ball) {
			glPushMatrix();
			glTranslatef(ball_model.pos.x, ball_model.pos.y, 0.);

			double f = 0.;
			glColor3d(1., 0., 0.);
			glBegin(GL_LINE_STRIP);
			for (int i = 0; i <= 12; ++i) {
				glVertex3d(cos(f) * (500.), sin(f) * (500.), 0);
				f = f + (2 * BSmart::pi / 12);
			}
			glEnd();
			glPopMatrix();
		}

		//defense area
		if (brit->defense_area == 0) {
			glPushMatrix();
			draw_defense_area(200);
			glPopMatrix();
		} else if (brit->defense_area == 1) {
			glPushMatrix();
			glScalef(-1.0, 1.0, 1.0);
			draw_defense_area(200);
			glPopMatrix();
		}

		//line for smth (something?!)
		if (brit->line_for_smth.p1.x != -1) {
			glPushMatrix();
			glLineWidth(3);
			glColor3d(1., 0., 0.);
			glBegin(GL_LINES);
			glVertex3d(brit->line_for_smth.p1.x, brit->line_for_smth.p1.y, 0.);
			glVertex3d(brit->line_for_smth.p2.x, brit->line_for_smth.p2.y, 0.);
			glEnd();
			glLineWidth(2);
			glPopMatrix();
		}

		//string output for rules
		glPushMatrix();
		glColor3d(1., 1., 1.);
		GLfloat x = (-BSmart::Field::half_field_width + 100);
		GLfloat y = (BSmart::Field::half_field_height - 200) - (rule_counter * 250);
		glRasterPos2f(x, y);
		std::string tmp = "";

		std::string string;
		if (brit->rule_number < 1 || brit->rule_number > 42) {
			string = "unknown";
			std::ostringstream o;
			o << "Bad index for rule: " << brit->rule_number << std::endl;
			LOG4CXX_WARN(logger, o.str());
		} else {
			string = Global::rulenames[brit->rule_number - 1];
		}
		if (brit->rule_number == 29) {
			string += " by ";
			if (brit->rule_breaker.x == 0)
				string += "Yellow ";
			else
				string += "Blue ";
			int_to_string(tmp, brit->rule_breaker.y);
			string += tmp;
			string += " New Standing: ";
			int_to_string(tmp, brit->standing.x);
			string += tmp;
			string += ":";
			int_to_string(tmp, brit->standing.y);
			string += tmp;
		} else if (brit->rule_breaker.x != -1) {
			string += " by ";
			if (brit->rule_breaker.x == 0)
				string += "Yellow ";
			else
				string += "Blue ";
			int_to_string(tmp, brit->rule_breaker.y);
			string += tmp;
		}

		int len, i;
		len = string.size();
		void *font = GLUT_BITMAP_TIMES_ROMAN_24;
		for (i = 0; i < len; i++) {
			glutBitmapCharacter(font, string[i]);
		}
		glPopMatrix();

		rule_counter++;
	}

	//Draw Play_States
	gamestate->set_play_state((BSmart::Game_States::Play_State) internal_play_states.x);
	std::string play_state_intern(gamestate->play_state_string());

	std::string string = "internal Play_State: ";
	string += play_state_intern;

	glPushMatrix();
	glColor3d(1., 1., 1.);
	GLfloat x = -1480.;
	GLfloat y = 2052.;
	glRasterPos2f(x, y);

	int len, i;
	len = string.size();
	for (i = 0; i < len; i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, string[i]);
	}
	glPopMatrix();

	gamestate->set_play_state((BSmart::Game_States::Play_State) internal_play_states.y);
	std::string play_state_intern_next(gamestate->play_state_string());

	string = "next internal Play_State: ";
	string += play_state_intern_next;

	glPushMatrix();
	glColor3d(1., 1., 1.);
	x = 1020.;
	y = 2052.;
	glRasterPos2f(x, y);

	len = string.size();
	for (i = 0; i < len; i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, string[i]);
	}
	glPopMatrix();

	glLineWidth(1);
}

