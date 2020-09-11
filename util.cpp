vector<string> split1(string s)
{
  vector<string> ans; string w;
  for(int i=0; i<s.size(); i++)
    {
      if(s[i] == ' ')
	{
	  if(w.size()) ans.push_back(w);
	  w = "";
	}
      else if(i == s.size()-1)
	{
	  w += s[i];
	  if(w.size()) ans.push_back(w);
	}
      else
	{
	  w += s[i];
	}
    }
  return ans;
}

bool in(vector<int> a, int x)
{
  for(int i=0; i<a.size(); i++)
    {
      if(a[i] == x) return true;
    }
  return false;
}

string to_string1(int x)
{
  if(x == 0) return "0";
  string ans = ""; 
  while(x)
    {
      ans += ('0' + x%10);
      x = x/10;
    }
  reverse(ans.begin(), ans.end());
  return ans;
}





