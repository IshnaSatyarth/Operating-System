#include <iostream>
#include <vector>
#include <deque>

using namespace std;

class PRDS_LRU {
public:
	PRDS_LRU(int x) {};  // x denotes the number of size of the page vector (max # of pages in main memory)

	void push(int x) { q.push_back(x); };
	int pop() { int x = q.front(); q.pop_front(); return(x); };
	int size() { return q.size(); };

	//find X in q, then returns its iterator
	deque<int>::iterator findX(int x) { return find(q.begin(), q.end(), x); }
	deque<int>::iterator endItr() { return q.end(); }
	void eraseItr(deque<int>::iterator itr) { q.erase(itr); }

	deque<int> q;

};

int Page_Replacement_LRU(vector<int>& pages, int nextpage, PRDS_LRU* p)
{
	int res = 0;
	deque<int>::iterator itr;
	int i;
	bool found = false;
	for (i = 0; i < pages.size(); i++)
	{
		if (pages[i] == nextpage)
		{
			itr = p->findX(nextpage);
			p->q.erase(itr);
			p->push(nextpage);
			found = true;
			return -1;
		}
	}
	
	if (found == false) {
		for (i = 0; i < pages.size(); i++) {
			if (pages[i] == -1) {
				p->push(nextpage);
				return i;
			}
		}
		int to_replace = p->pop();
		for (i = 0; i < pages.size(); i++)
		{
			if (pages[i] == to_replace) {
				p->push(nextpage);
				return i;
			}
		}
	}
}
