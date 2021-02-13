#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <vector>
#include <set>
#include <stack>
#include <queue>
#include <unordered_map>
#include "Structs.h"
using namespace std;
#define INF 0x3f3f3f3f
class PublicTransport
{
public:

	static string file_name; //!< Default file

	//! Default constructor
	PublicTransport();
	//! Reads data from a text file. If the file is empty, it calls the default constructor
	PublicTransport(ifstream& file);

	//! Finds a bus stop's index, by given bus stop name
	int find_bus_stop(string name) const;
	//! Finds the fastest path by using modified Dijkstra algorithm
	void find_fastest_path();

	//! Checks if there are any courses in the line
	bool contains_courses(int line);
	//! Adds 1 bus stop on front or on back of a currently existing bus line
	bool extend_line(int line, const string& name1, const string& name2);
	//! Adds a new course to a currently existing bus line (adds another bus from the same line going through the line in other time)
	bool add_new_course(int line);
	//! Adds a new line
	bool add_new_line();

	//! Removes 1 bus stop
	void remove_bus_stop();
	//! Removes 1 course from a line
	bool remove_course(int line);
	//! Removes a whole line and all courses from that line
	bool remove_line(int line);

	//! Loads data from a text file
	void load_data(ifstream& file);
	//! Saves data to a text file
	void save_data(ofstream& file);

	//! Shows bus stops and a list with the busses that go through the bus stop
	void print_bus_stops_schedule();
	//! Shows all courses from a selected line or all lines
	void print_courses();
	//! Shows the bus stops from a selected line or all lines
	void print_line();
	//! Shows the bus stops locations on a map (More information about location logic -> Help menu, 1. & 8. ) 
	void print_order();
	//! Help menu
	void print_help();

	//! Type any symbol to continue the program execution	
	void type_to_continue();
	//! Executes user's order
	void users_option(int option);
	//! Main menu
	void menu();

private:	

	//! Creates all courses (Creates all graph edges)
	void make_map();
	//! Iterates through bus list to find the correct position
	list<Bus>::iterator  find_place(int line, int bs);
	//! Shows how to reach from one stop to another 
	void print_path(int bs1, int bs2, const vector<Path>& dist);
	//! Relaxing edges
	void update_shortest_time(vector<Path>& dist, set<int>& setds, const Course& c, int current_stop);

	//! Adds a bus stop location to the map
	void add_stop_to_order(int bs1, int bs2, const string& name);
	//! Returns if a bus stop can be added to a line by checking bus stop locations
	bool check_stops_order(int current, int to_add, bool adding_back, const list<int>& order);
	//! A help function when creating a new bus stop in some cases
	bool create_name_and_order(const string& name, int bs1, int bs2, bool new_start);
	//! Checks the last string symbol. If it is ' the stop is considered as opposite
	bool is_reverse_stop(const string& name);

	//! Tries to add a bus stop to a line
	bool add_bus_stops_to_line(int stops_count, vector<string>& bs_names);
	//! Creates a bus stop and checks if it's location is right for the line that we are trying to create
	bool new_line_bs_order(int stops_count, const vector<string>& bs_names, bool rev_bs);
	
	//! Creates the 1st course of a new line
	void create_first_course(int newline, const vector<string>& bs_names);
	//! Adds a course (edge) to the vector of courses (edges)
	void add_course(int bs1, int bs2, int time_begin, int time_end, int bus_number);
	//! Sets the time when a bus travells to (arrives at) a bus stop
	void set_course_time(queue<int>& arrival_time, int line, int bs1, int bs2, int count, list<Bus>::iterator& bus, const string& name1, const string& name2, bool new_start);
	//! Creates a new bus stop with a given name
	void create_new_bs(const string& name);
	//! Calls "create_new_bs(...)" function and tries to set a course going through the new bus stop
	bool create_bs_set_time(queue<int>& arrival_time, int line, int bs1, int bs2, int count, bool new_start, const string& name1, const string& name2);
	//! Adds a stop to a line
	void add_stop_to_line(queue<int>& arrival_time, int line, int bs1, int bs2, int count, const string& name1, const string& name2, bool new_start);
	

	//! When removing a bus stop from the middle of a line, it redirects the courses from the previous and the next bus stop
	void redirecting_courses(int bs, int index);
	//! Removes a bus stop
	void delete_bs(int bs);
	//! Removes one course from a line
	void delete_course(int bs, int line);
	//! Zeroes class members
	void clear();


	int max_bus_stops_count; //!< Keeps the highest number of bus stops ever added in the program. Vectors have size = this variable
	int real_bs_count; //!< Keeps the real bus stops count
	int lines_count; //!< Keeps the lines count
	list<int> stops_order; //!< Keeps the stops order
	list<int> opposite_stops_order; //!< Keeps the opposite stops order. Note that if A -> B -> C then C' -> B' -> A'
	vector<BusStop> bus_stops; //!< Vector keeping all bus stops (Vertices of the graph). Any index in the vector is equal to a bus stop number. The vector size is not reduced when removing stops. The index is only marked
	unordered_map<string, int> bs_index; //!< By given bus stop name, it keeps the bus stop index
	priority_queue<int, vector<int>, less<int>> availabe_num; //!< When removing a bus stop it keeps the stop's number. A new stop will receive a number from the queue and an index from bus_stop vector 
	map<int, TransportLine> lines; //!< Keeps all bus lines
	vector<list<Course>> courses; //!< Keeps all courses. These are the edges of the graph
	
};

