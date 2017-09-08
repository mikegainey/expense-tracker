// =============================================================================
// Expense tracker
// =============================================================================

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <cctype>
#include <sstream>
#include <vector>
#include <cmath>
#include <ctime>
using namespace std;

struct Date
{
  int year;
  int month;
  int day;
};

struct Transaction
{
  Date date;
  string type;
  float amount;
  string description;
  string categories;
};

// ---------------------------------------------------------
// function prototypes
// ---------------------------------------------------------
void getTrans(vector<Transaction> &transactions);	// get Transactions (struct) from the input file to the vector transactions
void displayTrans(Transaction&);			// display a transaction on the screen
void report1(string, Date, Date, vector<Transaction> &);	// display a one-line report (given a category, startDate, endDate)
bool dateFilter(Date currentDate, Date lowerBound, Date upperBound);	// is the currentDate within the given range?
bool catFilter(string cat, string cats);				// is the given category found in the categories string?
void reportYoY(string category, Date, vector<Transaction> &);		// display 365-day reports for the last two years
void report8(string category, Date, vector<Transaction> &);		// print reports for the last 8 90-day periods

Date todaysDate();				// get current date (UTC)
string DateToString(Date date);			// convert a Date (struct) to a printable string
Date stoDate(string dateString);		// converts a date (string) to a Date (struct)
bool isDate(string word);			// does a date (string) conform to the format yyyy-mm-dd?
bool greaterThan(Date fst, Date snd);		// is the first Date >= the second Date?
bool lessThan(Date fst, Date snd);		// is the first Date <= the first Date?
int dateCount(Date startDate, Date endDate);	// return the number of days between the given dates
bool isLeap(int year);				// is the given year a leap year?
Date subDays(Date, int);			// subtract days from the given date
void dateDec(Date &);				// subtract 1 day from the given date
Date addDays(Date, int);			// add days to the given date
void dateInc(Date &);				// add 1 day to the given date
string trim(string word);			// trim whitespace from a string (front and back)
// ---------------------------------------------------------
// main()
// ---------------------------------------------------------
int main()
{

  Date today = todaysDate();
  cout << "\n\nToday's date is " << DateToString(today);
  vector<Transaction> transactions; // create the vector of transactions

  getTrans(transactions); // read transactions from the file and add to the transactions vector
  
  string category;
  cout << "\n\nChoose a category: ";
  getline(cin, category);
  cout << "\n\n";
  
  // report8(category, today, transactions); // display 90-day reports for the last 8 periods

  // cout <<"\n\n";

  // cout << "Previous two years:\n\n";
  reportYoY(category, today, transactions); // display one-year reports for the last two years

  cout << "\n";
  
  return 0;
}

// ---------------------------------------------------------
// reportYoY:  report year-over-year (last 2 years)
// ---------------------------------------------------------
void reportYoY(string category, Date today, vector<Transaction> &transactions)
{
  for (int y = 1; y >= 0; y--) {
    int startyear = today.year - (y+1);
    int endyear = today.year - y;
    Date startdate = Date {startyear, today.month, today.day};
    dateInc(startdate);
    Date enddate = Date {endyear, today.month, today.day};
    report1(category, startdate, enddate, transactions);
  }
}

// ---------------------------------------------------------
// generate 90-day reports for the last 8 periods
// ---------------------------------------------------------
void report8(string category, Date today, vector<Transaction> &transactions)
{
  Date startDate[8];
  for (int i = 0; i < 8; i++)
    {
      startDate[i] = subDays(addDays(today,1), (i + 1) * 90);
    }

  cout << "Previous 8 90-day periods:";
  for (int i = 7; i >= 0; i--)
    {
      report1(category, startDate[i], addDays(startDate[i],89), transactions);
    }
}

// ---------------------------------------------------------
// display a one-line report
// ---------------------------------------------------------
void report1(string category, Date startDate, Date endDate, vector<Transaction> &transactions)
{
  float totalAmount = 0;
  
  for (int trans = 0; trans < transactions.size(); trans++)
    {
      if (catFilter(category, transactions[trans].categories) &&
      	  dateFilter(transactions[trans].date, startDate, endDate))
      	{
      	  displayTrans(transactions[trans]);
      	  totalAmount += transactions[trans].amount;
      	}
    }

  // display a report line on selected date range & category
  int totalDays = dateCount(startDate, endDate);
  cout << fixed << setprecision(2);
  cout << "From " << DateToString(startDate) << " to " << DateToString(endDate);
  cout << " (" << totalDays << " days): ";
  cout << category << " total = $" << setw(8) << totalAmount;
  cout << " ($" << totalAmount/totalDays << "/day, $";
  cout << setprecision(0);
  cout << round((totalAmount/totalDays)*30) << "/month, $";
  cout << round((totalAmount/totalDays)*365) << "/year)\n\n";
}

// ---------------------------------------------------------
// load transactions vector from expense.org
// ---------------------------------------------------------
void getTrans(vector<Transaction> &transactions)
{

  ifstream inFile;
  inFile.open("expense.org");

  if (inFile) {

    string word;      
    while (getline(inFile, word, '|')) {
      word = trim(word);

      if (isDate(word)) {
	Transaction trans;
	trans.date = stoDate(word);

	(getline(inFile, word, '|'));
	trans.type = trim(word);

	(getline(inFile, word, '|'));
	trans.amount = stof(word);

	(getline(inFile, word, '|'));
	trans.description = trim(word);
	
	(getline(inFile, word, '|'));
	trans.categories = trim(word);

	transactions.push_back(trans);  // add the new Transaction trans to the transactions vector
      }
    }
  }
  
else // inFile.open didn't work
    {
      cout << "\nThe file did not open!\n\n";
    }
  
  inFile.close();
}

// ---------------------------------------------------------
// today() return current date as a Date struct
//   It's really the UTC date, but that's good enough.
// ---------------------------------------------------------
Date todaysDate()
{
  time_t t = time(0);
  tm * dtg = localtime(&t);
  int y = 1900 + dtg->tm_year;
  int m = 1 + dtg->tm_mon;
  int d = dtg->tm_mday;
  return Date {y, m, d};
}

// ---------------------------------------------------------
// isDate: does the string conform to the date format 2016-07-15?
// ---------------------------------------------------------
bool isDate(string word)
{
	return isdigit(word[0]) &&
		isdigit(word[1]) &&
		isdigit(word[2]) &&
		isdigit(word[3]) &&
		word[4] == '-' &&
		isdigit(word[5]) &&
		isdigit(word[6]) &&
		word[7] == '-' &&
		isdigit(word[8]) &&
		isdigit(word[9]);
}

// ---------------------------------------------------------
// trim: trims whitespace from the beginning and end of a string
// ---------------------------------------------------------
string trim(string word)
{
	int a = word.find_first_not_of(' ');
	int b = word.find_last_not_of(' ');
	int len = word.length();
	
	string result;
	if (a != -1)
	  result = word.substr(a,b-a+1);
	else
	  result = "false";
	return result;
}
// ---------------------------------------------------------
// Display a Transaction struct on one line
// ---------------------------------------------------------
void displayTrans(Transaction &trans)
{
  string type; // expense, income, etc.
  cout << fixed << setprecision(2);
  cout << DateToString(trans.date) << "  ";
  cout << setw(9) << left << trans.type.substr(0, 7);
  cout << "$" << setw(7) << right << trans.amount << "  ";
  cout << setw(60) << left << trans.description.substr(0, 60);
  cout << setw(35) << left << trans.categories.substr(0, 35);
  cout << endl;
}

// ---------------------------------------------------------
// DateToString: convert a Date to a string
// ---------------------------------------------------------
string DateToString(Date date)
{
	ostringstream ss;
	string mpad, dpad;
	(date.month < 10) ? mpad = "0" : mpad = "";
  	(date.day < 10) ? dpad = "0" : dpad = "";
  	ss << date.year << '-';
  	ss << mpad << date.month << '-';
  	ss << dpad << date.day;
  	string result = ss.str();
	return result;
}

// ---------------------------------------------------------
// convert string date to struct Date
// ---------------------------------------------------------
Date stoDate(string dateString)
{
  int year = stoi(dateString.substr(0, 4));
  int month = stoi(dateString.substr(5, 2));
  int day = stoi(dateString.substr(8, 2));

  // cout << year << endl;
  // cout << month << endl;
  // cout << day << endl;

  Date date = { year, month, day };
  return date;
}

// ---------------------------------------------------------
// greaterThan; relational function for Date values
//   This is really >=
// ---------------------------------------------------------
bool greaterThan(Date fst, Date snd)
{
  bool result;

  if (fst.year > snd.year) result = true;
  else if (snd.year > fst.year) result = false;
  else {
    if (fst.month > snd.month) result = true;
    else if (snd.month > fst.month) result = false;
    else {
      if (fst.day > snd.day) result = true;
      else if (snd.day > fst.day) result = false;
      else result = true;
    }
  }
  return result;
}

// ---------------------------------------------------------
// lessThan; relational function for Date values
//   This is really <=
// ---------------------------------------------------------
bool lessThan(Date fst, Date snd)
{
  bool result;
  
  if (fst.year < snd.year) result = true;
  else if (snd.year < fst.year) result = false;
  else {
    if (fst.month < snd.month) result = true;
    else if (snd.month < fst.month) result = false;
    else {
      if (fst.day < snd.day) result = true;
      else if (snd.day < fst.day) result = false;
      else result = true;
    }
  }
  return result;
}

// ---------------------------------------------------------
// bool dateFilter(currentDate, lowerBound, upperBound)
// ---------------------------------------------------------
bool dateFilter(Date currentDate, Date lowerBound, Date upperBound)
{
  return (greaterThan(currentDate, lowerBound) &&
	  lessThan(currentDate, upperBound));
}

// ---------------------------------------------------------
// return true if the chosen category is in trans.categories
// ---------------------------------------------------------
bool catFilter(string cat, string cats)
{
	int pos = cats.find(cat);
	if (pos == -1)
		return false;
	else
		return true;
}

// ---------------------------------------------------------
// dateCount = number of days between dates (inclusive)
// ---------------------------------------------------------
int dateCount(Date startDate, Date endDate)
{
  int daysin[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  int year, month, day, totalDays = 0;
  cout << endl;

  // if startDate.month == endDate.month
  if (startDate.year == endDate.year && startDate.month == endDate.month)
    totalDays = endDate.day - startDate.day + 1;
  else
    {
      // add days in startDate.month
      year = startDate.year; month = startDate.month;
      isLeap(year) ? daysin[2] = 29 : daysin[2] = 28;
      // cout << "year " << year << "  " << "month " << month << ": add " << (daysin[month] - startDate.day + 1) << " days\n";
      totalDays += daysin[month] - startDate.day + 1;
      month++;

      // add days in in-between months 
      while (year < endDate.year || month < endDate.month)
	{
	  isLeap(year) ? daysin[2] = 29 : daysin[2] = 28;
	  totalDays += daysin[month];
	  // cout << "year " << year << "  " << "month " << month << ": add " << daysin[month] << " days\n";
	  if (month < 12)
	    month++;
	  else
	    {
	      month = 1;
	      year++;
	    }
	}

      // add days in endDate.month
      // cout << "year " << year << ", " << "month " << month << ": add " << endDate.day << " days\n";
      totalDays += endDate.day;
     }

  return totalDays;
}

// ---------------------------------------------------------
// return true if the given year is a leap year
// ---------------------------------------------------------
bool isLeap(int year)
{
  bool leap = false;
  if (year % 4 == 0) leap = true;
  if (year % 100 == 0) leap = false;
  if (year % 400 == 0) leap = true;
  return leap;
}

// ---------------------------------------------------------
// Date subDays(Date, 90) // adds 90 days to the given Date (using dateDec)
// ---------------------------------------------------------
Date subDays(Date date, int days)
{
  for (int i = 1; i <= days; i++)
    { dateDec(date);
    }
  return date;
}

// ---------------------------------------------------------
// dateDec(date); // decrement a Date by one day
// ---------------------------------------------------------
void dateDec(Date &date)
{
  int daysin[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  isLeap(date.year) ? daysin[2] = 29 : daysin[2] = 28;
	
  if (date.day > 1)
    date.day--;
  else if (date.month > 1)
    {
      date.month--;
      date.day = daysin[date.month];
    }
  else
    {
      date.year--;
      date.month = 12;
      date.day = daysin[date.month];
    }
}

// ---------------------------------------------------------
// Date addDays(Date, 90) // adds 90 days to the given Date (using dateInc)
// ---------------------------------------------------------
Date addDays(Date date, int days)
{
  for (int i = 1; i <= days; i++)
    { dateInc(date);
    }
  return date;
}

// ---------------------------------------------------------
// increment a Date by one day
// ---------------------------------------------------------
void dateInc(Date &date)
{
  int daysin[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  isLeap(date.year) ? daysin[2] = 29 : daysin[2] = 28;
	
  if (date.day < daysin[date.month])
    date.day++;
  else if (date.month < 12)
    {
      date.month++;
      date.day = 1;
    }
  else
    {
      date.year++;
      date.month = 1;
      date.day = 1;
    }
}

