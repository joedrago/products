#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <string>
#include <vector>
#include <map>

using namespace std;

typedef vector<string> StringVector;
typedef map<string, StringVector> StringVectorMap;

void appendAll(StringVector &v, const string &s)
{
    // This function needs to ensure that the vector has SOMETHING in it when it ends,
    // even if it is just a single empty string, otherwise you'll be "multiplying"
    // a string vector with a size of 0, which will make a new vector of length zero.

    if(v.size())
    {
        for(StringVector::iterator it = v.begin(); it != v.end(); ++it)
        {
            *it = *it + s;
        }
    }
    else
    {
        v.push_back(s);
    }
}

const char * product(StringVectorMap &vars, const char *in, StringVector &out, char end = 0)
{
    string leftovers;
    const char *c = in;
    for(; *c && *c != end; ++c)
    {
        if(*c == '{')
        {
            // Append any additional text we've seen to each entry in list we have so far
            appendAll(out, leftovers);
            leftovers = "";

            ++c;

            bool uppercase = false;
            const char *colon = strchr(c, ':');
            const char *nextBackslash = strchr(c, '\\');
            const char *nextOpenBrace = strchr(c, '{');
            const char *nextCloseBrace = strchr(c, '}');
            if( colon
            && (!nextBackslash  || colon < nextBackslash)
            && (!nextOpenBrace  || colon < nextOpenBrace)
            && (!nextCloseBrace || colon < nextCloseBrace))
            {
                for(; c != colon; ++c)
                {
                    switch(*c)
                    {
                        case 'u': 
                            uppercase = true;
                            break;
                    }
                }
                ++c;
            }

            // Now look up all variables contained in the {}
            StringVector varnames;
            StringVector temp;
            c = product(vars, c, varnames, '}');

            // Loop over all found variables and multiply the strings into a new list
            for(StringVector::iterator outer = out.begin(); outer != out.end(); ++outer)
            {
                for(StringVector::iterator inner = varnames.begin(); inner != varnames.end(); ++inner)
                {
                    StringVector &v = vars[*inner];
                    if(v.size())
                    {
                        for(StringVector::iterator vit = v.begin(); vit != v.end(); ++vit)
                        {
                            string val = *vit;
                            if(uppercase)
                            {
                                std::transform(val.begin(), val.end(), val.begin(), ::toupper);
                            }
                            temp.push_back(*outer + val);
                        }
                    }
                    else
                    {
                        temp.push_back(*outer);
                    }
                }
            }
            out.swap(temp);
        }
        else
        {
            // Just append 
            leftovers += *c;
        }
    }

    // If there was stuff trailing at the end, be sure to grab it
    appendAll(out, leftovers);
    return c;
}

void test(StringVectorMap &vars, const char *s)
{
    printf("-------------------------------------------\n");
    printf("Testing: '%s'\n", s);

    StringVector out;
    product(vars, s, out);
    for(StringVector::iterator it = out.begin(); it != out.end(); ++it)
    {
        printf("    %s\n", it->c_str());
    }
}

int main(int argc, char **argv)
{
    StringVectorMap vars;
    vars["FOO"].push_back("foo");
    vars["BAR"].push_back("bar1");
    vars["BAR"].push_back("bar2");
    vars["BAZ"].push_back("baz1");
    vars["BAZ"].push_back("baz2");
    vars["BAZ"].push_back("baz3");

    vars["INDIRECT"].push_back("FOO");
    vars["INDIRECT"].push_back("BAR");
    vars["INDIRECT"].push_back("BAZ");

    test(vars, "hello {FOO} {BAR} {BAZ}!");
    test(vars, "hello {u:FOO} {BAR} {BAZ}!");
    test(vars, "hello {{INDIRECT}}!");
    test(vars, "hello {u:{INDIRECT}}!");
    return 0;
}
