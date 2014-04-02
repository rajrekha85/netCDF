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

	/*string xs, ys;*/

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


	/*boost::shared_array<int> ele;
	boost::shared_array<double> x = boost::shared_array<double>(new double[node]);
	boost::shared_array<double> y = boost::shared_array<double>(new double[node]);*/
	vector<boost::shared_array<float>> depth;

	for (int i = 0; i < attnames.size(); ++i)
		depth.push_back(boost::shared_array<float>(new float[node]));	

	/*ele = boost::shared_array<int>(new int[nele*3]);*/

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
						size_t end[] = {95,1};
						size_t start[] = {1,0};
						nc_get_vara_float (ncid, i, start, end, depth[j].get());
						
						//boost::shared_array<float> wind=depth[0].get();
						boost::shared_array<float> sa_wind= depth[0];
						for ( int z = 0; z < 10; z++ )
						{
							std::cout<<setprecision(3);
							cout<<"\n";
							//cout << "*(bptr + " << z << ") : ";
							cout << sa_wind[z];
						}

						found = true;
					}

				}
			}
			/*if (!found)
			{
				if (finder.find(xs) != -1 && finder.size() == xs.size())
				{
					size_t end = node;
					size_t start = 0;
					nc_get_vara_double (ncid, i, &start, &end, x.get());
				}
				else if (finder.find(ys) != -1 && finder.size() == xs.size())
				{
					size_t end = node;
					size_t start = 0;
					nc_get_vara_double (ncid, i, &start, &end, y.get());
				}
				else if (finder.find("element") != -1)
				{
					size_t end[] = {nele,3};
					size_t start[] = {0,0};
					nc_get_vara_int (ncid, i, start, end, ele.get());
				}
			}*/
		}		 
	}



	//boost::shared_array<int> ele = boost::shared_array<int>(new int[nele]);

	//for (int i = 1; i < nele/3; i++)
	//{
	//	int index = i*3;
	//	int ind = ele.get()[index]-1;
	//	double x1 = x.get()[ind];
	//	double y1 = y.get()[ind];
	//	double z1 = depth[0].get()[ind];
	//	cout<<"z1  "<<z1;
	//	index++;
	//	ind = ele.get()[index]-1;
	//	double x2 = x.get()[ind];
	//	double y2 = y.get()[ind];
	//	double z2 = depth[0].get()[ind];
	//	cout<<"z2  "<<z2;
	//	index++;
	//	ind = ele.get()[index]-1;
	//	double x3 = x.get()[ind];
	//	double y3 = y.get()[ind];
	//	double z3 = depth[0].get()[ind];
	//	cout<<"z3   "<<z3;
	//	/*if (z1 > -999 && z2 > -999 && z3 > -999)
	//	{
	//	fout << "v " << x1 << " " << z1 << " " << y1 << endl;
	//	fout << "v " << x2 << " " << z2 << " " << y2 << endl;
	//	fout << "v " << x3 << " " << z3 << " " << y3 << endl;
	//	fout << "f " << count << " " << count + 1 << " " << count + 2 << endl;
	//	count += 3;
	//	}*/
	//}



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