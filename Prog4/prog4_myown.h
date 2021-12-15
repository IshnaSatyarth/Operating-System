
#include <iostream>
#include <vector>
#include <deque>

using namespace std;

class PRDS_MyOwn {
public:
	PRDS_MyOwn(int x) {};  // x denotes the number of size of the page vector (max # of pages in main memory)

	void push(int x) { q.push_back(x); };
	int pop() { int x = q.front(); q.pop_front(); return(x); };
	int size() { return q.size(); };

	deque<int> q;

};

int predict(vector<int>& pages, vector<int> pagelist, int index) {
	int res = -1, farthest = index;
	for (int i = 0; i < pages.size(); i++) {
		int j;
		for (j = index; j < pagelist.size(); j++) {
			if (pages[i] == pagelist[j]) {
				if (j > farthest) {
					farthest = j;
					res = i;
				}
				break;
			}
		}
		if (j == pagelist.size()) return i;
	}
	return (res == -1) ? 0 : res;
}

int Page_Replacement_MyOwn(vector<int>& pages, int nextpage, PRDS_MyOwn* p, vector<int> pagelist, int nextIndex)
{
	int res = 0;
	int i;
	bool found = false;
	for (i = 0; i < pages.size(); i++)
	{
		if (pages[i] == nextpage)
		{
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
		int j = predict(pages, pagelist, nextIndex + 1);
		pages[j] = nextpage;
		return j;
	}
}
