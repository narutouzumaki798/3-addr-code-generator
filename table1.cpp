//#include <bits/stdc++.h>
//using namespace std;

class table1{
public:
	int num_row;
	int num_col;
	string entry[50][50];
	int col_size[50];
	int row;
	int col;

	vector<string> column_names;
	table1()
	{
		num_row = 0; num_col = 0;
		for(int i=0; i<50; i++)
			for(int j=0; j<50; j++)
				entry[i][j] = "";
		for(int i=0; i<50; i++) col_size[i] = 0;
		row = 0;
		col = 0;
	}

	void tab_print(string s, int x)
	{
		if(x < s.size())
		{
			printf("\ntab print error\n");
			exit(1);
		}
		printf(" ");
		printf("%s", s.c_str());
		int tmp = x - s.size();
		while(tmp--) printf(" ");
		printf("  |");
	}

	void line1(int x, char ch)
	{
		printf("\n");
		printf("  ");
		while(x--) printf("%c", ch);
		printf("\n");
	}

	void insert(string a)
	{
		if(col == 0) num_row++;
		entry[row][col] = a;
		col_size[col] = max(col_size[col], (int)a.size());
		if(col == num_col-1)
		{
			row++;
			col = 0;
		}
		else col++;
	}


	void show(string heading)
	{
		int len = 1;
		for(int i=0; i<num_col; i++)
			len += (col_size[i] + 4);
		
		printf("\n\n  %s", heading.c_str()); 

		// printf("len = %d\n", len);
		line1(len, '=');
		printf("  |");
		for(int i=0; i<column_names.size(); i++)
		{
			tab_print(column_names[i], col_size[i]);
		}
		line1(len, '=');

		for(int i=0; i<num_row; i++)
		{
			printf("  |");
			for(int j=0; j<num_col; j++)
			{
				tab_print(entry[i][j], col_size[j]);
			}
			if(i == num_row-1)
				line1(len, '=');
			else
				line1(len, '-');
		}
		printf("\n\n");
	}

	void add_column(string a)
	{
		int c = column_names.size();
		col_size[c] = (int)a.size();
		column_names.push_back(a);
		num_col++;
	}

};

//int main()
//{
//	printf("table1 test:\n");
//	table1 a;
//	a.add_column("col 1");
//	a.add_column("columns name 2");
//	a.add_column("col 3");
//
//	a.insert("abc"); a.insert("def");
//	a.insert("ghi"); a.insert("jkl"); a.insert("abcde");
//	a.insert("ok"); a.insert("hmm aaaa tttt rrrrrrrr");
//
//	a.show("ttt");
//
//	return 0;
//}


