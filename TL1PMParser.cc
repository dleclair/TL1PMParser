

#include <map>
#include <iostream>
#include <fstream>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;
using namespace boost::algorithm;


struct PMBin
{
    string bin_date;
    string bin_time;
    float opr_min;
    float opr_max;
    float opt_min;
    float opt_max;

    bool initialize(string mondat, string montim, string montype, float monval)
    {
        bin_date = mondat;
        bin_time = montim;
        bool add_it = true;
        if (montype == "OPT-MAX")
            opt_max = monval;
        else if (montype == "OPT-MIN")
            opt_min = monval;
        else if (montype == "OPR-MAX")
            opr_max = monval;
        else if (montype == "OPR-MIN")
            opr_min = monval;
        else
        {
            //cout << "skipping montype: " << montype << endl;
            add_it = false;
        }

        return (add_it);
    }
};




int main(int argc, const char *argv[])
{
    ifstream infile(argv[1]);

    string line;

    typedef map<int, PMBin> BinMap;
    typedef map<string, BinMap> AIDMap;

    AIDMap aid_map;

    while (infile >> line)
    {
        

        if (line.find("\"ROB") != string::npos)
        {
            erase_all(line, "\"");
            
            typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
            char_separator<char> sep(":,", "");
            tokenizer tokens(line, sep);

            //tokenizer<escaped_list_separator<char> > tok(line);
            int count = 0;
            string aid = "";
            PMBin bin;
            string montype = "";
            int bin_index = -1;
            float monval = 0.0;
            string mondat = "";
            string montim = "";

            for(tokenizer::iterator i=tokens.begin(); i != tokens.end(); i++)
            {
                count++;

                switch(count)
                {
                    case 1:
                        aid = *i;
                        break;
                    case 3:
                        montype = *i;
                        break;
                    case 4:
                        monval = atof(i->c_str());
                        break;
                    case 8:
                        mondat = *i;
                        break;
                    case 9:
                        montim = *i;
                        break;
                    case 10:
                        bin_index = atoi(i->c_str());
                        break;
                    default:
                        break;
                }
            }

            AIDMap::iterator i;
            if ((i = aid_map.find(aid)) != aid_map.end())
            {
                BinMap::iterator j;
                if ((j = i->second.find(bin_index)) != i->second.end())
                {
                    //bin exists, add to it.
                    if (montype == "OPT-MAX")
                        j->second.opt_max = monval;
                    else if (montype == "OPT-MIN")
                        j->second.opt_min = monval;
                    else if (montype == "OPR-MAX")
                        j->second.opr_max = monval;
                    else if (montype == "OPR-MIN")
                        j->second.opr_min = monval;
                    //else
                        //cout << "skipping montype: " << montype << endl;
                }
                else
                {
                    PMBin bin;
                    if (bin.initialize(mondat, montim, montype, monval))
                    {
                        i->second[bin_index] = bin;
                    }
                }
            }
            else
            {
                PMBin bin;
                if (bin.initialize(mondat, montim, montype, monval))
                {
                    BinMap binmap;
                    binmap[bin_index] = bin;
                    aid_map[aid] = binmap;
                }
            }
        }
    }

    infile.close();

    for (AIDMap::iterator i = aid_map.begin(); i != aid_map.end(); i++)
    {
        cout << "For AID=" << i->first << endl;
        for (BinMap::iterator j = i->second.begin(); j != i->second.end(); j++)
        {
            if ((j->second.opt_max - j->second.opt_min) > 3.0)
            {
                cout << "bin:" << j->first;
                cout << ", date:" << j->second.bin_date;
                cout << ", time:" << j->second.bin_time;
                cout << ", opt-max:" << j->second.opt_max;
                cout << ", opt-min:" << j->second.opt_min;
                cout << endl;
            }
            
            if ((j->second.opr_max - j->second.opr_min) > 3.0)
            {
                cout << "bin:" << j->first;
                cout << ", date:" << j->second.bin_date;
                cout << ", time:" << j->second.bin_time;
                cout << ", opr-max:" << j->second.opr_max;
                cout << ", opr-min:" << j->second.opr_min;
                cout << endl;
            }
        }
        cout << endl;
    }

    return 0;

}
