#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <limits>

using namespace std;

class Route {
private:
    int routeID;
    string fromCity;
    string toCity;
    double fare;
    int seatCapacity;
    vector<bool> seats; // false = available, true = booked

public:
    Route() {
        routeID = 0;
        fromCity = "";
        toCity = "";
        fare = 0.0;
        seatCapacity = 0;
    }

    Route(int id, const string& from, const string& to, double routeFare, int capacity) {
        routeID = id;
        fromCity = from;
        toCity = to;
        fare = routeFare;
        seatCapacity = capacity;
        seats.assign(seatCapacity, false);
    }

    int getRouteID() const {
        return routeID;
    }

    string getFromCity() const {
        return fromCity;
    }

    string getToCity() const {
        return toCity;
    }

    double getFare() const {
        return fare;
    }

    int getSeatCapacity() const {
        return seatCapacity;
    }

    bool isValidSeat(int seatNo) const {
        return seatNo >= 1 && seatNo <= seatCapacity;
    }

    bool isSeatBooked(int seatNo) const {
        if (!isValidSeat(seatNo)) {
            return false;
        }
        return seats[seatNo - 1];
    }

    bool bookSeat(int seatNo) {
        if (!isValidSeat(seatNo) || seats[seatNo - 1]) {
            return false;
        }
        seats[seatNo - 1] = true;
        return true;
    }

    bool releaseSeat(int seatNo) {
        if (!isValidSeat(seatNo) || !seats[seatNo - 1]) {
            return false;
        }
        seats[seatNo - 1] = false;
        return true;
    }

    int getBookedSeatCount() const {
        int count = 0;
        for (int i = 0; i < seatCapacity; i++) {
            if (seats[i]) {
                count++;
            }
        }
        return count;
    }

    int getAvailableSeatCount() const {
        return seatCapacity - getBookedSeatCount();
    }

    void displayRoute() const {
        cout << "Route ID      : " << routeID << endl;
        cout << "From          : " << fromCity << endl;
        cout << "To            : " << toCity << endl;
        cout << "Fare          : " << fixed << setprecision(2) << fare << endl;
        cout << "Seat Capacity : " << seatCapacity << endl;
        cout << "Available     : " << getAvailableSeatCount() << endl;
        cout << "Booked        : " << getBookedSeatCount() << endl;
        cout << "-----------------------------" << endl;
    }

    string seatsToString() const {
        string result = "";
        for (int i = 0; i < seatCapacity; i++) {
            if (seats[i]) {
                result += '1';
            } else {
                result += '0';
            }
        }
        return result;
    }

    void loadSeatsFromString(const string& seatData) {
        seats.assign(seatCapacity, false);
        for (int i = 0; i < seatCapacity && i < (int)seatData.length(); i++) {
            seats[i] = (seatData[i] == '1');
        }
    }

    void clearAllSeats() {
        seats.assign(seatCapacity, false);
    }
};

class Ticket {
private:
    int ticketID;
    string passengerName;
    int routeID;
    vector<int> seatNumbers;
    double totalAmount;

public:
    Ticket() {
        ticketID = 0;
        passengerName = "";
        routeID = 0;
        totalAmount = 0.0;
    }

    Ticket(int id, const string& name, int rID, const vector<int>& seats, double farePerSeat) {
        ticketID = id;
        passengerName = name;
        routeID = rID;
        seatNumbers = seats;
        totalAmount = farePerSeat * seatNumbers.size();
    }

    int getTicketID() const {
        return ticketID;
    }

    string getPassengerName() const {
        return passengerName;
    }

    int getRouteID() const {
        return routeID;
    }

    vector<int> getSeatNumbers() const {
        return seatNumbers;
    }

    double getTotalAmount() const {
        return totalAmount;
    }

    string seatListToString() const {
        stringstream ss;
        for (int i = 0; i < (int)seatNumbers.size(); i++) {
            ss << seatNumbers[i];
            if (i != (int)seatNumbers.size() - 1) {
                ss << ",";
            }
        }
        return ss.str();
    }

    void displayTicket() const {
        cout << "Ticket ID      : " << ticketID << endl;
        cout << "Passenger Name : " << passengerName << endl;
        cout << "Route ID       : " << routeID << endl;
        cout << "Seat Numbers   : " << seatListToString() << endl;
        cout << "Total Amount   : " << fixed << setprecision(2) << totalAmount << endl;
        cout << "-----------------------------" << endl;
    }
};

class ReservationSystem {
private:
    vector<Route> routes;
    vector<Ticket> tickets;
    int nextTicketID;

    const string ROUTE_FILE = "routes.txt";
    const string TICKET_FILE = "tickets.txt";

    int findRouteIndexByID(int routeID) const {
        for (int i = 0; i < (int)routes.size(); i++) {
            if (routes[i].getRouteID() == routeID) {
                return i;
            }
        }
        return -1;
    }

    int findTicketIndexByID(int ticketID) const {
        for (int i = 0; i < (int)tickets.size(); i++) {
            if (tickets[i].getTicketID() == ticketID) {
                return i;
            }
        }
        return -1;
    }

    string joinSeats(const vector<int>& seatNumbers) const {
        stringstream ss;
        for (int i = 0; i < (int)seatNumbers.size(); i++) {
            ss << seatNumbers[i];
            if (i != (int)seatNumbers.size() - 1) {
                ss << ",";
            }
        }
        return ss.str();
    }

    vector<int> splitSeats(const string& text) const {
        vector<int> seatNumbers;
        stringstream ss(text);
        string part;

        while (getline(ss, part, ',')) {
            if (!part.empty()) {
                stringstream numStream(part);
                int seat;
                numStream >> seat;
                if (!numStream.fail()) {
                    seatNumbers.push_back(seat);
                }
            }
        }
        return seatNumbers;
    }

    bool isDuplicateSeatInSameBooking(const vector<int>& selectedSeats, int seatNo) const {
        for (int i = 0; i < (int)selectedSeats.size(); i++) {
            if (selectedSeats[i] == seatNo) {
                return true;
            }
        }
        return false;
    }

    void initializeDefaultRoutes() {
        if (!routes.empty()) {
            return;
        }

        routes.push_back(Route(101, "Chennai", "Hyderabad", 850.00, 30));
        routes.push_back(Route(102, "Chennai", "Bangalore", 600.00, 30));
        routes.push_back(Route(103, "Hyderabad", "Vijayawada", 500.00, 30));
        saveRoutesToFile();
    }

    void rebuildSeatStatusFromTickets() {
        for (int i = 0; i < (int)routes.size(); i++) {
            routes[i].clearAllSeats();
        }

        for (int i = 0; i < (int)tickets.size(); i++) {
            int routeIndex = findRouteIndexByID(tickets[i].getRouteID());
            if (routeIndex != -1) {
                vector<int> seats = tickets[i].getSeatNumbers();
                for (int j = 0; j < (int)seats.size(); j++) {
                    routes[routeIndex].bookSeat(seats[j]);
                }
            }
        }
    }

public:
    ReservationSystem() {
        nextTicketID = 100;
        loadRoutesFromFile();
        loadTicketsFromFile();
        if (routes.empty()) {
            initializeDefaultRoutes();
        }
        rebuildSeatStatusFromTickets();
    }

    void loadRoutesFromFile() {
        routes.clear();
        ifstream fin(ROUTE_FILE.c_str());

        if (!fin) {
            return;
        }

        string line;
        while (getline(fin, line)) {
            if (line.empty()) {
                continue;
            }

            stringstream ss(line);
            string idStr, from, to, fareStr, capacityStr, seatData;

            getline(ss, idStr, '|');
            getline(ss, from, '|');
            getline(ss, to, '|');
            getline(ss, fareStr, '|');
            getline(ss, capacityStr, '|');
            getline(ss, seatData);

            if (idStr.empty() || from.empty() || to.empty() || fareStr.empty() || capacityStr.empty()) {
                continue;
            }

            int id = atoi(idStr.c_str());
            double fare = atof(fareStr.c_str());
            int capacity = atoi(capacityStr.c_str());

            Route route(id, from, to, fare, capacity);
            if (!seatData.empty()) {
                route.loadSeatsFromString(seatData);
            }

            routes.push_back(route);
        }

        fin.close();
    }

    void saveRoutesToFile() {
        ofstream fout(ROUTE_FILE.c_str());

        for (int i = 0; i < (int)routes.size(); i++) {
            fout << routes[i].getRouteID() << "|"
                 << routes[i].getFromCity() << "|"
                 << routes[i].getToCity() << "|"
                 << fixed << setprecision(2) << routes[i].getFare() << "|"
                 << routes[i].getSeatCapacity() << "|"
                 << routes[i].seatsToString() << endl;
        }

        fout.close();
    }

    void loadTicketsFromFile() {
        tickets.clear();
        ifstream fin(TICKET_FILE.c_str());

        if (!fin) {
            return;
        }

        string line;
        int maxTicketID = 99;

        while (getline(fin, line)) {
            if (line.empty()) {
                continue;
            }

            stringstream ss(line);
            string ticketIDStr, name, routeIDStr, seatText, amountStr;

            getline(ss, ticketIDStr, '|');
            getline(ss, name, '|');
            getline(ss, routeIDStr, '|');
            getline(ss, seatText, '|');
            getline(ss, amountStr);

            if (ticketIDStr.empty() || name.empty() || routeIDStr.empty() || seatText.empty() || amountStr.empty()) {
                continue;
            }

            int ticketID = atoi(ticketIDStr.c_str());
            int routeID = atoi(routeIDStr.c_str());
            double amount = atof(amountStr.c_str());
            vector<int> seatNumbers = splitSeats(seatText);

            int routeIndex = findRouteIndexByID(routeID);
            double farePerSeat = 0.0;
            if (routeIndex != -1 && !seatNumbers.empty()) {
                farePerSeat = amount / seatNumbers.size();
            }

            Ticket ticket(ticketID, name, routeID, seatNumbers, farePerSeat);
            tickets.push_back(ticket);

            if (ticketID > maxTicketID) {
                maxTicketID = ticketID;
            }
        }

        nextTicketID = maxTicketID + 1;
        fin.close();
    }

    void saveTicketsToFile() {
        ofstream fout(TICKET_FILE.c_str());

        for (int i = 0; i < (int)tickets.size(); i++) {
            fout << tickets[i].getTicketID() << "|"
                 << tickets[i].getPassengerName() << "|"
                 << tickets[i].getRouteID() << "|"
                 << joinSeats(tickets[i].getSeatNumbers()) << "|"
                 << fixed << setprecision(2) << tickets[i].getTotalAmount() << endl;
        }

        fout.close();
    }

    void displayAvailableRoutes() const {
        if (routes.empty()) {
            cout << "No routes available." << endl;
            return;
        }

        cout << "\n===== AVAILABLE ROUTES =====" << endl;
        for (int i = 0; i < (int)routes.size(); i++) {
            routes[i].displayRoute();
        }
    }

    void bookTicket() {
        int routeID;
        cout << "\nEnter Route ID: ";
        cin >> routeID;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input for Route ID." << endl;
            return;
        }

        int routeIndex = findRouteIndexByID(routeID);
        if (routeIndex == -1) {
            cout << "Invalid Route ID." << endl;
            return;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        string passengerName;
        cout << "Enter Passenger Name: ";
        getline(cin, passengerName);

        if (passengerName.empty()) {
            cout << "Passenger name cannot be empty." << endl;
            return;
        }

        cout << "Enter seat numbers one by one (-1 to stop):" << endl;
        vector<int> selectedSeats;

        while (true) {
            int seatNo;
            cout << "Seat No: ";
            cin >> seatNo;

            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid seat input." << endl;
                continue;
            }

            if (seatNo == -1) {
                break;
            }

            if (!routes[routeIndex].isValidSeat(seatNo)) {
                cout << "Seat number must be within 1 and "
                     << routes[routeIndex].getSeatCapacity() << "." << endl;
                continue;
            }

            if (isDuplicateSeatInSameBooking(selectedSeats, seatNo)) {
                cout << "Duplicate seat selected in same booking." << endl;
                continue;
            }

            if (routes[routeIndex].isSeatBooked(seatNo)) {
                cout << "Seat " << seatNo << " is already booked." << endl;
                continue;
            }

            selectedSeats.push_back(seatNo);
            cout << "Seat " << seatNo << " selected successfully." << endl;
        }

        if (selectedSeats.empty()) {
            cout << "No seats selected. Booking cancelled." << endl;
            return;
        }

        for (int i = 0; i < (int)selectedSeats.size(); i++) {
            routes[routeIndex].bookSeat(selectedSeats[i]);
        }

        Ticket newTicket(nextTicketID, passengerName, routeID, selectedSeats, routes[routeIndex].getFare());
        tickets.push_back(newTicket);
        nextTicketID++;

        saveRoutesToFile();
        saveTicketsToFile();

        cout << "\nTicket booked successfully." << endl;
        newTicket.displayTicket();
    }

    void cancelTicket() {
        int ticketID;
        cout << "\nEnter Ticket ID to cancel: ";
        cin >> ticketID;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid Ticket ID input." << endl;
            return;
        }

        int ticketIndex = findTicketIndexByID(ticketID);
        if (ticketIndex == -1) {
            cout << "Ticket ID does not exist." << endl;
            return;
        }

        int routeIndex = findRouteIndexByID(tickets[ticketIndex].getRouteID());
        if (routeIndex != -1) {
            vector<int> seats = tickets[ticketIndex].getSeatNumbers();
            for (int i = 0; i < (int)seats.size(); i++) {
                routes[routeIndex].releaseSeat(seats[i]);
            }
        }

        tickets.erase(tickets.begin() + ticketIndex);

        saveRoutesToFile();
        saveTicketsToFile();

        cout << "Ticket cancelled successfully. Seats released." << endl;
    }

    void searchTicket() const {
        int ticketID;
        cout << "\nEnter Ticket ID to search: ";
        cin >> ticketID;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid Ticket ID input." << endl;
            return;
        }

        int ticketIndex = findTicketIndexByID(ticketID);
        if (ticketIndex == -1) {
            cout << "Ticket not found." << endl;
            return;
        }

        cout << "\n===== TICKET DETAILS =====" << endl;
        tickets[ticketIndex].displayTicket();

        int routeIndex = findRouteIndexByID(tickets[ticketIndex].getRouteID());
        if (routeIndex != -1) {
            cout << "Route          : "
                 << routes[routeIndex].getFromCity() << " -> "
                 << routes[routeIndex].getToCity() << endl;
            cout << "Fare Per Seat  : "
                 << fixed << setprecision(2) << routes[routeIndex].getFare() << endl;
            cout << "-----------------------------" << endl;
        }
    }

    void revenuePerRoute() const {
        if (routes.empty()) {
            cout << "No routes available." << endl;
            return;
        }

        cout << "\n===== REVENUE PER ROUTE =====" << endl;
        for (int i = 0; i < (int)routes.size(); i++) {
            double revenue = 0.0;
            for (int j = 0; j < (int)tickets.size(); j++) {
                if (tickets[j].getRouteID() == routes[i].getRouteID()) {
                    revenue += tickets[j].getTotalAmount();
                }
            }

            cout << "Route ID " << routes[i].getRouteID()
                 << " (" << routes[i].getFromCity() << " -> " << routes[i].getToCity() << ")"
                 << " : " << fixed << setprecision(2) << revenue << endl;
        }
    }

    void totalTicketsBookedPerRoute() const {
        if (routes.empty()) {
            cout << "No routes available." << endl;
            return;
        }

        cout << "\n===== TOTAL TICKETS BOOKED PER ROUTE =====" << endl;
        for (int i = 0; i < (int)routes.size(); i++) {
            int count = 0;
            for (int j = 0; j < (int)tickets.size(); j++) {
                if (tickets[j].getRouteID() == routes[i].getRouteID()) {
                    count++;
                }
            }

            cout << "Route ID " << routes[i].getRouteID()
                 << " (" << routes[i].getFromCity() << " -> " << routes[i].getToCity() << ")"
                 << " : " << count << " ticket(s)" << endl;
        }
    }

    void mostPopularRoute() const {
        if (routes.empty()) {
            cout << "No routes available." << endl;
            return;
        }

        int maxCount = -1;
        int bestRouteIndex = -1;

        for (int i = 0; i < (int)routes.size(); i++) {
            int count = 0;
            for (int j = 0; j < (int)tickets.size(); j++) {
                if (tickets[j].getRouteID() == routes[i].getRouteID()) {
                    count++;
                }
            }

            if (count > maxCount) {
                maxCount = count;
                bestRouteIndex = i;
            }
        }

        cout << "\n===== MOST POPULAR ROUTE =====" << endl;
        if (bestRouteIndex == -1) {
            cout << "No booking data available." << endl;
        } else {
            cout << "Route ID       : " << routes[bestRouteIndex].getRouteID() << endl;
            cout << "Route          : " << routes[bestRouteIndex].getFromCity()
                 << " -> " << routes[bestRouteIndex].getToCity() << endl;
            cout << "Total Bookings : " << maxCount << endl;
        }
    }

    void allReports() const {
        revenuePerRoute();
        totalTicketsBookedPerRoute();
        mostPopularRoute();
    }

    void menu() {
        int choice;

        do {
            cout << "\n========== BUS RESERVATION SYSTEM ==========" << endl;
            cout << "1. Display Available Routes" << endl;
            cout << "2. Book Ticket" << endl;
            cout << "3. Cancel Ticket" << endl;
            cout << "4. Search Ticket by Ticket ID" << endl;
            cout << "5. Revenue Per Route" << endl;
            cout << "6. Total Tickets Booked Per Route" << endl;
            cout << "7. Most Popular Route" << endl;
            cout << "8. Show All Reports" << endl;
            cout << "9. Exit" << endl;
            cout << "Enter your choice: ";
            cin >> choice;

            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid menu choice." << endl;
                continue;
            }

            switch (choice) {
                case 1:
                    displayAvailableRoutes();
                    break;
                case 2:
                    bookTicket();
                    break;
                case 3:
                    cancelTicket();
                    break;
                case 4:
                    searchTicket();
                    break;
                case 5:
                    revenuePerRoute();
                    break;
                case 6:
                    totalTicketsBookedPerRoute();
                    break;
                case 7:
                    mostPopularRoute();
                    break;
                case 8:
                    allReports();
                    break;
                case 9:
                    saveRoutesToFile();
                    saveTicketsToFile();
                    cout << "Exiting program..." << endl;
                    break;
                default:
                    cout << "Invalid choice. Please enter 1 to 9." << endl;
            }

        } while (choice != 9);
    }
};

int main() {
    ReservationSystem system;
    system.menu();
    return 0;
}