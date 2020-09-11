#include <bits/stdc++.h>
using namespace std;
string sym_name[100];
int term[100];
vector<int> prod[100];
string sprod[100];
int NUM_PROD;
int NUM_SYM;
vector<int> follow[100];
int identifier[100];
string mode;
string action_s[10];

#include "util.cpp"

#include "table1.cpp"
table1 tab1;
table1 tab2;

#include "automaton.cpp"
#include "code_generator.cpp"

void augment_grammar()
{
  sym_name[0] = sym_name[1] + "'"; term[0] = 0;
  sprod[0] = sym_name[0] + " -> " + sym_name[1];
  NUM_SYM++;
  NUM_PROD++;
  sym_name[99] = "$"; term[99] = 1; // $ symbol
}

void compute_productions()
{
  for(int i=0; i<NUM_PROD; i++)
    {
      string s = sprod[i];
      vector<string> words = split1(s);
      for(int j=0; j<words.size(); j++)
	{
	  string w = words[j];
	  if(w == "->") continue;
	  for(int k=0; k<NUM_SYM; k++)
	    if(w == sym_name[k]) prod[i].push_back(k);
	}
    }

  // printf("\n");
  // for(int i=0; i<NUM_PROD; i++)
  // {
  // 	for(int j=0; j<prod[i].size(); j++)
  // 		printf("%d  ", prod[i][j]);
  // 	printf("\n");
  // }
}


void symbols()
{
  // symbols
  NUM_SYM = 12;
  sym_name[1] = "S";     term[1] = 0;   identifier[1] = 0;
  sym_name[2] = "E";     term[2] = 0;   identifier[2] = 0;
  sym_name[3] = "+";     term[3] = 1;   identifier[3] = 0;
  sym_name[4] = "-";     term[4] = 1;   identifier[4] = 0;
  sym_name[5] = ";";     term[5] = 1;   identifier[5] = 0;
  sym_name[6] = "=";     term[6] = 1;   identifier[6] = 0;
  sym_name[7] = "(";     term[7] = 1;   identifier[7] = 0;
  sym_name[8] = ")";     term[8] = 1;   identifier[8] = 0;
  sym_name[9] = "[";     term[9] = 1;   identifier[9] = 0;
  sym_name[10] = "]";    term[10] = 1;  identifier[10] = 0;
  sym_name[11] = "id";   term[11] = 1;  identifier[11] = 1;
  sym_name[12] = "ptr";  term[12] = 1;  identifier[12] = 1;
  sym_name[99] = "$";    term[99] = 1;  identifier[99] = 0;

  action_s[0] = "error";
  action_s[1] = "shift";
  action_s[2] = "reduce";
  action_s[3] = "goto";
  action_s[4] = "accept";
}

void grammar1() // ambiguous
{
  // productions
  NUM_PROD = 6;
  sprod[1] = "S -> id = E ;";
  sprod[2] = "E -> E + E";
  sprod[3] = "E -> - E";
  sprod[4] = "E -> ( E )";
  sprod[5] = "E -> ptr [ id ]";
  sprod[6] = "E -> id";
  
  //follow
  follow[0].push_back(99); // S'
  
  follow[1].push_back(99); // S
  
  follow[2].push_back(3);  // E
  follow[2].push_back(5);
  follow[2].push_back(8);
}
void grammar2()
{
  // productions
  NUM_PROD = 13;
  sprod[1] = "S -> id = E ;";
  sprod[2] = "E -> E + id";
  sprod[3] = "E -> E + ptr [ id ]";
  sprod[4] = "E -> E - id";
  sprod[5] = "E -> E - ptr [ id ]";
  sprod[6] = "E -> E + ( E )";
  sprod[7] = "E -> E - ( E )";
  sprod[8] = "E -> id";
  sprod[9] = "E -> - id";
  sprod[10] = "E -> ptr [ id ]";
  sprod[11] = "E -> - ptr [ id ]";
  sprod[12] = "E -> ( E )";
  sprod[13] = "E -> - ( E )";
  
  //follow
  follow[0].push_back(99);
  
  follow[1].push_back(99);
  
  follow[2].push_back(3);
  follow[2].push_back(4);
  follow[2].push_back(5);
  follow[2].push_back(8);
}

void show_input()
{
  printf("input:\n");
  for(int i=0; i<input.size(); i++)
    {
      symbol_inst s = input[i];
      if(identifier[s.type])
	printf("%s", s.lexeme.c_str());
      else
	printf("%s", sym_name[s.type].c_str());
      printf(" ");
    }
  printf("\n");
}


int main()
{
  mode = "debug";

  symbols();
  grammar1();
  augment_grammar();
  compute_productions();

  lr0_item t(0);
  automaton_state i0(t);
  // printf("debug check 1\n");
  i0 = closure(i0);
  // printf("debug check 2\n");
  generate_automaton(i0);

  tab1.add_column("state");
  for(int i=0; i<NUM_SYM; i++)
    tab1.add_column(sym_name[i]);
  tab1.add_column("$");
  show_automaton();

  tab1.show("parse table");

  generate_parse_table();

  mannual_symtab();
  mannual_expression_input();
  printf("tokenized input:\n");
  for(int i=0; i<input.size(); i++)
   {
     printf("%s %d\n", input[i].lexeme.c_str(), input[i].type);
   }
  printf("\n\n");
  show_input();
  parse();

  return 0;
}











