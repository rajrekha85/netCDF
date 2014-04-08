#include <stdlib.h>
#include<math.h>
#include <stdio.h>
#include <netcdf.h>
#include <iomanip>


#include<vector>
#include <fstream>
#include<boost\algorithm\string.hpp>
#include<boost\shared_array.hpp>
#include<boost\program_options.hpp>

using namespace std;

#define FILE_NAME "fort.74.nc" 
#define OUTPUT_FILE_NAME "fort.74.write.nc"
#define NX 8
#define NY 11
#define NTIME 96
//#define timestep 96
#define ERRCODE 2
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(ERRCODE);}

int main()
{
	float threshold;
	int nodeCount=0;
	int printNode = 0;
	cout<<"Enter the threshold value : ";
	cin>>threshold;
	int retval, ncid, ndims, nvars, ngatts, unlimdimid;
	int nc_output_file_id;

	int nele = 0;
	int node = 0;
	int timestep=95;
	float data_out[96];
	int time;
	bool hasTime = false;

	vector<string> attnames;
	attnames.push_back("windx");	 
	attnames.push_back("windy");	 

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
		else if (finder.find("time") != -1)
		{
			time = lenp;
			if (time > 1)
				hasTime = true;
		}
	}

	vector<boost::shared_array<float>> windx;
	vector<boost::shared_array<float>> windy;
	boost::shared_array<float> sa_windy;
	boost::shared_array<float> sa_windx;
	windx.push_back(boost::shared_array<float>(new float[node]));	
	windy.push_back(boost::shared_array<float>(new float[node]));	


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
						nc_get_vara_float (ncid, i, start, end, windx[0].get());
						sa_windx= windx[0];
						found = true;
					}

					else if (attnames[j] == "windy")
					{	
						size_t end[] = {1,node};
						size_t start[] = {timestep,0};
						nc_get_vara_float (ncid, i, start, end, windy[0].get());
						sa_windy= windy[0];
						found = true;
					}

				}
			}

		}		 
	}
	
	for(int p=0;p<30;p++)
	{
		/*cout<<"\n**********************";
		cout<<"\nNode : "<<p;
		cout<<"\nWindx : "<<sa_windx[p];
		cout<<"\nWindy : "<<sa_windy[p];*/
		if((sqrt(sa_windx[p]*sa_windx[p]+sa_windy[p]*sa_windy[p])) > threshold)
		{
			cout<<"\n"<<"Node : "<<p;
			cout<<"\n"<<"Wind Speed : "<<(sqrt(sa_windx[p]*sa_windx[p]+sa_windy[p]*sa_windy[p]));
			nodeCount=nodeCount+1;
			data_out[printNode]= p;
			printNode=printNode+1;
		}
		    
	}

	int dimids[2];
	int node_dimid;
	int varid;
	

	if ((retval = nc_create(OUTPUT_FILE_NAME, NC_CLOBBER, &nc_output_file_id)))
		ERR(retval);
	if ((retval = nc_def_dim(nc_output_file_id, "node", nodeCount, &node_dimid)))
		ERR(retval);	

	dimids[0] = node_dimid;

	if ((retval = nc_def_var(nc_output_file_id, "node", NC_INT, 1,
		dimids, &varid)))
		ERR(retval);
	if ((retval = nc_enddef(nc_output_file_id)))
		ERR(retval);
	if ((retval = nc_put_var_float(nc_output_file_id, varid, &data_out[0])))
		ERR(retval);
	if ((retval = nc_close(nc_output_file_id)))
		ERR(retval);

	if ((retval = nc_close(ncid)))
		ERR(retval);

	printf("\n\n\n");
	printf("Success reading the file %s!\n", FILE_NAME);
	printf("\nSuccess creating the output file %s!\n", FILE_NAME);
	return 0;
}