#include "PublicTransport.h"
using namespace std;

string PublicTransport::file_name = "file.txt";

PublicTransport::PublicTransport()
{
	max_bus_stops_count = 0;
	real_bs_count = 0;
	lines_count = 0;
}
PublicTransport::PublicTransport(ifstream& file)
{
	load_data(file);
}

int PublicTransport::find_bus_stop(string name) const
{
	return bs_index.find(name) != bs_index.end() ? bs_index.find(name)->second : -1;
}

void PublicTransport::make_map()
{
	courses.clear();
	courses.resize(max_bus_stops_count);
	for (const pair<int,TransportLine> p_it : lines)
	{
		TransportLine it_line = p_it.second;
		for (list<int>::iterator it_bs = it_line.bus_stops.begin(); it_bs != prev(it_line.bus_stops.end()); ++it_bs)
		{
			list<Bus>::iterator it_bus_start = bus_stops[*it_bs].busses.begin();
			list<Bus>::iterator it_bus_end = bus_stops[*next(it_bs)].busses.begin();
			for (it_bus_end; it_bus_end != bus_stops[*next(it_bs)].busses.end() && it_bus_start != bus_stops[*it_bs].busses.end(); ++it_bus_end)
			{
				while (it_bus_start != bus_stops[*it_bs].busses.end() && it_bus_start->number != it_line.line_number)
					++it_bus_start;

				if (it_bus_start != bus_stops[*it_bs].busses.end() && it_bus_end->number == it_line.line_number && !it_bus_end->connected)
				{
					it_bus_end->connected = true;
					add_course(*it_bs, *next(it_bs), it_bus_start->arrival_time, it_bus_end->arrival_time, it_line.line_number);
					++it_bus_start;
				}
			}
		}
	}
}

list<Bus>::iterator PublicTransport::find_place(int line, int bs)
{	
	list<Bus>::iterator bus = bus_stops[bs].busses.begin();
	while (bus != bus_stops[bs].busses.end() && bus->number < line)
		++bus;

	return bus;
}

bool PublicTransport::check_stops_order(int current, int to_add, bool adding_back, const list<int>& seq)
{
//	if (adding_back)
	{
		for (const int bs : seq)
		{
			if (bs == current)
				return true;
			if (bs == to_add)
				return false;
		}
	}
	/*else
	{
		for (const int bs : seq)
		{
			if (bs == current)
				return false;
			if (bs == to_add)
				return true;
		}
	}
	*/
	return true;
}

bool PublicTransport::create_name_and_order(const string& name, int bs1, int bs2, bool new_start)
{
	if (name[name.size() - 1] == '\'' && name[name.size() - 2] == '\'')
	{
		cout << name << " is invalid bus name! Please read the help panel!" << endl;
		return false;
	}

	create_new_bs(name);

	if (new_start)
		bs1 = find_bus_stop(name);
	else
		bs2 = find_bus_stop(name);

	if (*prev(name.end()) != '\'')
	{
		if (!check_stops_order(bs1, bs2, !new_start, stops_order))
			return false;
	}
	else
	{
		if (!check_stops_order(bs1, bs2, !new_start, opposite_stops_order))
			return false;
	}

	return true;
}

bool PublicTransport::is_reverse_stop(const string& name)
{
	if (name[name.size() - 1] == '\'')
		return true;
	return false;
}

bool PublicTransport::add_bus_stops_to_line(int stops_count, vector<string>& bs_names)
{
	cout << 1 << ". ";
	cin >> bs_names[0];
	if (bs_names[0][bs_names[0].size() - 1] == '\'' && bs_names[0][bs_names[0].size() - 2] == '\'')
	{
		cout << "Invalid bus name has been detected! Please read the help panel" << endl;
		return false;
	}
	bool bs_rev = is_reverse_stop(bs_names[0]);

	for (int i = 1; i < stops_count; ++i)
	{
		cout << i + 1 << ". ";
		cin >> bs_names[i];
		for (int j = 0; j < i; j++)
		{
			if (bs_names[i] == bs_names[j])
			{
				cout << "Invalid line! The line can't contain cycles!" << endl;
				return false;
			}

		}
		if (bs_names[i][bs_names[i].size() - 1] == '\'' && bs_names[i][bs_names[i].size() - 2] == '\'')
		{
			cout << "Invalid bus name has been detected! Please read the help panel" << endl;
			return false;
		}
		if (bs_rev != is_reverse_stop(bs_names[i]))
		{
			cout << "Invalid bus line! It can't contain bus stops from both road lanes" << endl;
			return false;
		}
	}

	return true;
}

bool PublicTransport::new_line_bs_order(int stops_count, const vector<string>& bs_names, bool rev_bs)
{
	if (find_bus_stop(bs_names[0]) == -1)
		create_new_bs(bs_names[0]);

	string prev = bs_names[0];
	for (int i = 1; i < stops_count; ++i)
	{
		if (find_bus_stop(bs_names[i]) == -1)
			create_new_bs(bs_names[i]);
		if (!rev_bs)
		{
			if (!check_stops_order(find_bus_stop(prev), find_bus_stop(bs_names[i]), true, stops_order))
			{
				cout << "\nLogical mistake has been found in bus stops order! Please check the current bus stops oreder from the main menu!" << endl;
				return false;
			}
		}
		else
		{
			if (!check_stops_order(find_bus_stop(prev), find_bus_stop(bs_names[i]), true, opposite_stops_order))
			{
				cout << "\nLogical mistake has been found in bus stops order! Please check the current bus stops order from the main menu!" << endl;
				return false;
			}
		}
		prev = bs_names[i];
	}

	return true;
}

void PublicTransport::add_stop_to_order(int bs1, int bs2, const string& name)
{
	print_order();
	string str;
	cout << "\nAfter which stop do you want to add bus stop " << name << "?\nType any invalid bus name to add it on first position.\nNote: "<<name<<"' will be added in the correct position automatically\n--> ";
	cin >> str;
	int b = find_bus_stop(str);
	if (b == -1 || str == name)
	{
		stops_order.push_front(bs1);
		opposite_stops_order.push_back(bs2);
	}
	else
	{
		list<int>::iterator it = stops_order.begin();
		while (*it != b)
			++it;
		stops_order.insert(++it, bs1);

		b = bus_stops[b].reverse_stop;
		list<int>::iterator it2 = opposite_stops_order.begin();
		while (*it2 != b)
			++it2;
		opposite_stops_order.insert(it2, bs2);
	}
}

void PublicTransport::create_first_course(int newline, const vector<string>& bs_names)
{
	int index1 = find_bus_stop(bs_names[0]);
	lines.find(newline)->second.bus_stops.push_back(index1);

	int index2 = find_bus_stop(bs_names[1]);
	lines.find(newline)->second.bus_stops.push_back(index2);

	cout << "\nCreating bus line " << newline << endl << endl;
	int prev_stop_time1 = 0, prev_stop_time2 = 0, time1, time2;

	cout << "Creating relation " << bs_names[0] << " --> " << bs_names[1] << endl;
	cout << "The bus arrives at " << bs_names[0] << " at:  ";
	cin >> time1;
	while (time1 < prev_stop_time1)
	{
		cout << "The bus can't be at bus stop " << bs_names[0] << " earlier than " << prev_stop_time1 << "' !\nEnter again... ";
		cin >> time1;
	}
	prev_stop_time1 = time1;
	bus_stops[index1].busses.insert(find_place(newline, index1), Bus(newline, time1));
	bus_stops[index1].busses_count++;

	cout << "The bus traveling from " << bs_names[0] << " at " << time1 << "' arrives at bus stop " << bs_names[1] << " at:  ";
	cin >> time2;
	while (time2 <= max(prev_stop_time1, prev_stop_time2))
	{
		cout << "The bus can't be at bus stop " << bs_names[1] << " earlier than " << max(prev_stop_time1, prev_stop_time2) + 1 << "' !\nEnter again... ";
		cin >> time2;
	}
	prev_stop_time2 = time2;
	bus_stops[index2].busses.insert(find_place(newline, index2), Bus(newline, time2));
	bus_stops[index2].busses_count++;

	add_course(index1, index2, time1, time2, newline);
}

void PublicTransport::add_course(int bs1, int bs2, int time_begin, int time_end, int bus_number)
{
	courses[bs1].push_back(Course(bs2, time_begin, time_end, bus_number));
}

void PublicTransport::update_shortest_time(vector<Path>& dist, set<int>& setds, const Course& c, int current_stop)
{
	int reverse_stop;
	int v = c.destination;
	int time = c.time_end;

	if (dist[v].time > time)
	{
		if (dist[v].time != INF)
			setds.erase(v);

		dist[v].time = time;
		dist[v].from = current_stop;
		dist[v].bus = c.bus;
		setds.insert(v);

		reverse_stop = bus_stops[v].reverse_stop;
		if (dist[reverse_stop].time > time)
		{
			if (dist[reverse_stop].time != INF)
				setds.erase(reverse_stop);

			dist[reverse_stop].time = time;
			dist[reverse_stop].from = v;
			dist[reverse_stop].bus = -1;
			setds.insert(reverse_stop);
		}
	}
}

void PublicTransport::print_path(int bs1, int bs2, const vector<Path>& dist)
{
	stack<Path> full_path; // Where it is coming from, the time when it arrives and the bus number
	stack<int> bus_stop;  // Current stop
	while (bs1 != dist[bs2].from)
	{
		full_path.push(Path(dist[bs2].time, dist[bs2].from, dist[bs2].bus));
		bus_stop.push(bs2);
		bs2 = dist[bs2].from;
	}
	full_path.push(Path(dist[bs2].time, dist[bs2].from, dist[bs2].bus));
	bus_stop.push(bs2);

	while (!full_path.empty())
	{
		if (full_path.top().bus != -1)
			cout << "Travel to bus stop " << bus_stops[bus_stop.top()].name << " with bus: " << full_path.top().bus << ". You will arrive at: " << full_path.top().time << endl;
		else
			cout << "Cross the road to reach the opposite bus stop ( " << bus_stops[bus_stops[full_path.top().from].reverse_stop].name <<" )"<< endl;
		full_path.pop();
		bus_stop.pop();
	}
}

void PublicTransport::find_fastest_path()
{
	string name1, name2;
	int start_time, bs1, bs2;

	cout << "Enter name of a bus stop:\nStart --> ";
	cin >> name1;
	bs1 = find_bus_stop(name1);
	if (bs1 == -1)
	{
		cout << "The bus stop doesn't exist!" << endl;
		return;
	}
	cout << "End --> ";
	cin >> name2;
	bs2 = find_bus_stop(name2);
	if(bs2 == -1)
	{
		cout << "The bus stop doesn't exist!" << endl;
		return;
	}
	cout << "Time begin --> ";
	cin >> start_time;
	if (start_time < 0)
	{
		cout << "Invalid time!" << endl;
		return;
	}

	set<int> setds;
	vector<Path> dist(max_bus_stops_count, Path(INF, -1, -1));

	int reverse_stop = bus_stops[bs1].reverse_stop;

	setds.insert(bs1);
	dist[bs1].time = start_time;
	dist[bs1].from = bs1;
	dist[reverse_stop].time = start_time;
	dist[reverse_stop].from = bs1;

	int u = *(setds.begin());
	setds.erase(setds.begin());
	setds.insert(reverse_stop);

	for (const Course& c : courses[u])
	{
		if (c.time_begin >= start_time)
			update_shortest_time(dist, setds, c, u);
	}

	while (!setds.empty())
	{
		int u = *(setds.begin());
		setds.erase(setds.begin());

		for (const Course& c : courses[u])
		{
			if (c.time_begin >= dist[u].time)
			{
				update_shortest_time(dist, setds, c, u);
			}
		}
	}

	if (dist[bs2].time == INF)
		cout << "\nThe bus stop cannot be reached!" << endl;
	else
	{
		cout << "\nStart at stop: " << bus_stops[bs1].name << ",  at time: " << start_time << endl;
		print_path(bs1, bs2, dist);
	}
}

bool PublicTransport::contains_courses(int line)
{
	int bs = *lines.find(line)->second.bus_stops.begin();
	for (list<Course>::iterator it2 = courses[bs].begin(); it2 != courses[bs].end() && it2->bus <= line; ++it2)
	{
		if (it2->bus == line)
			return true;
	}

	return false;
}

void PublicTransport::print_bus_stops_schedule()
{
	if (real_bs_count < 1)
	{
		cout << "There aren't any bus stops on the map!" << endl;
		return;
	}
	cout << "Schedule:" << endl;
	for (int i = 0; i < max_bus_stops_count; ++i)
	{
		if (bus_stops[i].number != -1)
		{
			cout << "\nBus stop \" " << bus_stops[i].name << " \" (" << bus_stops[i].number << "): " << endl;
			if (bus_stops[i].busses_count == 0)
				cout << "There aren't any lines going through this bus stop!" << endl;
			else
			{
				for (Bus b : bus_stops[i].busses)
					cout << "Bus " << b.number << " arrives at " << b.arrival_time << endl;
			}
		}
	}
	cout << endl;
}

void PublicTransport::print_courses()
{
	if (lines_count < 1)
	{
		cout << "No any public transport lines have been found!" << endl;
		return;
	}

	cout << "Do you want to see all courses or only for a selected line?" << endl;
	cout << "Enter 1 to see all courses or enter 2 to select a line\n--> ";
	int option = -1;
	do {
		cin >> option;
		switch (option)
		{
		case 1:
		{
			for (int i = 0; i < max_bus_stops_count; ++i)
			{
				if (!courses[i].empty())
				{
					cout << "\nBus stop \" " << bus_stops[i].name << " \" (" << bus_stops[i].number << "): " << endl;
					for (Course c : courses[i])
					{
						cout << "Bus " << c.bus << " arrives at " << c.time_begin << "' and travels to bus stop \" " << bus_stops[c.destination].name << " \" for " << c.time_end - c.time_begin << " minutes." << endl;
					}
				}
			}
			break;
		}
		case 2:
		{
			int line;
			cout << "Enter line: ";
			cin >> line;
			if (lines.find(line) == lines.end())
			{
				cout << "The line doesn't exist!" << endl;
				return;
			}
			for (int i = 0; i < max_bus_stops_count; ++i)
			{
				for (Course c : courses[i])
				{
					if (c.bus == line)
					{
						cout << "Bus " << line << " travels from bus stop \" " << bus_stops[i].name << " \" at " << c.time_begin << " to bus stop \" " << bus_stops[c.destination].name << " \" for " << c.time_end - c.time_begin << " minutes." << endl;
					}
				}
			}
			break;
		}
		default:
		{
			cout << "\nInvalid option! Enter again!\n--> ";
			option = -1;
			break;
		}
		}
	} while (option == -1);

	cout << endl;
}

void PublicTransport::print_line()
{
	if (lines_count < 1)
	{
		cout << "No any public transport lines have been found!" << endl;
		return;
	}

	cout << "Do you want to see all lines or just a selected line?" << endl;
	cout << "Enter 1 to see all lines or enter 2 to select a line\n--> ";
	int option = -1;
	do {
		cin >> option;
		switch (option)
		{
		case 1:
		{
			for (pair<int, TransportLine> p : lines) 
			{
				cout << "\nTransport line " << p.first << ":" << endl;
				for (list<int>::iterator bs = p.second.bus_stops.begin(); bs != prev(p.second.bus_stops.end()); ++bs)
				{
					cout << bus_stops[*bs].name << " --> ";
				}
				cout << bus_stops[*prev(p.second.bus_stops.end())].name << endl;
			}
			break;
		}
		case 2:
		{
			int line;
			cout << "Enter line: ";
			cin >> line;
			if (lines.find(line) == lines.end())
			{
				cout << "The line doesn't exist!" << endl << endl;
				return;
			}

			cout << "\nTransport line " << line << ":" << endl;
			for (list<int>::iterator bs = lines.find(line)->second.bus_stops.begin(); bs != prev(lines.find(line)->second.bus_stops.end()); ++bs)
			{
				cout << bus_stops[*bs].name << " --> ";
			}
			cout << bus_stops[*prev(lines.find(line)->second.bus_stops.end())].name << endl;

			break;
		}
		default:
		{
			cout << "\nInvalid option! Enter again!\n--> ";
			option = -1;
			break;
		}
		}
	} while (option == -1);
	cout << endl;
}

void PublicTransport::print_order()
{
	cout << "\nBus stops locations represented on a line" << endl;
	if (stops_order.empty())
	{
		cout << "No bus stops have been found!" << endl;
		return;
	}

	list<int>::iterator bs = stops_order.begin();
	while (bs != prev(stops_order.end()))
	{
		cout << bus_stops[*bs].name << " --> ";
			++bs;
	}
	cout << bus_stops[*bs].name << endl << endl;

	list<int>::iterator bs2 = opposite_stops_order.begin();
	while (bs2 != prev(opposite_stops_order.end()))
	{
		cout << bus_stops[*bs2].name << " --> ";
		++bs2;
	}
	cout << bus_stops[*bs2].name << endl << endl;
}

void PublicTransport::print_help()
{
	cout << endl;
	cout << "1.  Each stop has it's opposite stop which is automatically added with the same name and \" ' \" in the end" << endl;
	cout << "2.  If you add a stop with \" ' \" in the end of the name, the opposite stop will be created without that symbol in the end" << endl;
	cout << "3.  You cannot add a bus stop with name in which the last 2 symbols are \" ' \"" << endl;
	cout << "4.  A bus line can contain only bus stops from 1 kind (with or without \" ' \")" << endl;
	cout << "5.  The time between each 2 bus stops is atleast 1" << endl;
	cout << "6.  When adding a new course you can't cross times (If 1 course begins at 1 at stop A and end at 5 at stop B, a new course beginning after 1 can't end before 5)" << endl;
	cout << "7.  Extending a line supports only adding a bus stop on back or on front of an existing line" << endl;
	cout << "8.  If you want to add a stop in the middle, you will need to create a new line" << endl;
	cout << "9.  You need to follow the bus stops order when you extend a line or creating a new line" << endl;
	cout << "10. The bus stops order means that if A -> B -> C, C can't go directly to A. From C you go to C' -> B' -> A'" << endl;
	cout << "11. Creating a new line doesn't add automatically it's opposite line" << endl;
	cout << "12. Going from any bus stop to it's opposite stop takes 0 time" << endl;
	cout << "13. Deleting a bus stop will delete it's opposite bus stop too" << endl;
	cout << "14. Deleting a bus stop will redirect all courses going through the bus stop, keeping the time when the bus arrives at the previous and at the next bus stop" << endl;
	cout << "15. By deleting a bus stop, if 1 bus stop remains in the line, the line will be removed" << endl;
	cout << "16. Default file for reading and writing data is \"file.txt\"" << endl << endl;

}

void PublicTransport::type_to_continue()
{	
	string str;
	cout << "\nType anything to go back to the main menu... ";
	cin >> str;
	cout << endl;
}

void PublicTransport::users_option(int option)
{		
	switch (option)
	{
	case 1:
	{
		cout << "\n\t\tFinding the fastest path\n";
		if (real_bs_count < 1)
		{
			cout << "Not enough bus stops / lines added to the map! Please add more before doing this action!" << endl;
			type_to_continue();
			break;
		}
		else
			find_fastest_path();

		type_to_continue();
		break;
	}
	case 2:
	{
		cout << "\n\t\tAdd new line to the map\n";
		if (add_new_line())
			cout << "\nA new line has been successfully added!" << endl;
		else
			cout << "A new line has not been added but some changes migh have been made!" << endl;

		type_to_continue();
		break;
	}
	case 3:
	{
		int line;
		cout << "\n\t\tAdd new course\n";
		cout << "Enter the line where you want to add a new course --> ";
		cin >> line;
		if (add_new_course(line))
			cout << "\nA new course for line " << line << " has been added!" << endl;
		else
			cout << "No changes have been made!" << endl;

		type_to_continue();
		break;
	}
	case 4:
	{
		int line;
		string name1, name2;
		cout << "\n\t\tExtend an existing line\n";
		cout << "Enter line --> ";
		cin >> line;
		if (lines.find(line) == lines.end())
		{
			cout << "The line doesn't exist!" << endl;
			type_to_continue();
			break;
		}
		if (!contains_courses(line))
		{
			cout << "Before extending the line, make sure it contains atleast 1 course!" << endl;
			type_to_continue();
			return;
		}

		cout << "Enter a bus stop --> ";
		cin >> name1;
		cout << "Enter a bus stop --> ";
		cin >> name2;

		bool check1 = is_reverse_stop(name1);
		bool check2 = is_reverse_stop(name2);
		if (check1 != check2)
		{
			cout << "Invalid line! You can't connect bus stops from different lanes of the road!" << endl;
			type_to_continue();
			break;
		}

		if (extend_line(line, name1, name2))
			cout << "\nLine " << line << " has been successfully extended!" << endl;
		else
			cout << "The line has not been extended! Some changes might have been made!" << endl;

		type_to_continue();
		break;
	}
	case 5:
	{
		int line;
		cout << "\n\t\tRemove a line from the map" << endl;
		cout << "Enter the line which you want to remove --> ";
		cin >> line;
		if (remove_line(line))
			cout << "\nLine " << line << " has been removed! Please check the bus stops schedule!" << endl;
		else
			cout << "No changes have been made!" << endl;

		type_to_continue();
		break;
	}
	case 6:
	{
		int line;
		cout << "\n\t\tRemove a course from an existing line" << endl;
		cout << "Enter line --> ";
		cin >> line;
		if (remove_course(line))
			cout << "\nA course has been removed from the line" << endl;
		else
			cout << "No changes have been made!" << endl;

		type_to_continue();
		break;
	}
	case 7:
	{
		cout << "\n\t\tRemove a bus stop from the line" << endl;
		remove_bus_stop();
		type_to_continue();
		break;
	}
	case 8:
	{
		cout << "\n\t\tCheck public transport lines" << endl;
		print_line();
		type_to_continue();
		break;
	}
	case 9:
	{
		cout << "\n\t\tCheck courses schedule" << endl;
		print_courses();
		type_to_continue();
		break;
	}
	case 10:
	{
		cout<<"\n\t\tCheck bus stops schedule" << endl;
		print_bus_stops_schedule();
		type_to_continue();
		break;
	}
	case 11:
	{
		cout << "\n\t\tBus stops order" << endl;
		print_order();
		type_to_continue();
		break;
	}
	case 12:
	{
		string new_file;
		cout << "\n\t\tLoad data from other text file" << endl;
		cout << "Enter file's name --> ";
		cin >> new_file;
		ifstream nf(new_file);
		if (!nf.is_open())
		{
			cout << "The text file has not been opened!\nNo changes have been made!" << endl;
			type_to_continue();
			break;
		}
		clear();
		load_data(nf);
		nf.close();

		type_to_continue();
		break;
	}
	case 13:
	{
		int choice = 0;
		cout << "\n\t\tSave data" << endl;
		cout << "Enter 1 if you want to save data in another text file or another number to save it in the default text file\nEnter --> ";
		cin >> choice;
		if (choice == 1)
		{
			string new_file;
			cout << "Enter file's name --> ";
			cin >> new_file;
			ofstream nf(new_file);
			if (!nf.is_open())
			{
				cout << "The file could not be opened! Data is not saved!" << endl;
				type_to_continue();
				break;
			}
			else
				save_data(nf);
			nf.close();

			type_to_continue();
			break;
		}
		ofstream def_file(file_name);
		save_data(def_file);
		def_file.close();

		type_to_continue();
		break;
	}
	case 14:
	{
		cout << "\n\t\tHelp menu" << endl;
		print_help();
		type_to_continue();
		break;
	}
	case 0:
	{
		cout << "\nExiting the program..." << endl;
		break;
	}
	default:
	{
		cout << "\nInvalid option! Try again!\n\n";
		break;
	}
	}

	cout << endl;
}

void PublicTransport::menu()
{
	int option;
	do
	{
		cout << "\t\tMain menu\n\n";
		cout << "1.  Find the fastest path" << endl;
		cout << "2.  Add new line to the map" << endl;
		cout << "3.  Add new course to an existing line" << endl;
		cout << "4.  Extend an existing line" << endl;
		cout << "5.  Remove a line from the map" << endl;
		cout << "6.  Remove a course from an existing line" << endl;
		cout << "7.  Remove a bus stop from the map" << endl;
		cout << "8.  Check public transport lines" << endl;
		cout << "9.  Check courses schedule" << endl;
		cout << "10. Check bus stops schedule" << endl;
		cout << "11. Check bus stops order" << endl;
		cout << "12. Load data from other text file" << endl;
		cout << "13. Save data" << endl;
		cout << "14. Help" << endl;
		cout << "0.  Exit" << endl << endl;
		cout << "Enter  -->  ";
		cin >> option;

		users_option(option);

	} while (option != 0);
}

void PublicTransport::set_course_time(queue<int>& arrival_time, int line, int bs1, int bs2, int count, list<Bus>::iterator& bus, const string& name1, const string& name2, bool new_start)
{
	int time, prev_bus_time;
	if (!new_start)
		prev_bus_time = 0;
	else
		prev_bus_time = INF;

	for (int i = 0; i < count; ++i)
	{
		cout << endl << "Creating relation " << name1 << " --> " << name2 << endl;
		int prev_stop_time = arrival_time.front();
		if (!new_start)
		{
			cout << "Bus " << line << ", travelling from " << name1 << " at: " << prev_stop_time << " - reaches " << name2 << " at: ";
			cin >> time;
			while (time <= max(prev_stop_time, prev_bus_time))
			{
				cout << "Invalid time! The bus can't arrive at that bus stop earlier than " << max(prev_stop_time + 1, prev_bus_time + 1) << endl;
				cout << "Enter again:  ";
				cin >> time;
				cout << endl;
			}
			bus = bus_stops[bs2].busses.insert(bus, Bus(line, time));
		}
		else
		{
			cout << "The bus arrives at " << name2 << ", at " << prev_stop_time << ".\nEnter the time when the bus arrives at " << name1 << "... ";
			cin >> time;
			while (time >= min(prev_stop_time, prev_bus_time))
			{
				cout << "Invalid time! The bus can't arrive at that bus stop later than " << min(prev_stop_time - 1, prev_bus_time - 1) << endl;
				cout << "Enter again:  ";
				cin >> time;
				cout << endl;
			}
			bus = bus_stops[bs1].busses.insert(bus, Bus(line, time));
		}

		add_course(bs1, bs2, prev_stop_time, time, line);
		arrival_time.pop();
		prev_bus_time = time;
		++bus;
	}
}

void PublicTransport::create_new_bs(const string& name)
{	
	int bs1, bs2;
	if (!availabe_num.empty())
	{
		bs1 = availabe_num.top();
		availabe_num.pop();

		if (!availabe_num.empty())
		{
			bs2 = availabe_num.top();
			availabe_num.pop();
		}
		else
		{
			bs2 = max_bus_stops_count;
			++max_bus_stops_count;
			courses.resize(max_bus_stops_count + 1);
			bus_stops.resize(max_bus_stops_count + 1);
		}
	}
	else
	{
		bs1 = max_bus_stops_count;
		bs2 = max_bus_stops_count + 1;
		max_bus_stops_count += 2;
		courses.resize(max_bus_stops_count + 2);
		bus_stops.resize(max_bus_stops_count + 2);
	}

	bus_stops[bs1].name = name;
	bus_stops[bs1].busses_count = 0;
	bus_stops[bs1].number = bs1;
	bus_stops[bs1].reverse_stop = bs2;
	bs_index.insert(make_pair(name, bs1));

	// Opposite stop
	if (name[name.size() - 1] == '\'')
	{
		for (unsigned int i = 0; i < name.size() - 1; ++i)
			bus_stops[bs2].name += name[i];
	}
	else
		bus_stops[bs2].name = name + "'";

	bus_stops[bs2].busses_count = 0;
	bus_stops[bs2].number = bs2;
	bus_stops[bs2].reverse_stop = bs1;
	bs_index.insert(make_pair(name + "'", bs2));
	real_bs_count += 2;

	if (name[name.size() - 1] != '\'')
		add_stop_to_order(bs1, bs2, bus_stops[bs1].name);
	else
		add_stop_to_order(bs2, bs1, bus_stops[bs2].name);

	cout << "\nNew bus stop with name \" " << bus_stops[bs1].name << " \" and number " << bs1 << " has been created!" << endl;
	cout << "Bus stop \" " << bus_stops[bs2].name << " \" with number " << bs2 << " has been created on the opposite lane of the road!" << endl;
}

bool PublicTransport::create_bs_set_time(queue<int>& arrival_time, int line, int bs1, int bs2, int count, bool new_start, const string& name1, const string& name2)
{	
	if (new_start)
	{
		if (!create_name_and_order(name1, bs1, bs2, new_start))
			return false;
	
		lines.find(line)->second.stops_count++;
		bs1 = find_bus_stop(name1);
		list<Bus>::iterator bus = bus_stops[bs1].busses.begin();
		set_course_time(arrival_time, line, bs1, bs2, count, bus, name1, name2, new_start);
		lines.find(line)->second.bus_stops.push_front(bs1);
		bus_stops[bs1].busses_count++;
	}
	else
	{
		if (!create_name_and_order(name2, bs1, bs2, new_start))
			return false;

		lines.find(line)->second.stops_count++;
		bs2 = find_bus_stop(name2);
		list<Bus>::iterator bus = bus_stops[bs2].busses.begin();
		set_course_time(arrival_time, line, bs1, bs2, count, bus, name1, name2, new_start);
		lines.find(line)->second.bus_stops.push_back(bs2);
		bus_stops[bs2].busses_count++;
	}


	return true;
}

void PublicTransport::add_stop_to_line(queue<int>& arrival_time, int line, int bs1, int bs2, int count, const string& name1, const string& name2, bool new_start)
{
	if (new_start)
		swap(bs1, bs2);

	bus_stops[bs2].busses_count++;
	list<Bus>::iterator bus = find_place(line, bs2);

	if (!new_start)
	{
		set_course_time(arrival_time, line, bs1, bs2, count, bus, name1, name2, false);
		lines.find(line)->second.bus_stops.push_back(bs2);
	}
	else
	{
		set_course_time(arrival_time, line, bs2, bs1, count, bus, name1, name2, true);
		lines.find(line)->second.bus_stops.push_front(bs2);
	}

	lines.find(line)->second.stops_count++;

}

bool PublicTransport::extend_line(int line, const string& name1, const string& name2)
{
	int bs1 = find_bus_stop(name1);
	int bs2 = find_bus_stop(name2);

	if (bs1 == *prev(lines.find(line)->second.bus_stops.end()))
	{
		for (const int it : lines.find(line)->second.bus_stops)
		{
			if (it == bs2)
				return false;
		}

		int count = 0;
		queue<int> arrival_time;
		for (const Bus bus : bus_stops[bs1].busses)
		{
			if (bus.number == line)
			{
				++count;
				arrival_time.push(bus.arrival_time);
			}
		}

		if (bs2 != -1)
		{
			if (*prev(name1.end()) != '\'')
			{
				if (!check_stops_order(bs1, bs2, true, stops_order))
				{
					cout << "Logical error in bus stop order!" << endl;
					return false;
				}
			}
			else
			{
				if (!check_stops_order(bs1, bs2, true, opposite_stops_order))
				{
					cout << "Logical error in bus stop order!" << endl;
					return false;
				}
			}

			add_stop_to_line(arrival_time, line, bs1, bs2, count, name1, name2, false);
		}
		else
			if (!create_bs_set_time(arrival_time, line, bs1, bs2, count, false, name1, name2))
				return false;

	}
	else
	{
		if (bs2 != *lines.find(line)->second.bus_stops.begin())
			return false;

		for (const int it : lines.find(line)->second.bus_stops)
		{
			if (it == bs1)
				return false;
		}

		int count = 0;
		queue<int> arrival_time;
		for (const Bus bus : bus_stops[bs2].busses)
		{
			if (bus.number == line)
			{
				++count;
				arrival_time.push(bus.arrival_time);
			}
		}

		if (bs1 != -1)
		{
			if (*prev(name1.end()) != '\'')
			{
				if (!check_stops_order(bs1, bs2, false, stops_order))
					return false;
			}
			else
			{
				if (!check_stops_order(bs1, bs2, false, opposite_stops_order))
					return false;
			}

			add_stop_to_line(arrival_time, line, bs1, bs2, count, name1, name2, true);
		}
		else
			if (!create_bs_set_time(arrival_time, line, bs1, bs2, count, true, name1, name2))
				return false;
	}

	return true;
}

bool PublicTransport::add_new_course(int line)
{
	if (lines.find(line) == lines.end())
	{
		cout << "The line doesn't exist!" << endl;
		return false;
	}

	int time1, time2, index = 0;
	bool correct = true;
	list<int>::iterator it = lines.find(line)->second.bus_stops.begin();

	cout << "Creating a new course for line " << line << endl;
	cout << "\nEnter time for relation " << bus_stops[*it].name << " --> " << bus_stops[*next(it)].name << endl;
	cout << "Bus " << line << " begins the course from bus stop " << bus_stops[*it].name << " at... ";
	list<Bus>::iterator bus = bus_stops[*it].busses.begin();

	do
	{
		cin >> time1;
		correct = true;
		for (bus; bus != bus_stops[*it].busses.end(); ++bus)
		{
			if (bus->number == line)
			{
				if (bus->arrival_time == time1)
				{
					cout << "\nThere is already a course for the line beginning at that time! Please select another time!" << endl;
					cout << "Enter again... ";
					index = 0;
					correct = false;
					break;
				}

				if (bus->arrival_time < time1)
					++index;
				else
					break;
			}
		}
	} while (!correct);

	bus_stops[*it].busses.insert(bus, Bus(line, time1));
	bus_stops[*it].busses_count++;
	++it;

	for (it; it != lines.find(line)->second.bus_stops.end(); ++it)
	{
		list<Bus>::iterator bus = bus_stops[*it].busses.begin();
		for (bus; bus != bus_stops[*it].busses.end() && bus->number != line; ++bus) {}

		for (int i = 0; i < index; ++i)
			++bus;

		bool correct = true;
		cout << "\nRelation " << bus_stops[*prev(it)].name << " --> " << bus_stops[*it].name << endl;
		cout << "The bus traveling from bus stop " << bus_stops[*prev(it)].name << " at " << time1 << "'. arrives at " << bus_stops[*it].name << " at ";
		
		int prev_bus_time = 0;
		if (index != 0)
			prev_bus_time = prev(bus)->arrival_time;	
		do
		{
			correct = true;
			cin >> time2;
			if (bus != bus_stops[*it].busses.end() && bus->number == line)
			{

				if (time2 < max(time1 + 1, prev_bus_time) || time2 > bus->arrival_time)
				{
					cout << "Invalid time! The bus can arrive at bus stop " << bus_stops[*it].name << " in interval [" << max(time1 + 1, prev_bus_time) << ", " << bus->arrival_time << "]" << endl;
					cout << "Enter again... ";
					correct = false;
				}
			}
			else if (time2 < max(time1 + 1, prev_bus_time))
			{
				cout << "Invalid time! The bus can't arrive at bus stop " << bus_stops[*it].name << " earlier than " << max(time1 + 1, prev_bus_time) << endl;
				cout << "Enter again... ";
				correct = false;
			}
			cout << endl;
		} while (!correct);

		bus_stops[*it].busses.insert(bus, Bus(line, time2));
		bus_stops[*it].busses_count++;
		add_course(*prev(it), *it, time1, time2, line);
		time1 = time2;
	}

	return true;
}

bool PublicTransport::add_new_line()
{
	int stops_count, courses, newstops = 0, newline;
	cout << "\nNew line number: ";
	cin >> newline;
	if (lines.find(newline) != lines.end())
	{
		cout << "The line already exists!" << endl;
		return false;
	}

	cout << "\nHow many bus stops do you want to add in the new bus line... ";
	cin >> stops_count;
	cout << "\nHow many courses do you want the line to contain... ";
	cin >> courses;

	if (stops_count < 2 || courses < 1)
	{
		cout << "\nInvalid input! You need atleast 2 bus stops and 1 course to create a line!" << endl;
		return false;
	}

	cout << "\nEnter " << stops_count << " bus stops names in the order that busses should move." << endl;
	cout << "*Help -> You can add bus stops that aren't in the map yet by just typing a name of a bus stop which doesn't exist" << endl;

	vector<string> bs_names(stops_count);
	if (!add_bus_stops_to_line(stops_count, bs_names))
		return false;
	
	if (!new_line_bs_order(stops_count, bs_names, is_reverse_stop(bs_names[0])))
		return false;

	lines.insert(make_pair(newline,TransportLine(newline, 2)));	

	create_first_course(newline, bs_names);

	for (int j = 1; j < stops_count - 1; ++j)
		extend_line(newline, bs_names[j], bs_names[j + 1]);

	cout << endl;

	++lines_count;
	for (int i = 1; i < courses; ++i)
		add_new_course(newline);

	return true;
}

void PublicTransport::redirecting_courses(int bs, int line)
{
	list<int>::iterator it_line = lines.find(line)->second.bus_stops.begin();
	while (*next(it_line) != bs)
		++it_line;

	list<Course>::iterator it = courses[*it_line].begin();
	while (it->bus != line)
		++it;

	int next_bs = courses[bs].begin()->destination;
	int time;
	while (!bus_stops[bs].busses.empty() && bus_stops[bs].busses.begin()->number == line)
	{
		time = courses[bs].begin()->time_end;
		it->destination = next_bs;
		it->time_end = time;

		bus_stops[bs].busses_count--;
		bus_stops[bs].busses.pop_front();
		courses[bs].pop_front();
		++it;
	}

	cout << "Courses from line " << line << " have been redirected!" << endl;
	++it_line;
	lines.find(line)->second.bus_stops.remove(bs);
	lines.find(line)->second.stops_count--;
}

void PublicTransport::remove_bus_stop()
{
	string name;
	cout << "Enter the name of the bus stop that you want to remove\n--> ";
	cin >> name;
	int bs = find_bus_stop(name);
	if (bs == -1)
	{
		cout << "The bus stop doesn't exist!" << endl;
		return;
	}

	int rbs = bus_stops[bs].reverse_stop;

	delete_bs(bs);
	delete_bs(rbs);

	real_bs_count -= 2;
	cout << "Bus stop " << bus_stops[bs].name << " and " << bus_stops[rbs].name << " have been deleted!" << endl;
}

bool PublicTransport::remove_course(int line)
{
	if (lines.find(line) == lines.end())
	{
		cout << "The line doesn't exist!" << endl;
		return false;
	}

	int bs = *lines.find(line)->second.bus_stops.begin();
	int count = 0;

	cout << "Courses from this line: " << endl;
	for (list<Course>::iterator it2 = courses[bs].begin(); it2 != courses[bs].end() && it2->bus <= line; ++it2)
	{
		if (it2->bus == line)
		{
			++count;
			cout << count << ". Course beginning at " << it2->time_begin << endl;
		}
	}
	if (count == 0)
	{
		cout << "There aren't any courses in this line!" << endl;
		return false;
	}

	int option = 0;
	if (count > 1)
	{
		cout << "\nWhich course do you want to remove?\n--> ";
		cin >> option;
		if (option < 1 || option > count)
		{
			cout << "Invalid course!" << endl;
			return false;
		}
	}

	delete_course(line, option);
	return true;
}

bool PublicTransport::remove_line(int line)
{
	if (lines.find(line) == lines.end())
	{
		cout << "The line doesn't exist!" << endl;
		return false;
	}

	for (int bs : lines.find(line)->second.bus_stops)
	{
		list<Bus>::iterator b = bus_stops[bs].busses.begin();
		while (b != bus_stops[bs].busses.end())
		{
			if (b->number == line)
			{
				--bus_stops[bs].busses_count;
				b = bus_stops[bs].busses.erase(b);
			}
			else
				++b;
		}
		if (bs != *prev(lines.find(line)->second.bus_stops.end()))
		{
			list<Course>::iterator c = courses[bs].begin();
			while (c != courses[bs].end())
			{
				if (c->bus == line)
					c = courses[bs].erase(c);
				else
					++c;
			}
		}
	}

	lines.erase(line);
	--lines_count;

	return true;
}

void PublicTransport::load_data(ifstream& file)
{
	file.seekg(0, ios::end);
	if (file.tellg() == 0)
	{
		cout << "The file is empty!\nDefault data is loaded\n" << endl;
		PublicTransport();
		return;
	}
	file.seekg(0, ios::beg);

	file >> max_bus_stops_count >> real_bs_count >> lines_count;

	bus_stops.resize(max_bus_stops_count);
	TransportLine tl;
	Bus bus;
	string name;
	int num;

	for (int i = 0; i < real_bs_count / 2; ++i)
	{
		file >> num;
		stops_order.push_back(num);
	}

	for (int i = 0; i < real_bs_count / 2; ++i)
	{
		file >> num;
		opposite_stops_order.push_back(num);
	}

	for (int i = 0; i < lines_count; ++i)
	{
		file >> tl.line_number >> tl.stops_count;
		for (int j = 0; j < tl.stops_count; ++j)
		{
			file >> num;
			tl.bus_stops.push_back(num);
		}
		lines.insert(make_pair(tl.line_number, tl));
		tl.bus_stops.clear();
	}

	for (int i = 0; i < max_bus_stops_count; ++i)
	{
		file >> num;
		if (file.eof())
			break;
		if (num != i)
		{
			while (num != i)
			{
				availabe_num.push(i);
				bus_stops[i].number = -1;
				++i;
			}
		}

		bus_stops[i].number = num;
		file >> bus_stops[i].name >> bus_stops[i].busses_count >> bus_stops[i].reverse_stop;
		bs_index.insert(make_pair(bus_stops[i].name, bus_stops[i].number));
		for (int j = 0; j < bus_stops[i].busses_count; ++j)
		{
			file >> bus.number >> bus.arrival_time;
			bus_stops[i].busses.push_back(bus);
		}
	}

	make_map();
	cout << "Data has been successfully loaded!" << endl;
}

void PublicTransport::save_data(ofstream& file)
{
	if (!file.is_open())
	{
		cout << "Unable to save data!" << endl;
		return;
	}

	file.clear();
	file << max_bus_stops_count << " " << real_bs_count  << " " << lines_count << endl << endl;

	for (const int bs : stops_order)
		file << bs << " ";
	file << endl; // Makes the text file easier for reading
	for (const int bs : opposite_stops_order)
		file << bs << " ";
	file << endl << endl;

	for (const pair<int, TransportLine> p : lines)
	{
		TransportLine tl = p.second;
		file << tl.line_number << " " << tl.stops_count << endl;
		for (const int bs : tl.bus_stops)
			file << bs << " ";
		file << endl << endl;
	}
	file << endl;

	for (int i = 0; i < max_bus_stops_count; ++i)
	{
		BusStop bs = bus_stops[i];
		if (bs.number != -1)
		{
			file << bs.number << endl;
			file << bs.name << " " << bs.busses_count << " " << bs.reverse_stop << endl;
			for (const Bus bus : bs.busses)
				file << bus.number << " " << bus.arrival_time << endl;
			file << endl;
		}
	}

	cout << "Data has been successfully saved!" << endl;
}

void PublicTransport::delete_bs(int bs)
{
	while (bus_stops[bs].busses_count != 0)
	{
		int line = bus_stops[bs].busses.begin()->number;
		if (*lines.find(line)->second.bus_stops.begin() == bs) // The line begins with that bus stop
		{
			int count = 0;
			while (!courses[bs].empty() && courses[bs].begin()->bus == line)
			{
				courses[bs].pop_front();
				bus_stops[bs].busses.pop_front();
				bus_stops[bs].busses_count--;
				count++;
			}

			lines.find(line)->second.bus_stops.pop_front();
			--lines.find(line)->second.stops_count;

			if (lines.find(line)->second.stops_count == 1)
			{
				//for (int i = 0; i < count; ++i)
					//delete_course(line, 0);
				//lines.erase(line);
				//--lines_count;
				remove_line(line);
				cout << "The whole line " << line << " has been removed because the bus stops remaining in the line were less than 1" << endl;
			}

		}
		else if (*prev(lines.find(line)->second.bus_stops.end()) == bs) // The line ends with that bus stop
		{
			int prev_bs = *prev(prev(lines.find(line)->second.bus_stops.end()));
			int count = 0;
			list<Course>::iterator it = courses[prev_bs].begin();
			while (it->bus != line)
				++it;

			while (it != courses[prev_bs].end() && it->bus == line)
			{
				it = courses[prev_bs].erase(it);
				++count;
			}

			for (int i = 0; i < count; ++i)
			{
				bus_stops[bs].busses.pop_front();
				bus_stops[bs].busses_count--;
			}
			lines.find(line)->second.bus_stops.pop_back();
			--lines.find(line)->second.stops_count;

			if (lines.find(line)->second.stops_count == 1)
			{
				/*for (int i = 0; i < count; ++i)
					delete_course(line, 0);
				lines.erase(line);
				--lines_count;
				*/
				remove_line(line);
				cout << "The whole line " << line << " has been removed because the bus stops remaining in the line were less than 1" << endl;
			}
		}
		else
		{
			redirecting_courses(bs, line);
		}
	}

	availabe_num.push(bus_stops[bs].number);
	bus_stops[bs].number = -1;
	bs_index.erase(bus_stops[bs].name);
	if (*prev(bus_stops[bs].name.end()) != '\'')
		stops_order.remove(bs);
	else
		opposite_stops_order.remove(bs);

}

void PublicTransport::delete_course(int line, int count)
{
	for (list<int>::iterator it = lines.find(line)->second.bus_stops.begin(); it != lines.find(line)->second.bus_stops.end(); ++it)
	{
		list<Bus>::iterator b = bus_stops[*it].busses.begin();
		list<Course>::iterator c = courses[*it].begin();
		if (next(it) != lines.find(line)->second.bus_stops.end())
		{
			while (c != courses[*it].end() && c->bus < line)
				++c;
			for (int i = 1; i < count; ++i)
				++c;
			courses[*it].erase(c);
		}

		while (b->number < line)
			++b;

		for (int i = 1; i < count; ++i)
			++b;

		bus_stops[*it].busses_count--;
		bus_stops[*it].busses.erase(b);
	}
}

void PublicTransport::clear()
{
	max_bus_stops_count = 0;
	real_bs_count = 0;
	lines_count = 0;
	stops_order.clear();
	opposite_stops_order.clear();
	bus_stops.clear();
	bs_index.clear();
	while (!availabe_num.empty())
		availabe_num.pop();
	lines.clear();
	courses.clear();
}
