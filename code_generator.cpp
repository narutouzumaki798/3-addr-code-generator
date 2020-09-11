int mem_idx;
string mem[100]; // each element is assumed to be single byte
char temp_code[100];
int tmp_idx;

class symbol_inst{
public:
  int type;
  string lexeme;
  string code;
  int addr;
  symbol_inst(int t, string l)
  {
    type = t; 
    lexeme = l;
    code = "";
    addr = -1;
  }
};


class symbol_table_entry{
public:
  string lexeme;
  int type; //  11 = id, 12 = ptr (according to initial symbol definition)
  int addr; // location in memory

  symbol_table_entry(string l) // id
  {
    type = 11;
    lexeme = l;
    addr = mem_idx;
    mem[mem_idx] = l;     // occupy 4 bytes
    mem[mem_idx+1] = l;
    mem[mem_idx+2] = l;
    mem[mem_idx+3] = l;
    mem_idx += 4;
  }
  symbol_table_entry(string l, int size) // ptr
  {
    type = 12;
    lexeme = l;
    addr = mem_idx;
    mem[mem_idx]   = l + " addr " + to_string1(mem_idx+4);
    mem[mem_idx+1] = l + " addr " + to_string1(mem_idx+4);
    mem[mem_idx+2] = l + " addr " + to_string1(mem_idx+4);
    mem[mem_idx+3] = l + " addr " + to_string1(mem_idx+4);
    for(int i=mem_idx+4; i<mem_idx+(size+1)*4; i++)
      {
	mem[i] = l;
      }
    mem_idx += (size + 1)*4; // 4 bytes for pointer for + 4*size bytes for "size" no of integers
  }
};

class symbol_table{
public:
  vector<symbol_table_entry> tab;
  void insert(string l) // id
  {
    symbol_table_entry tmp(l);
    tab.push_back(tmp);
  }
  void insert(string l, int n) // ptr
  {
    symbol_table_entry tmp(l, n);
    tab.push_back(tmp);
  }
  int get(string l) // returns address of symbl with lexeme = l
  {
    for(int i=0; i<tab.size(); i++) 
      {
	if(tab[i].lexeme == l) return tab[i].addr;
      }
    return -1;
  }
  symbol_inst check(string l) // checks if symbol with lexmeme = l exists in sym_tab and returns corresponding to sym_inst
  {
    for(int i=0; i<tab.size(); i++)
      {
	if(tab[i].lexeme == l)
	  {
	    symbol_inst t(tab[i].type, l);
	    return t;
	  }
      }
    symbol_inst t(-1, "ttt"); // if such symbol does not exist
    return t;
  }
};

symbol_table sym_tab;
void reset_mem()
{
  for(int i=0; i<100; i++) mem[i] = "";
}
void mannual_symtab()
{
  reset_mem();
  mem_idx = 0; tmp_idx = 0;
  int n; string l; int v;
  if(mode == "debug") n = 3;
  else
    {
      printf("No of variables:  ");
      cin >> n;
    }
  for(int i=1; i<=n; i++)
    {
      if(mode == "debug")
	l = "var" + to_string1(i);
      else
	{
	  printf("variable %d lexeme:  ", i+1);
	  cin >> l;
	}
      sym_tab.insert(l);
    }
  if(mode == "debug")
    {
      l = "arr1"; n = 5;
    }
  else
  {
    cout << "Enter aray name:  ";
    cin >> l;
    cout << "enter size of array:  ";
    cin >> n;
  }
  sym_tab.insert(l, n);
  printf("\n\n");
}

vector<symbol_inst> input;
void mannual_expression_input()
{
  string exp;
  if(mode == "debug")
    {
      exp = "var1 = arr1 [ var2 ] + var3 ;";
    }
  else
    {
      printf("Enter Expression:  ");
      getline(cin, exp); if(exp.size() == 0) getline(cin, exp);
    }
  vector<string> words = split1(exp);
  for(int i=0; i<words.size(); i++)
    {
      string w = words[i];
      symbol_inst t = sym_tab.check(w); // check if symbol is in sym_tab
      if(t.type != -1)
	{
	  input.push_back(t);
	  continue;
	}
      int flag = 0;
      for(int j=0; j<NUM_SYM; j++)
	{
	  if(identifier[j]) continue;
	  if(w == sym_name[j])
	    {
	    symbol_inst t = symbol_inst(j, sym_name[j]);
	    input.push_back(t);
	    flag = 1;
	    break;
	    }
	}
      if(flag == 0)
	{
	  printf("Unknown Symmbol: %s\n\n", w.c_str());
	  exit(1);
	}
    }
  input.push_back(symbol_inst(99, "$"));
}



vector<int> state_stack;
vector<symbol_inst> sym_stack;

int new_temp()
{
  string t = "t" + to_string1(tmp_idx); tmp_idx++;
  mem[mem_idx] = t;
  mem[mem_idx + 1] = t;
  mem[mem_idx + 2] = t;
  mem[mem_idx + 3] = t;
  mem_idx += 4;
  return mem_idx - 4;
}

// actions:
// 0 : error
// 1 : shift
// 2 : reduce
// 3 : goto
// 4 : accept
symbol_inst generate_3_addr_code()
{
  int curr_state = 0;
  int inp_idx = 0;
  state_stack.push_back(0);
  int err_itr = 0;
  while(1)
    {
      err_itr++;
      if(err_itr > 100)
	{
	  printf("err_itr > 100\n\n");
	  exit(1);
	}

      int curr_state = state_stack[state_stack.size() - 1];
      int curr_inp = input[inp_idx].type;

      int curr_action = ptable[curr_state][curr_inp].action;
      int curr_arg = ptable[curr_state][curr_inp].arg;

      // printf("curr_state: %d  curr_inp: %s   curr_action: %s   curr_arg: %d\n",
      // 	     curr_state, sym_name[curr_inp].c_str(), action_s[curr_action].c_str(), curr_arg); fflush(stdout);

      if(curr_action == 0)
	{
	  printf("\n\n  Syntax Error\n\n");
	  exit(1);
	}
      else if(curr_action == 1) // shift
	{
	  sym_stack.push_back(input[inp_idx]);
	  state_stack.push_back(curr_arg);
	  inp_idx++;
	}
      else if(curr_action == 2) // reduce
	{
	  vector<int> curr_prod = prod[curr_arg];
	  int a = curr_prod.size()-1;
	  int b = sym_stack.size()-1;
	  vector<symbol_inst> rhs;
	  int itr = (int)curr_prod.size()-1;
	  // printf("reduce itr : %d\n", itr); fflush(stdout);
	  while(itr--)
	    {
	      if(a > (curr_prod.size()-1) || a < 0 || b > (sym_stack.size()-1) || b < 0)
		{
		  printf("Parse Unknown Error 1\n");
		  exit(1);
		}
	      if(curr_prod[a] != sym_stack[b].type)
		{
		  printf("Reduce Error\n");
		  exit(1);
		}
	      rhs.push_back(sym_stack[b]);
	      a--; b--;
	      sym_stack.pop_back();
	      state_stack.pop_back();
	    }
	  // printf("reduce check 2\n"); fflush(stdout);

	  reverse(rhs.begin(), rhs.end());
	  symbol_inst lhs(curr_prod[0], sym_name[curr_prod[0]]);
	  int prod_id = curr_arg;

	  if(prod_id == 0)     // S' -> S
	    {
	      // accept hoe jabe
	    }
	  else if(prod_id == 1) // S -> id = E ;
	    {
	      rhs[0].addr = sym_tab.get(rhs[0].lexeme);
	      sprintf(temp_code, "mem[%d] = mem[%d]", rhs[0].addr, rhs[2].addr);
	      string temp_code_string = temp_code;
	      // lhs.code = rhs[0].code + "\n" + rhs[2].code + "\n" + temp_code_string;
	      lhs.code = rhs[2].code + "\n" + temp_code_string; // id r jonno code lage na
	    }
	  else if(prod_id == 2) //  E -> E + E
	    {
	      lhs.addr = new_temp();
	      sprintf(temp_code, "mem[%d] = mem[%d] + mem[%d]", lhs.addr, rhs[0].addr, rhs[2].addr);
	      string temp_code_string = temp_code;
	      lhs.code = rhs[0].code + "\n" + rhs[2].code + "\n" + temp_code_string;
	    }
	  else if(prod_id == 3) //  E -> - E
	    {
	      lhs.addr = new_temp();
	      sprintf(temp_code, "mem[%d] = - mem[%d]", lhs.addr, rhs[1].addr);
	      string temp_code_string = temp_code;
	      lhs.code = rhs[1].code + "\n" + temp_code_string;
	    }
	  else if(prod_id == 4) //  E -> ( E )
	    {
	      lhs.addr = rhs[1].addr;
	      lhs.code = rhs[1].code;
	    }
	  else if(prod_id == 5) //  E -> ptr [ id ]
	    {
	      lhs.addr = new_temp();
	      rhs[0].addr = sym_tab.get(rhs[0].lexeme); // rhs[0] is ptr
	      rhs[2].addr = sym_tab.get(rhs[2].lexeme); // rhs[2] is id
	      symbol_inst t1(11, ""); t1.addr = new_temp();
	      symbol_inst t2(11, ""); t2.addr = new_temp();

	      sprintf(temp_code, "mem[%d] = mem[%d] * 4", t1.addr, rhs[2].addr);
	      lhs.code = temp_code;

	      sprintf(temp_code, "mem[%d] = mem[%d] + mem[%d]", t2.addr, rhs[0].addr, t1.addr);
	      string tmp_code_string = temp_code;
	      lhs.code = lhs.code + "\n" + tmp_code_string;

	      sprintf(temp_code, "mem[%d] = mem[mem[%d]]", lhs.addr, t2.addr);
	      tmp_code_string = temp_code;
	      lhs.code = lhs.code + "\n" + tmp_code_string;
	    }
	  else if(prod_id == 6) //  E -> id
	    {
	      lhs.addr = sym_tab.get(rhs[0].lexeme); // rhs[0] is id
	      lhs.code = "";
	    }

	  sym_stack.push_back(lhs);

	  curr_state = state_stack[state_stack.size()-1];
	  state_stack.push_back(ptable[curr_state][lhs.type].arg); // go to
	}
      else if(curr_action == 3) // go to
	{
	  // not possible here 
	}
      else if(curr_action == 4) // accept
	{
	  // printf("sym_stack[0] : %s\n", sym_name[sym_stack[0].type].c_str());
	  // printf("sym_stack[1] : %s\n", sym_name[sym_stack[1].type]);
	  symbol_inst ans = sym_stack[0];
	  return ans;
	}
    }
}

void show_memory()
{
  int bytes = 25;
  for(int i=0; i<8; i++)
    {
      if(i%2 == 0) tab2.add_column("addr");
      else tab2.add_column("val");
    }
  for(int i=0; i<bytes*4; i++)
    {
      string m = "mem[" + to_string1(i) + "]";
      tab2.insert(m);
      string v = mem[i];
      tab2.insert(v);
    }
  tab2.show("memory");
}

void parse()
{
  symbol_inst ans = generate_3_addr_code();
  string code = ans.code;
  string fcode;
  for(int i=0; i<code.size()-1; i++)
    {
      if(code[i] == '\n' && code[i+1] == '\n') code[i+1] = (char)0;
    }
  for(int i=0; i<code.size(); i++)
    {
      if(code[i] != (char)0) 
	fcode += code[i];
    }
  printf("\n\n 3 addr code:\n");
  printf(" ------------\n");
  cout << fcode;
  printf("\n----------------\n");
  show_memory();
}

/*
  sprod[1] = "S -> id = E ;";
  sprod[2] = "E -> E + E";
  sprod[3] = "E -> - E";
  sprod[4] = "E -> ( E )";
  sprod[5] = "E -> ptr [ id ]";
  sprod[6] = "E -> id";
*/
