     #include <stdlib.h>
     #include <stdio.h>
     #include <netcdf.h>
     
     #define FILE_NAME "fort.74.nc"    
     #define NX 8
     #define NY 11
	 #define total 96
     #define ERRCODE 2
     #define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(ERRCODE);}
     
     int main()
     {
        int ncid, varid;   /* netCDF id for the file and the data variable */  
		int data_in[total];
        int x, retval;
     
        if ((retval = nc_open(FILE_NAME, NC_NOWRITE, &ncid))) /* Open the file. NC_NOWRITE asks for a read-only access */
           ERR(retval);     
       
        if ((retval = nc_inq_varid(ncid, "time", &varid)))  /* Get the varid of the data variable, based on its name. */
           ERR(retval);  

		if ((retval = nc_get_var_int(ncid, varid, &data_in[0])))
           ERR(retval);
     
        /* Check the data and print the data */
		for (x = 0; x < total; x++)
		{
			printf("\n");
		    printf("%3d", data_in[x]);
		}
     
        /* Close the file. */
        if ((retval = nc_close(ncid)))
           ERR(retval);
     
		printf("\n");
        printf("SUCCESS reading time data from the file %s!\n", FILE_NAME);
		system("PAUSE");
        return 0;
     }
	 