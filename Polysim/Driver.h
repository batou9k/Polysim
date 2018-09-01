#pragma once
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <set>
#include <map>
#include <sstream>
#include <assert.h>
#include <algorithm> 

using namespace std;

struct SimpleRec {
	double cr;
	string name;
	SimpleRec() :cr(0) {};
	SimpleRec(const string& inName , const double& crIn) :cr(crIn),name(inName){};
};

struct Record {
	map<string, vector<string>> lociLib; //loci id as key, rows as value

	Record(){};
	~Record() {};
	Record(const string& locID, const string& row) {
		
		lociLib.insert(make_pair(locID,vector<string>(1,row)));
	}

};

struct Stat {
	int le1;
	int g1;
	int le2;
	int g2;
	int tot;
	string name;

	Stat() :le1(0), g1(0), le2(0), g2(0), tot(0) {};
};

vector<string> split(const string &s, char delim);
void printUniqueNameRecords(ofstream& ofs, ofstream& ofs_unique, const string& locName, map<string, Record>& recordLib, vector<int>& count);
void printSharedRecords(ofstream& ofshared, ofstream&   ofshared_uniq, ofstream&  ofs5, ofstream&  ofs5_unique, ofstream&  ofs30,
	ofstream&  ofs30_unique, const string& locName, map<string, Record>& recordLib5, map<string, Record>& recordLib30,
	vector<int>& count5d, vector<int>& count30d);
void readRecord(ifstream& myfile, char delim, set<string>& uniqueList, map<string, Record>& recordLib, string& hdr, Stat& s, vector<Stat>& summaryStat);



class Driver {
	set<string> unique5, unique30;
	map<string, Record> recordLib5, recordLib30;
	string hdr;
	ofstream of5, of30, ofshared, ofshared_uniq, of5_unique, of30_unique,of5_stat, of30_stat, ofcount,
		of5_unique_stat, of30_unique_stat, of5_shared_stat, of30_shared_stat;
	std::ifstream if30,if5;
	Stat s5, s30;
	vector<Stat> s5Summary, s30Summary, s5SharedSummary, s30SharedSummary, s5_unique_summary, s30_unique_summary;
	string infile5d, infile30d;
public:
	Driver(const string & file5d, const string & file30d) {
		infile5d = file5d;
		infile30d = file30d;

		if5.open(file5d, std::ifstream::in);
		if30.open(file30d, std::ifstream::in);
		
		assert(if5.is_open());
		assert(if30.is_open());

		of5.open("All_unique_" + file5d, std::ofstream::out);
		of5_unique_stat.open("Stat_All_unique_"+file5d , std::ofstream::out);
		of5_unique.open("Single_unique_"+file5d, std::ofstream::out);
		of30.open("All_unique_"+ file30d, std::ofstream::out);
		of30_unique_stat.open("Stat_All_unique_" + file30d, std::ofstream::out);
		of30_unique.open("Single_unique_" + file30d, std::ofstream::out);
		ofshared.open("All_shared_"+file5d + "_" + file30d, std::ofstream::out);
		of5_shared_stat.open("Stat_All_shared_" + file5d, std::ofstream::out);
		of30_shared_stat.open("Stat_All_shared_" + file30d, std::ofstream::out);
		ofshared_uniq.open("Single_shared_" + file5d +"_"+ file30d, std::ofstream::out);
		of5_stat.open("Stat_" + file5d, std::ofstream::out);
		of30_stat.open("Stat_" + file30d, std::ofstream::out);
		ofcount.open("Count_" + file5d+file30d, std::ofstream::out);

		readRecord(if5, ' \t', unique5, recordLib5,hdr,s5, s5Summary);
		readRecord(if30, ' \t', unique30, recordLib30,hdr,s30,s30Summary);

		of5 << hdr << endl;
		of5_unique << hdr << endl;
		of30 << hdr << endl;
		of30_unique << hdr << endl;
		ofshared << hdr << " \t" << hdr << endl;
		ofshared_uniq <<hdr << " \t" << hdr << endl;
	}

	void processRecord();

	~Driver() {
		if5.close();
		if30.close();
		of5.close();
		of30.close();
		of5_unique.close();
		of30_unique.close();
		ofshared.close();
		ofshared_uniq.close();
		ofcount.close();
		
		of5_stat.close();
		of5_shared_stat.close();
		of30_stat.close();
		of30_shared_stat.close();

	}
};