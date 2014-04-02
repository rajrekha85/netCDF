#include <stdlib.h>
#include <stdio.h>
#include <netcdf.h>
#include <iomanip>

#include<vector>
#include <fstream>
#include<boost\algorithm\string.hpp>
#include<boost\shared_array.hpp>
#include<boost\program_options.hpp>
#include<boost\program_options\parsers.hpp>

using namespace std;

#define FILE_NAME "fort.74.nc"    
#define NX 8
#define NY 11
#define timestep 96
#define ERRCODE 2
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(ERRCODE);}

int main()
{
	int retval, ncid, ndims, nvars, ngatts, unlimdimid;

	int nele = 0;
	int node = 0;

	vector<string> attnames;
	attnames.push_back("windx");	 

	if ((retval = nc_open(FILE_NAME, NC_NOWRITE, &ncid)))
		return false;

	nc_inq(ncid, &ndims, &nvars, &ngatts, &unlimdimid);

	for (int i = 0; i < ndims; ++i)
	{
		using namespace std;
		char* name = new char[1024];
		size_t lenp = 0;
		nc_inq_dim(ncid,i,name,&lenp);
		string finder = name;
		if (finder.find("nele") != -1)
			nele = lenp;
		else if (finder.find("node") != -1)
			node = lenp;		
	}

	vector<boost::shared_array<float>> depth;

	for (int i = 0; i < attnames.size(); ++i)
		depth.push_back(boost::shared_array<float>(new float[node]));	

	for (int i = 0; i < nvars; ++i)
	{
		using namespace std;
		char* name = new char[1024];
		nc_inq_varname(ncid,i,name);
		string finder = name;
		bool found = false;
		{
			for (int j = 0; j < attnames.size(); ++j)
			{ 
				if (finder.find(attnames[j]) != -1)
				{
					if (attnames[j] == "windx")
					{	
						size_t end[] = {1,node};
						size_t start[] = {95,0};
						nc_get_vara_float (ncid, i, start, end, depth[j].get());
						boost::shared_array<float> sa_wind= depth[0];
						for ( int z = 0; z < 30; z++ )
						{
							std::cout<<setprecision(3);
							cout<<"\n";
							cout << sa_wind[z];
						}

						found = true;
					}

				}
			}

		}		 
	}


	if ((retval = nc_close(ncid)))
		ERR(retval);

	printf("\n");
	printf("SUCCESS reading time data from the file %s!\n", FILE_NAME);
	system("PAUSE");
	return 0;
}