#pragma once
#include <string>
#include <list>
#include <map>
using namespace std;

struct TransportLine
{
	int line_number; //!< Line number
	int stops_count; //!< Number of stops that the line contains
	list<int> bus_stops; //!< List of all stops numbers that the line contains, sorted (list.front() is where the line begins, list.end() is where the line ends)
	TransportLine(int line_number = 0, int stops_count = 0) : line_number(line_number), stops_count(stops_count) {};
};

struct Bus
{
	int number; //!< Bus number = on the line it is moving
	int arrival_time; //!< The time it arrives on a stop
	bool connected = false;
	Bus(int n = -1, int time = -1) : number(n), arrival_time(time) {};
};

//! Graph Vertices
struct BusStop
{
	int number; //!< Bus stop number, equals to bus stops vector's index
	int reverse_stop; //!< Keeps the opposide stop number
	string name; //!< Bus stop name
	int busses_count; //!< The count of all busses going through the bus stop
	list<Bus> busses; //!< List of all busses going through the bus stop
	BusStop() = default;
};

//! Graph Edges
struct Course
{
	int destination; //!< Number of a bus stop, where it is headed
	int time_begin; //!< The time when a bus starts travelling toward the destionation stop
	int time_end; //!< The time when a bus arrives at the destination stop
	int bus; //!< The bus number doing the course
	Course(int destination = -1, int time_begin = -1, int time_end = -1, int bus = -1) :
		destination(destination),
		time_begin(time_begin),
		time_end(time_end),
		bus(bus) {};
};

//! A struct used to show the shortest path between 2 bus stops
struct Path
{
	int time; //!< The time when a bus arrives at any bus stop
	int from; //!< The number of the previous bus stop
	int bus; //!< Bus number
	Path(int t, int f, int b) : time(t), from(f), bus(b) {};
};