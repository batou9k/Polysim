#include "stdafx.h"
#include "Driver.h"

vector<string> split(const string &s, char delim) {
	stringstream ss(s);
	string item;
	vector<string> tokens;
	while (getline(ss, item, delim)) {
		tokens.push_back(item);
	}
	return tokens;
}

void getStat(Stat&s, set<string>& uniqueList, vector<double>& cr, vector < SimpleRec>& rcList, vector<Stat>& summaryStat) {
	s.g1 = count_if(cr.begin(), cr.end(), [](double val) {return val > 1; });
	s.le1 = count_if(cr.begin(), cr.end(), [](double val) {return val <= 1; });
	s.g2 = count_if(cr.begin(), cr.end(), [](double val) {return val > 2; });
	s.le2 = count_if(cr.begin(), cr.end(), [](double val) {return val <= 2; });
	s.tot = cr.size();

	for (auto it = uniqueList.begin(); it != uniqueList.end(); ++it) {
		Stat tempS;
		tempS.name = *it;
		tempS.tot = count_if(rcList.begin(), rcList.end(), [it](SimpleRec& val) {return (*it == val.name); });
		tempS.g1 = count_if(rcList.begin(), rcList.end(), [it](SimpleRec& val) {return ((*it == val.name) && (val.cr > 1)); });
		tempS.le1 = count_if(rcList.begin(), rcList.end(), [it](SimpleRec& val) {return ((*it == val.name) && (val.cr <= 1)); });
		tempS.g2 = count_if(rcList.begin(), rcList.end(), [it](SimpleRec& val) {return ((*it == val.name) && (val.cr > 2)); });
		tempS.le2 = count_if(rcList.begin(), rcList.end(), [it](SimpleRec& val) {return ((*it == val.name) && (val.cr <= 2)); });
		summaryStat.push_back(tempS);
	}

	sort(summaryStat.begin(), summaryStat.end(), [](const Stat& lhs, const Stat& rhs) {return lhs.tot > rhs.tot; });
}

void readRecord(ifstream& myfile, char delim, set<string>& uniqueList, map<string, Record>& recordLib, string& hdr,Stat& s, vector<Stat>& summaryStat){

	string line;
	vector<string> strList;
	vector<double> cr;
	vector < SimpleRec> rcList;
	getline(myfile, line); //skip header
	hdr = line;
	while (getline(myfile, line))
	{
		strList = split(line, delim);

		string locName = strList[4];
		if (locName.find("gene@") != string::npos)continue;
		if (locName.find("SAT") != string::npos)continue;
		cr.push_back(stof(strList[12]));
		string locID = strList[16];
		uniqueList.insert(locName);
		auto itName = recordLib.find(locName);
		if (itName == recordLib.end()) {
			recordLib[locName] = Record(locID,line);
		}
		else
		{
			itName->second.lociLib[locID].push_back(line);
		}

		rcList.emplace_back(SimpleRec(locName, stof(strList[12])));

	}

	getStat(s, uniqueList, cr, rcList, summaryStat);

	myfile.close();
}

void printStat(const Stat& s, const vector<Stat>& summary, ofstream& ofs) {

	ofs << "Total" << "\t" << s.tot << endl;
	ofs << "CR>1" << "\t" << s.g1 << endl;
	ofs << "CR<=1" << "\t" << s.le1 << endl;
	ofs << "CR>2" << "\t" << s.g2 << endl;
	ofs << "CR<=2" << "\t" << s.le2 << endl;
	ofs << endl;
	ofs << "Name" << "\t" << "Total" << "\t" << "CR>1" << "\t" << "CR<=1" << "\t" << "CR>2" << "\t" << "CR<=2" <<endl;
	
	auto printSummary = [&ofs](const Stat& s) {
		ofs << s.name << "\t";
		ofs << s.tot << "\t";
		ofs << s.g1 << "\t";
		ofs << s.le1 << "\t";
		ofs << s.g2 << "\t";
		ofs << s.le2 << endl;
	};

	for_each(summary.begin(), summary.end(), printSummary);

	ofs.close();
}

void printCount(vector<int> count,ofstream& ofs) {
	ofs << "\t";
	ofs << "common TE" << "\t" << "unique TE" << endl;
	ofs << "uq TE reads\t" << count[0]<< "\t" << count[1] <<endl;
	ofs << "total TE reads\t" << count[2] << "\t" << count[3] << endl;

}


void createSummary(const string& fileName, Stat& s,vector<Stat>& summaryStat) {
	set<string> uniqueList;
	ifstream myfile;
	myfile.open(fileName, std::ifstream::in);

	string line;
	vector<string> strList;
	vector<double> cr;
	vector < SimpleRec> rcList;
	getline(myfile, line); //skip header
	while (getline(myfile, line))
	{
		strList = split(line, ' \t');
		string locName = strList[4];
		cr.push_back(stof(strList[12]));
		uniqueList.insert(locName);
		rcList.emplace_back(SimpleRec(locName, stof(strList[12])));
	}

	getStat(s, uniqueList,cr,rcList,summaryStat);

	myfile.close();
}

void createSharedSummary(const string& fileName, Stat& s5_shared, Stat&  s30_shared,vector<Stat>& summaryStat5, vector<Stat>& summaryStat30) {
	set<string> uniqueList5, uniqueList30;
	ifstream myfile;
	myfile.open(fileName, std::ifstream::in);
	const int Diff = 19;

	string line;
	vector<string> strList;
	vector<double> cr5, cr30;
	vector < SimpleRec> rcList5, rcList30;
	getline(myfile, line); //skip header
	while (getline(myfile, line))
	{
		strList = split(line, ' \t');
		if (!strList[0].empty()) {
			string locName5 = strList[4];
			cr5.push_back(stof(strList[12]));
			uniqueList5.insert(locName5);
			rcList5.emplace_back(SimpleRec(locName5, stof(strList[12])));
		}
		if (!strList[0+Diff].empty()) {
			string locName30 = strList[4 + Diff];
			cr30.push_back(stof(strList[12 + Diff]));
			uniqueList30.insert(locName30);
			rcList30.emplace_back(SimpleRec(locName30, stof(strList[12 + Diff])));
		}
	}

	getStat(s5_shared, uniqueList5, cr5, rcList5, summaryStat5);
	getStat(s30_shared, uniqueList30, cr30, rcList30, summaryStat30);

	myfile.close();
}

void Driver::processRecord() {

	vector<int> count5d(4,0), count30d(4, 0);
	for (auto it5 = unique5.begin(); it5 != unique5.end(); ++it5) {

		auto it30 = unique30.find(*it5);

		if (it30 == unique30.end()) {
			printUniqueNameRecords(of5, of5_unique, *it5, recordLib5,count5d);
		}
		else
		{
			printSharedRecords(ofshared, ofshared_uniq, of5, of5_unique, of30, of30_unique,*it5,recordLib5, recordLib30,count5d,count30d);
		}
	}

	for (auto it30 = unique30.begin(); it30 != unique30.end(); ++it30) {
		auto it5 = unique5.find(*it30);
		if (it5 == unique5.end()) {
			printUniqueNameRecords(of30, of30_unique,*it30, recordLib30,count30d);
		}
	}

	//of5.close();
	//of30.close();
	//ofshared.close();
	
	Stat s5_unique, s30_unique,s5_shared, s30_shared;
	createSummary("All_unique_" + infile5d, s5_unique,s5_unique_summary);
	createSummary("All_unique_" + infile30d, s30_unique, s30_unique_summary);
	createSharedSummary("All_shared_" + infile5d + "_" + infile30d, s5_shared, s30_shared,s5SharedSummary, s30SharedSummary);

	printStat(s5, s5Summary, of5_stat);
	printStat(s30, s30Summary, of30_stat);

	printStat(s5_unique, s5_unique_summary, of5_unique_stat);
	printStat(s30_unique, s30_unique_summary, of30_unique_stat);
	printStat(s5_shared, s5SharedSummary,of5_shared_stat);
	printStat(s30_shared, s30SharedSummary,of30_shared_stat);

	printCount(count5d, ofcount);
	for (auto i = 0; i < 4; ++i) { ofcount << endl; };
	printCount(count30d,ofcount);
}


void printUniqueNameRecords(ofstream& ofs, ofstream& ofs_unique, const string& locName, map<string, Record>& recordLib, vector<int>& count) {

	auto it = recordLib.find(locName);
	if (it != recordLib.end()) {
		for (auto itloc = it->second.lociLib.begin(); itloc != it->second.lociLib.end(); ++itloc)
		{
			for (int i = 0; i < itloc->second.size(); ++i) {
				ofs << itloc->second[i] << endl;
				++count[3];
			}
			ofs_unique << itloc->second[0] << endl;
			++count[1];
		}
	}

}


void printUniqueLocRecords(ofstream& ofs, vector<string>& locList, vector<int>& count) {

	for_each(locList.begin(), locList.end(), [&count, &ofs](const string& line) {ofs << line << endl; ++count[3]; });
}

void printSharedRecords(ofstream& ofshared, ofstream&   ofshared_uniq, ofstream&  ofs5, ofstream&  ofs5_unique,
	ofstream&  ofs30, ofstream&  ofs30_unique,
	const string& locName, map<string, Record>& recordLib5, map<string, Record>& recordLib30,
	vector<int>& count5d, vector<int>& count30d) {

	auto it5 = recordLib5.find(locName);
	auto it30 = recordLib30.find(locName);

	for (auto itLoc5 = it5->second.lociLib.begin(); itLoc5 != it5->second.lociLib.end(); ++itLoc5) {
		auto itLoc30 = it30->second.lociLib.find(itLoc5->first);
		if (itLoc30 == it30->second.lociLib.end()) {
			//print 5 as unique
			printUniqueLocRecords(ofs5, itLoc5->second,count5d);
			ofs5_unique << itLoc5->second[0] << endl;
			++count5d[1];
		}
		else
		{
			//process shared 
			if (itLoc5->second.size() <= itLoc30->second.size()) {
				int i = 0;
				ofshared_uniq << itLoc5->second[i] << "\t" << itLoc30->second[i] << endl;
				++count5d[0];
				++count30d[0];
				for (; i < itLoc5->second.size(); ++i) {
					ofshared << itLoc5->second[i] << "\t" << itLoc30->second[i] << endl;
					++count5d[2];
					++count30d[2];
				}
				for (; i < itLoc30->second.size(); ++i) {
					for (int i = 0; i < 19; ++i)
					{
						ofshared << "\t";
					}
					ofshared <<itLoc30->second[i] << endl;
					++count30d[2];
				}
			}
			else
			{
				int i = 0;
				ofshared_uniq << itLoc5->second[i] << "\t" << itLoc30->second[i] << endl;
				++count5d[0];
				++count30d[0];
				for (; i < itLoc30->second.size(); ++i) {
					ofshared << itLoc5->second[i] << "\t" << itLoc30->second[i] << endl;
					++count5d[2];
					++count30d[2];
				}
				for (; i < itLoc5->second.size(); ++i) {
					ofshared << itLoc5->second[i] << "\t";
					++count5d[2];
					for (int i = 0; i < 19; ++i)
					{
						ofshared << "\t";
					}
					ofshared << endl;
				}
				
			}
		}
	}

	for (auto itLoc30 = it30->second.lociLib.begin(); itLoc30 != it30->second.lociLib.end(); ++itLoc30) {
		auto itLoc5= it5->second.lociLib.find(itLoc30->first);
		if (itLoc5== it5->second.lociLib.end()) {
			//print unique 30
			printUniqueLocRecords(ofs30, itLoc30->second, count30d);
			ofs30_unique << itLoc30->second[0] << endl;
			++count30d[1];
		}
	}

}