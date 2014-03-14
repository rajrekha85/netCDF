     #include <stdlib.h>
     #include <stdio.h>
     #include <netcdf.h>
     
     #define FILE_NAME "simple.nc"    
     #define NX 6
     #define NY 12
     #define ERRCODE 2
     #define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(ERRCODE);}
     
     int main()
     {
        int ncid, varid;   /* netCDF id for the file and the data variable */  
        int data_in[NX][NY];
        int x, y, retval;
     
        if ((retval = nc_open(FILE_NAME, NC_NOWRITE, &ncid))) /* Open the file. NC_NOWRITE asks for a read-only access */
           ERR(retval);     
       
        if ((retval = nc_inq_varid(ncid, "data", &varid)))  /* Get the varid of the data variable, based on its name. */
           ERR(retval);     
        
        if ((retval = nc_get_var_int(ncid, varid, &data_in[0][0]))) /* Read the data. */
           ERR(retval);
     
        /* Check the data and print the data */
        for (x = 0; x < NX; x++)
           for (y = 0; y < NY; y++)
     	 if (data_in[x][y] != x * NY + y)
     	    return ERRCODE;
		 else
		 {
			 printf("\n");
			 printf("%3d", data_in[x][y]);
		 }
     
        /* Close the file. */
        if ((retval = nc_close(ncid)))
           ERR(retval);
     
        printf("*** SUCCESS reading example file %s!\n", FILE_NAME);
		system("PAUSE");
        return 0;
     }
	 