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
//#define node 1181186
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

	vector<boost::shared_array<float>> depth;

	for (int i = 0; i < attnames.size(); ++i)
		depth.push_back(boost::shared_array<float>(new float[node]));	

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
						size_t start[] = {timestep,0};
						nc_get_vara_float (ncid, i, start, end, depth[j].get());

						float *bptr=depth[j].get();

						for ( int i = 0; i < 5; i++ )
						{
							std::cout<<setprecision(3);
							cout << "*(bptr + " << i << ") : ";
							cout << *(bptr + i) << endl;
						}

						found = true;
					}

				}
			}
		}		 
	}

	boost::shared_array<int> ele = boost::shared_array<int>(new int[nele]);



	/*for (int i = 1; i < nele; i++)
	{
	int index = i;
	int ind = ele.get()[index]-1;
	double z1 = depth[0].get()[ind];
	cout<<"\n";
	cout<<"z1"<<z1;
	}*/

	if ((retval = nc_close(ncid)))
		ERR(retval);

	printf("\n");
	printf("SUCCESS reading time data from the file %s!\n", FILE_NAME);
	system("PAUSE");
	return 0;
}