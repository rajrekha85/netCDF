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
#define ERRCODE 2
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(ERRCODE);}

int main()
{
	float threshold;
	int nodeCount=0;
	int printNode = 0;
	int data_time[NTIME];
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
			nc_get_var_int(ncid, i, &data_time[0]);
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

	vector<boost::shared_array<float>> testing_windx;
	vector<boost::shared_array<float>> testing_windy;

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
						int countNodex=1;
						for(int incNode=0;incNode<10;incNode++)
						{
							size_t start[] = {0,incNode};
							size_t end[] = {95,countNodex};
							boost::shared_array<float> temp(new float[node]);
							nc_get_vara_float (ncid, i, start, end, temp.get());
							testing_windx.push_back(temp);
						}
						found = true;
					}

					else if (attnames[j] == "windy")
					{	
						int countNodey=1;						
						for(int incNodeY=0;incNodeY<10;incNodeY++)
						{
							size_t start[] = {0,incNodeY};
							size_t end[] = {95,countNodey};
							boost::shared_array<float> temp(new float[node]);
							nc_get_vara_float (ncid, i, start, end, temp.get());
							testing_windy.push_back(temp);
						}
						found = true;
					}
				}
			}

		}		 
	}


	for(int loopNode=0;loopNode<10;loopNode++)
	{
		for(int timeStepLoop=0;timeStepLoop<96;timeStepLoop++)
		{
			if((sqrt((testing_windx[loopNode].get()[timeStepLoop])+(testing_windy[loopNode].get()[timeStepLoop]))) > threshold)
			{
				nodeCount=nodeCount+1;
				data_out[printNode]= data_time[timeStepLoop];
				printNode=printNode+1;
				break;
			}

		}
	}

	int dimids[2];
	int node_dimid;
	int varid;


	if ((retval = nc_create(OUTPUT_FILE_NAME, NC_CLOBBER, &nc_output_file_id)))
		ERR(retval);
	if ((retval = nc_def_dim(nc_output_file_id, "time", nodeCount, &node_dimid)))
		ERR(retval);	

	dimids[0] = node_dimid;

	if ((retval = nc_def_var(nc_output_file_id, "time", NC_INT, 1,
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
	printf("\nSuccess creating the output file %s!\n", OUTPUT_FILE_NAME);
	return 0;
}