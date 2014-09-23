#include<iostream>
#include<fstream>
#include<cstring>
#include<cmath>
#include<dirent.h>
using namespace std;

struct content
{
	string strTime;
	string text;
};

struct Time
{
	int h;
	int m;
	int s;
	int ms;
	int tms;
};

content getNext(ifstream &fin);
string &nextch(ifstream &fin, string &test, int length);
Time str2time(string t);
void showTime(Time, ofstream &fout);
void convert(ofstream &fout, ifstream &fin, content A, int line);

int main()
{
	dirent *ep;
	DIR *dp;
	dp = opendir("./");
	ep = readdir(dp);
	while (ep != NULL)
	{
		string name = ep->d_name;
		if (name.find(".txt") != string::npos)
		{
			ifstream fin;
			fin.open(&name[0]);
			content test = getNext(fin);
			if (test.strTime.size() > 0)
			{
				string oname = string(&(name[0]), name.size() - 4)
								+ ".YTBCC_eng.srt";
				ofstream fout;
				fout.open(&oname[0]);
				convert(fout, fin, test, 1);
				fout.close();
			}
			fin.close();
		}
		ep = readdir(dp);
	}
}

void convert(ofstream &fout, ifstream &fin, content A, int line)
{
	content N = getNext(fin);
	if (N.strTime.size() > 0)
	{

		Time F = str2time(A.strTime);
		Time L = str2time(N.strTime);
		if (F.tms + 10000 >= L.tms)
		{
			fout << line << endl;
			showTime(F, fout);
			fout << " --> ";
			showTime(L, fout);
			fout << endl;
			fout << A.text << endl << endl;
		}
		else
		{
			fout << line << endl;
			showTime(F, fout);
			fout << " --> ";
			Time temp;
			temp.tms = F.tms + 10000;  //limit 10 seconds per line
			temp.h = temp.tms / 3600000;
			temp.m = (temp.tms - temp.h * 3600000) / 60000;
			temp.s = (temp.tms - temp.h * 3600000 - temp.m * 60000) / 1000;
			temp.ms = F.ms;
			showTime(temp, fout);
			fout << endl;
			fout << A.text << endl << endl;
		}
		convert(fout, fin, N, line + 1);
	}
	else
	{
		fout << line << endl;
		Time Pr = str2time(A.strTime);
		showTime(Pr, fout);
		fout << " --> ";
		Time Ne;
		Ne.ms = Pr.ms;
		int all_t = Pr.h * 3600 + Pr.m * 60 + Pr.s + 6;
		Ne.h = all_t / 3600;
		Ne.m = (all_t - Ne.h * 3600) / 60;
		Ne.s = all_t - Ne.h * 3600 - Ne.m * 60;
		showTime(Ne, fout);
		fout << endl;
		fout << A.text << endl;
	}
}

content getNext(ifstream &fin)
{
	string strTime, text;
	string keyTime = "data-time=\"";
	string keyText = "<div class=\"caption-line-text\">";
	int size_ktime = keyTime.size();
	int size_ktext = keyText.size();
	bool jud = 1; //judge if our time and text are
	              //contained in txt file
	              //1 == be not contained
	string test;
	//get strTime
	test = nextch(fin, test, size_ktime);
	if (test != "\0")
	{
		do
		{
			if (test == keyTime)
			{
				jud = 0;
				break;
			}
			else
				test = nextch(fin, test, size_ktime);
		}while (test != "\0");
		if (jud)
			return {"\0", "\0"};
		else
		{
			char ch = fin.get();
			while(ch != '\"')
			{
				strTime = strTime + ch;
				ch = fin.get();
			}
			//ensure the form of strTime is XXX.XXX
			jud = 1;//here jud = 1 means that the form is XXX,no '.'
			for (int i = 0; i < strTime.size(); i++)
			{
				if(strTime[i] == '.')
				{
					jud = 0;
					break;
				}
			}
			if (jud)
				strTime = strTime + ".0";
		}
	}
	else
		return {"\0", "\0"};
	//get text
	jud = 1;
	test = string();
	test = nextch(fin, test, size_ktext);
	if (test != "\0")
	{
		do
		{
			if (test == keyText)
			{
				jud = 0;
				break;
			}
			else
				test = nextch(fin, test, size_ktext);
		} while (test != "\0");
		if (jud)
			return {"\0", "\0"};
		else
		{
			char ch = fin.get();
			while (ch != '<')
			{
				text = text + ch;
				ch = fin.get();
			}
		}
	}
	else
		return {"\0", "\0"};

	return {strTime, text};
}

string &nextch(ifstream &fin, string &test, int length)
{
	int n = test.size();
	if(n > 0)
	{
		char ch = fin.get();
		if (!fin.eof())
		{
			for (int i = 0; i < n - 1; i++)
				test[i] = test[i + 1];

			test[n - 1] = ch;
			return test;
		}
		else
		{
			test = "\0";
			return test;
		}
	}
	else
	{
		char ch = fin.get();
		for (int i = 0; i < length; i++)
		{
			if (!fin.eof())
			{
				test = test + ch;
				if (i < length - 1)
					ch = fin.get();
			}
			else
			{
				test = "\0";
				return test;
			}
		}
		return test;
	}
}

Time str2time(string t)
{
	int k = 0;
	while (t[k] != '.')
	{
		k++;
	}
	int h, m, s, ms, tms, time;
	time = 0;
	for (int i = 0; i < k; i++)
	{
		time = time + (t[i] - 48) * int(pow(10.0, k - i - 1));
	}
	h = time / 3600;
	m = (time - h * 3600) / 60;
	s = time - h * 3600 - m * 60;
	ms = 0;
	int length = t.size();
	for (int i = k + 1; i < length; i++)
		ms = ms + (t[i] - 48) * int(pow(10.0, 3 - i + k));
	tms = time * 1000 + ms;
	Time E = {h, m, s, ms, tms};
	return E;
}

void showTime(Time T, ofstream &fout)
{
	if (T.h < 10)
		fout << "0" << T.h;
	else
		fout << T.h;

	fout << ":";

	if (T.m < 10)
		fout << "0" << T.m;
	else
		fout << T.m;

	fout << ":";

	if (T.s < 10)
		fout << "0" << T.s;
	else
		fout << T.s;

	if (T.ms == 0)
		fout << ",000";
	else
	{
		fout << ",";
		if (T.ms < 10)
			fout << "00" << T.ms;
		else if (T.ms < 100)
			fout << "0" << T.ms;
		else fout << T.ms;
	}
}
