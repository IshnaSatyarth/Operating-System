
#include <iostream>
#include <vector>
#include <deque>

using namespace std;

class PRDS_2nd {
public:
	PRDS_2nd(int x) {};  // x denotes the number of size of the page vector (max # of pages in main memory)

	void push(int x) { q.push_back(x); };
	int pop() { int x = q.front(); q.pop_front(); return(x); };
	int size() { return q.size(); };

	deque<int> q;

};

int Page_Replacement_2nd(vector<int>& pages, int nextpage, PRDS_2nd* p, vector<int>& referenceNum)
{
	int res = 0;
	int i;
	bool found = false;
	for (i = 0; i < pages.size(); i++)
	{
		if (pages[i] == nextpage)
		{
			if (referenceNum[i] < 3) referenceNum[i] ++;
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
		while (true) {
			int to_replace = p->pop();
			for (int i = 0; i < pages.size(); i++) {
				if (pages[i] == to_replace) {
					if (referenceNum[i] == 0) {
						p->push(nextpage);
						pages[i] = nextpage;
						return i;
					}
					else {
						referenceNum[i] --;
						p->push(to_replace);
						break;
					}
				}
			}
		}
	}
}
