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
  bool is_int;
  //clone the data structure
  //learn about erase() and vector iterators
  vector<string> t_att = table.get_att(); //list of attributes from the table
  vector<Column> t_col;

  for(int i = 0; i < u_att.size(); i++) //start searching for the attributes the user is looking for...
    for(int j = 0; j < t_att.size(); j++) //...at the beginning of the list of attributes in the table
      if(u_att.at(i).equals(t_att.at(j))) //found the attribute in the table
      {
        t_col = table.get_col(j);
        for(int k = 0; k < t_col.size(); k++) //find matching values
        {
          op = u_op.at(i);
          if(!(eval(u_arg.at(i), t_col.at(k), op, is_int))) //tuple kind != what you're looking for
          {
            //remove that record from the clone
          }
        }
      }
}

//same for project

//same for rename

bool eval(string u_arg, string t_arg, Op op, bool is_int)
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
    //some default stuff
    break;
  }
}