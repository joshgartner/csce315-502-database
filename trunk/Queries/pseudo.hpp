//https://svn.cse.tamu.edu

#include "our_stuff.h"
#include <vector>
#include <string> 

enum Op {EQ, NE, LT, LTE, GT, GTE};

using namespace std;

/**
 * Select columns in att from table after applying op arg
 */
Table select(Table table, vector<string> u_att, vector<string> u_arg, vector<Op> u_op)
{
  Op op;
  //clone the data structure
  //learn about erase() and vector iterators
  vector<string> t_att = table.get_att(); //list of attributes from the table
  vector<Column> t_col;

  for(int i = 0; i < u_att.size(); i++) //start search for user-defined attributes...
    for(int j = 0; j < t_att.size(); j++) //...at top of table attribute list
      if(u_att.at(i).equals(t_att.at(j))) //found the attribute in the table attribute list
      {
        t_col = table.get_col(j);
        for(int k = 0; k < t_col.size(); k++) //find matching values
        {
          op = u_op.at(i);
          if(!(eval(u_arg.at(i), t_col.at(k), op))) //tuple kind != search value
          {
            //FIXME
            //remove that record from the clone
            //check vitest.cpp
            //start_iterator is at beginning of clone
            //start_iterator + k SHOULD lead to the record to be removed...
          }
        }
      }
  return something;
}

Table project(Table table, vector<string> u_att, vector<string> u_arg)
{
  vector<string> t_att = table.get_att(); //list of attributes from the table

  //FIXME
  for(int i = 0; i < u_att.size(); i++) //start search for user-defined attributes...
    for(int j = 0; j < t_att.size(); j++) //...at top of table attribute list
      if(u_att.at(i).equals(t_att.at(j))) //found the attribute in the table attribute list
      {
        //now just add this value to the view with the new projected name
      }
  return something;
}

//does rename actually change the name in the table?
Table project(Table table, vector<string> u_att, vector<string> u_arg)
{
  //FIXME
  return something;
}

bool eval(string u_arg, string t_arg, Op op)
{
  switch(op)
  {
  case EQ:
    return(u_arg.compare(t_arg) == 0);
    break;
  case NE:
    return(u_arg.compare(t_arg) != 0);
    break;
  case LT:
    return(u_arg.compare(t_arg) < 0);
    break;
  case LTE:
    return(u_arg.compare(t_arg) <= 0);
    break;
  case GT:
    return(u_arg.compare(t_arg) > 0);
    break;
  case GTE:
    return(u_arg.compare(t_arg) >= 0);
    break;
  default:
    cerr << "ERROR: Unknown operator\n" << endl;
    break;
  }
  return false;
}
