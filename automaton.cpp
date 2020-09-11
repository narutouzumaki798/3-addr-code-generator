
class lr0_item{
	public:
		int prod_id;
		int d; // postion of dot
		lr0_item()
		{
			d = 1; 
		}
		lr0_item(int p_id)
		{
			prod_id = p_id;
			d = 1;
		}
		lr0_item move_dot()
		{
		  lr0_item ans; ans.prod_id = prod_id;
			if(d != prod[prod_id].size())
			d++;
			ans.d = d;
			return ans;
		}
		bool end() // whether . is at end
		{
			if(d == prod[prod_id].size())  return true;
			return false;
		}

		int hash1()
		{
			int a = prod_id; int b = d;
			return 100*a + b;
		}
  void show()
  {
    int i;
    printf("%s --> ", sym_name[prod[prod_id][0]].c_str());
    for(i=1; i<prod[prod_id].size(); i++)
      {
	if(d == i) printf(".");
    	printf("%s ", sym_name[prod[prod_id][i]].c_str());
      }
    if(d == i) printf(".");
  }
};

// action
// 0 : error
// 1 : shift
// 2 : reduce
// 3 : goto
// 4 : accept
class automaton_state{
	public:
		vector<lr0_item> items;
		vector<int> action[100]; 
		vector<int> arg[100];
		automaton_state()
		{
		  items.clear();
			for(int i=0; i<100; i++)
			{
			  action[i].clear(); arg[i].clear();
			}
		}
		automaton_state(lr0_item t)
		{
			items.push_back(t);
			for(int i=0; i<100; i++)
			{
			  action[i].clear(); arg[i].clear();
			}
		}
		automaton_state(const automaton_state& that)
		{
		  items.clear();
			for(int i=0; i<that.items.size(); i++)
				this->items.push_back(that.items[i]);
			for(int i=0; i<100; i++)
			{
				this->action[i] = that.action[i];
				this->arg[i] = that.arg[i];
			}
		}
		void operator=(const automaton_state& that)
		{
		  items.clear();
			for(int i=0; i<that.items.size(); i++)
				this->items.push_back(that.items[i]);
			for(int i=0; i<100; i++)
			{
				this->action[i] = that.action[i];
				this->arg[i] = that.arg[i];
			}
		}
		lr0_item operator[](int x)
		{
			return items[x];
		}
		int size()
		{
			return (int)items.size();
		}
  bool empty()
  {
    if(items.size() == 0) return true;
    return false;
  }
		bool has(lr0_item t)
		{
			int h = t.hash1();
			for(int i=0; i<items.size(); i++)
				if(items[i].hash1() == h) return true;
			return false;
		}
		void insert(lr0_item t)
		{
			if(has(t)) return;
			items.push_back(t);
		}
		bool same_as(automaton_state that)
		{
			if(this->items.size() != that.items.size()) return 0;
			vector<int> c,d;
			for(int i=0; i<this->items.size(); i++)
			{
				c.push_back(this->items[i].hash1());
				d.push_back(that.items[i].hash1());
			}
			sort(c.begin(), c.end());
			sort(d.begin(), d.end());
			for(int i=0; i<c.size(); i++)
				if(c[i] != d[i]) return 0;
			return 1;
		}
  void show()
  {
    printf("------\n");
    for(int i=0; i<items.size(); i++)
      {
	lr0_item t = items[i];
	t.show();
	printf("\n");
      }
    printf("------\n\n\n"); fflush(stdout);
  }
};

int idx;
automaton_state automaton[100];

automaton_state closure(automaton_state a)
{
	automaton_state old_a = a;
	while(1)
	{
	  //a.show(); // debug
		for(int i=0; i<a.size(); i++) // for each lr0 item in state
		{
			lr0_item t = a[i];
			int dot = t.d; int prod_id = t.prod_id;

			if(dot == prod[prod_id].size()) continue; // dot at end
			int next_sym = prod[prod_id][dot];
			for(int j=0; j<NUM_PROD; j++) // for each production with curr next_sym as lhs
			{
				if(prod[j][0] != next_sym) continue; // take only productions with next_sym as lhs

				lr0_item tmp(j); // create trivial lr0_item (dot at front) from current production
				if(a.has(tmp)) continue; // lr0_item already in state
				a.insert(tmp);
			}
		}
		if(a.same_as(old_a)) break;
		old_a = a;
	}
	return a;
}

int exists(automaton_state a)
{
	for(int i=0; i<idx; i++)
	{
		if(a.same_as(automaton[i]))
		  return i;
	}
	return -1;
}

// actions:
// 0 : error
// 1 : shift
// 2 : reduce
// 3 : goto
// 4 : accept
void generate_automaton(automaton_state a)
{
	automaton[idx++] = a;
	int curr_state = idx - 1;

	// printf("curr state: %d\n", curr_state); fflush(stdout);
	//a.show(); // debug

	vector<int> used_syms;

	// reduces
	for(int i=0; i<a.size(); i++) // iterate through lr0 items
	{
		lr0_item t = a[i];
		int dot = t.d; int prod_id = t.prod_id;
		int lhs = prod[prod_id][0];

		if(dot == prod[prod_id].size()) // . at end : reduce
		{
			for(int j=0; j<follow[lhs].size(); j++) // followers of lhs
			{
				int follower = follow[lhs][j];
				int act = (prod_id == 0)?4:2;
				automaton[curr_state].action[follower].push_back(act);
				automaton[curr_state].arg[follower].push_back(prod_id);
			}
		}
	}

	// shifts, gotos
	for(int i=0; i<NUM_SYM; i++)
	{
		automaton_state b;
		for(int j=0; j<a.size(); j++) // items in curr state
		{
			lr0_item t = a[j];
			int dot = t.d; int prod_id = t.prod_id;

			if(dot == prod[prod_id].size()) continue; // . at end, skip

			int next_sym = prod[prod_id][dot];
			if(next_sym != i) continue;  // take only productions with curr symbol as next_sym

			if(!a[j].end())
			  b.insert(a[j].move_dot());
		}
		b = closure(b);
		if(b.empty()) continue;
		int e = exists(b);
		if(e == -1) // new state
		  {
		    int act = (term[i])?1:3;
		    automaton[curr_state].action[i].push_back(act); // set action
		    automaton[curr_state].arg[i].push_back(idx); // set argument (new state will reside at idx)
		    generate_automaton(b);
		  }
		else
		  {
		    int act = (term[i])?1:3;
		    automaton[curr_state].action[i].push_back(act); // set action
		    automaton[curr_state].arg[i].push_back(e); // set argument
		  }
	}
}


void show_automaton()
{
  for(int i=0; i<idx; i++)
    {
      printf("state %d:\n", i);
      automaton[i].show();

      tab1.insert(to_string1(i));
      for(int j=0; j<=NUM_SYM; j++)
	{
	  if(j == NUM_SYM) j = 99;
	  //if(automaton[i].action[j].size() == 0) continue;
	  //printf("%s: ", sym_name[j].c_str());
	  string entry = "";
	  for(int k=0; k<automaton[i].action[j].size(); k++)
	    {
	      int act = automaton[i].action[j][k];
	      int arg = automaton[i].arg[j][k];
	      string act_s = "ttt";
	      if(act == 1) act_s = "shift";
	      if(act == 2) act_s = "reduce";
	      if(act == 3) act_s = "goto";
	      if(act == 4) act_s = "accept";
	      
	      //printf("%s-%d  ", act_s.c_str(), arg);
	      string tmp = act_s + " " + to_string1(arg);
	      if(k < automaton[i].action[j].size() - 1)
		tmp = tmp + ", ";
	      entry += tmp;
	    }
	  tab1.insert(entry);
	  // printf("\n");
	}
      // printf("==========\n\n\n\n");
    }
}

class parse_table_cell{
public:
  int action;
  int arg;
};
parse_table_cell ptable[100][100];

void generate_parse_table()
{
  for(int i=0; i<idx; i++)
    {
      for(int j=0; j<=NUM_SYM; j++)
	{
	  if(j == NUM_SYM) j = 99;

	  if(i == 6 && sym_name[j] == "+") // conflict resolution
	    {
	      ptable[i][j].action = 2;
	      ptable[i][j].arg = 2;
	      continue;
	    }
	  if(i == 8 && sym_name[j] == "+")
	    {
	      ptable[i][j].action = 2;
	      ptable[i][j].arg = 3;
	      continue;
	    }
	  if(automaton[i].action[j].size() == 0) 
	    {
	      ptable[i][j].action = 0;
	      ptable[i][j].arg = 0;
	      continue;
	    }
	  ptable[i][j].action = automaton[i].action[j][0];
	  ptable[i][j].arg = automaton[i].arg[j][0];
	}
    }
}

